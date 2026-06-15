# Basic workflow

A normal ToolkitBuild workflow has four steps.

## 1. Write the manifest

Create `tbuild.deps.lua` in the CMake project root.

```lua
package("MyProject")
version("0.1.0")

default_config("debug")
default_platform("host")
default_arch("x64")

require("fmt", {
    source = "https://github.com/fmtlib/fmt.git",
    ref = "10.2.1",
    build = "cmake",
    artifacts = {
        include_dirs = {"include"},
        libs = {"fmt"}
    }
})
```

## 2. Detect or select a toolchain

```bash
tbuild sdk detect
tbuild sdk list
tbuild sdk select windows-msvc-x64
```

The selected toolchain is stored in local `.tbuild` state and should not be committed.

## 3. Restore dependencies

```bash
tbuild restore --config debug
```

This fetches sources, builds dependency packages if needed, exports artifacts, and updates `tbuild.lock.toml`.

## 4. Generate CMake files

```bash
tbuild generate --config debug
```

Then include the generated file from your CMake project.

## Recommended Git policy

Commit:

```text
tbuild.deps.lua
tbuild.lock.toml
docs explaining required toolchains
```

Ignore:

```text
.tbuild/
```
