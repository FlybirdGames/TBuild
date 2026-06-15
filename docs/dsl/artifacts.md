# Artifacts

Artifacts describe what a dependency exposes to the consuming CMake project.

## Basic example

```lua
artifacts = {
    include_dirs = {"include"},
    lib_dirs = {"lib"},
    bin_dirs = {"bin"},
    libs = {"fmt"},
    defines = {"FMT_HEADER_ONLY=0"}
}
```

## Fields

| Field | Type | Status | Meaning |
| --- | --- | --- | --- |
| `mode` | string | Stable | Artifact copy/materialization mode |
| `include_dirs` | array | Stable | Header directories |
| `lib_dirs` | array | Stable | Library search directories |
| `bin_dirs` | array | Stable | Binary/runtime search directories |
| `bin_files` | array | Stable | Runtime binaries |
| `libs` | array | Stable | Library names |
| `lib_files` | array | Stable | Library file paths |
| `lib_files_by_config` | map | Experimental | Config-specific library files |
| `defines` | array | Stable | Compile definitions |
| `system_libs` | array | Stable | Platform system libraries |
| `frameworks` | array | Stable | Apple frameworks |

## Debug/release library names

```lua
artifacts = {
    include_dirs = {"include"},
    lib_files_by_config = {
        debug = {"lib/mylibd.lib"},
        release = {"lib/mylib.lib"}
    }
}
```

## System libraries

```lua
artifacts = {
    libs = {"mylib"},
    system_libs = {"ws2_32", "bcrypt"}
}
```

## Apple frameworks

```lua
artifacts = {
    frameworks = {"Foundation", "CoreFoundation"}
}
```
