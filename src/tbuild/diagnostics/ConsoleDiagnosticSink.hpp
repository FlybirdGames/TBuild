/*
 * @Author: cubevlmu khfahqp@gmail.com
 * @LastEditors: cubevlmu khfahqp@gmail.com
 * Copyright (c) 2026 by FlybirdGames, All Rights Reserved. 
 */

#pragma once

#include "tbuild/diagnostics/DiagnosticSink.hpp"

namespace toolkit
{
    class ConsoleDiagnosticSink final : public DiagnosticSink
    {
    public:
        void report(const Diagnostic &diagnostic) override;
    };

} // namespace toolkit
