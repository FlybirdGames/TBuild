cmake_minimum_required(VERSION 3.25)
project(tbuild_basic_cmake LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TBUILD_DEPS_FILE "${CMAKE_SOURCE_DIR}/.tbuild/generated/cmake/tbuild_deps.cmake")
if(NOT EXISTS "${TBUILD_DEPS_FILE}")
    message(FATAL_ERROR "Run: tbuild restore --root ${CMAKE_SOURCE_DIR} && tbuild generate --root ${CMAKE_SOURCE_DIR}")
endif()
include("${TBUILD_DEPS_FILE}")

add_executable(tbuild_basic src/main.cpp)
target_link_libraries(tbuild_basic PRIVATE fmt)
