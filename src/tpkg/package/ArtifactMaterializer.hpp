/*
 * @Author: cubevlmu khfahqp@gmail.com
 * @LastEditors: cubevlmu khfahqp@gmail.com
 * Copyright (c) 2026 by FlybirdGames, All Rights Reserved. 
 */

#pragma once

#include "tpkg/model/DependencyDesc.hpp"
#include "tpkg/package/PackageBuilderUtil.hpp"

namespace toolkit
{
    class DiagnosticSink;
}

namespace toolkit
{
    class ArtifactFiles
    {
    public:
        static bool materialize(const BuildUtil::Context &context,
                                const DependencyArtifacts &requested,
                                DependencyArtifacts &exported,
                                DiagnosticSink &diagnostics);

    private:
        static std::filesystem::path input(const BuildUtil::Context &context, const std::string &value);
        static std::string output(const std::string &value);
        static bool copy(const BuildUtil::Context &context,
                         const std::vector<std::string> &requested,
                         std::vector<std::string> &exported,
                         const char *field,
                         DiagnosticSink &diagnostics);
    };
} // namespace toolkit
