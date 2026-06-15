/*
 * @Author: cubevlmu khfahqp@gmail.com
 * @LastEditors: cubevlmu khfahqp@gmail.com
 * Copyright (c) 2026 by FlybirdGames, All Rights Reserved. 
 */
#include "tbuild/config/LockFile.hpp"
#include "tbuild/diagnostics/JsonDiagnosticSink.hpp"
#include "tbuild/package/PackageArtifact.hpp"
#include "tbuild/package/PackageArtifactStore.hpp"
#include "tbuild/resolve/PackageArtifactResolver.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>

namespace
{

    std::filesystem::path makeTempDir()
    {
        const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
        auto path = std::filesystem::temp_directory_path() / ("tkb-artifact-resolve-" + std::to_string(stamp));
        std::filesystem::create_directories(path);
        return path;
    }

    void writeArtifact(const std::filesystem::path &root, const std::string &artifactId, const std::string &lib)
    {
        toolkit::PackageArtifact artifact;
        artifact.name = "Pkg";
        artifact.commit = "local";
        artifact.artifactId = artifactId;
        artifact.root = root / ".tbuild/artifacts" / artifactId;
        artifact.artifacts.libDirs.push_back("lib");
        artifact.artifacts.libs.push_back(lib);
        std::filesystem::create_directories(artifact.root / "lib");
#ifdef _WIN32
        std::ofstream(artifact.root / "lib" / (lib + ".lib")).put('\0');
#else
        std::ofstream(artifact.root / "lib" / ("lib" + lib + ".a")).put('\0');
#endif
        toolkit::JsonDiagnosticSink diagnostics;
        ASSERT_TRUE(toolkit::ArtifactStore::write(artifact, diagnostics)) << diagnostics.diagnostics().dump();
    }

    toolkit::LockedPackage lockedPackage(const std::string &artifactId,
                                         const std::string &config,
                                         const std::string &toolchain,
                                         bool overridden = false)
    {
        toolkit::LockedPackage locked;
        locked.name = "Pkg";
        locked.source = overridden ? "C:/override/Pkg" : "third_party/Pkg";
        locked.sourceType = overridden ? "local" : "local";
        locked.config = config;
        locked.toolchainId = toolchain;
        locked.artifactId = artifactId;
        locked.overridden = overridden;
        locked.overridePath = overridden ? "C:/override/Pkg" : "";
        return locked;
    }

    TEST(PackageArtifactResolverTests, ResolvesArtifactMetadataToPaths)
    {
        const auto root = makeTempDir();

        toolkit::PackageArtifact artifact;
        artifact.name = "Pkg";
        artifact.commit = "local";
        artifact.artifactId = "Pkg/local";
        artifact.root = root / ".tbuild/artifacts/Pkg/local";
        artifact.artifacts.includeDirs.push_back("include");
        artifact.artifacts.libDirs.push_back("lib");
        artifact.artifacts.binDirs.push_back("bin");
        artifact.artifacts.libs.push_back("Pkg");
        artifact.artifacts.defines.push_back("PKG=1");

        std::filesystem::create_directories(artifact.root / "include");
        std::filesystem::create_directories(artifact.root / "lib");
        std::filesystem::create_directories(artifact.root / "bin");
#ifdef _WIN32
        std::ofstream(artifact.root / "lib" / "Pkg.lib").put('\0');
#else
        std::ofstream(artifact.root / "lib" / "libPkg.a").put('\0');
#endif

        toolkit::JsonDiagnosticSink diagnostics;
        ASSERT_TRUE(toolkit::ArtifactStore::write(artifact, diagnostics)) << diagnostics.diagnostics().dump();

        toolkit::LockedPackage locked;
        locked.name = "Pkg";
        locked.artifactId = "Pkg/local";
        toolkit::LockFile lockFile;
        lockFile.packages.push_back(locked);

        toolkit::PackageArtifactResolver resolver;
        ASSERT_TRUE(resolver.load(lockFile, root, diagnostics)) << diagnostics.diagnostics().dump();

        const auto *resolved = resolver.find("Pkg");
        ASSERT_NE(resolved, nullptr);
        EXPECT_EQ(resolved->includeDirs[0], artifact.root / "include");
        EXPECT_EQ(resolved->libDirs[0], artifact.root / "lib");
        EXPECT_EQ(resolved->binDirs[0], artifact.root / "bin");
        EXPECT_EQ(resolved->libs[0], "Pkg");
        EXPECT_EQ(resolved->defines[0], "PKG=1");

        std::filesystem::remove_all(root);
    }

    TEST(PackageArtifactResolverTests, LoadsOnlyMatchingConfigToolchain)
    {
        const auto root = makeTempDir();
        writeArtifact(root, "Pkg/debug-win", "PkgDebug");
        writeArtifact(root, "Pkg/release-win", "PkgRelease");
        writeArtifact(root, "Pkg/debug-android", "PkgAndroid");

        toolkit::LockFile lockFile;
        lockFile.packages.push_back(lockedPackage("Pkg/release-win", "release", "windows-msvc-x64"));
        lockFile.packages.push_back(lockedPackage("Pkg/debug-android", "debug", "android-clang-arm64"));
        lockFile.packages.push_back(lockedPackage("Pkg/debug-win", "debug", "windows-msvc-x64"));

        toolkit::JsonDiagnosticSink diagnostics;
        toolkit::PackageArtifactResolver resolver;
        ASSERT_TRUE(resolver.load(lockFile, root, "debug", "windows-msvc-x64", toolkit::DependencyOverrideSet{}, diagnostics)) << diagnostics.diagnostics().dump();
        const auto *resolved = resolver.find("Pkg");
        ASSERT_NE(resolved, nullptr);
        ASSERT_EQ(resolved->libs.size(), 1u);
        EXPECT_EQ(resolved->libs[0], "PkgDebug");

        std::filesystem::remove_all(root);
    }

    TEST(PackageArtifactResolverTests, LoadsOverrideArtifactOnlyWhenOverrideEnabled)
    {
        const auto root = makeTempDir();
        writeArtifact(root, "Pkg/normal", "PkgNormal");
        writeArtifact(root, "Pkg/override", "PkgOverride");

        toolkit::LockFile lockFile;
        lockFile.packages.push_back(lockedPackage("Pkg/normal", "debug", "windows-msvc-x64"));
        lockFile.packages.push_back(lockedPackage("Pkg/override", "debug", "windows-msvc-x64", true));

        toolkit::JsonDiagnosticSink diagnostics;
        toolkit::PackageArtifactResolver resolver;
        ASSERT_TRUE(resolver.load(lockFile, root, "debug", "windows-msvc-x64", toolkit::DependencyOverrideSet{}, diagnostics)) << diagnostics.diagnostics().dump();
        ASSERT_NE(resolver.find("Pkg"), nullptr);
        EXPECT_EQ(resolver.find("Pkg")->libs[0], "PkgNormal");

        toolkit::DependencyOverrideSet overrides;
        overrides.overrides["Pkg"].path = "C:/override/Pkg";
        ASSERT_TRUE(resolver.load(lockFile, root, "debug", "windows-msvc-x64", overrides, diagnostics)) << diagnostics.diagnostics().dump();
        ASSERT_NE(resolver.find("Pkg"), nullptr);
        EXPECT_EQ(resolver.find("Pkg")->libs[0], "PkgOverride");

        std::filesystem::remove_all(root);
    }

} // namespace
