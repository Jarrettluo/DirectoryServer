# DirectoryServer

A simple directory sharing server for Windows with a modern GUI interface. Share your local directories over the network and access them from any device with a web browser.

## Features

- **Modern Windows GUI**: Clean, Windows 11-style interface with dark mode support
- **Directory Browsing**: Navigate through shared directories with a web-based file explorer
- **File Download**: Download files directly from the web interface
- **Real-time Status**: Monitor server status and access URLs
- **Easy Setup**: Simple one-click directory selection and server start/stop

## Screenshots

*(Add screenshots here when available)*

## Requirements

- Windows 10 or later
- MinGW-w64 compiler (for building from source)
- CMake (optional, for advanced builds)

## Building from Source

### Using the provided build script (Recommended)

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/DirectoryServer.git
   cd DirectoryServer
   ```

2. Run the build script:
   ```bash
   build.bat
   ```

This will compile the project and automatically start the application.

### Using CMake

1. Install CMake and MinGW-w64
2. Navigate to the src directory:
   ```bash
   cd src
   ```
3. Generate build files:
   ```bash
   cmake .
   ```
4. Build:
   ```bash
   make
   ```

## Usage

1. **Select Directory**: Click the "Browse..." button to choose a directory to share
2. **Start Server**: Click "▶ Start Server" to begin sharing
3. **Access Files**: Open the displayed URL in any web browser on your network
4. **Stop Server**: Click "⏹ Stop Server" when done

## How It Works

- The server runs on port 8080 by default
- Generates HTML directory listings with modern styling
- Supports file downloads and navigation
- Uses Windows native APIs for optimal performance

## Project Structure

```
DirectoryServer/
├── src/
│   ├── main.cpp          # Application entry point
│   ├── gui/
│   │   ├── gui.cpp       # Windows GUI implementation
│   │   └── gui.h
│   ├── server/
│   │   ├── server.cpp    # HTTP server logic
│   │   └── server.h
│   ├── utils/
│   │   ├── utils.cpp     # Utility functions (MIME types, HTML generation)
│   │   └── utils.h
│   └── CMakeLists.txt    # CMake build configuration
├── build.bat             # Windows build script
└── README.md
```

## Dependencies

- **WinSock2**: Network communication
- **Windows Common Controls**: GUI components
- **DWM API**: Window styling and effects
- **Shell API**: Directory browsing

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Author

Jarrett - luojiarui2@163.com

## Version

v1.0.0