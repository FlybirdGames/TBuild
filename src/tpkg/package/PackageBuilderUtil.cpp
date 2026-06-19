/*
 * @Author: cubevlmu khfahqp@gmail.com
 * @LastEditors: cubevlmu khfahqp@gmail.com
 * Copyright (c) 2026 by FlybirdGames, All Rights Reserved. 
 */

#include "tpkg/package/PackageBuilderUtil.hpp"

#include "tpkg/core/FileSystem.hpp"
#include "tpkg/diagnostics/DiagnosticSink.hpp"
#include "tpkg/package/ArtifactMaterializer.hpp"
#include "tpkg/package/ArtifactValidator.hpp"
#include "tpkg/package/BuildVariableExpander.hpp"
#include "tpkg/package/CMakeArgumentBuilder.hpp"
#include "tpkg/package/ToolchainContextResolver.hpp"

#include <utility>

namespace toolkit
{

    bool BuildUtil::toolchain(Context &context,
                              const std::string &preferredToolchain,
                              const ToolchainRequirements &requirements,
                              DiagnosticSink *diagnostics)
    {
        return BuildToolchain::populate(context, preferredToolchain, requirements, diagnostics);
    }

    std::string BuildUtil::expand(std::string value, const Context &context)
    {
        return BuildVar::expand(std::move(value), context);
    }

    std::vector<std::string> BuildUtil::expand(const std::vector<std::string> &values, const Context &context)
    {
        return BuildVar::expand(values, context);
    }

    std::map<std::string, std::string> BuildUtil::expand(const std::map<std::string, std::string> &values, const Context &context)
    {
        return BuildVar::expand(values, context);
    }

    std::map<std::string, std::string> BuildUtil::cmakeOptions(const DependencyDesc &dependency)
    {
        return CMakeArgs::options(dependency);
    }

    std::vector<std::string> BuildUtil::cmakeArgs(const DependencyDesc &dependency, const Context &context)
    {
        return CMakeArgs::configure(dependency, context);
    }

    std::vector<std::string> BuildUtil::wrap(const std::filesystem::path &workingDir,
                                             const std::map<std::string, std::string> &env,
                                             const std::string &executable,
                                             const std::vector<std::string> &args)
    {
        return CMakeArgs::wrap(workingDir, env, executable, args);
    }

    bool BuildUtil::materialize(const Context &context,
                                const DependencyArtifacts &requested,
                                DependencyArtifacts &exported,
                                DiagnosticSink &diagnostics)
    {
        return ArtifactFiles::materialize(context, requested, exported, diagnostics);
    }

    bool BuildUtil::markBuilt(const Context &context, DiagnosticSink &diagnostics)
    {
        std::string error;
        if (!File::write(context.buildDir / ".tpkg_build_complete", "built\n", &error))
        {
            diagnostics.error("failed to write package build stage marker: " + error);
            return false;
        }
        return true;
    }

    bool BuildUtil::validate(const Context &context,
                             const DependencyArtifacts &artifacts,
                             DiagnosticSink &diagnostics)
    {
        return ArtifactCheck::validate(context, artifacts, diagnostics);
    }

} // namespace toolkit
