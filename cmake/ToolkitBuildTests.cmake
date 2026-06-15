find_package(GTest CONFIG REQUIRED)

set(TBUILD_TEST_SOURCES
    tests/tbuild/BuildModelTests.cpp
    tests/tbuild/ArchiveTests.cpp
    tests/tbuild/CMakeDependencyExporterTests.cpp
    tests/tbuild/DependencyResolverTests.cpp
    tests/tbuild/LuaDslTests.cpp
    tests/tbuild/LockFileTests.cpp
    tests/tbuild/PackageArtifactResolverTests.cpp
    tests/tbuild/PackageBuilderUtilTests.cpp
    tests/tbuild/ToolchainTests.cpp
)

add_executable(tbuild_tests ${TBUILD_TEST_SOURCES})
target_link_libraries(tbuild_tests PRIVATE tbuild_core GTest::gtest GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tbuild_tests)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/tests" PREFIX tests FILES ${TBUILD_TEST_SOURCES})
