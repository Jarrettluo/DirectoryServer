#pragma once

#include <winsock2.h>
#include <string>
#include <thread>

// 全局服务器变量声明
extern SOCKET g_serverSocket;
extern std::thread g_serverThread;
extern bool g_serverRunning;

// 服务器相关函数声明
void StartServer();
void StopServer();
void ServerThread();
void HandleClient(SOCKET clientSocket);