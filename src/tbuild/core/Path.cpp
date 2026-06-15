/*
 * @Author: cubevlmu khfahqp@gmail.com
 * @LastEditors: cubevlmu khfahqp@gmail.com
 * Copyright (c) 2026 by FlybirdGames, All Rights Reserved.
 */

#include "tbuild/core/Path.hpp"

#include <algorithm>

namespace toolkit
{

    std::string Path::normalizePath(const std::filesystem::path &path)
    {
        auto value = path.generic_string();
        while (value.find("//") != std::string::npos)
        {
            value.erase(value.find("//"), 1);
        }
        return value;
    }

    std::string Path::sanitizePackagePath(std::string value)
    {
        std::replace(value.begin(), value.end(), '\\', '/');
        std::replace(value.begin(), value.end(), ':', '_');
        std::replace(value.begin(), value.end(), '?', '_');
        std::replace(value.begin(), value.end(), '*', '_');
        std::replace(value.begin(), value.end(), '"', '_');
        std::replace(value.begin(), value.end(), '<', '_');
        std::replace(value.begin(), value.end(), '>', '_');
        std::replace(value.begin(), value.end(), '|', '_');
        while (!value.empty() && value.front() == '/')
        {
            value.erase(value.begin());
        }
        return value;
    }

} // namespace toolkit
