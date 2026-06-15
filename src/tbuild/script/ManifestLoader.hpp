/*
 * @Author: cubevlmu khfahqp@gmail.com
 * @LastEditors: cubevlmu khfahqp@gmail.com
 * Copyright (c) 2026 by FlybirdGames, All Rights Reserved. 
 */

#pragma once

#include "tbuild/model/BuildModel.hpp"

#include <filesystem>

namespace toolkit
{

    class DiagnosticSink;

    struct ManifestLoadResult
    {
        bool ok = false;
        BuildModel model;
    };

    class ManifestLoader
    {
    public:
        ManifestLoadResult load(const std::filesystem::path &manifestPath, DiagnosticSink &diagnostics);
    };

} // namespace toolkit
