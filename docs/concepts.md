# Core concepts

## Dependency manager, not build system

ToolkitBuild manages dependencies for CMake projects. It may build third-party dependency packages as part of restore, but it does not replace your project build graph.

Your application or library remains a CMake project.

## Manifest

`tbuild.deps.lua` declares the dependency graph, source locations, build method, artifacts, and platform-specific overrides.

## Lockfile

`tbuild.lock.toml` records resolved source state, such as Git commits or archive hashes. Commit this file to make restore reproducible.

## Package source cache

`.tbuild/packages/` stores fetched dependency sources.

## Package build cache

`.tbuild/build-packages/` stores dependency build workspaces.

## Artifact cache

`.tbuild/artifacts/` stores exported dependency artifacts used by generated CMake integration files.

## Generated CMake integration

`tbuild generate` writes CMake files under `.tbuild/generated/cmake/` by default. Your `CMakeLists.txt` includes the generated file and links the imported targets or libraries it defines.

## Toolchain profile

A toolchain profile describes compilers, linkers, SDK roots, binary tools, system include directories, and other platform-specific information needed for dependency builds.

## Local override

An override replaces a dependency source with a local path during development without editing the main manifest permanently.
