# Mirrors

Mirrors provide fallback sources for Git or archive dependencies.

## Example

```lua
require("fmt", {
    source = "https://github.com/fmtlib/fmt.git",
    mirrors = {
        "https://gitee.com/mirrors/fmt.git"
    },
    ref = "10.2.1",
    build = "cmake"
})
```

## Policy

- Keep the primary source canonical.
- Mirrors should contain equivalent source history or archive content.
- For archives, use `sha256` whenever possible.
- Do not use mirrors to silently switch to unrelated forks.
