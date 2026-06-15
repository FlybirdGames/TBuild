/*
 * @Author: cubevlmu khfahqp@gmail.com
 * @LastEditors: cubevlmu khfahqp@gmail.com
 * Copyright (c) 2026 by FlybirdGames, All Rights Reserved. 
 */

#include "tbuild/script/LuaDslDiagnostics.hpp"

#include "tbuild/diagnostics/DiagnosticSink.hpp"

namespace toolkit
{

    void reportUnsupportedProjectDsl(DiagnosticSink &diagnostics, const std::string &name)
    {
        diagnostics.error("project build DSL '" + name + "' is not supported in tbuild.deps.lua; use require()/require_local() to declare dependencies");
    }

} // namespace toolkit
