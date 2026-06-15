# Package cache

ToolkitBuild stores dependency state under `.tbuild/`.

## Main directories

```text
.tbuild/packages/          fetched dependency sources
.tbuild/build-packages/    dependency build workspaces
.tbuild/artifacts/         exported dependency artifacts
.tbuild/generated/cmake/   generated CMake integration files
.tbuild/toolchains/        detected toolchain cache
```

## Artifact identity

Artifact identity is derived from dependency source state and build-relevant configuration. Do not depend on cache directory names manually; consume generated CMake output instead.

## Inspect packages

```bash
tbuild packages
tbuild packages --verbose
```

## Garbage collect unused artifacts

Dry run:

```bash
tbuild packages gc
```

Apply:

```bash
tbuild packages gc --apply
```

Options:

```bash
tbuild packages gc --package fmt --builds --sources
```
