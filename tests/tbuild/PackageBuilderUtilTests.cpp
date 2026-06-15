#include "tbuild/package/PackageBuilderUtil.hpp"
#include "tbuild/package/CustomPackageBuilder.hpp"
#include "tbuild/package/PackageArtifactStore.hpp"
#include "tbuild/diagnostics/JsonDiagnosticSink.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>

namespace {

std::filesystem::path makeTempDir() {
    const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
    auto path = std::filesystem::temp_directory_path() / ("tkb-builder-util-" + std::to_string(stamp));
    std::filesystem::create_directories(path);
    return path;
}

void writeText(const std::filesystem::path& path, const std::string& content) {
    std::filesystem::create_directories(path.parent_path());
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    out << content;
}

TEST(PackageBuilderUtilTests, MapsCMakeAbstractionsToOptions) {
    toolkit::DependencyDesc dependency;
    dependency.name = "Pkg";
    dependency.buildType = "cmake";
    dependency.linkage = "static";
    dependency.runtime = "static";
    dependency.pic = "true";

    const auto options = toolkit::BuildUtil::cmakeOptions(dependency);
    EXPECT_EQ(options.at("BUILD_SHARED_LIBS"), "OFF");
    EXPECT_EQ(options.at("CMAKE_MSVC_RUNTIME_LIBRARY"), "MultiThreaded$<$<CONFIG:Debug>:Debug>");
    EXPECT_EQ(options.at("CMAKE_POSITION_INDEPENDENT_CODE"), "ON");
}

TEST(PackageBuilderUtilTests, CMakeOptionsOverrideAbstractions) {
    toolkit::DependencyDesc dependency;
    dependency.linkage = "static";
    dependency.runtime = "static";
    dependency.pic = "true";
    dependency.cmake.options["BUILD_SHARED_LIBS"] = "ON";
    dependency.cmake.options["CMAKE_MSVC_RUNTIME_LIBRARY"] = "MultiThreadedDLL";
    dependency.cmake.options["CMAKE_POSITION_INDEPENDENT_CODE"] = "OFF";

    const auto options = toolkit::BuildUtil::cmakeOptions(dependency);
    EXPECT_EQ(options.at("BUILD_SHARED_LIBS"), "ON");
    EXPECT_EQ(options.at("CMAKE_MSVC_RUNTIME_LIBRARY"), "MultiThreadedDLL");
    EXPECT_EQ(options.at("CMAKE_POSITION_INDEPENDENT_CODE"), "OFF");
}

TEST(PackageBuilderUtilTests, BuildsCMakeConfigureArgs) {
    toolkit::DependencyDesc dependency;
    dependency.name = "Pkg";
    dependency.cmake.generator = "Ninja";
    dependency.cmake.buildType = "Release";
    dependency.cmake.toolchainFile = "$(source)/toolchain.cmake";
    dependency.cmake.configureArgs.push_back("-DEXTRA=$(package)");

    toolkit::BuildUtil::Context context{
        "S:/Pkg",
        "B:/Pkg",
        "A:/Pkg",
        "Pkg",
        "Release",
        "windows",
        "x64",
    };

    const auto args = toolkit::BuildUtil::cmakeArgs(dependency, context);
    EXPECT_NE(std::find(args.begin(), args.end(), "-G"), args.end());
    EXPECT_NE(std::find(args.begin(), args.end(), "-DCMAKE_BUILD_TYPE=Release"), args.end());
    EXPECT_NE(std::find(args.begin(), args.end(), "-DCMAKE_TOOLCHAIN_FILE=S:/Pkg/toolchain.cmake"), args.end());
    EXPECT_NE(std::find(args.begin(), args.end(), "-DEXTRA=Pkg"), args.end());
}

TEST(PackageBuilderUtilTests, WrapsMakeCommandWithWorkingDirectoryEnvAndArgs) {
    const auto args = toolkit::BuildUtil::wrap(
        "S:/Pkg",
        {{"CC", "clang"}, {"PREFIX", "A:/Pkg"}},
        "make",
        {"-j8", "STATIC=1", "all"});

    ASSERT_GE(args.size(), 10u);
    EXPECT_EQ(args[0], "-E");
    EXPECT_EQ(args[1], "chdir");
    EXPECT_EQ(args[2], "S:/Pkg");
    EXPECT_NE(std::find(args.begin(), args.end(), "CC=clang"), args.end());
    EXPECT_NE(std::find(args.begin(), args.end(), "PREFIX=A:/Pkg"), args.end());
    EXPECT_NE(std::find(args.begin(), args.end(), "make"), args.end());
    EXPECT_NE(std::find(args.begin(), args.end(), "STATIC=1"), args.end());
}

TEST(PackageBuilderUtilTests, ExpandsCustomCommandVariables) {
    toolkit::BuildUtil::Context context{
        "S:/Pkg",
        "B:/Pkg",
        "A:/Pkg",
        "Pkg",
        "Debug",
        "windows",
        "x64",
    };
    context.cc = "clang-cl";
    context.archiver = "llvm-lib";
    EXPECT_EQ(
        toolkit::BuildUtil::expand("$(cc) install.py --prefix=$(artifact) --name=$(package) --cfg=$(config) --ar=$(archiver)", context),
        "clang-cl install.py --prefix=A:/Pkg --name=Pkg --cfg=Debug --ar=llvm-lib");
}

TEST(PackageBuilderUtilTests, CopiesArtifactEntriesAndKeepsExportPathsRelative) {
    const auto root = makeTempDir();
    const auto source = root / "source";
    const auto build = root / "build";
    const auto artifact = root / "artifact";
    writeText(source / "include/Pkg.hpp", "#pragma once\n");
    writeText(build / "lib/Pkg.lib", "");
    writeText(build / "bin/Pkg.dll", "");

    toolkit::BuildUtil::Context context{
        source,
        build,
        artifact,
        "Pkg",
        "Debug",
        "windows",
        "x64",
    };
    toolkit::DependencyArtifacts requested;
    requested.mode = "copy";
    requested.includeDirs.push_back("include");
    requested.libDirs.push_back("lib");
    requested.binDirs.push_back("bin");
    requested.libFiles.push_back("lib/Pkg.lib");

    toolkit::DependencyArtifacts exported;
    toolkit::JsonDiagnosticSink diagnostics;
    ASSERT_TRUE(toolkit::BuildUtil::materialize(context, requested, exported, diagnostics))
        << diagnostics.diagnostics().dump();

    EXPECT_TRUE(std::filesystem::exists(artifact / "include/Pkg.hpp"));
    EXPECT_TRUE(std::filesystem::exists(artifact / "lib/Pkg.lib"));
    EXPECT_TRUE(std::filesystem::exists(artifact / "bin/Pkg.dll"));
    EXPECT_EQ(exported.includeDirs, std::vector<std::string>{"include"});
    EXPECT_EQ(exported.libDirs, std::vector<std::string>{"lib"});
    EXPECT_EQ(exported.binDirs, std::vector<std::string>{"bin"});
    EXPECT_EQ(exported.libFiles, std::vector<std::string>{"lib/Pkg.lib"});

    std::filesystem::remove_all(root);
}

TEST(PackageBuilderUtilTests, AcceptsWindowsCoffLogicalLibraryName) {
    const auto root = makeTempDir();
    const auto artifact = root / "artifact";
    writeText(artifact / "include/lua.h", "#pragma once\n");
    writeText(artifact / "lib/lua_static.lib", "");

    toolkit::BuildUtil::Context context{
        root / "source",
        root / "build",
        artifact,
        "lua-static",
        "Release",
        "windows",
        "x64",
    };
    context.archiver = "D:/Sdks/llvm/bin/llvm-lib.exe";

    toolkit::DependencyArtifacts requested;
    requested.includeDirs.push_back("include");
    requested.libDirs.push_back("lib");
    requested.libs.push_back("lua_static");

    toolkit::DependencyArtifacts exported;
    toolkit::JsonDiagnosticSink diagnostics;
    ASSERT_TRUE(toolkit::BuildUtil::materialize(context, requested, exported, diagnostics))
        << diagnostics.diagnostics().dump();

    std::filesystem::remove_all(root);
}

TEST(PackageBuilderUtilTests, AcceptsWindowsMingwLogicalLibraryName) {
    const auto root = makeTempDir();
    const auto artifact = root / "artifact";
    writeText(artifact / "include/lua.h", "#pragma once\n");
    writeText(artifact / "lib/liblua.a", "");

    toolkit::BuildUtil::Context context{
        root / "source",
        root / "build",
        artifact,
        "lua",
        "Release",
        "windows",
        "x64",
    };
    context.archiver = "D:/Sdks/mingw64/bin/ar.exe";

    toolkit::DependencyArtifacts requested;
    requested.includeDirs.push_back("include");
    requested.libDirs.push_back("lib");
    requested.libs.push_back("lua");

    toolkit::DependencyArtifacts exported;
    toolkit::JsonDiagnosticSink diagnostics;
    ASSERT_TRUE(toolkit::BuildUtil::materialize(context, requested, exported, diagnostics))
        << diagnostics.diagnostics().dump();

    std::filesystem::remove_all(root);
}

TEST(PackageBuilderUtilTests, ReportsAttemptedLibraryCandidatesWhenMissing) {
    const auto root = makeTempDir();
    const auto artifact = root / "artifact";
    writeText(artifact / "include/lua.h", "#pragma once\n");
    std::filesystem::create_directories(artifact / "lib");

    toolkit::BuildUtil::Context context{
        root / "source",
        root / "build",
        artifact,
        "lua-static",
        "Release",
        "windows",
        "x64",
    };
    context.archiver = "D:/Sdks/llvm/bin/llvm-lib.exe";

    toolkit::DependencyArtifacts requested;
    requested.includeDirs.push_back("include");
    requested.libDirs.push_back("lib");
    requested.libs.push_back("lua_static");

    toolkit::DependencyArtifacts exported;
    toolkit::JsonDiagnosticSink diagnostics;
    ASSERT_FALSE(toolkit::BuildUtil::materialize(context, requested, exported, diagnostics));
    const auto text = diagnostics.diagnostics().dump();
    EXPECT_NE(text.find("lua_static.lib"), std::string::npos);
    EXPECT_NE(text.find("liblua_static.lib"), std::string::npos);

    std::filesystem::remove_all(root);
}

TEST(PackageBuilderUtilTests, CustomBuilderPreservesInstalledLibraryAndValidatesLogicalLibName) {
    const auto root = makeTempDir();
    const auto source = root / "source";
    const auto build = root / "build";
    const auto artifactDir = root / "artifact";
    std::filesystem::create_directories(source);

    toolkit::DependencyDesc dependency;
    dependency.name = "lua-static";
    dependency.buildType = "custom";
    dependency.artifacts.includeDirs.push_back("include");
    dependency.artifacts.libDirs.push_back("lib");
    dependency.artifacts.libs.push_back("lua_static");

#ifdef _WIN32
    dependency.commands.build.push_back(
        R"cmd(if not exist "$(artifact)\lib" mkdir "$(artifact)\lib" && echo lib>"$(artifact)\lib\lua_static.lib" && if not exist "$(artifact)\lib\lua_static.lib" exit /b 1)cmd");
    dependency.commands.install.push_back(
        R"cmd(if not exist "$(artifact)\include" mkdir "$(artifact)\include" && echo #pragma once>"$(artifact)\include\lua.h")cmd");
    const auto expectedLib = artifactDir / "lib/lua_static.lib";
#else
    dependency.commands.build.push_back(
        "mkdir -p '$(artifact)/lib' && : > '$(artifact)/lib/liblua_static.a' && test -f '$(artifact)/lib/liblua_static.a'");
    dependency.commands.install.push_back(
        "mkdir -p '$(artifact)/include' && printf '#pragma once\n' > '$(artifact)/include/lua.h'");
    const auto expectedLib = artifactDir / "lib/liblua_static.a";
#endif

    toolkit::PackageArtifact artifact;
    toolkit::JsonDiagnosticSink diagnostics;
    toolkit::CustomPackageBuilder builder;
    ASSERT_TRUE(builder.build(dependency, source, build, artifactDir, "local", root, {}, "debug", toolkit::PackageBuildOptions{}, artifact, diagnostics))
        << diagnostics.diagnostics().dump();
    ASSERT_TRUE(toolkit::ArtifactStore::write(artifact, diagnostics)) << diagnostics.diagnostics().dump();

    EXPECT_TRUE(std::filesystem::is_regular_file(expectedLib));
    EXPECT_TRUE(std::filesystem::is_regular_file(artifactDir / "artifact.toml"));

    toolkit::DependencyArtifacts metadata;
    ASSERT_TRUE(toolkit::ArtifactStore::read(artifactDir / "artifact.toml", metadata, diagnostics))
        << diagnostics.diagnostics().dump();

    toolkit::BuildUtil::Context context;
    context.artifactDir = artifactDir;
#ifdef _WIN32
    context.platform = "windows";
#else
    context.platform = "linux";
#endif
    ASSERT_TRUE(toolkit::BuildUtil::validate(context, metadata, diagnostics))
        << diagnostics.diagnostics().dump();

    std::filesystem::remove(expectedLib);
    ASSERT_FALSE(toolkit::BuildUtil::validate(context, metadata, diagnostics));
    const auto text = diagnostics.diagnostics().dump();
#ifdef _WIN32
    EXPECT_NE(text.find("lua_static.lib"), std::string::npos);
#else
    EXPECT_NE(text.find("liblua_static.a"), std::string::npos);
#endif

    std::filesystem::remove_all(root);
}

} // namespace
