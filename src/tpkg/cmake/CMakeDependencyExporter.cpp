#include "tpkg/cmake/CMakeDependencyExporter.hpp"

#include "tpkg/core/FileSystem.hpp"
#include "tpkg/diagnostics/DiagnosticSink.hpp"

#include <cctype>
#include <filesystem>
#include <sstream>
#include <string>
#include <vector>

namespace toolkit
{
    namespace
    {
        std::string cmakeString(const std::string &value)
        {
            std::string result = "\"";
            for (const auto ch : value)
            {
                if (ch == '\\' || ch == '"' || ch == '$')
                {
                    result.push_back('\\');
                }
                result.push_back(ch);
            }
            result.push_back('"');
            return result;
        }

        std::string cmakePath(const std::filesystem::path &path)
        {
            return cmakeString(path.generic_string());
        }

        std::string targetName(const std::string &packageName)
        {
            std::string result;
            result.reserve(packageName.size());
            for (const unsigned char ch : packageName)
            {
                if (std::isalnum(ch) || ch == '_' || ch == '-' || ch == '.' || ch == '+')
                {
                    result.push_back(static_cast<char>(ch));
                }
                else
                {
                    result.push_back('_');
                }
            }
            return "tpkg::" + (result.empty() ? std::string("package") : result);
        }

        void writeStringProperty(std::ostringstream &stream,
                                 const std::string &target,
                                 const char *property,
                                 const std::vector<std::string> &values)
        {
            if (values.empty())
            {
                return;
            }
            stream << "set_property(TARGET " << target << " PROPERTY " << property << "\n";
            for (const auto &value : values)
            {
                stream << "    " << cmakeString(value) << "\n";
            }
            stream << ")\n\n";
        }

        void writePathProperty(std::ostringstream &stream,
                               const std::string &target,
                               const char *property,
                               const std::vector<std::filesystem::path> &values)
        {
            if (values.empty())
            {
                return;
            }
            stream << "set_property(TARGET " << target << " PROPERTY " << property << "\n";
            for (const auto &value : values)
            {
                stream << "    " << cmakePath(value) << "\n";
            }
            stream << ")\n\n";
        }

        bool hasPathSyntax(const std::string &value)
        {
            return value.find('/') != std::string::npos || value.find('\\') != std::string::npos;
        }

        std::vector<std::filesystem::path> libraryCandidates(const std::filesystem::path &dir, const std::string &lib)
        {
            std::vector<std::filesystem::path> candidates;
            const auto valuePath = std::filesystem::path(lib);
            if (hasPathSyntax(lib) || valuePath.has_extension())
            {
                candidates.push_back(valuePath.is_absolute() ? valuePath : dir / valuePath);
                return candidates;
            }
            for (const auto &pattern : {
                     std::string("{}.lib"),
                     std::string("lib{}.lib"),
                     std::string("lib{}.a"),
                     std::string("lib{}.so"),
                     std::string("lib{}.dylib"),
                     std::string("{}.a"),
                     std::string("{}.so"),
                     std::string("{}.dylib"),
                 })
            {
                auto text = pattern;
                text.replace(text.find("{}"), 2, lib);
                candidates.push_back(dir / text);
            }
            return candidates;
        }

        bool resolvePackageLibraries(const ResolvedDependencyArtifact &package,
                                     std::vector<std::string> &linkItems,
                                     DiagnosticSink &diagnostics)
        {
            for (const auto &libFile : package.libFiles)
            {
                if (!std::filesystem::is_regular_file(libFile))
                {
                    diagnostics.error("package " + package.name + " declares library file but it is missing: " + libFile.generic_string());
                    return false;
                }
                linkItems.push_back(libFile.generic_string());
            }

            for (const auto &lib : package.libs)
            {
                bool found = false;
                std::vector<std::filesystem::path> tried;
                for (const auto &libDir : package.libDirs)
                {
                    for (const auto &candidate : libraryCandidates(libDir, lib))
                    {
                        tried.push_back(candidate);
                        if (std::filesystem::is_regular_file(candidate))
                        {
                            linkItems.push_back(candidate.generic_string());
                            found = true;
                            break;
                        }
                    }
                    if (found)
                    {
                        break;
                    }
                }
                if (!found)
                {
                    std::ostringstream message;
                    message << "package " << package.name << " declares library '" << lib << "' but no matching library file was found";
                    if (!package.libDirs.empty())
                    {
                        message << "\nsearched:";
                        for (const auto &candidate : tried)
                        {
                            message << "\n  " << candidate.generic_string();
                        }
                    }
                    diagnostics.error(message.str());
                    return false;
                }
            }

            linkItems.insert(linkItems.end(), package.systemLibs.begin(), package.systemLibs.end());
            for (const auto &framework : package.frameworks)
            {
                linkItems.push_back("-framework " + framework);
            }
            return true;
        }

