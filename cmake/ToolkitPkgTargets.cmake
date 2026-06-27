file(GLOB TPKG_CLI_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tpkg/cli/*.cpp"
)
file(GLOB TPKG_CMAKE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tpkg/cmake/*.cpp"
)
file(GLOB TPKG_CONFIG_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tpkg/config/*.cpp"
)
file(GLOB TPKG_CORE_MODULE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tpkg/core/*.cpp"
)
file(GLOB TPKG_DIAGNOSTICS_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tpkg/diagnostics/*.cpp"
)
file(GLOB TPKG_MODEL_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tpkg/model/*.cpp"
)
file(GLOB TPKG_PACKAGE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tpkg/package/*.cpp"
)
file(GLOB TPKG_RESOLVE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tpkg/resolve/*.cpp"
)
file(GLOB TPKG_SCRIPT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tpkg/script/*.cpp"
)
file(GLOB TPKG_TOOLCHAIN_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tpkg/toolchain/*.cpp"
)
file(GLOB TPKG_UTILS_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tpkg/utils/*.cpp"
)

set(TPKG_CORE_SOURCES
    ${TPKG_CLI_SOURCES}
    ${TPKG_CMAKE_SOURCES}
    ${TPKG_CONFIG_SOURCES}
    ${TPKG_CORE_MODULE_SOURCES}
    ${TPKG_DIAGNOSTICS_SOURCES}
    ${TPKG_MODEL_SOURCES}
    ${TPKG_PACKAGE_SOURCES}
    ${TPKG_RESOLVE_SOURCES}
    ${TPKG_SCRIPT_SOURCES}
    ${TPKG_TOOLCHAIN_SOURCES}
    ${TPKG_UTILS_SOURCES}
)

add_library(tpkg_core STATIC ${TPKG_CORE_SOURCES})
target_include_directories(tpkg_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_compile_definitions(tpkg_core PUBLIC TPKG_VERSION=\"${PROJECT_VERSION}\")
tpkg_link_core_dependencies(tpkg_core)

if(MSVC)
    target_compile_options(tpkg_core PRIVATE /utf-8 /W4 /permissive-)
else()
    target_compile_options(tpkg_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(tpkg "${CMAKE_CURRENT_SOURCE_DIR}/src/tpkg/main.cpp")
target_link_libraries(tpkg PRIVATE tpkg_core)
set_target_properties(tpkg PROPERTIES OUTPUT_NAME tpkg)

install(TARGETS tpkg
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tpkg/main.cpp"
    ${TPKG_CORE_SOURCES}
)
