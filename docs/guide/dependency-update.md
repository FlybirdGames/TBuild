# Dependency update

`update` changes lockfile entries for one package or all packages.

## Update one package

```bash
tpkg update fmt
tpkg restore fmt
```

## Update all packages

```bash
tpkg update --all
tpkg restore
```

## With toolchain and config

```bash
tpkg update fmt --config release --toolchain windows-msvc-x64
```

## Policy

Do not use update when you only want a clean rebuild. Use restore cleanup options instead.

Use update when you intentionally want a new resolved source state.
