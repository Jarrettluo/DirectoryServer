#include "utils.h"
#include <string>
#include <unordered_map>
#include <sstream>
#include <filesystem>
#include <codecvt>
#include <locale>
#include <chrono>
#include <iomanip>
#include <cstdio>
#include <ctime>

namespace fs = std::filesystem;

// 获取MIME类型
std::string GetMimeType(const std::string& extension)
{
    static std::unordered_map<std::string, std::string> mimeTypes = {
        {".html", "text/html; charset=utf-8"},
        {".htm", "text/html; charset=utf-8"},
        {".txt", "text/plain; charset=utf-8"},
        {".css", "text/css; charset=utf-8"},
        {".js", "application/javascript; charset=utf-8"},
        {".json", "application/json; charset=utf-8"},
        {".xml", "application/xml; charset=utf-8"},
        {".jpg", "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".png", "image/png"},
        {".gif", "image/gif"},
        {".bmp", "image/bmp"},
        {".ico", "image/x-icon"},
        {".pdf", "application/pdf"},
        {".zip", "application/zip"},
        {".rar", "application/x-rar-compressed"},
        {".mp3", "audio/mpeg"},
        {".mp4", "video/mp4"},
        {".avi", "video/x-msvideo"},
        {".mov", "video/quicktime"},
        {".doc", "application/msword"},
        {".docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
        {".xls", "application/vnd.ms-excel"},
        {".xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
        {".ppt", "application/vnd.ms-powerpoint"},
        {".pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"},
        {".7z", "application/x-7z-compressed"}
    };

    auto it = mimeTypes.find(extension);
    if (it != mimeTypes.end())
    {
        return it->second;
    }
    
    return "application/octet-stream";
}

// HTML转义
std::string HtmlEscape(const std::string& str)
{
    std::string result;
    for (char c : str)
    {
        switch (c)
        {
        case '&': result += "&amp;"; break;
        case '<': result += "&lt;"; break;
        case '>': result += "&gt;"; break;
        case '"': result += "&quot;"; break;
        case '\'': result += "&apos;"; break;
        default: result += c; break;
        }
    }
    return result;
}

// URL解码
std::string UrlDecode(const std::string& str)
{
    std::string result;
    char ch;
    for (size_t i = 0; i < str.length(); i++)
    {
        if (str[i] == '%')
        {
            if (i + 2 < str.length())
            {
                int hex;
                std::stringstream ss(str.substr(i + 1, 2));
                ss >> std::hex >> hex;
                ch = static_cast<char>(hex);
                result += ch;
                i += 2;
            }
        }
        else if (str[i] == '+')
        {
            result += ' ';
        }
        else
        {
            result += str[i];
        }
    }
    return result;
}

// 生成目录列表HTML（使用现代样式）
std::string GenerateDirectoryListing(const std::wstring& path, const std::string& requestPath)
{
    std::stringstream html;
    
    html << "<!DOCTYPE html>\n";
    html << "<html>\n";
    html << "<head>\n";
    html << "    <meta charset='UTF-8'>\n";
    html << "    <meta name='viewport' content='width=device-width, initial-scale=1.0'>\n";
    html << "    <title>📁 " << requestPath << " - 目录列表</title>\n";
    html << "    <link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0/css/all.min.css'>\n";
    html << "    <style>\n";
    html << "        * {\n";
    html << "            margin: 0;\n";
    html << "            padding: 0;\n";
    html << "            box-sizing: border-box;\n";
    html << "        }\n";
    html << "        body {\n";
    html << "            font-family: 'Segoe UI', 'Microsoft YaHei', sans-serif;\n";
    html << "            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);\n";
    html << "            min-height: 100vh;\n";
    html << "            padding: 20px;\n";
    html << "        }\n";
    html << "        .container {\n";
    html << "            max-width: 1200px;\n";
    html << "            margin: 0 auto;\n";
    html << "        }\n";
    html << "        .header {\n";
    html << "            background: white;\n";
    html << "            border-radius: 12px;\n";
    html << "            padding: 25px 30px;\n";
    html << "            margin-bottom: 25px;\n";
    html << "            box-shadow: 0 4px 20px rgba(0,0,0,0.08);\n";
    html << "            display: flex;\n";
    html << "            align-items: center;\n";
    html << "            gap: 20px;\n";
    html << "        }\n";
    html << "        .header-icon {\n";
    html << "            font-size: 40px;\n";
    html << "            color: #4f46e5;\n";
    html << "        }\n";
    html << "        .header-text h1 {\n";
    html << "            color: #1f2937;\n";
    html << "            font-size: 24px;\n";
    html << "            margin-bottom: 5px;\n";
    html << "        }\n";
    html << "        .header-text p {\n";
    html << "            color: #6b7280;\n";
    html << "            font-size: 14px;\n";
    html << "        }\n";
    html << "        .breadcrumb {\n";
    html << "            background: white;\n";
    html << "            border-radius: 8px;\n";
    html << "            padding: 15px 20px;\n";
    html << "            margin-bottom: 20px;\n";
    html << "            box-shadow: 0 2px 10px rgba(0,0,0,0.05);\n";
    html << "        }\n";
    html << "        .breadcrumb a {\n";
    html << "            color: #4f46e5;\n";
    html << "            text-decoration: none;\n";
    html << "            transition: color 0.2s;\n";
    html << "        }\n";
    html << "        .breadcrumb a:hover {\n";
    html << "            color: #3730a3;\n";
    html << "            text-decoration: underline;\n";
    html << "        }\n";
    html << "        .files-table {\n";
    html << "            background: white;\n";
    html << "            border-radius: 12px;\n";
    html << "            overflow: hidden;\n";
    html << "            box-shadow: 0 4px 20px rgba(0,0,0,0.08);\n";
    html << "        }\n";
    html << "        table {\n";
    html << "            width: 100%;\n";
    html << "            border-collapse: collapse;\n";
    html << "        }\n";
    html << "        th {\n";
    html << "            background: #f8fafc;\n";
    html << "            padding: 16px 20px;\n";
    html << "            text-align: left;\n";
    html << "            font-weight: 600;\n";
    html << "            color: #475569;\n";
    html << "            font-size: 14px;\n";
    html << "            border-bottom: 1px solid #e2e8f0;\n";
    html << "        }\n";
    html << "        td {\n";
    html << "            padding: 16px 20px;\n";
    html << "            border-bottom: 1px solid #f1f5f9;\n";
    html << "            color: #334155;\n";
    html << "        }\n";
    html << "        tr:last-child td {\n";
    html << "            border-bottom: none;\n";
    html << "        }\n";
    html << "        tr:hover {\n";
    html << "            background: #f8fafc;\n";
    html << "        }\n";
    html << "        .file-icon {\n";
    html << "            font-size: 18px;\n";
    html << "            margin-right: 12px;\n";
    html << "            width: 24px;\n";
    html << "            text-align: center;\n";
    html << "            display: inline-block;\n";
    html << "        }\n";
    html << "        .folder-icon {\n";
    html << "            color: #f59e0b;\n";
    html << "        }\n";
    html << "        .file-icon-file {\n";
    html << "            color: #4f46e5;\n";
    html << "        }\n";
    html << "        .file-name a {\n";
    html << "            color: #1f2937;\n";
    html << "            text-decoration: none;\n";
    html << "            font-weight: 500;\n";
    html << "            transition: color 0.2s;\n";
    html << "        }\n";
    html << "        .file-name a:hover {\n";
    html << "            color: #4f46e5;\n";
    html << "        }\n";
    html << "        .file-type {\n";
    html << "            background: #e0e7ff;\n";
    html << "            color: #4f46e5;\n";
    html << "            padding: 4px 10px;\n";
    html << "            border-radius: 12px;\n";
    html << "            font-size: 12px;\n";
    html << "            font-weight: 500;\n";
    html << "        }\n";
    html << "        .file-size {\n";
    html << "            color: #64748b;\n";
    html << "            font-family: 'Consolas', monospace;\n";
    html << "        }\n";
    html << "        .file-time {\n";
    html << "            color: #64748b;\n";
    html << "            font-size: 13px;\n";
    html << "        }\n";
    html << "        .empty-state {\n";
    html << "            text-align: center;\n";
    html << "            padding: 60px 20px;\n";
    html << "            color: #64748b;\n";
    html << "        }\n";
    html << "        .empty-state i {\n";
    html << "            font-size: 48px;\n";
    html << "            margin-bottom: 20px;\n";
    html << "            color: #cbd5e1;\n";
    html << "        }\n";
    html << "        @media (max-width: 768px) {\n";
    html << "            .header {\n";
    html << "                flex-direction: column;\n";
    html << "                text-align: center;\n";
    html << "                padding: 20px;\n";
    html << "            }\n";
    html << "            th, td {\n";
    html << "                padding: 12px 15px;\n";
    html << "            }\n";
    html << "        }\n";
    html << "    </style>\n";
    html << "</head>\n";
    html << "<body>\n";
    html << "    <div class='container'>\n";
    html << "        <div class='header'>\n";
    html << "            <div class='header-icon'>📁</div>\n";
    html << "            <div class='header-text'>\n";
    html << "                <h1>目录列表</h1>\n";
    html << "                <p>" << requestPath << "</p>\n";
    html << "            </div>\n";
    html << "        </div>\n";
    
    // 面包屑导航
    html << "        <div class='breadcrumb'>\n";
    html << "            <a href='/'>🏠 根目录</a>";
    
    std::string currentPath = "";
    std::istringstream pathStream(requestPath);
    std::string segment;
    while (std::getline(pathStream, segment, '/'))
    {
        if (!segment.empty())
        {
            currentPath += "/" + segment;
            html << " &gt; <a href='" << currentPath << "'>" << HtmlEscape(segment) << "</a>";
        }
    }
    
    html << "        </div>\n";
    
    html << "        <div class='files-table'>\n";
    html << "            <table>\n";
    html << "                <thead>\n";
    html << "                    <tr>\n";
    html << "                        <th style='width: 45%;'>名称</th>\n";
    html << "                        <th style='width: 15%;'>类型</th>\n";
    html << "                        <th style='width: 15%;'>大小</th>\n";
    html << "                        <th style='width: 25%;'>修改时间</th>\n";
    html << "                    </tr>\n";
    html << "                </thead>\n";
    html << "                <tbody>\n";
    
    try
    {
        bool hasEntries = false;
        
        // 添加上级目录链接（如果不是根目录）
        if (requestPath != "/")
        {
            hasEntries = true;
            html << "                    <tr>\n";
            html << "                        <td class='file-name'>\n";
            html << "                            <span class='file-icon folder-icon'>📁</span>\n";
            html << "                            <a href='../'>[上级目录]</a>\n";
            html << "                        </td>\n";
            html << "                        <td><span class='file-type'>目录</span></td>\n";
            html << "                        <td class='file-size'>-</td>\n";
            html << "                        <td class='file-time'>-</td>\n";
            html << "                    </tr>\n";
        }
        
        // 遍历目录
        for (const auto& entry : fs::directory_iterator(path))
        {
            hasEntries = true;
            std::wstring filename = entry.path().filename().wstring();
            
            // 将宽字符文件名转换为UTF-8
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            std::string utf8Filename = converter.to_bytes(filename);
            
            html << "                    <tr>\n";
            
            // 文件名和图标
            html << "                        <td class='file-name'>\n";
            
            if (entry.is_directory())
            {
                html << "                            <span class='file-icon folder-icon'>📁</span>\n";
            }
            else
            {
                // 根据文件扩展名显示不同图标
                std::string ext = fs::path(entry.path()).extension().string();
                if (ext == ".txt" || ext == ".md") html << "                            <span class='file-icon file-icon-file'>📝</span>\n";
                else if (ext == ".pdf") html << "                            <span class='file-icon file-icon-file'>📕</span>\n";
                else if (ext == ".zip" || ext == ".rar" || ext == ".7z") html << "                            <span class='file-icon file-icon-file'>📦</span>\n";
                else if (ext == ".jpg" || ext == ".png" || ext == ".gif") html << "                            <span class='file-icon file-icon-file'>🖼️</span>\n";
                else if (ext == ".mp4" || ext == ".avi" || ext == ".mov") html << "                            <span class='file-icon file-icon-file'>🎬</span>\n";
                else if (ext == ".mp3" || ext == ".wav") html << "                            <span class='file-icon file-icon-file'>🎵</span>\n";
                else html << "                            <span class='file-icon file-icon-file'>📄</span>\n";
            }
            
            // URL编码
            std::string encodedFilename;
            for (char c : utf8Filename)
            {
                if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || 
                    c == '-' || c == '_' || c == '.' || c == '~')
                {
                    encodedFilename += c;
                }
                else
                {
                    char hex[4];
                    sprintf(hex, "%%%02X", static_cast<unsigned char>(c));
                    encodedFilename += hex;
                }
            }
            
            // 构建URL
            std::string itemUrl = requestPath;
            if (itemUrl.back() != '/') itemUrl += '/';
            itemUrl += encodedFilename;
            if (entry.is_directory()) itemUrl += '/';
            
            html << "                            <a href='" << itemUrl << "'>" << HtmlEscape(utf8Filename);
            if (entry.is_directory()) html << "/";
            html << "</a>\n";
            html << "                        </td>\n";
            
            // 文件类型
            if (entry.is_directory())
            {
                html << "                        <td><span class='file-type'>目录</span></td>\n";
                html << "                        <td class='file-size'>-</td>\n";
            }
            else
            {
                html << "                        <td><span class='file-type'>文件</span></td>\n";
                
                // 文件大小（格式化为可读形式）
                auto fileSize = entry.file_size();
                if (fileSize < 1024)
                {
                    html << "                        <td class='file-size'>" << fileSize << " B</td>\n";
                }
                else if (fileSize < 1024 * 1024)
                {
                    html << "                        <td class='file-size'>" << (fileSize / 1024) << " KB</td>\n";
                }
                else if (fileSize < 1024 * 1024 * 1024)
                {
                    html << "                        <td class='file-size'>" << std::fixed << std::setprecision(1) << (fileSize / (1024.0 * 1024.0)) << " MB</td>\n";
                }
                else
                {
                    html << "                        <td class='file-size'>" << std::fixed << std::setprecision(1) << (fileSize / (1024.0 * 1024.0 * 1024.0)) << " GB</td>\n";
                }
            }
            
            // 修改时间
            auto ftime = fs::last_write_time(entry.path());
            auto duration = ftime.time_since_epoch();
            auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
            time_t cftime = seconds.count();
            struct tm timeinfo;
            localtime_s(&timeinfo, &cftime);
            char timeStr[64];
            strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &timeinfo);
            html << "                        <td class='file-time'>" << timeStr << "</td>\n";
            
            html << "                    </tr>\n";
        }
        
        if (!hasEntries)
        {
            html << "                    <tr><td colspan='4' class='empty-state'><i class='fas fa-folder-open'></i><br>空目录</td></tr>\n";
        }
    }
    catch (const std::exception& e)
    {
        html << "                    <tr><td colspan='4' class='empty-state'><i class='fas fa-exclamation-triangle'></i><br>无法读取目录: " << e.what() << "</td></tr>\n";
    }
    
    html << "                </tbody>\n";
    html << "            </table>\n";
    html << "        </div>\n";
    html << "    </div>\n";
    html << "    <script>\n";
    html << "        // 添加一些交互效果\n";
    html << "        document.addEventListener('DOMContentLoaded', function() {\n";
    html << "            const rows = document.querySelectorAll('tbody tr');\n";
    html << "            rows.forEach(row => {\n";
    html << "                row.addEventListener('click', function(e) {\n";
    html << "                    if (e.target.tagName !== 'A') {\n";
    html << "                        const link = this.querySelector('a');\n";
    html << "                        if (link) {\n";
    html << "                            window.location = link.href;\n";
    html << "                        }\n";
    html << "                    }\n";
    html << "                });\n";
    html << "            });\n";
    html << "        });\n";
    html << "    </script>\n";
    html << "</body>\n";
    html << "</html>\n";
    
    return html.str();
}