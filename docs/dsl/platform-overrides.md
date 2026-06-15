# Platform overrides

Platform overrides adjust dependency fields for a target platform.

## Example

```lua
require("libexample", {
    source = "https://example.com/libexample.git",
    ref = "v1.0.0",
    build = "cmake",
    artifacts = {
        include_dirs = {"include"},
        libs = {"example"}
    },
    platforms = {
        windows = {
            artifacts = {
                libs = {"example", "ws2_32"}
            }
        },
        android = {
            cmake = {
                options = {
                    EXAMPLE_ANDROID = true
                }
            }
        }
    }
})
```

## Policy

- Keep common fields at the dependency root.
- Put only platform differences inside overlays.
- Do not duplicate the whole dependency table inside every platform block.
