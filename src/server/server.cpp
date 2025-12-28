#include "server.h"
#include <winsock2.h>
#include <windows.h>
#include <string>
#include <vector>
#include <thread>
#include <filesystem>
#include <codecvt>
#include <locale>
#include <fstream>
#include <sstream>
#include "../utils/utils.h"

// 外部变量声明
extern std::wstring g_selectedPath;
extern HWND g_hWnd;
extern HWND g_hStatusLabel;
extern HWND g_hStartBtn;
extern HWND g_hStopBtn;
extern HWND g_hBrowseBtn;
extern HWND g_hCopyBtn;
extern HWND g_hUrlLabel;

// 服务器配置
const int PORT = 8080;

// 全局服务器变量
SOCKET g_serverSocket = INVALID_SOCKET;
std::thread g_serverThread;
bool g_serverRunning = false;

namespace fs = std::filesystem;

// 处理客户端请求
void HandleClient(SOCKET clientSocket)
{
    char buffer[4096];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    
    if (bytesReceived > 0)
    {
        buffer[bytesReceived] = '\0';
        
        // 解析请求行
        std::string request(buffer);
        std::istringstream iss(request);
        std::string method, path, protocol;
        iss >> method >> path >> protocol;
        
        // URL解码路径
        std::string decodedPath = UrlDecode(path);
        
        // 构建本地文件路径
        std::string relativePath = decodedPath;
        if (relativePath == "/")
            relativePath = "";
        else if (relativePath.length() > 0 && relativePath[0] == '/')
            relativePath = relativePath.substr(1);
        
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::wstring wRelativePath;
        try
        {
            wRelativePath = converter.from_bytes(relativePath);
        }
        catch (...)
        {
            wRelativePath = std::wstring(relativePath.begin(), relativePath.end());
        }
        
        fs::path fullPath = fs::path(g_selectedPath) / wRelativePath;
        
        try
        {
            fullPath = fs::canonical(fullPath);
            fs::path basePath = fs::canonical(g_selectedPath);
            
            if (fullPath.wstring().length() < basePath.wstring().length() ||
                fullPath.wstring().compare(0, basePath.wstring().length(), basePath.wstring()) != 0)
            {
                std::string response = 
                    "HTTP/1.1 403 Forbidden\r\n"
                    "Content-Type: text/plain; charset=utf-8\r\n"
                    "Content-Length: 13\r\n"
                    "Connection: close\r\n"
                    "\r\nAccess Forbidden";
                
                send(clientSocket, response.c_str(), response.length(), 0);
                closesocket(clientSocket);
                return;
            }
        }
        catch (const fs::filesystem_error&)
        {
            // 继续处理
        }
        
        if (fs::is_directory(fullPath))
        {
            std::string html = GenerateDirectoryListing(fullPath.wstring(), decodedPath);
            
            std::string response = 
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html; charset=utf-8\r\n"
                "Content-Length: " + std::to_string(html.length()) + "\r\n"
                "Connection: close\r\n"
                "\r\n" + html;
            
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        else if (fs::exists(fullPath))
        {
            std::ifstream file(fullPath, std::ios::binary);
            if (file)
            {
                file.seekg(0, std::ios::end);
                size_t fileSize = file.tellg();
                file.seekg(0, std::ios::beg);
                
                std::string extension = fs::path(fullPath).extension().string();
                std::string mimeType = GetMimeType(extension);
                
                std::string header = 
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: " + mimeType + "\r\n"
                    "Content-Length: " + std::to_string(fileSize) + "\r\n"
                    "Connection: close\r\n"
                    "\r\n";
                
                send(clientSocket, header.c_str(), header.length(), 0);
                
                char fileBuffer[8192];
                while (!file.eof())
                {
                    file.read(fileBuffer, sizeof(fileBuffer));
                    send(clientSocket, fileBuffer, file.gcount(), 0);
                }
            }
            else
            {
                std::string response = 
                    "HTTP/1.1 403 Forbidden\r\n"
                    "Content-Type: text/plain; charset=utf-8\r\n"
                    "Content-Length: 13\r\n"
                    "Connection: close\r\n"
                    "\r\nAccess Forbidden";
                
                send(clientSocket, response.c_str(), response.length(), 0);
            }
        }
        else
        {
            std::string response = 
                "HTTP/1.1 404 Not Found\r\n"
                "Content-Type: text/plain; charset=utf-8\r\n"
                "Content-Length: 9\r\n"
                "Connection: close\r\n"
                "\r\nNot Found";
            
            send(clientSocket, response.c_str(), response.length(), 0);
        }
    }
    
    closesocket(clientSocket);
}

// 服务器线程函数
void ServerThread()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        MessageBox(g_hWnd, L"Winsock初始化失败", L"错误", MB_ICONERROR);
        return;
    }
    
    g_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (g_serverSocket == INVALID_SOCKET)
    {
        MessageBox(g_hWnd, L"创建套接字失败", L"错误", MB_ICONERROR);
        WSACleanup();
        return;
    }
    
    int opt = 1;
    setsockopt(g_serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
    
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);
    
    if (bind(g_serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        MessageBox(g_hWnd, L"绑定端口失败", L"错误", MB_ICONERROR);
        closesocket(g_serverSocket);
        WSACleanup();
        return;
    }
    
    if (listen(g_serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        MessageBox(g_hWnd, L"监听失败", L"错误", MB_ICONERROR);
        closesocket(g_serverSocket);
        WSACleanup();
        return;
    }
    
    // 获取本机IP地址
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    hostent* host = gethostbyname(hostname);
    
    std::vector<std::wstring> ipAddresses;
    ipAddresses.push_back(L"127.0.0.1");
    
    if (host)
    {
        for (int i = 0; host->h_addr_list[i] != nullptr; i++)
        {
            std::string ipStr = inet_ntoa(*(in_addr*)host->h_addr_list[i]);
            if (ipStr != "127.0.0.1")
            {
                ipAddresses.push_back(std::wstring(ipStr.begin(), ipStr.end()));
            }
        }
    }
    
    // 更新界面显示URL
    std::wstring urlText = L"";
    for (size_t i = 0; i < ipAddresses.size(); i++)
    {
        if (i > 0) urlText += L" \r\n或 ";
        urlText += L"http://" + ipAddresses[i] + L":" + std::to_wstring(PORT);
    }
    
    // 在主线程中更新UI
    SendMessage(g_hWnd, WM_USER + 100, 0, (LPARAM)new std::wstring(urlText));
    
    // 主循环接受连接
    while (g_serverRunning)
    {
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(g_serverSocket, &readSet);
        
        timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        
        int selectResult = select(0, &readSet, NULL, NULL, &timeout);
        
        if (selectResult > 0 && FD_ISSET(g_serverSocket, &readSet))
        {
            sockaddr_in clientAddr;
            int clientAddrSize = sizeof(clientAddr);
            
            SOCKET clientSocket = accept(g_serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);
            if (clientSocket != INVALID_SOCKET)
            {
                std::thread(HandleClient, clientSocket).detach();
            }
        }
    }
    
    closesocket(g_serverSocket);
    WSACleanup();
}

// 启动服务器
void StartServer()
{
    // 检查服务器是否已经在运行
    if (g_serverRunning)
    {
        MessageBox(g_hWnd, L"服务器已经在运行中", L"提示", MB_ICONINFORMATION);
        return;
    }
    
    if (g_selectedPath.empty())
    {
        MessageBox(g_hWnd, L"请先选择目录", L"提示", MB_ICONINFORMATION);
        return;
    }
    
    if (!fs::exists(g_selectedPath))
    {
        MessageBox(g_hWnd, L"选择的目录不存在", L"错误", MB_ICONERROR);
        return;
    }
    
    g_serverRunning = true;
    g_serverThread = std::thread(ServerThread);
    
    // 更新界面状态
    SetWindowText(g_hStatusLabel, L"[运行中] 服务器运行中");
    EnableWindow(g_hStartBtn, FALSE);
    EnableWindow(g_hStopBtn, TRUE);
    EnableWindow(g_hBrowseBtn, FALSE);
    EnableWindow(g_hCopyBtn, TRUE);
}

// 停止服务器
void StopServer()
{
    g_serverRunning = false;
    
    if (g_serverThread.joinable())
    {
        g_serverThread.join();
    }
    
    // 更新界面状态
    SetWindowText(g_hStatusLabel, L"[已停止] 服务器已停止");
    EnableWindow(g_hStartBtn, TRUE);
    EnableWindow(g_hStopBtn, FALSE);
    EnableWindow(g_hBrowseBtn, TRUE);
    EnableWindow(g_hCopyBtn, FALSE);
    SetWindowText(g_hUrlLabel, L"暂无地址");
}