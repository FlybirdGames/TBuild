/*
 * @Author: cubevlmu khfahqp@gmail.com
 * @LastEditors: cubevlmu khfahqp@gmail.com
 * Copyright (c) 2026 by FlybirdGames, All Rights Reserved.
 */

#include "tpkg/cli/Commands.hpp"

#include "tpkg/config/JsonModelWriter.hpp"
#include "tpkg/core/Logger.hpp"

#include <filesystem>

namespace toolkit
{
    int Commands::dumpModel(const std::filesystem::path &workspaceRoot)
    {
        ConsoleDiagnosticSink diagnostics;
        BuildModel model;
        if (!CliWorkspace::load(workspaceRoot, model, diagnostics))
        {
            return 1;
        }
        LogInfo("{}", JsonModel::write(model));
        return 0;
    }

} // namespace toolkit
