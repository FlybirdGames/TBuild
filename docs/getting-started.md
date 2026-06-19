# Getting started

This guide creates a minimal CMake project that restores dependencies through Toolkit Package Manager.

## 1. Create `tpkg.lua`

```lua
package("HelloTPKG")
version("0.1.0")
default_config("debug")
default_platform("host")
default_arch("x64")

require("fmt", {
    source = "https://github.com/fmtlib/fmt.git",
    ref = "10.2.1",
    build = "cmake",
    cmake = {
        options = {
            FMT_DOC = false,
            FMT_TEST = false
        }
    },
    artifacts = {
        include_dirs = {"include"},
        libs = {"fmt"}
    }
})
```

## 2. Detect toolchains

```bash
tpkg sdk detect
tpkg sdk list
```

Choose a profile ID shown by `sdk list`, for example `windows-msvc-x64`.

## 3. Restore dependencies

```bash
tpkg restore --config debug --toolchain windows-msvc-x64
```

This reads `tpkg.lua`, fetches dependency sources, builds dependencies if needed, writes or updates `tpkg.lock.toml`, and stores artifacts under `.tpkg/`.

## 4. Generate CMake integration

```bash
tpkg generate --config debug --toolchain windows-msvc-x64
```

The default output is:

```text
.tpkg/generated/cmake/
```

## 5. Consume from CMake

```cmake
cmake_minimum_required(VERSION 3.25)
project(HelloTPKG LANGUAGES CXX)

include("${CMAKE_SOURCE_DIR}/.tpkg/generated/cmake/tpkg_deps.cmake")

add_executable(hello src/main.cpp)
target_link_libraries(hello PRIVATE fmt)
```

## 6. Keep the right files in Git

Commit:

```text
tpkg.lua
tpkg.lock.toml
CMakeLists.txt
```

Do not commit:

```text
.tpkg/
```

## Next steps

- [Basic workflow](guide/basic-workflow.md)
- [DSL overview](dsl/overview.md)
- [Command reference](reference/command-reference.md)
