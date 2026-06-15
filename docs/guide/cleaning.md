# Cleaning

Use targeted cleanup first.

## Clean generated integration files

```bash
tbuild clean
```

## Clean one dependency

```bash
tbuild clean fmt --artifacts --sources
```

## Remove the lockfile

```bash
tbuild clean --lock
```

Only do this when you intentionally want to resolve dependencies again.

## Remove all local state

```bash
tbuild clean --all
```

This removes `.tbuild/` and should be treated as a full local reset.
