# DSL reference

This reference lists public manifest functions and dependency fields.

## Global functions

| Function | Status | Purpose |
| --- | --- | --- |
| `package(name)` | Stable | Root package name |
| `version(value)` | Stable | Root package version |
| `default_config(value)` | Stable | Default build config |
| `default_platform(value)` | Stable | Default target platform |
| `default_arch(value)` | Stable | Default target architecture |
| `cmake(table)` | Experimental | Root CMake integration options |
| `dependency_overrides(table)` | Experimental | Manifest-level dependency overrides |
| `require(name, table)` | Stable | Declare dependency |
| `require_local(path_or_name, table)` | Stable | Declare local dependency |
| `when(condition, function)` | Experimental | Conditional manifest block |
| `include(path)` | Stable | Include another manifest fragment |

## Dependency fields

| Field | Type | Status |
| --- | --- | --- |
| `name` | string | Stable |
| `source` | string | Stable |
| `source_type` | string | Stable |
| `mirrors` | array string | Stable |
| `sha256` | string | Stable |
| `ref` | string | Stable |
| `subdir` | string | Stable |
| `strip_components` | integer | Stable |
| `patches` | array string | Experimental |
| `patch_hashes` | array string | Experimental |
| `build` | string | Stable |
| `linkage` | string | Experimental |
| `runtime` | string | Experimental |
| `pic` | bool/string | Experimental |
| `cmake` | table | Stable |
| `make` | table | Stable |
| `configure` | table | Stable |
| `commands` | table | Stable for custom builds |
| `artifacts` | table | Stable |
| `toolchain_requirements` | table | Experimental |
| `dependencies` | array string | Stable |
| `export_defines` | bool | Experimental |
| `platforms` | table | Experimental |

## Build types

| Value | Status | Meaning |
| --- | --- | --- |
| `header_only` | Stable | No build; export headers |
| `prebuilt` | Stable | Use existing binary artifacts |
| `cmake` | Stable | Configure/build/install with CMake |
| `make` | Stable | Build with Make |
| `configure_make` | Stable | Run configure, then Make |
| `custom` | Experimental | Run custom command lists |

## CMake fields

```lua
cmake = {
    generator = "Ninja",
    build_type = "Release",
    options = {
        SOME_OPTION = false
    },
    configure_args = {},
    build_args = {},
    build_targets = {},
    install_args = {},
    env = {},
    toolchain_file = "",
    install = true,
    install_target = "install"
}
```

## Artifact fields

See [Artifacts](../dsl/artifacts.md).
