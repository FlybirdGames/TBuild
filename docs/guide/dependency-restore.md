# Dependency restore

`restore` is the main dependency preparation command.

```bash
tbuild restore [package] [options]
```

## Common usage

```bash
tbuild restore
tbuild restore --config release
tbuild restore --toolchain windows-msvc-x64
tbuild restore fmt
```

## Reproducible restore

Use the lockfile exactly:

```bash
tbuild restore --locked
```

`--locked` should not update `tbuild.lock.toml`.

## Build-only and export-only

Build a dependency workspace without exporting artifacts:

```bash
tbuild restore dxc --build-only
```

Export artifacts from an already-built workspace:

```bash
tbuild restore dxc --export-only
```

Force a clean dependency build workspace:

```bash
tbuild restore dxc --rebuild
```

## Restore outputs

Restore may update:

```text
.tbuild/packages/
.tbuild/build-packages/
.tbuild/artifacts/
tbuild.lock.toml
```
