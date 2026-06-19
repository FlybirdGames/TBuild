/*
 * @Author: cubevlmu khfahqp@gmail.com
 * @LastEditors: cubevlmu khfahqp@gmail.com
 * Copyright (c) 2026 by FlybirdGames, All Rights Reserved. 
 */

#include "tpkg/package/ToolchainContextResolver.hpp"

#include "tpkg/core/StringUtil.hpp"
#include "tpkg/diagnostics/DiagnosticSink.hpp"
#include "tpkg/toolchain/ToolchainDetector.hpp"
#include "tpkg/toolchain/ToolchainRegistry.hpp"

namespace toolkit
{
    bool BuildToolchain::matches(const ToolchainProfile &profile, const ToolchainRequirements &requirements)
    {
        const auto family = StringUtils::toLower(requirements.family);
        const auto abi = StringUtils::toLower(requirements.abi);
        const auto binaryFormat = StringUtils::toLower(requirements.binaryFormat);
        const auto compilerKind = StringUtils::toLower(profile.compilerKind);
        const auto sdkKind = StringUtils::toLower(profile.sdkKind);
        const auto id = StringUtils::toLower(profile.id);

        if (!family.empty())
        {
            if (family == "msvc-abi" || family == "msvc")
            {
                if (compilerKind != "msvc" && compilerKind != "clang-cl")
                {
                    return false;
                }
            }
            else if (family == "gnu" || family == "mingw")
            {
                if (compilerKind != "gcc" && compilerKind != "clang")
                {
                    return false;
                }
                if (family == "mingw" && sdkKind != "mingw" && id.find("mingw") == std::string::npos)
                {
                    return false;
                }
            }
            else if (family != compilerKind)
            {
                return false;
            }
        }
        if (!abi.empty())
        {
            if (abi == "msvc")
            {
                if (compilerKind != "msvc" && compilerKind != "clang-cl")
                {
                    return false;
                }
            }
            else if (abi == "gnu" || abi == "mingw")
            {
                if (compilerKind != "gcc" && compilerKind != "clang")
                {
                    return false;
                }
            }
            else if (abi != compilerKind)
            {
                return false;
            }
        }
        if (!binaryFormat.empty())
        {
            if (binaryFormat == "coff")
            {
                if (compilerKind != "msvc" && compilerKind != "clang-cl")
                {
                    return false;
                }
            }
            else if (binaryFormat == "archive" || binaryFormat == "ar")
            {
                if (compilerKind == "msvc" || compilerKind == "clang-cl")
                {
                    return false;
                }
            }
        }
        return true;
    }

    bool BuildToolchain::has(const ToolchainRequirements &requirements)
    {
        return !requirements.family.empty() || !requirements.abi.empty() || !requirements.binaryFormat.empty();
    }

    bool BuildToolchain::populate(BuildUtil::Context &context,
                                  const std::string &preferredToolchain,
                                  const ToolchainRequirements &requirements,
                                  DiagnosticSink *providedDiagnostics)
    {
        class NullDiagnosticSink final : public DiagnosticSink
        {
        public:
            void report(const Diagnostic &) override {}
        };

        NullDiagnosticSink diagnostics;
        DiagnosticSink &sink = providedDiagnostics ? *providedDiagnostics : diagnostics;
        const auto profiles = detectHostToolchains(sink);
        if (profiles.empty())
        {
            return false;
        }

        std::string requested = preferredToolchain;
        if (requested.empty() && !context.workspaceRoot.empty())
        {
            LocalToolchainConfig config;
            if (!readLocalToolchainConfig(context.workspaceRoot, config, sink))
            {
                return false;
            }
            requested = config.preferredToolchain;
        }

        const ToolchainProfile *selected = nullptr;
        if (!requested.empty())
        {
            for (const auto &profile : profiles)
            {
                if (profile.id == requested)
                {
                    selected = &profile;
                    break;
                }
            }
            if (!selected)
            {
                sink.error("requested toolchain was not detected: " + requested);
                return false;
            }
            if (!matches(*selected, requirements))
            {
                sink.error("selected toolchain does not satisfy package toolchain requirements: " + selected->id);
                return false;
            }
        }
        else
        {
            for (const auto &profile : profiles)
            {
                if (profile.complete && matches(profile, requirements))
                {
                    selected = &profile;
                    break;
                }
            }
            if (!selected && !has(requirements))
            {
                for (const auto &profile : profiles)
                {
                    if (matches(profile, requirements))
                    {
                        selected = &profile;
                        break;
                    }
                }
            }
            if (!selected)
            {
                sink.error("no complete detected toolchain satisfies package toolchain requirements; pass --toolchain <id> to select a local profile explicitly");
                return false;
            }
        }

        sink.info("selected toolchain: " + selected->id);
        context.platform = selected->platform;
        context.arch = selected->targetArch;
        context.cc = selected->compiler.string();
        context.cxx = selected->cxxCompiler.empty() ? selected->compiler.string() : selected->cxxCompiler.string();
        context.linker = selected->linker.string();
        context.archiver = selected->archiver.string();
        context.rc = selected->resourceCompiler.string();
        context.mt = selected->manifestTool.string();
        context.cmake = selected->cmake.empty() ? "cmake" : selected->cmake.string();
        context.ninja = selected->ninja.empty() ? "ninja" : selected->ninja.string();
        context.git = selected->git.empty() ? "git" : selected->git.string();
        context.environment = selected->environment;
        return true;
    }

} // namespace toolkit
