#include "tpkg/core/FileSystem.hpp"
#include "tpkg/core/Hash.hpp"
#include "tpkg/diagnostics/JsonDiagnosticSink.hpp"
#include "tpkg/package/ArtifactGc.hpp"
#include "tpkg/package/PackageCache.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>

namespace
{
    std::filesystem::path makeGcTempDir()
    {
        const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
        auto path = std::filesystem::temp_directory_path() / ("tkb-artifact-gc-" + std::to_string(stamp));
        std::filesystem::create_directories(path);
        return path;
    }

    void makeArtifactDir(const std::filesystem::path &root, const std::string &artifactId)
    {
        std::filesystem::create_directories(root / ".tpkg" / "artifacts" / artifactId);
    }

    void makeBuildCacheDir(const std::filesystem::path &root, const std::string &package, const std::string &cache)
    {
        std::filesystem::create_directories(root / ".tpkg" / "build-packages" / package / cache);
    }

    toolkit::LockedPackage lockedPackage(const std::string &name)
    {
        toolkit::LockedPackage package;
        package.name = name;
        package.artifactId = name + "/live";
        package.sourceType = "git";
        package.source = "https://example.invalid/" + name + ".git";
        package.ref = "v1";
        package.config = "debug";
        package.toolchainId = "windows-msvc-x64";
        return package;
    }

    TEST(ArtifactGcTests, DryRunDoesNotDeleteUnreferencedArtifact)
    {
        const auto root = makeGcTempDir();
        makeArtifactDir(root, "fmt/live");
        makeArtifactDir(root, "fmt/stale");

        toolkit::LockFile lockFile;
        toolkit::LockedPackage package;
        package.name = "fmt";
        package.artifactId = "fmt/live";
        lockFile.packages.push_back(package);

        toolkit::JsonDiagnosticSink diagnostics;
        ASSERT_TRUE(toolkit::garbageCollectArtifacts(lockFile, root, false, {}, diagnostics)) << diagnostics.diagnostics().dump();
        EXPECT_TRUE(toolkit::File::dir(root / ".tpkg" / "artifacts" / "fmt" / "stale"));

        std::filesystem::remove_all(root);
    }

    TEST(ArtifactGcTests, ApplyDeletesUnreferencedArtifact)
    {
        const auto root = makeGcTempDir();
        makeArtifactDir(root, "fmt/live");
        makeArtifactDir(root, "fmt/stale");

        toolkit::LockFile lockFile;
        toolkit::LockedPackage package;
        package.name = "fmt";
        package.artifactId = "fmt/live";
        lockFile.packages.push_back(package);

        toolkit::JsonDiagnosticSink diagnostics;
        ASSERT_TRUE(toolkit::garbageCollectArtifacts(lockFile, root, true, "fmt", diagnostics)) << diagnostics.diagnostics().dump();
        EXPECT_TRUE(toolkit::File::dir(root / ".tpkg" / "artifacts" / "fmt" / "live"));
        EXPECT_FALSE(toolkit::File::dir(root / ".tpkg" / "artifacts" / "fmt" / "stale"));

        std::filesystem::remove_all(root);
    }

    TEST(ArtifactGcTests, ApplyDeletesStaleBuildCaches)
    {
        const auto root = makeGcTempDir();
        makeBuildCacheDir(root, "fmt", "debug-windows-msvc-x64");
        makeBuildCacheDir(root, "fmt", "release-windows-msvc-x64");
        makeBuildCacheDir(root, "old", "debug-windows-msvc-x64");

        toolkit::LockFile lockFile;
        lockFile.packages.push_back(lockedPackage("fmt"));

        toolkit::JsonDiagnosticSink diagnostics;
        ASSERT_TRUE(toolkit::garbageCollectPkgCaches(lockFile, root, "debug", "windows-msvc-x64", true, {}, diagnostics)) << diagnostics.diagnostics().dump();
        EXPECT_TRUE(toolkit::File::dir(root / ".tpkg" / "build-packages" / "fmt" / "debug-windows-msvc-x64"));
        EXPECT_FALSE(toolkit::File::dir(root / ".tpkg" / "build-packages" / "fmt" / "release-windows-msvc-x64"));
        EXPECT_FALSE(toolkit::File::dir(root / ".tpkg" / "build-packages" / "old" / "debug-windows-msvc-x64"));

        std::filesystem::remove_all(root);
    }

