# DirectoryServer

一个简单的Windows目录共享服务器，具有现代化的GUI界面。通过网络共享您的本地目录，并允许任何设备通过网页浏览器访问。

## 功能特性

- **现代Windows GUI**：干净的Windows 11风格界面，支持深色模式
- **目录浏览**：通过基于网页的文件浏览器导航共享目录
- **文件下载**：直接从网页界面下载文件
- **实时状态**：监控服务器状态和访问URL
- **简单设置**：一键选择目录和启动/停止服务器

## 截图

*(在此处添加截图)*

## 系统要求

- Windows 10 或更高版本
- MinGW-w64 编译器（用于从源码构建）
- CMake（可选，用于高级构建）

## 从源码构建

### 使用提供的构建脚本（推荐）

1. 克隆仓库：
   ```bash
   git clone https://github.com/yourusername/DirectoryServer.git
   cd DirectoryServer
   ```

2. 运行构建脚本：
   ```bash
   build.bat
   ```

这将编译项目并自动启动应用程序。

### 使用CMake

1. 安装CMake和MinGW-w64
2. 导航到src目录：
   ```bash
   cd src
   ```
3. 生成构建文件：
   ```bash
   cmake .
   ```
4. 构建：
   ```bash
   make
   ```

## 使用方法

1. **选择目录**：点击"浏览..."按钮选择要共享的目录
2. **启动服务器**：点击"▶ 启动服务器"开始共享
3. **访问文件**：在网络上的任何浏览器中打开显示的URL
4. **停止服务器**：完成后点击"⏹ 停止服务器"

## 工作原理

- 服务器默认运行在8080端口
- 生成具有现代样式的HTML目录列表
- 支持文件下载和导航
- 使用Windows原生API以获得最佳性能

## 项目结构

```
DirectoryServer/
├── src/
│   ├── main.cpp          # 应用程序入口点
│   ├── gui/
│   │   ├── gui.cpp       # Windows GUI实现
│   │   └── gui.h
│   ├── server/
│   │   ├── server.cpp    # HTTP服务器逻辑
│   │   └── server.h
│   ├── utils/
│   │   ├── utils.cpp     # 工具函数（MIME类型，HTML生成）
│   │   └── utils.h
│   └── CMakeLists.txt    # CMake构建配置
├── build.bat             # Windows构建脚本
└── README.md
```

## 依赖项

- **WinSock2**：网络通信
- **Windows通用控件**：GUI组件
- **DWM API**：窗口样式和效果
- **Shell API**：目录浏览

## 贡献

1. Fork仓库
2. 创建功能分支
3. 进行更改
4. 彻底测试
5. 提交拉取请求

## 许可证

本项目采用MIT许可证 - 详情请见LICENSE文件。

## 作者

Jarrett - luojiarui2@163.com

## 版本

v1.0.0