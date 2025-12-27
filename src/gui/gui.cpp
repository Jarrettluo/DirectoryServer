#include "gui.h"
#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>
#include <dwmapi.h>
#include <string>
#include <vector>
#include <filesystem>

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

// 外部变量声明
extern std::wstring g_selectedPath;
extern HFONT g_hFontNormal;
extern HFONT g_hFontTitle;
extern HFONT g_hFontSmall;

// 自定义控件 ID
#define ID_BROWSE_BTN 1001
#define ID_START_BTN 1002
#define ID_STOP_BTN 1003
#define ID_COPY_BTN 1004

// 颜色定义 - Windows 11 现代风格
const COLORREF WINDOW_BG = RGB(243, 243, 243);      // 浅灰背景
const COLORREF CARD_BG = RGB(255, 255, 255);        // 白色卡片
const COLORREF ACCENT_COLOR = RGB(0, 120, 212);     // Windows 11 主题色
const COLORREF TEXT_PRIMARY = RGB(30, 30, 30);      // 主要文字
const COLORREF TEXT_SECONDARY = RGB(96, 96, 96);    // 次要文字
const COLORREF BORDER_COLOR = RGB(229, 229, 229);   // 边框颜色
const COLORREF SUCCESS_COLOR = RGB(16, 124, 16);    // 成功状态
const COLORREF ERROR_COLOR = RGB(196, 43, 28);      // 错误状态
const COLORREF HOVER_COLOR = RGB(230, 243, 255);    // 悬停颜色

// 字体大小
const int FONT_SIZE_TITLE = 16;
const int FONT_SIZE_NORMAL = 12;
const int FONT_SIZE_SMALL = 10;

// 全局GUI变量
HWND g_hWnd = NULL;
HWND g_hPathEdit = NULL;
HWND g_hBrowseBtn = NULL;
HWND g_hStartBtn = NULL;
HWND g_hStopBtn = NULL;
HWND g_hStatusLabel = NULL;
HWND g_hUrlLabel = NULL;
HWND g_hCopyBtn = NULL;

// 创建自定义字体
HFONT CreateCustomFont(int size, int weight, bool italic)
{
    return CreateFont(
        -size, 0, 0, 0, weight,
        italic, FALSE, FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE,
        L"Segoe UI"
    );
}

// 应用现代样式
void ApplyModernStyle(HWND hWnd)
{
    // 启用非客户区的深色模式
    BOOL useDarkMode = TRUE;
    DwmSetWindowAttribute(hWnd, 20, &useDarkMode, sizeof(useDarkMode));
    
    // 设置窗口圆角（需要 Windows 11）
    DWM_WINDOW_CORNER_PREFERENCE cornerPreference = DWMWCP_ROUND;
    DwmSetWindowAttribute(hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &cornerPreference, sizeof(cornerPreference));
}

