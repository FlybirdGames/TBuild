find_package(GTest CONFIG REQUIRED)

set(TPKG_TEST_SOURCES
    tests/tpkg/BuildModelTests.cpp
    tests/tpkg/ArchiveTests.cpp
    tests/tpkg/CMakeDependencyExporterTests.cpp
    tests/tpkg/DependencyResolverTests.cpp
    tests/tpkg/LuaDslTests.cpp
    tests/tpkg/LockFileTests.cpp
    tests/tpkg/PackageArtifactResolverTests.cpp
    tests/tpkg/PackageBuilderUtilTests.cpp
    tests/tpkg/ToolchainTests.cpp
)

add_executable(tpkg_tests ${TPKG_TEST_SOURCES})
target_link_libraries(tpkg_tests PRIVATE tpkg_core GTest::gtest GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tpkg_tests)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/tests" PREFIX tests FILES ${TPKG_TEST_SOURCES})
