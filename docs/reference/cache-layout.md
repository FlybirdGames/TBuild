<!--
 * @Author: cubevlmu khfahqp@gmail.com
 * @LastEditors: cubevlmu khfahqp@gmail.com
 * Copyright (c) 2026 by FlybirdGames, All Rights Reserved. 
-->
# Cache layout

Toolkit Package Manager uses `.tpkg/` for local generated state.

```text
.tpkg/
  packages/          dependency source cache
  build-packages/    dependency build workspaces
  artifacts/         exported artifacts
  generated/cmake/   generated CMake integration
  toolchains/        detected toolchain cache
  local.toml         local workspace preferences, when present
```

## Stability

The existence of these top-level concepts is stable. Exact internal artifact directory names are internal implementation details.
