/*
 * @Author: cubevlmu khfahqp@gmail.com
 * @LastEditors: cubevlmu khfahqp@gmail.com
 * Copyright (c) 2026 by FlybirdGames, All Rights Reserved. 
 */

#pragma once

#include "tpkg/model/PackageDesc.hpp"
#include "tpkg/model/WorkspaceDesc.hpp"

namespace toolkit
{

    class DiagnosticSink;

    struct BuildModel
    {
        WorkspaceDesc workspace;
        PackageDesc rootPackage;
    };

    bool validateBuildModel(const BuildModel &model, DiagnosticSink &diagnostics);

} // namespace toolkit
