/*
 * @Author: cubevlmu khfahqp@gmail.com
 * @LastEditors: cubevlmu khfahqp@gmail.com
 * Copyright (c) 2026 by FlybirdGames, All Rights Reserved.
 */

#pragma once

#include "tpkg/model/DependencyDesc.hpp"
#include "tpkg/model/DependencyOverride.hpp"

#include <string>
#include <vector>

namespace toolkit
{

    struct PackageDesc
    {
        std::string name;
        std::string version;
        std::vector<DependencyDesc> dependencies;
        std::vector<DependencyOverrideDesc> dependencyOverrides;
    };

} // namespace toolkit
