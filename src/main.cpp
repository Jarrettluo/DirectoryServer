#define UNICODE
#define _UNICODE

#include <winsock2.h>
#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <thread>
#include <ws2tcpip.h>
#include <filesystem>
#include <codecvt>
#include <locale>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <dwmapi.h>
#include <unordered_map>

// DWM API 定义（MinGW 兼容）
#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif

#ifndef DWM_WINDOW_CORNER_PREFERENCE
typedef enum {
    DWMWCP_DEFAULT = 0,
    DWMWCP_DONOTROUND = 1,
    DWMWCP_ROUND = 2,
    DWMWCP_ROUNDSMALL = 3
} DWM_WINDOW_CORNER_PREFERENCE;
#endif

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

namespace fs = std::filesystem;

// 包含模块头文件
#include "gui/gui.h"
#include "server/server.h"
#include "utils/utils.h"

// 全局变量定义
HFONT g_hFontNormal;
HFONT g_hFontTitle;
HFONT g_hFontSmall;
HBRUSH hBrushBackground;
HBRUSH hBrushCard;
std::wstring g_selectedPath;

// 自定义控件 ID
#define ID_BROWSE_BTN 1001
#define ID_START_BTN 1002
#define ID_STOP_BTN 1003
#define ID_COPY_BTN 1004

const int PORT = 8080;

// 主窗口入口点
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // 初始化公共控件
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_WIN95_CLASSES | ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&icex);

    // 创建画笔
    hBrushBackground = CreateSolidBrush(WINDOW_BG);
    hBrushCard = CreateSolidBrush(CARD_BG);

    // 注册窗口类
    WNDCLASSEX wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(WINDOW_BG);
    wcex.lpszClassName = L"DirectoryServerApp";
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    
    // 启用深色标题栏（如果系统支持）
    BOOL darkMode = TRUE;
    DwmSetWindowAttribute(GetDesktopWindow(), 20, &darkMode, sizeof(darkMode));

    RegisterClassEx(&wcex);

    // 创建窗口
    g_hWnd = CreateWindowEx(
        0,
        L"DirectoryServerApp",
        L"目录共享服务器",
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
        CW_USEDEFAULT, CW_USEDEFAULT,
        700, 550,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (!g_hWnd)
    {
        return FALSE;
    }

    // 初始化字体
    g_hFontNormal = CreateCustomFont(FONT_SIZE_NORMAL);
    g_hFontTitle = CreateCustomFont(FONT_SIZE_TITLE, FW_SEMIBOLD);
    g_hFontSmall = CreateCustomFont(FONT_SIZE_SMALL);

    // 应用现代样式
    ApplyModernStyle(g_hWnd);

    // 初始化界面控件
    InitializeControls(g_hWnd);

    ShowWindow(g_hWnd, nCmdShow);
    UpdateWindow(g_hWnd);

    // 消息循环
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 清理字体资源
    DeleteObject(g_hFontNormal);
    DeleteObject(g_hFontTitle);
    DeleteObject(g_hFontSmall);

    return (int)msg.wParam;
}

