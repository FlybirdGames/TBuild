# Dependency declaration

## `require(name, options)`

Declares a dependency.

```lua
require("fmt", {
    source = "https://github.com/fmtlib/fmt.git",
    ref = "10.2.1",
    build = "cmake"
})
```

## `require_local(path_or_name, options)`

Declares a local dependency source.

```lua
require_local("../third_party/mylib", {
    name = "mylib",
    build = "cmake"
})
```

Use local dependencies for private dependencies or monorepo-style development.

## Common fields

| Field | Type | Status | Meaning |
| --- | --- | --- | --- |
| `source` | string | Stable | Git URL, archive URL, or source path |
| `source_type` | string | Stable | `git`, `archive`, or `local` when inference is not enough |
| `mirrors` | array | Stable | Fallback sources |
| `ref` | string | Stable | Git branch, tag, or commit |
| `sha256` | string | Stable | Archive integrity hash |
| `subdir` | string | Stable | Project subdirectory inside dependency source |
| `strip_components` | integer | Stable | Archive path component stripping |
| `patches` | array | Experimental | Patch files to apply after fetch |
| `build` | string | Stable | Build method |
| `dependencies` | array | Stable | Transitive dependencies by name |
| `linkage` | string | Experimental | `default`, `static`, or `shared` intent |
| `runtime` | string | Experimental | Runtime linkage intent |
| `pic` | bool/string | Experimental | Position-independent-code intent |
| `export_defines` | bool | Experimental | Export dependency defines to consumers |

## Transitive dependencies

```lua
require("spdlog", {
    source = "https://github.com/gabime/spdlog.git",
    ref = "v1.13.0",
    build = "cmake",
    dependencies = {"fmt"}
})
```

Declare dependencies by manifest name, not by source URL.
