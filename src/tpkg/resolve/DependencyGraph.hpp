#pragma once

#include "tpkg/model/BuildModel.hpp"

#include <vector>

namespace toolkit
{

    class DiagnosticSink;

    class DepGraph
    {
    public:
        static bool build(const BuildModel &model,
                          std::vector<DependencyDesc> &orderedDependencies,
                          DiagnosticSink &diagnostics);
    };

} // namespace toolkit
