#pragma once

#include <filesystem>
#include <string>

namespace toolkit
{
    class Path
    {
    public:
        static std::string normalizePath(const std::filesystem::path &path);
        static std::string sanitizePackagePath(std::string value);
    };
} // namespace toolkit
