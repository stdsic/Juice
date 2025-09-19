#include <windows.h>
#include <winuser.h>
#define CLASS_NAME L"Juice"

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, int nCmdShow){
    WNDCLASSEX wcex = {
        sizeof(wcex),
        CS_HREDRAW | CS_VREDRAW,
        WndProc,
        0,0,
        hInst,
        NULL, LoadCursor(NULL, IDC_ARROW),
        NULL,
        NULL,
        CLASS_NAME,
        NULL
    };

    RegisterClassEx(&wcex);

    HWND hWnd = CreateWindowEx(
            WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
            CLASS_NAME,
            CLASS_NAME,
            WS_OVERLAPPEDWINDOW,
            0,0,0,0,
            NULL,
            (HMENU)NULL,
            hInst,
            NULL
            );

    ShowWindow(hWnd, SW_HIDE);

    MSG msg;
    while(GetMessage(&msg, NULL, 0,0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

#include "resource.h"
#define TRAY_NOTIFY (WM_APP+1)

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam){
    NOTIFYICONDATA nid;
    HMENU hMenu, hPopupMenu;
    POINT Mouse;
    static BOOL bActivate = FALSE;

    switch(iMessage){
        case WM_CREATE:
            ZeroMemory(&nid, sizeof(nid));
            nid.cbSize = sizeof(NOTIFYICONDATA);
            nid.hWnd = hWnd;
            nid.uID = 0;
            nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE | NIF_INFO;
            nid.uCallbackMessage = TRAY_NOTIFY;
            nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
            wcscpy(nid.szTip, L"Juice mode is on");
            wcscpy(nid.szInfo, L"Juice mode is activated.\r\nThe screen will not go to sleep.\r\nYou can find the program's icon in the system tray on the right side of the taskbar.\r\n");
            Shell_NotifyIcon(NIM_ADD, &nid);
            bActivate = TRUE;
            SetTimer(hWnd, 1, 10, NULL);
            return 0;

        case TRAY_NOTIFY:
            // wParam == uID, lParam == Action
            switch(lParam){
                case WM_RBUTTONDOWN:
                    hMenu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU1));
                    hPopupMenu = GetSubMenu(hMenu, 0);
                    GetCursorPos(&Mouse);
                    if(bActivate){
                        CheckMenuItem(hMenu, IDM_ACTIVATE, MF_BYCOMMAND | MF_CHECKED);
                        CheckMenuItem(hMenu, IDM_DEACTIVATE, MF_BYCOMMAND | MF_UNCHECKED);
                    }else{
                        CheckMenuItem(hMenu, IDM_ACTIVATE, MF_BYCOMMAND | MF_UNCHECKED);
                        CheckMenuItem(hMenu, IDM_DEACTIVATE, MF_BYCOMMAND | MF_CHECKED);
                    }
                    SetForegroundWindow(hWnd);
                    TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, Mouse.x, Mouse.y, 0, hWnd, NULL);
                    SetForegroundWindow(hWnd);
                    DestroyMenu(hPopupMenu);
                    DestroyMenu(hMenu);
                    break;
            }
            return 0;

        case WM_COMMAND:
            switch(wParam){
                case IDM_ACTIVATE:
                    if(bActivate){ break; }
                    bActivate = TRUE;
                    SetTimer(hWnd, 1, 10, NULL);
                    ZeroMemory(&nid, sizeof(nid));
                    nid.cbSize = sizeof(NOTIFYICONDATA);
                    nid.hWnd = hWnd;
                    nid.uID = 0;
                    nid.uFlags = NIF_TIP;
                    wcscpy(nid.szTip, L"Juice mode is on");
                    Shell_NotifyIcon(NIM_MODIFY, &nid);
                    break;

                case IDM_DEACTIVATE:
                    if(!bActivate){ break; }
                    bActivate = FALSE;
                    SetTimer(hWnd, 1, 10, NULL);
                    ZeroMemory(&nid, sizeof(nid));
                    nid.cbSize = sizeof(NOTIFYICONDATA);
                    nid.hWnd = hWnd;
                    nid.uID = 0;
                    nid.uFlags = NIF_TIP;
                    wcscpy(nid.szTip, L"Juice mode is off");
                    Shell_NotifyIcon(NIM_MODIFY, &nid);
                    break;

                case IDM_EXIT:
                    DestroyWindow(hWnd);
                    break;
            }
            return 0;

        case WM_TIMER:
            switch(wParam){
                case 1:
                    KillTimer(hWnd, 1);
                    if(bActivate){
                        SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);
                    }else{
                        SetThreadExecutionState(ES_CONTINUOUS);
                    }
                    break;
            }
            return 0;

        case WM_DESTROY:
            KillTimer(hWnd, 1);
            bActivate = FALSE;
            SetThreadExecutionState(ES_CONTINUOUS);
            nid.cbSize = sizeof(NOTIFYICONDATA);
            nid.hWnd = hWnd;
            nid.uID = 0;
            Shell_NotifyIcon(NIM_DELETE, &nid);
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hWnd, iMessage, wParam, lParam);
}
