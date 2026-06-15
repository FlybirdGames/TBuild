/*
 * @Author: cubevlmu khfahqp@gmail.com
 * @LastEditors: cubevlmu khfahqp@gmail.com
 * Copyright (c) 2026 by FlybirdGames, All Rights Reserved.
 */

#include "tbuild/cli/Commands.hpp"

#include "tbuild/config/JsonModelWriter.hpp"
#include "tbuild/core/Logger.hpp"

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
