# Build systems

The `build` field controls how a dependency package is restored into artifacts.

## `header_only`

```lua
require("nameof", {
    source = "https://github.com/Neargye/nameof.git",
    ref = "v0.10.3",
    build = "header_only",
    artifacts = {
        include_dirs = {"include"}
    }
})
```

## `prebuilt`

```lua
require("vendorlib", {
    source = "../vendor/vendorlib",
    source_type = "local",
    build = "prebuilt",
    artifacts = {
        include_dirs = {"include"},
        lib_dirs = {"lib"},
        libs = {"vendorlib"}
    }
})
```

## `cmake`

```lua
require("fmt", {
    source = "https://github.com/fmtlib/fmt.git",
    ref = "10.2.1",
    build = "cmake",
    cmake = {
        generator = "Ninja",
        options = {
            FMT_DOC = false,
            FMT_TEST = false
        },
        build_targets = {"fmt"},
        install = true
    }
})
```

## `make`

```lua
require("some_make_lib", {
    source = "https://example.com/some_make_lib.git",
    ref = "v1.0.0",
    build = "make",
    make = {
        args = {"PREFIX=$(artifact)"},
        targets = {"all"},
        install_targets = {"install"}
    }
})
```

## `configure_make`

```lua
require("autotools_lib", {
    source = "https://example.com/autotools_lib.tar.gz",
    source_type = "archive",
    build = "configure_make",
    configure = {
        args = {"--prefix=$(artifact)"}
    },
    make = {
        targets = {"all"},
        install_targets = {"install"}
    }
})
```

## `custom`

Use custom build only when existing build adapters are not sufficient. See [custom build](custom-build.md).
