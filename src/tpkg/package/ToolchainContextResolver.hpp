/*
 * @Author: cubevlmu khfahqp@gmail.com
 * @LastEditors: cubevlmu khfahqp@gmail.com
 * Copyright (c) 2026 by FlybirdGames, All Rights Reserved.
 */

#pragma once

#include "tpkg/model/DependencyDesc.hpp"
#include "tpkg/package/PackageBuilderUtil.hpp"
#include "tpkg/toolchain/Toolchain.hpp"

#include <string>

namespace toolkit
{
    class DiagnosticSink;
}

namespace toolkit
{
    class BuildToolchain
    {
    public:
        static bool populate(BuildUtil::Context &context,
                             const std::string &preferredToolchain = {},
                             const ToolchainRequirements &requirements = {},
                             DiagnosticSink *diagnostics = nullptr);

    private:
        static bool matches(const ToolchainProfile &profile, const ToolchainRequirements &requirements);
        static bool has(const ToolchainRequirements &requirements);
    };
} // namespace toolkit
