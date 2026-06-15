#include "tbuild/cli/Commands.hpp"

#include "tbuild/cmake/CMakeDependencyExporter.hpp"
#include "tbuild/config/LockFile.hpp"
#include "tbuild/core/FileSystem.hpp"
#include "tbuild/resolve/PackageArtifactResolver.hpp"

#include <nlohmann/json.hpp>

#include <filesystem>
#include <string>
#include <vector>

namespace toolkit
{
    bool loadRestoredPackagesForCommand(const std::filesystem::path &workspaceRoot,
                                        const std::string &toolchainId,
                                        const std::string &configOption,
                                        std::vector<ResolvedDependencyArtifact> &packages,
                                        CMakeDependencyExportMetadata *metadata,
                                        ConsoleDiagnosticSink &diagnostics,
                                        const std::vector<std::string> &cliOverrides,
                                        BuildModel *loadedModel)
    {
        BuildModel model;
        if (!CliWorkspace::load(workspaceRoot, model, diagnostics))
        {
            return false;
        }
        const auto config = resolveCommandConfigWithState(model, workspaceRoot, configOption, diagnostics);
        if (config.empty())
        {
            return false;
        }
        const auto effectiveToolchain = preferredToolchainForCommand(workspaceRoot, toolchainId, diagnostics);
        if (effectiveToolchain.empty())
        {
            return false;
        }
        ToolchainProfile selectedToolchain;
        if (!selectToolchainForRestore(workspaceRoot, effectiveToolchain, selectedToolchain, diagnostics))
        {
            return false;
        }

        DependencyOverrideSet overrides;
        if (!loadEffectiveDependencyOverrides(model, workspaceRoot, cliOverrides, overrides, diagnostics))
        {
            return false;
        }

        LockFile lockFile;
        if (!LockFile::read(workspaceRoot / "tbuild.lock.toml", lockFile, diagnostics))
        {
            return false;
        }
        if (!model.rootPackage.dependencies.empty() && lockFile.packages.empty())
        {
            diagnostics.error(dependenciesNotRestoredMessage(config, selectedToolchain.id));
            return false;
        }

        PackageArtifactResolver packageResolver;
        if (!packageResolver.load(lockFile, workspaceRoot, config, selectedToolchain.id, overrides, diagnostics))
        {
            return false;
        }
        for (const auto &dependency : model.rootPackage.dependencies)
        {
            if (!packageResolver.find(dependency.name))
            {
                diagnostics.error(dependenciesNotRestoredMessage(config, selectedToolchain.id));
                return false;
            }
        }
        packages = packageResolver.packages();
        if (loadedModel)
        {
            *loadedModel = model;
        }
        if (metadata)
        {
            metadata->toolchainId = selectedToolchain.id;
            metadata->compilerKind = selectedToolchain.compilerKind;
            metadata->config = config;
            metadata->arch = selectedToolchain.targetArch;
            metadata->runtime = inferRuntimeMetadata(lockFile, config, selectedToolchain.id);
            metadata->linkage = inferLinkageMetadata(lockFile, config, selectedToolchain.id);
        }
        return true;
    }

    int Commands::generate(const std::filesystem::path &workspaceRoot,
                        const std::string &toolchainId,
                        const std::string &configOption,
                        const std::filesystem::path &outputDir,
                        const std::vector<std::string> &cliOverrides)
    {
        ConsoleDiagnosticSink diagnostics;
        std::vector<ResolvedDependencyArtifact> packages;
        CMakeDependencyExportMetadata metadata;
        BuildModel model;
        if (!loadRestoredPackagesForCommand(workspaceRoot, toolchainId, configOption, packages, &metadata, diagnostics, cliOverrides, &model))
        {
            return 1;
        }
        const auto resolvedOutputDir = outputDir.empty()
                                           ? workspaceRoot / ".tbuild" / "generated" / "cmake"
                                           : (outputDir.is_relative() ? workspaceRoot / outputDir : outputDir);
        if (!CMakeDependency::writeFiles(packages, resolvedOutputDir, metadata, diagnostics))
        {
            return 1;
        }
        diagnostics.info("wrote CMake dependency files under " + resolvedOutputDir.string());
        diagnostics.info("CMake consumers can use: find_package(tbuild CONFIG REQUIRED)");
        diagnostics.info("add this path to CMAKE_PREFIX_PATH: " + resolvedOutputDir.string());

        if (model.workspace.generateCMakeUserPresets)
        {
            const auto presetsPath = workspaceRoot / "CMakeUserPresets.json";
            if (File::exists(presetsPath))
            {
                diagnostics.info("CMakeUserPresets.json already exists; not overwriting it");
                diagnostics.info("add this cache variable to an existing preset if desired: CMAKE_PREFIX_PATH=${sourceDir}/.tbuild/generated/cmake");
            }
            else
            {
                nlohmann::json preset;
                preset["version"] = 2;
                preset["configurePresets"] = nlohmann::json::array({{
                    {"name", "tbuild-deps"},
                    {"displayName", "tbuild dependency prefix"},
                    {"hidden", true},
                    {"cacheVariables", {{"CMAKE_PREFIX_PATH", "${sourceDir}/.tbuild/generated/cmake"}}},
                }});
                std::string error;
                if (!File::write(presetsPath, preset.dump(2) + "\n", &error))
                {
                    diagnostics.error("failed to write CMakeUserPresets.json: " + error);
                    return 1;
                }
                diagnostics.info("wrote optional CMakeUserPresets.json with hidden preset 'tbuild-deps'");
            }
        }
        return 0;
    }


} // namespace toolkit
