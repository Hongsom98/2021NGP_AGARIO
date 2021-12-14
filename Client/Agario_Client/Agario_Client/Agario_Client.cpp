#include "pch.h"
#include "framework.h"
#include "Agario_Client.h"
#include "UserDefine.h"
#include "ServerFunction.h"
#include "Player.h"
#include "GameObject.h"
#include "Map.h"
#define FPS 30


Map map;
POINT Mouse{ 0,0 };
TCHAR InputID[12] = { 0 };
HDC memDC;
RECT ClientRect;
HBITMAP hBitmap;
bool GameOver{ false };
POINT LowRank;

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

    /*
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
    */

    system_clock::time_point LastTime = system_clock::now();
    system_clock::time_point NowTime = LastTime;
    float FrameCnt = 1.f / FPS;

    while (msg.message != WM_QUIT)
    {
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            NowTime = system_clock::now();
            if (LastTime > NowTime) LastTime = NowTime;
            float dt = duration<double>(NowTime - LastTime).count();
            if (dt > FrameCnt) {
                Update();
                Render();
                LastTime = system_clock::now();
            }
        }
    }

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
            GetClientRect(hWnd, &ClientRect);
            break;
        case WM_CHAR:
            if (wParam == VK_ESCAPE) exit(0);
            if (wParam == VK_RETURN) {
                if (!isConnection) {
                    SendID(InputID);
                    if (RecvIDCheck()) {
                        isConnection = true; }
                    else {
                        memset(InputID, 0, 12);
                        len = 0;
                    }
                }
                if (GameOver) {
                    PostQuitMessage(0);
                }
            }
            else if (wParam == VK_BACK) {
                if (len == 0) break;
                InputID[--len] = 0;
            }
            else {
                if (len == 12 || isConnection) break;
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

void CheckGameOver()
{
    for (int i = 0; i < 3; ++i)
        if (player[i].GetRadius() == 0 && player[(i + 1) % 3].GetRadius() == 0) {
            GameOver = true;
            LowRank.x = i; LowRank.y = (i + 1) % 3;
            return;
        }
}

void Update()
{
    if (!isConnection || GameOver) return;

    GetCursorPos(&Mouse);
    ScreenToClient(hWnd, &Mouse);

    if (GetAsyncKeyState(0x5A) & 0x0001)
    {
        SendInputData(Mouse, 'z');
    }
    else if (GetAsyncKeyState(0x58) & 0x0001)
    {
        SendInputData(Mouse, 'x');
    }
    else
    {
        SendInputData(Mouse);
    }

    GameObejctPacket packet = RecvObjects();
    for (int i = 0; i < CLIENT; ++i) player[i].Update(packet.playerlist[i]);
    feeds.Update(packet.feedlist, packet.projectile);

    CheckGameOver();
}

void Render()
{
    HDC hdc = GetDC(hWnd);
    PatBlt(memDC, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, WHITENESS);

    if (GameOver) {
        TCHAR ranking[30];
        memset(ranking, 0, 30);
        TextOut(memDC, ClientRect.right / 2 - lstrlen("GAME OVER") / 2, ClientRect.bottom / 2 - 32, "GAME OVER", lstrlen("GAME OVER"));
        wsprintf(ranking, "1위 : %s - %d점", player[3 - LowRank.x - LowRank.y].GetID(), player[3 - LowRank.x - LowRank.y].GetScore());
        TextOut(memDC, ClientRect.right / 2 - lstrlen(ranking) / 2, ClientRect.bottom / 2 - 16, ranking, lstrlen(ranking));
        memset(ranking, 0, 30);
        if (player[LowRank.x].GetScore() > player[LowRank.y].GetScore()) {
            wsprintf(ranking, "2위 : %s - %d점", player[LowRank.x].GetID(), player[LowRank.x].GetScore());
            TextOut(memDC, ClientRect.right / 2 - lstrlen(ranking) / 2, ClientRect.bottom / 2, ranking, lstrlen(ranking));
            memset(ranking, 0, 30);
            wsprintf(ranking, "3위 : %s - %d점", player[LowRank.y].GetID(), player[LowRank.y].GetScore());
            TextOut(memDC, ClientRect.right / 2 - lstrlen(ranking) / 2, ClientRect.bottom / 2 + 16, ranking, lstrlen(ranking));
        }
        else {
            wsprintf(ranking, "2위 : %s - %d점", player[LowRank.y].GetID(), player[LowRank.y].GetScore());
            TextOut(memDC, ClientRect.right / 2 - lstrlen(ranking) / 2, ClientRect.bottom / 2, ranking, lstrlen(ranking));
            memset(ranking, 0, 30);
            wsprintf(ranking, "3위 : %s - %d점", player[LowRank.x].GetID(), player[LowRank.x].GetScore());
            TextOut(memDC, ClientRect.right / 2 - lstrlen(ranking) / 2, ClientRect.bottom / 2 + 16, ranking, lstrlen(ranking));
        }

        BitBlt(hdc, 0, 0, ClientRect.right, ClientRect.bottom, memDC, 0, 0, SRCCOPY);
    }
    else if (isConnection) {
        map.Draw(memDC);
        feeds.Draw(memDC);
        for (int i = 0; i < CLIENT; ++i) player[i].Draw(memDC);

        BitBlt(hdc, 0, 0, ClientRect.right, ClientRect.bottom, memDC, 0, 0, SRCCOPY);
    }
    else {
        TCHAR Log[8] = "LOGIN :";
        TextOut(memDC, WINDOW_WIDTH / 2-60 , WINDOW_HEIGHT / 2,
            Log, lstrlen(Log));
        MoveToEx(memDC, WINDOW_WIDTH / 2-70, WINDOW_HEIGHT / 2-10, NULL);
        LineTo(memDC, WINDOW_WIDTH / 2-70, WINDOW_HEIGHT/2 + 30);
        
        MoveToEx(memDC, WINDOW_WIDTH / 2 - 70, WINDOW_HEIGHT / 2 - 10, NULL);
        LineTo(memDC, WINDOW_WIDTH / 2 + 70, WINDOW_HEIGHT / 2 -10);

        MoveToEx(memDC, WINDOW_WIDTH / 2 + 70, WINDOW_HEIGHT / 2 - 10, NULL);
        LineTo(memDC, WINDOW_WIDTH / 2 + 70, WINDOW_HEIGHT / 2 + 30);

        MoveToEx(memDC, WINDOW_WIDTH / 2 - 70, WINDOW_HEIGHT / 2 + 30, NULL);
        LineTo(memDC, WINDOW_WIDTH / 2 + 70, WINDOW_HEIGHT / 2 + 30);

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