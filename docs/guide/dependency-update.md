# Dependency update

`update` changes lockfile entries for one package or all packages.

## Update one package

```bash
tbuild update fmt
tbuild restore fmt
```

## Update all packages

```bash
tbuild update --all
tbuild restore
```

## With toolchain and config

```bash
tbuild update fmt --config release --toolchain windows-msvc-x64
```

## Policy

Do not use update when you only want a clean rebuild. Use restore cleanup options instead.

Use update when you intentionally want a new resolved source state.
