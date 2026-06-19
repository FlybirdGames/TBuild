# Dependency restore

`restore` is the main dependency preparation command.

```bash
tpkg restore [package] [options]
```

## Common usage

```bash
tpkg restore
tpkg restore --config release
tpkg restore --toolchain windows-msvc-x64
tpkg restore fmt
```

## Reproducible restore

Use the lockfile exactly:

```bash
tpkg restore --locked
```

`--locked` should not update `tpkg.lock.toml`.

## Build-only and export-only

Build a dependency workspace without exporting artifacts:

```bash
tpkg restore dxc --build-only
```

Export artifacts from an already-built workspace:

```bash
tpkg restore dxc --export-only
```

Force a clean dependency build workspace:

```bash
tpkg restore dxc --rebuild
```

## Restore outputs

Restore may update:

```text
.tpkg/packages/
.tpkg/build-packages/
.tpkg/artifacts/
tpkg.lock.toml
```
