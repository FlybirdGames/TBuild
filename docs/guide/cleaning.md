# Cleaning

Use targeted cleanup first.

## Clean generated integration files

```bash
tpkg clean
```

## Clean one dependency

```bash
tpkg clean fmt --artifacts --sources
```

## Remove the lockfile

```bash
tpkg clean --lock
```

Only do this when you intentionally want to resolve dependencies again.

## Remove all local state

```bash
tpkg clean --all
```

This removes `.tpkg/` and should be treated as a full local reset.