// 窗口过程函数
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        {
            // 标题标签
            HWND hTitleLabel = CreateWindow(L"STATIC", L"目录共享服务器",
                WS_CHILD | WS_VISIBLE | SS_LEFT,
                50, 30, 300, 35,
                hWnd, NULL, NULL, NULL);
            SendMessage(hTitleLabel, WM_SETFONT, (WPARAM)g_hFontTitle, TRUE);
            
            // 路径标签
            HWND hPathLabel = CreateWindow(L"STATIC", L"共享目录:",
                WS_CHILD | WS_VISIBLE | SS_LEFT,
                50, 85, 80, 25,
                hWnd, NULL, NULL, NULL);
            SendMessage(hPathLabel, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
            
            // 路径编辑框（现代样式）
            g_hPathEdit = CreateWindowEx(
                WS_EX_CLIENTEDGE,
                L"EDIT", L"",
                WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                140, 83, 400, 30,
                hWnd, NULL, NULL, NULL);
            SendMessage(g_hPathEdit, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
            
            // 浏览按钮（现代样式）
            g_hBrowseBtn = CreateWindowEx(
                0,
                L"BUTTON", L"浏览...",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                550, 83, 100, 30,
                hWnd, (HMENU)ID_BROWSE_BTN, NULL, NULL);
            SendMessage(g_hBrowseBtn, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);

            // 服务器控制卡片
            HWND hCard2 = CreateWindow(L"STATIC", L"",
                WS_CHILD | WS_VISIBLE | SS_WHITERECT,
                30, 155, 640, 100,
                hWnd, NULL, NULL, NULL);
            
            HWND hServerLabel = CreateWindow(L"STATIC", L"服务器控制:",
                WS_CHILD | WS_VISIBLE | SS_LEFT,
                50, 170, 100, 25,
                hWnd, NULL, NULL, NULL);
            SendMessage(hServerLabel, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
            
            // 开始服务器按钮（现代样式）
            g_hStartBtn = CreateWindowEx(
                0,
                L"BUTTON", L"▶ 启动服务器",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_TEXT,
                140, 170, 150, 40,
                hWnd, (HMENU)ID_START_BTN, NULL, NULL);
            SendMessage(g_hStartBtn, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
            
            // 停止服务器按钮（现代样式）
            g_hStopBtn = CreateWindowEx(
                0,
                L"BUTTON", L"⏹ 停止服务器",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_TEXT | WS_DISABLED,
                310, 170, 150, 40,
                hWnd, (HMENU)ID_STOP_BTN, NULL, NULL);
            SendMessage(g_hStopBtn, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);

            // 状态标签
            g_hStatusLabel = CreateWindow(L"STATIC", L"服务器未启动",
                WS_CHILD | WS_VISIBLE | SS_LEFT,
                50, 225, 200, 25,
                hWnd, NULL, NULL, NULL);
            SendMessage(g_hStatusLabel, WM_SETFONT, (WPARAM)g_hFontSmall, TRUE);

            // URL显示卡片
            HWND hCard3 = CreateWindow(L"STATIC", L"",
                WS_CHILD | WS_VISIBLE | SS_WHITERECT,
                30, 275, 640, 120,
                hWnd, NULL, NULL, NULL);
            
            HWND hUrlTitleLabel = CreateWindow(L"STATIC", L"访问地址:",
                WS_CHILD | WS_VISIBLE | SS_LEFT,
                50, 290, 100, 25,
                hWnd, NULL, NULL, NULL);
            SendMessage(hUrlTitleLabel, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
            
            // URL标签
            g_hUrlLabel = CreateWindow(L"STATIC", L"http://localhost:8080",
                WS_CHILD | WS_VISIBLE | SS_LEFT,
                50, 320, 500, 25,
                hWnd, NULL, NULL, NULL);
            SendMessage(g_hUrlLabel, WM_SETFONT, (WPARAM)g_hFontSmall, TRUE);
            
            // 复制按钮（现代样式）
            g_hCopyBtn = CreateWindowEx(
                0,
                L"BUTTON", L"📋 复制链接",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                550, 318, 100, 30,
                hWnd, (HMENU)ID_COPY_BTN, NULL, NULL);
            SendMessage(g_hCopyBtn, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);

            // 信息标签
            HWND hInfoLabel = CreateWindow(L"STATIC", L"将选定的目录共享到网络，方便其他设备访问文件。",
                WS_CHILD | WS_VISIBLE | SS_LEFT,
                50, 355, 500, 40,
                hWnd, NULL, NULL, NULL);
            SendMessage(hInfoLabel, WM_SETFONT, (WPARAM)g_hFontSmall, TRUE);
        }
        break;
        
    case WM_CTLCOLORSTATIC:
        {
            HDC hdc = (HDC)wParam;
            SetTextColor(hdc, TEXT_PRIMARY);
            SetBkColor(hdc, WINDOW_BG);
            return (LRESULT)hBrushBackground;
        }
        break;
        
    case WM_CTLCOLOREDIT:
        {
            HDC hdc = (HDC)wParam;
            SetTextColor(hdc, TEXT_PRIMARY);
            SetBkColor(hdc, CARD_BG);
            return (LRESULT)hBrushCard;
        }
        break;
        
    case WM_CTLCOLORBTN:
        return (LRESULT)hBrushBackground;
        
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case ID_BROWSE_BTN:
                BrowseForFolder();
                break;
            case ID_START_BTN:
                StartServer();
                break;
            case ID_STOP_BTN:
                StopServer();
                break;
            case ID_COPY_BTN:
                CopyUrlToClipboard();
                break;
            }
        }
        break;
        
    case WM_USER + 100:  // 自定义消息：更新URL显示
        {
            std::wstring* url = (std::wstring*)lParam;
            SetWindowText(g_hUrlLabel, url->c_str());
            delete url;
            return 0;
        }
        break;
        
    case WM_DESTROY:
        StopServer();
        DeleteObject(hBrushBackground);
        DeleteObject(hBrushCard);
        PostQuitMessage(0);
        break;
        
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}