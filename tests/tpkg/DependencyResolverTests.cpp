#include "tpkg/config/LockFile.hpp"
#include "tpkg/core/FileSystem.hpp"
#include "tpkg/core/Process.hpp"
#include "tpkg/diagnostics/JsonDiagnosticSink.hpp"
#include "tpkg/model/BuildModel.hpp"
#include "tpkg/package/PackageCache.hpp"
#include "tpkg/resolve/DependencyResolver.hpp"
#include "tpkg/utils/Sha256.hpp"

#include <archive.h>
#include <archive_entry.h>
#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace
{

    std::filesystem::path makeTempDir()
    {
        const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
        auto path = std::filesystem::temp_directory_path() / ("tkb-resolve-" + std::to_string(stamp));
        std::filesystem::create_directories(path);
        return path;
    }

    void writeText(const std::filesystem::path &path, const std::string &content)
    {
        std::filesystem::create_directories(path.parent_path());
        std::ofstream out(path, std::ios::binary | std::ios::trunc);
        out << content;
    }

    bool runRequiredProcess(const std::string &executable, const std::vector<std::string> &args, std::string &output)
    {
        auto result = toolkit::Process::run(executable, args);
        output = result.output;
        return result.exitCode == 0;
    }

    bool directoryHasFiles(const std::filesystem::path &path)
    {
        if (!std::filesystem::is_directory(path))
        {
            return false;
        }
        return std::filesystem::directory_iterator(path) != std::filesystem::directory_iterator();
    }

    std::size_t directoryCount(const std::filesystem::path &path)
    {
        if (!std::filesystem::is_directory(path))
        {
            return 0;
        }
        std::size_t count = 0;
        for (const auto &entry : std::filesystem::directory_iterator(path))
        {
            if (entry.is_directory())
            {
                ++count;
            }
        }
        return count;
    }

    toolkit::BuildModel localHeaderOnlyModel(const std::string &name = "HeaderOnly")
    {
        toolkit::BuildModel model;
        toolkit::DependencyDesc dependency;
        dependency.name = name;
        dependency.source = "third_party/" + name;
        dependency.sourceType = "local";
        dependency.local = true;
        dependency.buildType = "header_only";
        dependency.artifacts.includeDirs.push_back("include");
        model.rootPackage.dependencies.push_back(dependency);
        return model;
    }

    void createZipEntry(struct archive *zip, const std::string &name, const std::string &content)
    {
        struct archive_entry *entry = archive_entry_new();
        archive_entry_set_pathname(entry, name.c_str());
        archive_entry_set_filetype(entry, AE_IFREG);
        archive_entry_set_perm(entry, 0644);
        archive_entry_set_size(entry, static_cast<la_int64_t>(content.size()));
        ASSERT_EQ(archive_write_header(zip, entry), ARCHIVE_OK);
        ASSERT_EQ(archive_write_data(zip, content.data(), content.size()), static_cast<la_ssize_t>(content.size()));
        archive_entry_free(entry);
    }

    void createHelloZip(const std::filesystem::path &path)
    {
        struct archive *zip = archive_write_new();
        archive_write_set_format_zip(zip);
        ASSERT_EQ(archive_write_open_filename(zip, path.string().c_str()), ARCHIVE_OK);
        createZipEntry(zip, "hello-lib-1.0/include/hello/hello.hpp", "#pragma once\n");
        archive_write_close(zip);
        archive_write_free(zip);
    }

    TEST(DependencyResolverTests, RestoresLocalHeaderOnlyAndPrebuiltPackages)
    {
        const auto dir = makeTempDir();
        std::filesystem::create_directories(dir / "third_party/HeaderOnly/include");
        std::filesystem::create_directories(dir / "third_party/Prebuilt/include");
        std::filesystem::create_directories(dir / "third_party/Prebuilt/lib");
        std::filesystem::create_directories(dir / "third_party/Prebuilt/bin");
        writeText(dir / "third_party/Prebuilt/lib/Prebuilt.lib", "");

        toolkit::BuildModel model;
        toolkit::DependencyDesc headerOnly;
        headerOnly.name = "HeaderOnly";
        headerOnly.source = "third_party/HeaderOnly";
        headerOnly.sourceType = "local";
        headerOnly.local = true;
        headerOnly.buildType = "header_only";
        headerOnly.artifacts.includeDirs.push_back("include");
        model.rootPackage.dependencies.push_back(headerOnly);

        toolkit::DependencyDesc prebuilt;
        prebuilt.name = "Prebuilt";
        prebuilt.source = "third_party/Prebuilt";
        prebuilt.sourceType = "local";
        prebuilt.local = true;
        prebuilt.buildType = "prebuilt";
        prebuilt.artifacts.includeDirs.push_back("include");
        prebuilt.artifacts.libDirs.push_back("lib");
        prebuilt.artifacts.binDirs.push_back("bin");
        prebuilt.artifacts.libs.push_back("Prebuilt");
        model.rootPackage.dependencies.push_back(prebuilt);

        toolkit::JsonDiagnosticSink diagnostics;
        toolkit::DependencyResolver resolver;
        ASSERT_TRUE(resolver.restore(model, dir, diagnostics)) << diagnostics.diagnostics().dump();

        EXPECT_TRUE(toolkit::File::exists(dir / "tpkg.lock.toml"));
        toolkit::LockFile lockFile;
        ASSERT_TRUE(toolkit::LockFile::read(dir / "tpkg.lock.toml", lockFile, diagnostics)) << diagnostics.diagnostics().dump();
        ASSERT_EQ(lockFile.packages.size(), 2u);
        for (const auto &locked : lockFile.packages)
        {
            EXPECT_FALSE(locked.buildHash.empty());
            EXPECT_EQ(locked.artifactId, locked.name + "/" + locked.commit + "-" + locked.buildHash);
            EXPECT_TRUE(toolkit::File::exists(dir / ".tpkg/artifacts" / locked.artifactId / "artifact.toml"));
        }
        EXPECT_EQ(toolkit::File::read(dir / "tpkg.lock.toml").find("E:"), std::string::npos);
        EXPECT_EQ(toolkit::File::read(dir / "tpkg.lock.toml").find("cache_path"), std::string::npos);

        std::filesystem::remove_all(dir);
    }

    TEST(DependencyResolverTests, RestoresLocalGitCMakePackageByCommit)
    {
        const auto dir = makeTempDir();
        const auto repo = dir / "TinyCMakeLib";
        writeText(repo / "CMakeLists.txt", R"cmake(
cmake_minimum_required(VERSION 3.20)
project(TinyCMakeLib LANGUAGES CXX)
add_library(TinyCMakeLib STATIC src/Tiny.cpp)
target_include_directories(TinyCMakeLib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
install(TARGETS TinyCMakeLib
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
install(DIRECTORY include/ DESTINATION include)
)cmake");
        writeText(repo / "include/tiny/Tiny.hpp", R"cpp(
#pragma once
namespace tiny {
int value();
}
)cpp");
        writeText(repo / "src/Tiny.cpp", R"cpp(
#include <tiny/Tiny.hpp>
namespace tiny {
int value() {
    return 42;
}
}
)cpp");

        std::string processOutput;
        ASSERT_TRUE(runRequiredProcess("git", {"-C", repo.string(), "init"}, processOutput)) << processOutput;
        ASSERT_TRUE(runRequiredProcess("git", {"-C", repo.string(), "checkout", "-B", "master"}, processOutput)) << processOutput;
        ASSERT_TRUE(runRequiredProcess("git", {"-C", repo.string(), "config", "user.email", "tiny@example.invalid"}, processOutput)) << processOutput;
        ASSERT_TRUE(runRequiredProcess("git", {"-C", repo.string(), "config", "user.name", "Tiny Test"}, processOutput)) << processOutput;
        ASSERT_TRUE(runRequiredProcess("git", {"-C", repo.string(), "add", "."}, processOutput)) << processOutput;
        ASSERT_TRUE(runRequiredProcess("git", {"-C", repo.string(), "commit", "-m", "init tiny cmake lib"}, processOutput)) << processOutput;

        toolkit::BuildModel model;
        toolkit::DependencyDesc dependency;
        dependency.name = "TinyCMakeLib";
        dependency.source = "TinyCMakeLib";
        dependency.sourceType = "git";
        dependency.ref = "master";
        dependency.subdir = ".";
        dependency.buildType = "cmake";
        dependency.artifacts.includeDirs.push_back("include");
        dependency.artifacts.libDirs.push_back("lib");
        dependency.artifacts.libs.push_back("TinyCMakeLib");
        model.rootPackage.dependencies.push_back(dependency);

        toolkit::JsonDiagnosticSink diagnostics;
        toolkit::DependencyResolver resolver;
        ASSERT_TRUE(resolver.restore(model, dir, diagnostics)) << diagnostics.diagnostics().dump();

        toolkit::LockFile lockFile;
        ASSERT_TRUE(toolkit::LockFile::read(dir / "tpkg.lock.toml", lockFile, diagnostics)) << diagnostics.diagnostics().dump();
        ASSERT_EQ(lockFile.packages.size(), 1u);
        const auto &locked = lockFile.packages[0];
        EXPECT_EQ(locked.name, "TinyCMakeLib");
        EXPECT_EQ(locked.ref, "master");
        EXPECT_NE(locked.commit, "master");
        EXPECT_EQ(locked.commit.size(), 40u);
        EXPECT_FALSE(locked.buildHash.empty());
        EXPECT_EQ(locked.artifactId, "TinyCMakeLib/" + locked.commit + "-" + locked.buildHash);

        toolkit::PackageCache cache(dir / ".tpkg/packages");
        EXPECT_TRUE(toolkit::File::dir(cache.source(dependency)));
        EXPECT_FALSE(toolkit::File::dir(cache.path(dependency, locked.commit)));
        EXPECT_TRUE(toolkit::File::dir(cache.build(dependency, "debug-default")));
        const auto artifactPath = dir / ".tpkg" / "artifacts" / locked.artifactId;
        EXPECT_TRUE(toolkit::File::dir(artifactPath / "include"));
        EXPECT_TRUE(toolkit::File::dir(artifactPath / "lib"));
        EXPECT_TRUE(directoryHasFiles(artifactPath / "lib"));
        EXPECT_TRUE(toolkit::File::exists(artifactPath / "artifact.toml"));

        const auto lockText = toolkit::File::read(dir / "tpkg.lock.toml");
        EXPECT_EQ(lockText.find("cache_path"), std::string::npos);
        EXPECT_EQ(lockText.find(dir.generic_string()), std::string::npos);

        toolkit::JsonDiagnosticSink secondDiagnostics;
        ASSERT_TRUE(resolver.restore(model, dir, secondDiagnostics)) << secondDiagnostics.diagnostics().dump();
        EXPECT_EQ(toolkit::File::read(dir / "tpkg.lock.toml"), lockText);
        const auto secondLog = secondDiagnostics.diagnostics().dump();
        EXPECT_NE(secondLog.find("skipping git sync and build"), std::string::npos);
        EXPECT_EQ(secondLog.find("fetching package"), std::string::npos);
        EXPECT_EQ(secondLog.find("cloning package"), std::string::npos);

        std::filesystem::remove_all(dir);
    }

    TEST(DependencyResolverTests, FailedGitPkgReusesSourceCheckoutOnNextRestore)
    {
        const auto dir = makeTempDir();
        const auto repo = dir / "BrokenPrebuilt";
        writeText(repo / "README.md", "broken prebuilt\n");

        std::string processOutput;
        ASSERT_TRUE(runRequiredProcess("git", {"-C", repo.string(), "init"}, processOutput)) << processOutput;
        ASSERT_TRUE(runRequiredProcess("git", {"-C", repo.string(), "checkout", "-B", "master"}, processOutput)) << processOutput;
        ASSERT_TRUE(runRequiredProcess("git", {"-C", repo.string(), "config", "user.email", "broken@example.invalid"}, processOutput)) << processOutput;
        ASSERT_TRUE(runRequiredProcess("git", {"-C", repo.string(), "config", "user.name", "Broken Test"}, processOutput)) << processOutput;
        ASSERT_TRUE(runRequiredProcess("git", {"-C", repo.string(), "add", "."}, processOutput)) << processOutput;
        ASSERT_TRUE(runRequiredProcess("git", {"-C", repo.string(), "commit", "-m", "init broken prebuilt"}, processOutput)) << processOutput;

        toolkit::BuildModel model;
        toolkit::DependencyDesc dependency;
        dependency.name = "BrokenPrebuilt";
        dependency.source = "BrokenPrebuilt";
        dependency.sourceType = "git";
        dependency.ref = "master";
        dependency.buildType = "prebuilt";
        dependency.artifacts.includeDirs.push_back("missing-include");
        model.rootPackage.dependencies.push_back(dependency);

        toolkit::DependencyResolver resolver;
        toolkit::JsonDiagnosticSink firstDiagnostics;
        ASSERT_FALSE(resolver.restore(model, dir, firstDiagnostics, "windows-msvc-x64", "debug"));

        toolkit::PackageCache cache(dir / ".tpkg/packages");
        const auto sourcePath = cache.source(dependency);
        EXPECT_TRUE(toolkit::File::dir(sourcePath / ".git"));
        EXPECT_EQ(directoryCount(cache.sourceRoot(dependency)), 1u);

        const auto buildDir = cache.build(dependency, "debug-windows-msvc-x64");
        writeText(buildDir / "stale.txt", "stale build cache\n");

        toolkit::JsonDiagnosticSink secondDiagnostics;
        ASSERT_FALSE(resolver.restore(model, dir, secondDiagnostics, "windows-msvc-x64", "debug"));
        EXPECT_TRUE(toolkit::File::dir(sourcePath / ".git"));
        EXPECT_EQ(directoryCount(cache.sourceRoot(dependency)), 1u);
        EXPECT_EQ(secondDiagnostics.diagnostics().dump().find("cloning package"), std::string::npos);
        EXPECT_TRUE(toolkit::File::exists(buildDir / "stale.txt"));

        toolkit::DependencyRestoreOptions rebuild;
        rebuild.rebuild = true;
        toolkit::JsonDiagnosticSink rebuildDiagnostics;
        ASSERT_FALSE(resolver.restore(model, dir, rebuildDiagnostics, "windows-msvc-x64", "debug", {}, rebuild));
        EXPECT_FALSE(toolkit::File::exists(buildDir / "stale.txt"));

        std::filesystem::remove_all(dir);
    }

    TEST(DependencyResolverTests, ArchiveDependencyRestoresHeaderOnlyPackage)
    {
        const auto dir = makeTempDir();
        std::filesystem::create_directories(dir / "third_party");
        const auto archivePath = dir / "third_party" / "hello-lib.zip";
        createHelloZip(archivePath);

        toolkit::BuildModel model;
        toolkit::DependencyDesc dependency;
        dependency.name = "hello-lib";
        dependency.source = "third_party/hello-lib.zip";
        dependency.sourceType = "archive";
        dependency.sha256 = toolkit::sha256File(archivePath);
        dependency.stripComponents = 1;
        dependency.buildType = "header_only";
        dependency.artifacts.includeDirs.push_back("include");
        model.rootPackage.dependencies.push_back(dependency);

        toolkit::JsonDiagnosticSink diagnostics;
        toolkit::DependencyResolver resolver;
        ASSERT_TRUE(resolver.restore(model, dir, diagnostics, "windows-msvc-x64", "debug")) << diagnostics.diagnostics().dump();

        toolkit::LockFile lockFile;
        ASSERT_TRUE(toolkit::LockFile::read(dir / "tpkg.lock.toml", lockFile, diagnostics)) << diagnostics.diagnostics().dump();
        ASSERT_EQ(lockFile.packages.size(), 1u);
        const auto &locked = lockFile.packages[0];
        EXPECT_EQ(locked.sourceType, "archive");
        EXPECT_EQ(locked.sha256, dependency.sha256);
        EXPECT_EQ(locked.resolvedArchiveHash, dependency.sha256);
        EXPECT_EQ(locked.stripComponents, 1);
        EXPECT_FALSE(locked.buildHash.empty());
        EXPECT_TRUE(toolkit::File::exists(dir / ".tpkg" / "artifacts" / locked.artifactId / "include" / "hello" / "hello.hpp"));

        std::filesystem::remove_all(dir);
    }

    TEST(DependencyResolverTests, RestoreDebugReleaseDoesNotOverwrite)
    {
        const auto dir = makeTempDir();
        std::filesystem::create_directories(dir / "third_party/HeaderOnly/include");

        toolkit::JsonDiagnosticSink diagnostics;
        toolkit::DependencyResolver resolver;
        const auto model = localHeaderOnlyModel();
        ASSERT_TRUE(resolver.restore(model, dir, diagnostics, "windows-msvc-x64", "debug")) << diagnostics.diagnostics().dump();
        ASSERT_TRUE(resolver.restore(model, dir, diagnostics, "windows-msvc-x64", "release")) << diagnostics.diagnostics().dump();

        toolkit::LockFile lockFile;
        ASSERT_TRUE(toolkit::LockFile::read(dir / "tpkg.lock.toml", lockFile, diagnostics)) << diagnostics.diagnostics().dump();
        ASSERT_EQ(lockFile.packages.size(), 2u);
        EXPECT_NE(lockFile.packages[0].config, lockFile.packages[1].config);
        EXPECT_NE(lockFile.packages[0].artifactId, lockFile.packages[1].artifactId);

        std::filesystem::remove_all(dir);
    }

    TEST(DependencyResolverTests, RestoreDifferentToolchainsDoesNotOverwrite)
    {
        const auto dir = makeTempDir();
        std::filesystem::create_directories(dir / "third_party/HeaderOnly/include");

        toolkit::JsonDiagnosticSink diagnostics;
        toolkit::DependencyResolver resolver;
        const auto model = localHeaderOnlyModel();
        ASSERT_TRUE(resolver.restore(model, dir, diagnostics, "windows-msvc-x64", "debug")) << diagnostics.diagnostics().dump();
        ASSERT_TRUE(resolver.restore(model, dir, diagnostics, "android-clang-arm64", "debug")) << diagnostics.diagnostics().dump();

        toolkit::LockFile lockFile;
        ASSERT_TRUE(toolkit::LockFile::read(dir / "tpkg.lock.toml", lockFile, diagnostics)) << diagnostics.diagnostics().dump();
        ASSERT_EQ(lockFile.packages.size(), 2u);
        EXPECT_NE(lockFile.packages[0].toolchainId, lockFile.packages[1].toolchainId);
        EXPECT_NE(lockFile.packages[0].artifactId, lockFile.packages[1].artifactId);

        std::filesystem::remove_all(dir);
    }

    TEST(DependencyResolverTests, RestoreOverrideAndNormalCanCoexist)
    {
        const auto dir = makeTempDir();
        std::filesystem::create_directories(dir / "third_party/HeaderOnly/include");
        std::filesystem::create_directories(dir / "override/HeaderOnly/include");

        toolkit::JsonDiagnosticSink diagnostics;
        toolkit::DependencyResolver resolver;
        const auto model = localHeaderOnlyModel();
        ASSERT_TRUE(resolver.restore(model, dir, diagnostics, "windows-msvc-x64", "debug")) << diagnostics.diagnostics().dump();

        toolkit::DependencyOverrideSet overrides;
        overrides.overrides["HeaderOnly"].path = dir / "override/HeaderOnly";
        ASSERT_TRUE(resolver.restore(model, dir, diagnostics, "windows-msvc-x64", "debug", overrides)) << diagnostics.diagnostics().dump();

        toolkit::LockFile lockFile;
        ASSERT_TRUE(toolkit::LockFile::read(dir / "tpkg.lock.toml", lockFile, diagnostics)) << diagnostics.diagnostics().dump();
        ASSERT_EQ(lockFile.packages.size(), 2u);
        EXPECT_NE(lockFile.packages[0].overridden, lockFile.packages[1].overridden);
        EXPECT_NE(lockFile.packages[0].artifactId, lockFile.packages[1].artifactId);

        std::filesystem::remove_all(dir);
    }

    TEST(DependencyResolverTests, RestoreLockedMissingEntryFails)
    {
        const auto dir = makeTempDir();
        auto model = localHeaderOnlyModel();
        std::filesystem::create_directories(dir / "third_party" / "HeaderOnly" / "include");

        toolkit::JsonDiagnosticSink diagnostics;
        toolkit::DependencyResolver resolver;
        toolkit::DependencyRestoreOptions options;
        options.locked = true;
        EXPECT_FALSE(resolver.restore(model, dir, diagnostics, "windows-msvc-x64", "debug", {}, options));
        EXPECT_NE(diagnostics.diagnostics().dump().find("restore --locked requires an existing lock entry"), std::string::npos);

        std::filesystem::remove_all(dir);
    }

    TEST(DependencyResolverTests, RestoreLockedDoesNotRewriteLockfile)
    {
        const auto dir = makeTempDir();
        auto model = localHeaderOnlyModel();
        std::filesystem::create_directories(dir / "third_party" / "HeaderOnly" / "include");

        toolkit::JsonDiagnosticSink diagnostics;
        toolkit::DependencyResolver resolver;
        toolkit::DependencyRestoreOptions options;
        options.toolchainContentHash = "toolchain-a";
        ASSERT_TRUE(resolver.restore(model, dir, diagnostics, "windows-msvc-x64", "debug", {}, options)) << diagnostics.diagnostics().dump();
        const auto before = toolkit::File::read(dir / "tpkg.lock.toml");

        toolkit::DependencyRestoreOptions locked;
        locked.locked = true;
        locked.toolchainContentHash = "toolchain-a";
        ASSERT_TRUE(resolver.restore(model, dir, diagnostics, "windows-msvc-x64", "debug", {}, locked)) << diagnostics.diagnostics().dump();
        EXPECT_EQ(toolkit::File::read(dir / "tpkg.lock.toml"), before);

        std::filesystem::remove_all(dir);
    }

    TEST(DependencyResolverTests, UpdatePackageOnlyUpdatesSpecifiedPackage)
    {
        const auto dir = makeTempDir();
        auto model = localHeaderOnlyModel("A");
        auto second = model.rootPackage.dependencies[0];
        second.name = "B";
        second.source = "third_party/B";
        model.rootPackage.dependencies.push_back(second);
        std::filesystem::create_directories(dir / "third_party" / "A" / "include");
        std::filesystem::create_directories(dir / "third_party" / "B" / "include");

        toolkit::JsonDiagnosticSink diagnostics;
        toolkit::DependencyResolver resolver;
        toolkit::DependencyRestoreOptions firstOptions;
        firstOptions.toolchainContentHash = "toolchain-a";
        ASSERT_TRUE(resolver.restore(model, dir, diagnostics, "windows-msvc-x64", "debug", {}, firstOptions)) << diagnostics.diagnostics().dump();

        toolkit::LockFile before;
        ASSERT_TRUE(toolkit::LockFile::read(dir / "tpkg.lock.toml", before, diagnostics)) << diagnostics.diagnostics().dump();
        ASSERT_EQ(before.packages.size(), 2u);

        toolkit::DependencyRestoreOptions updateOptions;
        updateOptions.updatePackage = "A";
        updateOptions.toolchainContentHash = "toolchain-b";
        ASSERT_TRUE(resolver.restore(model, dir, diagnostics, "windows-msvc-x64", "debug", {}, updateOptions)) << diagnostics.diagnostics().dump();

        toolkit::LockFile after;
        ASSERT_TRUE(toolkit::LockFile::read(dir / "tpkg.lock.toml", after, diagnostics)) << diagnostics.diagnostics().dump();
        ASSERT_EQ(after.packages.size(), 2u);
        const auto findHash = [](const toolkit::LockFile &lock, const std::string &name) {
            for (const auto &package : lock.packages)
            {
                if (package.name == name)
                {
                    return package.buildHash;
                }
            }
            return std::string{};
        };
        EXPECT_NE(findHash(before, "A"), findHash(after, "A"));
        EXPECT_EQ(findHash(before, "B"), findHash(after, "B"));

        std::filesystem::remove_all(dir);
    }

} // namespace