// 初始化界面控件
void InitializeControls(HWND hWnd)
{
    // 窗口图标
    HICON hIcon = LoadIcon(GetModuleHandle(NULL), IDI_APPLICATION);
    SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

    // 标题区域
    HWND hTitle = CreateWindow(L"STATIC", L"📁 目录共享服务器",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        30, 20, 400, 30,
        hWnd, NULL, NULL, NULL);
    SendMessage(hTitle, WM_SETFONT, (WPARAM)g_hFontTitle, TRUE);
    
    // 分隔线
    HWND hSeparator1 = CreateWindow(L"STATIC", L"",
        WS_CHILD | WS_VISIBLE | SS_ETCHEDHORZ,
        30, 55, 640, 1,
        hWnd, NULL, NULL, NULL);

    // 目录选择卡片
    HWND hCard1 = CreateWindow(L"STATIC", L"",
        WS_CHILD | WS_VISIBLE | SS_WHITERECT,
        30, 70, 640, 70,
        hWnd, NULL, NULL, NULL);
    
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
    
    // 服务器信息卡片
    HWND hCard3 = CreateWindow(L"STATIC", L"",
        WS_CHILD | WS_VISIBLE | SS_WHITERECT,
        30, 270, 640, 120,
        hWnd, NULL, NULL, NULL);
    
    HWND hStatusTitle = CreateWindow(L"STATIC", L"服务器状态:",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        50, 285, 100, 25,
        hWnd, NULL, NULL, NULL);
    SendMessage(hStatusTitle, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
    
    // 状态标签（现代样式）
    g_hStatusLabel = CreateWindow(L"EDIT", 
        L"🟢 服务器未运行",
        WS_CHILD | WS_VISIBLE | ES_READONLY | ES_AUTOHSCROLL,
        140, 285, 430, 30,
        hWnd, NULL, NULL, NULL);
    SendMessage(g_hStatusLabel, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
    
    HWND hUrlTitle = CreateWindow(L"STATIC", L"访问地址:",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        50, 325, 80, 25,
        hWnd, NULL, NULL, NULL);
    SendMessage(hUrlTitle, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
    
    // URL标签（现代样式）
    g_hUrlLabel = CreateWindow(L"EDIT", 
        L"暂无地址",
        WS_CHILD | WS_VISIBLE | ES_READONLY | ES_AUTOHSCROLL,
        140, 325, 350, 30,
        hWnd, NULL, NULL, NULL);
    SendMessage(g_hUrlLabel, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
    
    // 复制按钮
    g_hCopyBtn = CreateWindowEx(
        0,
        L"BUTTON", L"📋 复制",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        500, 325, 80, 30,
        hWnd, (HMENU)ID_COPY_BTN, NULL, NULL);
    SendMessage(g_hCopyBtn, WM_SETFONT, (WPARAM)g_hFontSmall, TRUE);
    EnableWindow(g_hCopyBtn, FALSE);
    
    // 说明卡片
    HWND hCard4 = CreateWindow(L"STATIC", L"",
        WS_CHILD | WS_VISIBLE | SS_WHITERECT,
        30, 405, 640, 100,
        hWnd, NULL, NULL, NULL);
    
    HWND hHelpTitle = CreateWindow(L"STATIC", L"💡 使用说明",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        50, 420, 100, 25,
        hWnd, NULL, NULL, NULL);
    SendMessage(hHelpTitle, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
    
    HWND hInstructions = CreateWindow(L"EDIT", 
        L"1. 选择要共享的目录\n"
        L"2. 点击启动服务器按钮\n"
        L"3. 在其他设备浏览器中输入显示的URL访问\n"
        L"4. 支持文件下载和目录浏览",
        WS_CHILD | WS_VISIBLE | ES_READONLY | ES_MULTILINE | ES_AUTOVSCROLL,
        140, 420, 510, 70,
        hWnd, NULL, NULL, NULL);
    SendMessage(hInstructions, WM_SETFONT, (WPARAM)g_hFontSmall, TRUE);
    
    // 底部状态栏
    HWND hStatusBar = CreateWindow(L"STATIC", L"",
        WS_CHILD | WS_VISIBLE | SS_ETCHEDHORZ,
        0, 515, 700, 1,
        hWnd, NULL, NULL, NULL);
    
    HWND hFooter = CreateWindow(L"STATIC", L"© 2023 目录共享服务器 - 端口: 8080",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        0, 520, 700, 20,
        hWnd, NULL, NULL, NULL);
    SendMessage(hFooter, WM_SETFONT, (WPARAM)g_hFontSmall, TRUE);
}

// 浏览文件夹
void BrowseForFolder()
{
    BROWSEINFO bi = {};
    bi.hwndOwner = g_hWnd;
    bi.lpszTitle = L"选择要共享的目录";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
    if (pidl)
    {
        wchar_t path[MAX_PATH];
        if (SHGetPathFromIDList(pidl, path))
        {
            g_selectedPath = path;
            SetWindowText(g_hPathEdit, path);
            
            // 启用开始按钮
            EnableWindow(g_hStartBtn, TRUE);
            
            // 更新状态
            SetWindowText(g_hStatusLabel, L"🟡 目录已选择，点击启动服务器");
        }
        CoTaskMemFree(pidl);
    }
}

// 复制URL到剪贴板
void CopyUrlToClipboard()
{
    wchar_t url[512];
    GetWindowText(g_hUrlLabel, url, 512);
    
    if (OpenClipboard(g_hWnd))
    {
        EmptyClipboard();
        size_t len = wcslen(url) + 1;
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len * sizeof(wchar_t));
        if (hMem)
        {
            wchar_t* pMem = (wchar_t*)GlobalLock(hMem);
            wcscpy_s(pMem, len, url);
            GlobalUnlock(hMem);
            SetClipboardData(CF_UNICODETEXT, hMem);
        }
        CloseClipboard();
        
        // 显示提示
        SetWindowText(g_hCopyBtn, L"✅ 已复制");
        Sleep(1000);
        SetWindowText(g_hCopyBtn, L"📋 复制");
    }
}