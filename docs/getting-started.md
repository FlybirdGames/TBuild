# Getting started

This guide creates a minimal CMake project that restores dependencies through ToolkitBuild.

## 1. Create `tbuild.deps.lua`

```lua
package("HelloTBuild")
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
tbuild sdk detect
tbuild sdk list
```

Choose a profile ID shown by `sdk list`, for example `windows-msvc-x64`.

## 3. Restore dependencies

```bash
tbuild restore --config debug --toolchain windows-msvc-x64
```

This reads `tbuild.deps.lua`, fetches dependency sources, builds dependencies if needed, writes or updates `tbuild.lock.toml`, and stores artifacts under `.tbuild/`.

## 4. Generate CMake integration

```bash
tbuild generate --config debug --toolchain windows-msvc-x64
```

The default output is:

```text
.tbuild/generated/cmake/
```

## 5. Consume from CMake

```cmake
cmake_minimum_required(VERSION 3.25)
project(HelloTBuild LANGUAGES CXX)

include("${CMAKE_SOURCE_DIR}/.tbuild/generated/cmake/tbuild_deps.cmake")

add_executable(hello src/main.cpp)
target_link_libraries(hello PRIVATE fmt)
```

## 6. Keep the right files in Git

Commit:

```text
tbuild.deps.lua
tbuild.lock.toml
CMakeLists.txt
```

Do not commit:

```text
.tbuild/
```

## Next steps

- [Basic workflow](guide/basic-workflow.md)
- [DSL overview](dsl/overview.md)
- [Command reference](reference/command-reference.md)
