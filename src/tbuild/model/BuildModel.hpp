/*
 * @Author: cubevlmu khfahqp@gmail.com
 * @LastEditors: cubevlmu khfahqp@gmail.com
 * Copyright (c) 2026 by FlybirdGames, All Rights Reserved. 
 */

#pragma once

#include "tbuild/model/PackageDesc.hpp"
#include "tbuild/model/WorkspaceDesc.hpp"

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
