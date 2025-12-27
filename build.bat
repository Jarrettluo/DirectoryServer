@echo off
echo Killing any existing DirectoryServer.exe...
taskkill /f /im DirectoryServer.exe >nul 2>&1
echo Deleting old executable...
del DirectoryServer.exe >nul 2>&1
echo Compiling DirectoryServer...
cd src
g++ -std=c++17 -o ../DirectoryServer.exe main.cpp gui/gui.cpp server/server.cpp utils/utils.cpp -lws2_32 -lcomctl32 -lole32 -lshell32 -ldwmapi -mwindows -DWIN32 -D_WIN32_WINNT=0x0600 -DUNICODE -D_UNICODE
cd ..
if %errorlevel% equ 0 (
    echo Compilation successful!
    echo Running program...
    start DirectoryServer.exe
) else (
    echo Compilation failed!
)
pause