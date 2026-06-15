# Cache layout

ToolkitBuild uses `.tbuild/` for local generated state.

```text
.tbuild/
  packages/          dependency source cache
  build-packages/    dependency build workspaces
  artifacts/         exported artifacts
  generated/cmake/   generated CMake integration
  toolchains/        detected toolchain cache
  local.toml         local workspace preferences, when present
```

## Stability

The existence of these top-level concepts is stable. Exact internal artifact directory names are internal implementation details.
