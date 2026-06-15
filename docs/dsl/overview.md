# DSL overview

`tbuild.deps.lua` is the dependency manifest for a ToolkitBuild workspace.

The DSL is Lua-based. Treat the manifest as trusted project code.

## Minimal structure

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

## Stability policy

- `package`, `version`, `require`, `require_local`, `include`, and `artifacts` are stable user-facing concepts.
- Platform overlays and custom commands are useful but should be treated as advanced features.
- Internal cache layout and generated artifact IDs are implementation details.

## Evaluation model

ToolkitBuild loads the manifest, builds an in-memory dependency model, validates it, resolves sources, restores artifacts, and then generates CMake integration files.