    TEST(ArtifactGcTests, DryRunDoesNotDeleteStaleBuildCaches)
    {
        const auto root = makeGcTempDir();
        makeBuildCacheDir(root, "fmt", "debug-windows-msvc-x64");
        makeBuildCacheDir(root, "fmt", "release-windows-msvc-x64");

        toolkit::LockFile lockFile;
        lockFile.packages.push_back(lockedPackage("fmt"));

        toolkit::JsonDiagnosticSink diagnostics;
        ASSERT_TRUE(toolkit::garbageCollectPkgCaches(lockFile, root, "debug", "windows-msvc-x64", false, {}, diagnostics)) << diagnostics.diagnostics().dump();
        EXPECT_TRUE(toolkit::File::dir(root / ".tpkg" / "build-packages" / "fmt" / "release-windows-msvc-x64"));

        std::filesystem::remove_all(root);
    }

    TEST(ArtifactGcTests, ApplyDeletesStaleGitSourceCachesForPackage)
    {
        const auto root = makeGcTempDir();
        toolkit::LockFile lockFile;
        auto package = lockedPackage("fmt");
        package.ref = "v2";
        lockFile.packages.push_back(package);

        toolkit::DependencyDesc dependency;
        dependency.name = package.name;
        dependency.source = package.source;
        dependency.ref = package.ref;
        toolkit::PackageCache cache(root / ".tpkg" / "packages");
        std::filesystem::create_directories(cache.source(dependency) / ".git");
        std::filesystem::create_directories(cache.sourceRoot(dependency) / "v1" / ".git");

        toolkit::JsonDiagnosticSink diagnostics;
        ASSERT_TRUE(toolkit::garbageCollectSourceCaches(lockFile, root, true, "fmt", diagnostics)) << diagnostics.diagnostics().dump();
        EXPECT_TRUE(toolkit::File::dir(cache.source(dependency)));
        EXPECT_FALSE(toolkit::File::dir(cache.sourceRoot(dependency) / "v1"));

        std::filesystem::remove_all(root);
    }

    TEST(ArtifactGcTests, ApplyDeletesStaleArchiveSourceCachesForPackage)
    {
        const auto root = makeGcTempDir();
        toolkit::LockFile lockFile;
        toolkit::LockedPackage package;
        package.name = "zlib";
        package.sourceType = "archive";
        package.source = "https://example.invalid/zlib.tar.gz";
        package.resolvedArchiveHash = toolkit::Hash::xxhash64Hex("live");
        package.artifactId = "zlib/live";
        lockFile.packages.push_back(package);

        const auto archiveRoot = root / ".tpkg" / "packages" / "_archives" / "zlib";
        const auto liveCacheKey = toolkit::Hash::xxhash64Hex(package.source);
        std::filesystem::create_directories(archiveRoot / liveCacheKey / "src");
        std::filesystem::create_directories(archiveRoot / toolkit::Hash::xxhash64Hex("stale") / "src");

        toolkit::JsonDiagnosticSink diagnostics;
        ASSERT_TRUE(toolkit::garbageCollectSourceCaches(lockFile, root, true, "zlib", diagnostics)) << diagnostics.diagnostics().dump();
        EXPECT_TRUE(toolkit::File::dir(archiveRoot / liveCacheKey));
        EXPECT_FALSE(toolkit::File::dir(archiveRoot / toolkit::Hash::xxhash64Hex("stale")));

        std::filesystem::remove_all(root);
    }
}
