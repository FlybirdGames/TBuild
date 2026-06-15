# Custom build

Custom build is an escape hatch for packages that do not fit `cmake`, `make`, `configure_make`, `header_only`, or `prebuilt`.

## Example

```lua
require("codegenlib", {
    source = "https://example.com/codegenlib.git",
    ref = "v1.0.0",
        build = "custom",
        commands = {
            configure = {
            "python configure.py --prefix $(artifact)"
            },
            build = {
                "python build.py --config $(config)"
            },
            install = {
            "python install.py --prefix $(artifact)"
            }
        },
    artifacts = {
        include_dirs = {"include"},
        lib_dirs = {"lib"},
        libs = {"codegenlib"}
    }
})
```

## Variables

Common variable expansion includes:

| Variable | Meaning |
| --- | --- |
| `$(source)` | Dependency source directory |
| `$(build)` | Dependency build directory |
| `$(artifact)` | Dependency install/export directory |
| `$(config)` | Active config |
| `$(package)` | Dependency package name |
| `$(platform)` | Active platform |
| `$(arch)` | Active architecture |
| `$(cc)` | C compiler path |
| `$(cxx)` | C++ compiler path |
| `$(linker)` | Linker path |
| `$(archiver)` | Archiver path |
| `$(rc)` | Resource compiler path |
| `$(mt)` | Manifest tool path |
| `$(cmake)` | CMake executable |
| `$(ninja)` | Ninja executable |
| `$(git)` | Git executable |

## Policy

Custom commands run as project-trusted commands. Do not run untrusted manifests.
