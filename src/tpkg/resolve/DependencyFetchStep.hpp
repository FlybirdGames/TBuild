#pragma once

#include "tpkg/config/LockFile.hpp"
#include "tpkg/model/DependencyDesc.hpp"
#include "tpkg/resolve/DependencyResolver.hpp"

#include <filesystem>
#include <string>

namespace toolkit
{

    class DiagnosticSink;
    class GitFetcher;
    class PackageCache;

    class DepFetch
    {
    public:
        struct Context
        {
            const std::filesystem::path &workspaceRoot;
            PackageCache &cache;
            GitFetcher &git;
            const LockFile &lockFile;
            const std::string &effectiveConfig;
            const std::string &preferredToolchain;
            const DependencyRestoreOptions &options;
        };

        struct Result
        {
            std::filesystem::path sourceRoot;
            std::string commit;
            bool skipPackage = false;
        };

        static bool source(DependencyDesc &dependency,
                           LockedPackage *locked,
                           bool wantsUpdate,
                           const Context &context,
                           Result &result,
                           DiagnosticSink &diagnostics);

        static bool urlLike(const std::string &source);

    private:
        static std::filesystem::path local(const std::filesystem::path &workspaceRoot,
                                           const DependencyDesc &dependency);
        static std::filesystem::path reusable(const PackageCache &cache,
                                              const DependencyDesc &dependency,
                                              const std::filesystem::path &preferredPath);
        static void move(const std::filesystem::path &from,
                         const std::filesystem::path &to,
                         DiagnosticSink &diagnostics);
        static void removeSiblings(const PackageCache &cache,
                                   const DependencyDesc &dependency,
                                   const std::filesystem::path &keepPath,
                                   DiagnosticSink &diagnostics);
        static DependencyDesc forGit(const std::filesystem::path &workspaceRoot,
                                     const DependencyDesc &dependency);
    };

} // namespace toolkit
