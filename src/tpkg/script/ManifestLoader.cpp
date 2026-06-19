/*
 * @Author: cubevlmu khfahqp@gmail.com
 * @LastEditors: cubevlmu khfahqp@gmail.com
 * Copyright (c) 2026 by FlybirdGames, All Rights Reserved. 
 */

#include "tpkg/script/ManifestLoader.hpp"

#include "tpkg/core/Environment.hpp"
#include "tpkg/core/FileSystem.hpp"
#include "tpkg/diagnostics/Diagnostic.hpp"
#include "tpkg/diagnostics/DiagnosticSink.hpp"
#include "tpkg/model/BuildModel.hpp"
#include "tpkg/script/LuaBindings.hpp"
#include "tpkg/script/LuaRuntime.hpp"

#include <sol/sol.hpp>

namespace toolkit
{
    class CountingDiagnosticSink final : public DiagnosticSink
    {
    public:
        explicit CountingDiagnosticSink(DiagnosticSink &inner)
            : m_inner(inner)
        {
        }

        void report(const Diagnostic &diagnostic) override
        {
            if (diagnostic.level == DiagnosticLevel::Error)
            {
                ++m_errorCount;
            }
            m_inner.report(diagnostic);
        }

        int errorCount() const
        {
            return m_errorCount;
        }

    private:
        DiagnosticSink &m_inner;
        int m_errorCount = 0;
    };

    ManifestLoadResult ManifestLoader::load(const std::filesystem::path &manifestPath, DiagnosticSink &diagnostics)
    {
        ManifestLoadResult result;
        CountingDiagnosticSink countedDiagnostics(diagnostics);
        if (!File::exists(manifestPath))
        {
            countedDiagnostics.error("missing tpkg.lua: " + manifestPath.string());
            return result;
        }

        try
        {
            LuaRuntime runtime;
            auto &lua = runtime.state();
            const auto platform = result.model.workspace.defaultPlatform.empty()
                                      ? Environment::hostPlatformName()
                                      : result.model.workspace.defaultPlatform;
            bindDsl(lua, result.model, countedDiagnostics, platform, manifestPath.parent_path(), manifestPath);

            sol::protected_function_result execResult = lua.script_file(manifestPath.string());
            if (!execResult.valid())
            {
                sol::error err = execResult;
                countedDiagnostics.error(err.what());
                return result;
            }

            if (!validateBuildModel(result.model, countedDiagnostics) || countedDiagnostics.errorCount() > 0)
            {
                return result;
            }

            result.ok = true;
            return result;
        }
        catch (const std::exception &ex)
        {
            countedDiagnostics.error(ex.what());
            return result;
        }
    }

} // namespace toolkit
