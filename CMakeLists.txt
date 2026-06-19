cmake_minimum_required(VERSION 3.25)

include(cmake/ToolkitPkgVcpkg.cmake)

project(ToolkitPkg VERSION 0.1.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

include(cmake/ToolkitPkgMsvcNinjaSdk.cmake)
include(cmake/ToolkitPkgDependencies.cmake)
include(cmake/ToolkitPkgTargets.cmake)

include(CTest)
if(BUILD_TESTING)
    include(cmake/ToolkitPkgTests.cmake)
endif()
