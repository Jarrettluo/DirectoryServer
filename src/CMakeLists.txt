cmake_minimum_required(VERSION 3.10)
project(DirectoryServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows设置
if(WIN32)
    add_definitions(-DWIN32 -D_WIN32_WINNT=0x0600)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
endif()

# 源文件
add_executable(DirectoryServer main.cpp)

# 链接库
target_link_libraries(DirectoryServer
    ws2_32
    comctl32
    ole32
    shell32
)