# Package cache

Toolkit Package Manager stores dependency state under `.tpkg/`.

## Main directories

```text
.tpkg/packages/          fetched dependency sources
.tpkg/build-packages/    dependency build workspaces
.tpkg/artifacts/         exported dependency artifacts
.tpkg/generated/cmake/   generated CMake integration files
.tpkg/toolchains/        detected toolchain cache
```

## Artifact identity

Artifact identity is derived from dependency source state and build-relevant configuration. Do not depend on cache directory names manually; consume generated CMake output instead.

## Inspect packages

```bash
tpkg packages
tpkg packages --verbose
```

## Garbage collect unused artifacts

Dry run:

```bash
tpkg packages gc
```

Apply:

```bash
tpkg packages gc --apply
```

Options:

```bash
tpkg packages gc --package fmt --builds --sources
```
