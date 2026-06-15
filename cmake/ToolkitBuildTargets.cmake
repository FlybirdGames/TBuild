file(GLOB TBUILD_CLI_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tbuild/cli/*.cpp"
)
file(GLOB TBUILD_CMAKE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tbuild/cmake/*.cpp"
)
file(GLOB TBUILD_CONFIG_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tbuild/config/*.cpp"
)
file(GLOB TBUILD_CORE_MODULE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tbuild/core/*.cpp"
)
file(GLOB TBUILD_DIAGNOSTICS_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tbuild/diagnostics/*.cpp"
)
file(GLOB TBUILD_MODEL_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tbuild/model/*.cpp"
)
file(GLOB TBUILD_PACKAGE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tbuild/package/*.cpp"
)
file(GLOB TBUILD_RESOLVE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tbuild/resolve/*.cpp"
)
file(GLOB TBUILD_SCRIPT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tbuild/script/*.cpp"
)
file(GLOB TBUILD_TOOLCHAIN_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tbuild/toolchain/*.cpp"
)
file(GLOB TBUILD_UTILS_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tbuild/utils/*.cpp"
)

set(TBUILD_CORE_SOURCES
    ${TBUILD_CLI_SOURCES}
    ${TBUILD_CMAKE_SOURCES}
    ${TBUILD_CONFIG_SOURCES}
    ${TBUILD_CORE_MODULE_SOURCES}
    ${TBUILD_DIAGNOSTICS_SOURCES}
    ${TBUILD_MODEL_SOURCES}
    ${TBUILD_PACKAGE_SOURCES}
    ${TBUILD_RESOLVE_SOURCES}
    ${TBUILD_SCRIPT_SOURCES}
    ${TBUILD_TOOLCHAIN_SOURCES}
    ${TBUILD_UTILS_SOURCES}
)

add_library(tbuild_core STATIC ${TBUILD_CORE_SOURCES})
target_include_directories(tbuild_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_compile_definitions(tbuild_core PUBLIC TBUILD_VERSION=\"${PROJECT_VERSION}\")
tbuild_link_core_dependencies(tbuild_core)

if(MSVC)
    target_compile_options(tbuild_core PRIVATE /utf-8 /W4 /permissive-)
else()
    target_compile_options(tbuild_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(tbuild "${CMAKE_CURRENT_SOURCE_DIR}/src/tbuild/main.cpp")
target_link_libraries(tbuild PRIVATE tbuild_core)
set_target_properties(tbuild PROPERTIES OUTPUT_NAME tbuild)

install(TARGETS tbuild RUNTIME DESTINATION bin)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tbuild/main.cpp"
    ${TBUILD_CORE_SOURCES}
)
