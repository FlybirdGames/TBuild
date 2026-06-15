/*
 * @Author: cubevlmu khfahqp@gmail.com
 * @LastEditors: cubevlmu khfahqp@gmail.com
 * Copyright (c) 2026 by FlybirdGames, All Rights Reserved. 
 */

#pragma once

#include "tbuild/model/DependencyDesc.hpp"
#include "tbuild/package/PackageArtifact.hpp"

#include <filesystem>

namespace toolkit
{

    class DiagnosticSink;

    struct PackageBuildOptions
    {
        bool runBuild = true;
        bool runExport = true;
    };

    class PackageBuilder
    {
    public:
        virtual ~PackageBuilder() = default;
        virtual bool build(const DependencyDesc &dependency,
                           const std::filesystem::path &sourceDir,
                           const std::filesystem::path &buildDir,
                           const std::filesystem::path &artifactDir,
                           const std::string &commit,
                           const std::filesystem::path &workspaceRoot,
                           const std::string &preferredToolchain,
                           const std::string &config,
                           const PackageBuildOptions &options,
                           PackageArtifact &artifact,
                           DiagnosticSink &diagnostics) const = 0;
    };

} // namespace toolkit
