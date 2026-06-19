cmake_minimum_required(VERSION 3.25)
project(tpkg_basic_cmake LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TPKG_DEPS_FILE "${CMAKE_SOURCE_DIR}/.tpkg/generated/cmake/tpkg_deps.cmake")
if(NOT EXISTS "${TPKG_DEPS_FILE}")
    message(FATAL_ERROR "Run: tpkg restore --root ${CMAKE_SOURCE_DIR} && tpkg generate --root ${CMAKE_SOURCE_DIR}")
endif()
include("${TPKG_DEPS_FILE}")

add_executable(tpkg_basic src/main.cpp)
target_link_libraries(tpkg_basic PRIVATE fmt)
