/*
 * @Author: cubevlmu khfahqp@gmail.com
 * @LastEditors: cubevlmu khfahqp@gmail.com
 * Copyright (c) 2026 by FlybirdGames, All Rights Reserved. 
 */

#include "tbuild/toolchain/ToolchainDetector.hpp"

#include "tbuild/core/Environment.hpp"
#include "tbuild/diagnostics/DiagnosticSink.hpp"
#include "tbuild/toolchain/ToolchainProbe.hpp"
#include "tbuild/toolchain/ToolchainRegistry.hpp"

#include <filesystem>
#include <vector>

namespace toolkit
{
    class ToolDetect
    {
    public:
        static std::vector<ToolchainProfile> host(const std::filesystem::path &workspaceRoot, DiagnosticSink &diagnostics);
        static std::filesystem::path cache(const std::filesystem::path &workspaceRoot);
        static bool writeHost(const std::filesystem::path &workspaceRoot, std::vector<ToolchainProfile> &profiles, DiagnosticSink &diagnostics);
    };

    std::vector<ToolchainProfile> ToolDetect::host(const std::filesystem::path &workspaceRoot, DiagnosticSink &diagnostics)
    {
        std::vector<ToolchainProfile> profiles;
        const auto platform = Environment::hostPlatformName();
        if (platform == "windows")
        {
            addWindowsProfiles(profiles, diagnostics);
        }
        else
        {
            addUnixProfiles(profiles);
        }
        addAndroidProfiles(profiles, workspaceRoot, diagnostics);

        if (profiles.empty())
        {
            diagnostics.warning("no C/C++ toolchains detected");
        }
        return profiles;
    }

    std::filesystem::path ToolDetect::cache(const std::filesystem::path &workspaceRoot)
    {
        return workspaceRoot / ".tbuild" / "toolchains" / "host.toml";
    }

    bool ToolDetect::writeHost(const std::filesystem::path &workspaceRoot, std::vector<ToolchainProfile> &profiles, DiagnosticSink &diagnostics)
    {
        profiles = host(workspaceRoot, diagnostics);
        return writeToolchainProfiles(cache(workspaceRoot), profiles, diagnostics);
    }

    std::vector<ToolchainProfile> detectHostToolchains(const std::filesystem::path &workspaceRoot, DiagnosticSink &diagnostics)
    {
        return ToolDetect::host(workspaceRoot, diagnostics);
    }

    std::vector<ToolchainProfile> detectHostToolchains(DiagnosticSink &diagnostics)
    {
        return detectHostToolchains({}, diagnostics);
    }

    std::filesystem::path hostToolchainCachePath(const std::filesystem::path &workspaceRoot)
    {
        return ToolDetect::cache(workspaceRoot);
    }

    bool detectAndWriteHostToolchains(const std::filesystem::path &workspaceRoot, std::vector<ToolchainProfile> &profiles, DiagnosticSink &diagnostics)
    {
        return ToolDetect::writeHost(workspaceRoot, profiles, diagnostics);
    }
}
