/*
 * @Author: cubevlmu khfahqp@gmail.com
 * @LastEditors: cubevlmu khfahqp@gmail.com
 * Copyright (c) 2026 by FlybirdGames, All Rights Reserved. 
 */

#include "tbuild/toolchain/ToolchainProbe.hpp"

#include "tbuild/core/Environment.hpp"
#include "tbuild/core/Process.hpp"

#include <filesystem>
#include <string>
#include <utility>
#include <vector>

namespace toolkit
{
    class UnixTc
    {
    public:
        static void add(std::vector<ToolchainProfile> &profiles);

    private:
        static std::filesystem::path xfind(const std::string &tool);
        static std::filesystem::path sdkPath(const std::string &sdk);
        static std::string sdkVersion(const std::string &sdk);
        static ToolchainProfile apple(const std::string &id,
                                      const std::string &sdkKind,
                                      const std::string &targetArch,
                                      const std::string &targetTriple,
                                      const std::string &deploymentTarget);
        static void addApple(std::vector<ToolchainProfile> &profiles);
        static ToolchainProfile unixProfile();
    };

    std::filesystem::path UnixTc::xfind(const std::string &tool)
    {
        const auto result = Process::run("xcrun", {"--find", tool});
        if (result.exitCode != 0)
        {
            return {};
        }
        const auto value = ToolProbe::trim(result.output);
        return value.empty() ? std::filesystem::path{} : std::filesystem::path(value);
    }

    std::filesystem::path UnixTc::sdkPath(const std::string &sdk)
    {
        const auto result = Process::run("xcrun", {"--sdk", sdk, "--show-sdk-path"});
        if (result.exitCode != 0)
        {
            return {};
        }
        const auto value = ToolProbe::trim(result.output);
        return value.empty() ? std::filesystem::path{} : std::filesystem::path(value);
    }

    std::string UnixTc::sdkVersion(const std::string &sdk)
    {
        const auto result = Process::run("xcrun", {"--sdk", sdk, "--show-sdk-version"});
        return result.exitCode == 0 ? ToolProbe::trim(result.output) : std::string{};
    }
    ToolchainProfile UnixTc::apple(const std::string &id,
                                   const std::string &sdkKind,
                                   const std::string &targetArch,
                                   const std::string &targetTriple,
                                   const std::string &deploymentTarget)
    {
        ToolchainProfile profile;
        profile.id = id;
        profile.platform = sdkKind == "macosx" ? "macos" : "ios";
        profile.hostArch = ToolProbe::arch();
        profile.targetArch = targetArch;
        profile.compilerKind = "clang";
        profile.sdkKind = sdkKind;
        profile.compiler = xfind("clang");
        profile.cxxCompiler = xfind("clang++");
        profile.linker = profile.cxxCompiler.empty() ? profile.compiler : profile.cxxCompiler;
        profile.archiver = xfind("ar");
        profile.libtool = xfind("libtool");
        profile.lipo = xfind("lipo");
        profile.codesign = xfind("codesign");
        profile.sdkRoot = sdkPath(sdkKind);
        profile.sdkVersion = sdkVersion(sdkKind);
        profile.targetTriple = targetTriple;
        profile.deploymentTarget = deploymentTarget;
        ToolProbe::appendDir(profile.binaryDirs, profile.compiler.parent_path());
        ToolProbe::addTools(profile);
        ToolProbe::addPath(profile);
        ToolProbe::complete(profile);
        return profile;
    }

    void UnixTc::addApple(std::vector<ToolchainProfile> &profiles)
    {
        if (Environment::hostPlatformName() != "macos")
        {
            return;
        }
        const auto xcodeSelect = Process::run("xcode-select", {"-p"});
        if (xcodeSelect.exitCode != 0)
        {
            return;
        }
        for (auto profile : {
                 apple("macos-clang-x64", "macosx", "x64", "x86_64-apple-macosx", "11.0"),
                 apple("macos-clang-arm64", "macosx", "arm64", "arm64-apple-macosx", "11.0"),
                 apple("ios-clang-arm64", "iphoneos", "arm64", "arm64-apple-ios", "13.0"),
                 apple("ios-simulator-clang-arm64", "iphonesimulator", "arm64", "arm64-apple-ios-simulator", "13.0"),
                 apple("ios-simulator-clang-x64", "iphonesimulator", "x64", "x86_64-apple-ios-simulator", "13.0"),
             })
        {
            if (!profile.compiler.empty() || !profile.cxxCompiler.empty())
            {
                profiles.push_back(std::move(profile));
            }
        }
    }
    ToolchainProfile UnixTc::unixProfile()
    {
        ToolchainProfile profile;
        profile.id = Environment::hostPlatformName() + "-clang-x64";
        profile.platform = Environment::hostPlatformName();
        profile.hostArch = "x64";
        profile.targetArch = "x64";
        profile.compilerKind = "clang";
        profile.sdkKind = "sysroot";
        profile.compiler = findExecutableOnPath("clang");
        profile.cxxCompiler = findExecutableOnPath("clang++");
        profile.linker = profile.cxxCompiler;
        profile.archiver = findExecutableOnPath("ar");
        ToolProbe::addTools(profile);
        ToolProbe::addPath(profile);
        ToolProbe::complete(profile);
        return profile;
    }

    void UnixTc::add(std::vector<ToolchainProfile> &profiles)
    {
        auto unix = unixProfile();
        if (!unix.compiler.empty() || !unix.cxxCompiler.empty())
        {
            profiles.push_back(std::move(unix));
        }
        addApple(profiles);
    }

    void addUnixProfiles(std::vector<ToolchainProfile> &profiles)
    {
        UnixTc::add(profiles);
    }

} // namespace toolkit
