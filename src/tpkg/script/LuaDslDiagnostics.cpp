/*
 * @Author: cubevlmu khfahqp@gmail.com
 * @LastEditors: cubevlmu khfahqp@gmail.com
 * Copyright (c) 2026 by FlybirdGames, All Rights Reserved. 
 */

#include "tpkg/script/LuaDslDiagnostics.hpp"

#include "tpkg/diagnostics/DiagnosticSink.hpp"

namespace toolkit
{

    void reportUnsupportedProjectDsl(DiagnosticSink &diagnostics, const std::string &name)
    {
        diagnostics.error("project build DSL '" + name + "' is not supported in tpkg.lua; use require()/require_local() to declare dependencies");
    }

} // namespace toolkit
