#pragma once

#include <windows.h>

// 全局GUI变量声明
extern HWND g_hWnd;
extern HWND g_hPathEdit;
extern HWND g_hBrowseBtn;
extern HWND g_hStartBtn;
extern HWND g_hStopBtn;
extern HWND g_hStatusLabel;
extern HWND g_hUrlLabel;
extern HWND g_hCopyBtn;

// 颜色定义
extern const COLORREF WINDOW_BG;
extern const COLORREF CARD_BG;
extern const COLORREF ACCENT_COLOR;
extern const COLORREF TEXT_PRIMARY;
extern const COLORREF TEXT_SECONDARY;
extern const COLORREF BORDER_COLOR;
extern const COLORREF SUCCESS_COLOR;
extern const COLORREF ERROR_COLOR;
extern const COLORREF HOVER_COLOR;

// 字体大小
extern const int FONT_SIZE_TITLE;
extern const int FONT_SIZE_NORMAL;
extern const int FONT_SIZE_SMALL;

// GUI相关函数声明
HFONT CreateCustomFont(int size, int weight = FW_NORMAL, bool italic = false);
void InitializeControls(HWND hWnd);
void BrowseForFolder();
void ApplyModernStyle(HWND hWnd);
void CopyUrlToClipboard();
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);