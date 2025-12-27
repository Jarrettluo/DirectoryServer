#pragma once

#include <string>
#include <unordered_map>

// 获取MIME类型
std::string GetMimeType(const std::string& extension);

// HTML转义
std::string HtmlEscape(const std::string& str);

// URL解码
std::string UrlDecode(const std::string& str);

// 生成目录列表HTML（使用现代样式）
std::string GenerateDirectoryListing(const std::wstring& path, const std::string& requestPath);