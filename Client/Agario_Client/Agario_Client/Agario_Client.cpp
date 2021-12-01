#include "pch.h"
#include "framework.h"
#include "Agario_Client.h"
#include "UserDefine.h"
#include "ServerFunction.h"
#include "Player.h"
#include "GameObject.h"
#include "Map.h"
#define FPS 30

Player player[3];
GameObject feeds;
Map map;
POINT camera{ 50, 50 };
POINT Mouse{ 0,0 };
TCHAR InputID[12] = { 0 };
bool isConnection{ false };
HDC memDC;
HBITMAP hBitmap;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_AGARIOCLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow)) return FALSE;

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_AGARIOCLIENT));

    MSG msg;
    memset(&msg, 0, sizeof(msg));
    BOOL PerformFlg = FALSE;
    LONGLONG NowTime = 0;
    LONGLONG LastTime = 0;
    LONGLONG Frequency = 0;
    LONGLONG OneFrameCnt = 0;

    if (::QueryPerformanceFrequency((LARGE_INTEGER*)&Frequency)) {
        PerformFlg = TRUE;
        OneFrameCnt = Frequency / FPS;
        ::QueryPerformanceCounter((LARGE_INTEGER*)&LastTime);
    }
    else {
        PerformFlg = FALSE;
        timeBeginPeriod(1);
        OneFrameCnt = 1000 / FPS;
        LastTime = (LONGLONG)::timeGetTime();
    }


    while (msg.message != WM_QUIT)
    {
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            if (PerformFlg) ::QueryPerformanceCounter((LARGE_INTEGER*)&NowTime);
            else {
                NowTime = ::timeGetTime();
                if (LastTime > NowTime) LastTime = NowTime;
            }
            if (NowTime >= LastTime + OneFrameCnt) {
                Update();
                Render();
                LastTime = NowTime;
            }
        }
    }
   
    if (PerformFlg == FALSE) timeEndPeriod(1);

    closesocket(sock);
    WSACleanup();

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_AGARIOCLIENT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_AGARIOCLIENT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; 

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, WINDOW_WIDTH, WINDOW_HEIGHT, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd) return FALSE;
  
   HDC hdc = GetDC(hWnd);
   memDC = CreateCompatibleDC(hdc);
   hBitmap = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
   SelectObject(memDC, (HBITMAP)hBitmap);
   ReleaseDC(hWnd, hdc);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int len = 0;

    switch (message)
    {
        case WM_CREATE:
            break;
        case WM_CHAR:
            if (wParam == VK_ESCAPE) exit(0);
            if (wParam == VK_RETURN) {
                if (!isConnection) {
                    SendID(InputID);
                    if (RecvIDCheck()) isConnection = true;
                    else {
                        memset(InputID, 0, 12);
                        len = 0;
                    }
                }
            }
            else if (wParam == VK_BACK) {
                if (len == 0) break;
                InputID[--len] = 0;
            }
            else {
                if (len == 12) break;
                InputID[len++] = (TCHAR)wParam;
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            return (INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return (INT_PTR)TRUE;
            }
            break;
    }
    return (INT_PTR)FALSE;
}

void Update()
{
    if (!isConnection) return;

    GetCursorPos(&Mouse);
    ScreenToClient(hWnd, &Mouse);

    if (GetKeyState(0x5A) & 0x8000)
    {
        SendInputData(Mouse, 'z');
        
    }
    else if (GetKeyState(0x58) & 0x8000)
    {
        SendInputData(Mouse, 'x');
    }
    else
    {
        SendInputData(Mouse);
    }

    GameObejctPacket packet = RecvObjects();
    for (int i = 0; i < CLIENT; ++i) player[i].Update(packet.playerlist[i]);
    feeds.Update(packet.feedlist);
}

void Render()
{
    HDC hdc = GetDC(hWnd);
    PatBlt(memDC, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, WHITENESS);

    if (isConnection) {
        map.Draw(memDC);
        for(int i = 0 ; i < CLIENT; ++i) player[i].Draw(memDC);
        feeds.Draw(memDC);

        for (int i = 0; i < CLIENT; ++i) {
            if (!strncmp(InputID, player[i].GetID(), 12)) {
                if(camera.x < camera.x + player[i].GetRadius() - 10.f)
                    camera.x += player[i].GetRadius() - 10.f;
                if (camera.y < camera.y + player[i].GetRadius() - 10.f)
                    camera.y += player[i].GetRadius() - 10.f;
                POINT playerCenter = player[i].GetCenter();
                StretchBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, memDC,
                    playerCenter.x - camera.x, playerCenter.y - camera.y,
                    camera.x * 2, camera.y * 2, SRCCOPY);
                break;
            }
        }
    }
    else {
        HPEN hpen = (HPEN)CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
        HPEN oldpen = (HPEN)SelectObject(hdc, hpen);
        SetBkMode(hdc, TRANSPARENT);
        TextOut(memDC, WINDOW_WIDTH / 2 - lstrlen(InputID), WINDOW_HEIGHT / 2,
            InputID, lstrlen(InputID));
        SelectObject(hdc, oldpen);
        DeleteObject(hpen);

        BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, memDC, 0, 0, SRCCOPY);
    }

    ReleaseDC(hWnd, hdc);
}