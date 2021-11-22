#include "pch.h"
#include "framework.h"
#include "Agario_Client.h"
#include "UserDefine.h"
#include "ServerFunction.h"
#include "Player.h"
#include "GameObject.h"
#include "Map.h"


#define MAX_LOADSTRING 100

HINSTANCE hInst;                                
WCHAR szTitle[MAX_LOADSTRING];                  
WCHAR szWindowClass[MAX_LOADSTRING];     
HWND hWnd;
Player player;
GameObject feeds;
Map map;
POINT camera{ 300, 300 };
TCHAR InputID[12] = { 0, };
bool isConnection{ false };

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void Update();
void Render();

void FORTEST()
{
    player.Init("TEST");
    Feed test[MAXFEED];
    test[0].Center = { 400,400 }; test[0].Radius = 10;
    test[1].Center = { 450,400 }; test[1].Radius = 10;
    feeds.Update(test);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, 
    _In_ LPWSTR    lpCmdLine, _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_AGARIOCLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_AGARIOCLIENT));

    MSG msg;
    memset(&msg, 0, sizeof(msg));

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            Update();
            Render();
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

   if (!hWnd)
   {
      return FALSE;
   }

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
        FORTEST();
        break;
    case WM_CHAR:
        if (wParam == VK_RETURN)
            SendID(InputID);
        else if (wParam == VK_BACK) {
            if (len == 0) break;
            InputID[--len] = 0;
        }
        else {
            if (len == 12) break;
            InputID[len++] = (TCHAR)wParam;
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
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
    if (isConnection) {

    }
    else {

    }
}

void Render()
{
    HDC hdc = GetDC(hWnd);
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, MAP_WIDTH, MAP_HEIGHT);
    SelectObject(memDC, (HBITMAP)hBitmap);
    PatBlt(memDC, 0, 0, MAP_WIDTH, MAP_HEIGHT, WHITENESS);

    if (isConnection) {
        map.Draw(memDC);
        player.Draw(memDC);
        feeds.Draw(memDC);

        POINT playerCenter = player.GetCenter();
        StretchBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, memDC,
            playerCenter.x - camera.x, playerCenter.y - camera.y,
            camera.x * 2, camera.y * 2, SRCCOPY);
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

    DeleteDC(memDC);
    ReleaseDC(hWnd, hdc);
}