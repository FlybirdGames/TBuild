# ToolkitBuild

ToolkitBuild is a cross-platform C/C++ dependency management system designed to serve CMake projects.

It is **not** a replacement for CMake, Ninja, Visual Studio, Xcode, or any native build backend. Its job is to restore source, archive, local, or prebuilt dependencies; build dependency artifacts when needed; write a lock file for reproducibility; and generate CMake integration files that your normal CMake project can consume.

## Project status

ToolkitBuild is under active development.

Current validation scope:

- Desktop platforms are the current tested target.
- Windows desktop workflows are the most exercised path.
- Linux and macOS are intended cross-platform targets but should be validated per project.
- Android toolchain profiles are supported and will continue to be tested.
- iOS support is planned but not implemented yet.

## Core workflow

```bash
# Check the local environment
tbuild doctor

# Detect local C/C++ and platform toolchains
tbuild sdk detect
tbuild sdk list

# Restore dependency sources and artifacts
tbuild restore --config debug --toolchain windows-msvc-x64

# Generate CMake integration files
tbuild generate --config debug --toolchain windows-msvc-x64
```

A normal project keeps using CMake:

```cmake
include("${CMAKE_SOURCE_DIR}/.tbuild/generated/cmake/tbuild_deps.cmake")
```

## Minimal `tbuild.deps.lua`

```lua
package("ExampleApp")
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

## Documentation

Start here:

- [Documentation index](docs/INDEX.md)
- [Getting started](docs/getting-started.md)
- [Basic workflow](docs/guide/basic-workflow.md)
- [DSL overview](docs/dsl/overview.md)
- [Command reference](docs/reference/command-reference.md)
- [Architecture overview](docs/architecture/overview.md)
- [Contributor guide](CONTRIBUTING.md)

## Repository layout

```text
src/tbuild/        ToolkitBuild implementation
cmake/             CMake project glue
docs/              User, reference, architecture, and contributor docs
examples/          Example dependency manifests and project layouts
tests/             Unit and integration-style tests
scripts/           Local helper scripts
```

## License

This snapshot preserves the existing project copyright notice. See [LICENSE](LICENSE).