        std::string cmakeSet(const char *name, const std::string &value)
        {
            return std::string("set(") + name + " " + cmakeString(value) + ")\n";
        }

        void writeAbiChecks(std::ostringstream &config)
        {
            config << R"cmake(
if(TPKG_COMPILER_KIND MATCHES "^(msvc|clang-cl)$" OR TPKG_TOOLCHAIN_ID MATCHES "msvc|clang-cl")
    if(NOT (MSVC OR CMAKE_CXX_COMPILER_ID MATCHES "MSVC|Clang" OR CMAKE_C_COMPILER_ID MATCHES "MSVC|Clang"))
        message(WARNING "tpkg artifacts were restored with an MSVC ABI toolchain (${TPKG_TOOLCHAIN_ID}), but CMake may not be using a compatible compiler. CMAKE_CXX_COMPILER_ID=${CMAKE_CXX_COMPILER_ID}")
    endif()
endif()

if(TPKG_COMPILER_KIND MATCHES "^(gcc|clang)$" AND TPKG_TOOLCHAIN_ID MATCHES "mingw")
    if(MSVC OR CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        message(FATAL_ERROR "tpkg artifacts were restored with a MinGW toolchain, but this CMake project is using MSVC")
    endif()
elseif(TPKG_TOOLCHAIN_ID MATCHES "mingw")
    if(MSVC OR CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        message(FATAL_ERROR "tpkg artifacts were restored with a MinGW toolchain, but this CMake project is using MSVC")
    endif()
endif()

if(TPKG_CONFIG STREQUAL "debug")
    if(DEFINED CMAKE_BUILD_TYPE)
        if(CMAKE_BUILD_TYPE AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
            message(FATAL_ERROR "tpkg debug artifacts cannot be used with CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
        endif()
    endif()
    if(DEFINED CMAKE_CONFIGURATION_TYPES)
        list(FIND CMAKE_CONFIGURATION_TYPES Debug _tpkg_debug_config_index)
        if(_tpkg_debug_config_index EQUAL -1)
            message(FATAL_ERROR "tpkg debug artifacts require a Debug configuration in this multi-config CMake project")
        endif()
    endif()
elseif(TPKG_CONFIG STREQUAL "release")
    if(DEFINED CMAKE_BUILD_TYPE)
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            message(FATAL_ERROR "tpkg release artifacts cannot be used with CMAKE_BUILD_TYPE=Debug")
        endif()
    endif()
    if(DEFINED CMAKE_CONFIGURATION_TYPES)
        list(LENGTH CMAKE_CONFIGURATION_TYPES _tpkg_config_count)
        if(_tpkg_config_count EQUAL 1)
            list(GET CMAKE_CONFIGURATION_TYPES 0 _tpkg_only_config)
            if(_tpkg_only_config STREQUAL "Debug")
                message(FATAL_ERROR "tpkg release artifacts cannot be used with a Debug-only multi-config CMake project")
            endif()
        endif()
    endif()
endif()

if(TPKG_ARCH)
    string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" _tpkg_consumer_arch)
    if(TPKG_ARCH STREQUAL "x64" AND _tpkg_consumer_arch AND NOT _tpkg_consumer_arch MATCHES "x86_64|amd64")
        message(FATAL_ERROR "tpkg artifact arch is x64 but CMake reports ${CMAKE_SYSTEM_PROCESSOR}")
    endif()
    if(TPKG_ARCH STREQUAL "x86" AND _tpkg_consumer_arch AND NOT _tpkg_consumer_arch MATCHES "x86|i.86")
        message(FATAL_ERROR "tpkg artifact arch is x86 but CMake reports ${CMAKE_SYSTEM_PROCESSOR}")
    endif()
    if(TPKG_ARCH STREQUAL "arm64" AND _tpkg_consumer_arch AND NOT _tpkg_consumer_arch MATCHES "aarch64|arm64")
        message(FATAL_ERROR "tpkg artifact arch is arm64 but CMake reports ${CMAKE_SYSTEM_PROCESSOR}")
    endif()
endif()

if(MSVC AND TPKG_RUNTIME MATCHES "static|dynamic")
    if(DEFINED CMAKE_MSVC_RUNTIME_LIBRARY)
        if(CMAKE_MSVC_RUNTIME_LIBRARY)
            if(TPKG_RUNTIME STREQUAL "static" AND CMAKE_MSVC_RUNTIME_LIBRARY MATCHES "DLL")
                message(FATAL_ERROR "tpkg artifacts use static MSVC runtime, but CMAKE_MSVC_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}")
            endif()
            if(TPKG_RUNTIME STREQUAL "dynamic" AND NOT CMAKE_MSVC_RUNTIME_LIBRARY MATCHES "DLL")
                message(FATAL_ERROR "tpkg artifacts use dynamic MSVC runtime, but CMAKE_MSVC_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}")
            endif()
        endif()
    endif()
endif()

)cmake";
        }
    }

    bool CMakeDependency::writeFiles(const std::vector<ResolvedDependencyArtifact> &packages,
                                   const std::filesystem::path &outputDir,
                                   const CMakeDependencyExportMetadata &metadata,
                                   DiagnosticSink &diagnostics)
    {
        std::string error;
        if (!File::mkdir(outputDir, &error))
        {
            diagnostics.error("failed to create CMake dependency export directory: " + error);
            return false;
        }

        std::ostringstream targets;
        targets << "# Generated by Toolkit Package Manager. Do not edit.\n\n";
        targets << "include_guard(GLOBAL)\n\n";
        for (const auto &package : packages)
        {
            const auto target = targetName(package.name);
            std::vector<std::string> linkItems;
            if (!resolvePackageLibraries(package, linkItems, diagnostics))
            {
                return false;
            }

            targets << "if(NOT TARGET " << target << ")\n";
            targets << "    add_library(" << target << " INTERFACE IMPORTED)\n";
            targets << "endif()\n\n";
            targets << "# " << package.name << "\n";

            writePathProperty(targets, target, "INTERFACE_INCLUDE_DIRECTORIES", package.includeDirs);

            // Only export defines if explicitly requested
            if (package.exportDefines)
            {
                writeStringProperty(targets, target, "INTERFACE_COMPILE_DEFINITIONS", package.defines);
            }
            writeStringProperty(targets, target, "INTERFACE_LINK_LIBRARIES", linkItems);
        }

        if (!File::write(outputDir / "tpkgTargets.cmake", targets.str(), &error))
        {
            diagnostics.error("failed to write CMake dependency targets: " + error);
            return false;
        }

        std::ostringstream config;
        config << "# Generated by Toolkit Package Manager. Do not edit.\n\n";
        config << "include_guard(GLOBAL)\n";
        config << cmakeSet("TPKG_TOOLCHAIN_ID", metadata.toolchainId);
        config << cmakeSet("TPKG_COMPILER_KIND", metadata.compilerKind);
        config << cmakeSet("TPKG_CONFIG", metadata.config);
        config << cmakeSet("TPKG_ARCH", metadata.arch);
        config << cmakeSet("TPKG_RUNTIME", metadata.runtime);
        config << cmakeSet("TPKG_LINKAGE", metadata.linkage);
        writeAbiChecks(config);
        config << "include(\"${CMAKE_CURRENT_LIST_DIR}/tpkgTargets.cmake\")\n";
        if (!File::write(outputDir / "tpkgConfig.cmake", config.str(), &error))
        {
            diagnostics.error("failed to write tpkgConfig.cmake: " + error);
            return false;
        }
        return true;
    }

    bool CMakeDependency::writeFiles(const std::vector<ResolvedDependencyArtifact> &packages,
                                   const std::filesystem::path &outputDir,
                                   DiagnosticSink &diagnostics)
    {
        return writeFiles(packages, outputDir, CMakeDependencyExportMetadata{}, diagnostics);
    }
}
