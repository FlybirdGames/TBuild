/*
 * @Author: cubevlmu khfahqp@gmail.com
 * @LastEditors: cubevlmu khfahqp@gmail.com
 * Copyright (c) 2026 by FlybirdGames, All Rights Reserved. 
 */

#include "tpkg/script/LuaCondition.hpp"

#include "tpkg/core/StringUtil.hpp"

namespace toolkit
{

    bool conditionMatches(const std::string &condition, const std::string &activePlatform)
    {
        const auto normalized = StringUtils::toLower(condition);
        if (normalized == "all" || normalized == "*")
        {
            return true;
        }
        if (normalized == "mac")
        {
            return activePlatform == "macos";
        }
        return normalized == activePlatform;
    }

    bool isValidCondition(const std::string &condition)
    {
        const auto normalized = StringUtils::toLower(condition);
        return normalized == "all" ||
               normalized == "*" ||
               normalized == "windows" ||
               normalized == "linux" ||
               normalized == "mac" ||
               normalized == "macos" ||
               normalized == "android" ||
               normalized == "ios";
    }

} // namespace toolkit
