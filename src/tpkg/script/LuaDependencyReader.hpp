/*
 * @Author: cubevlmu khfahqp@gmail.com
 * @LastEditors: cubevlmu khfahqp@gmail.com
 * Copyright (c) 2026 by FlybirdGames, All Rights Reserved. 
 */

#pragma once

#include "tpkg/model/BuildModel.hpp"

#include <sol/sol.hpp>

#include <filesystem>
#include <string>

namespace toolkit
{

    class DiagnosticSink;

    DependencyDesc dependencyFromOptions(const std::string &idOrSource,
                                         sol::table options,
                                         bool local,
                                         const std::string &activePlatform,
                                         const std::filesystem::path &workspaceRoot);
    void overlayDependencyOptions(sol::table options, DependencyDesc &dependency);
    void readDependencyOverrides(sol::table table, BuildModel &model, DiagnosticSink &diagnostics);

} // namespace toolkit
