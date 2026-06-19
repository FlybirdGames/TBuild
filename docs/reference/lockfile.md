# Lockfile

`tpkg.lock.toml` records resolved dependency state.

## Commit policy

Commit `tpkg.lock.toml` for applications and projects that need reproducible dependency restore.

## Locked restore

```bash
tpkg restore --locked
```

A locked restore should use existing lock entries and should not update the lockfile.

## Update policy

Use `tpkg update` when intentionally changing lock state.

```bash
tpkg update fmt
tpkg restore fmt
```

## Do not hand-edit by default

Manual edits can make source state inconsistent with cache state. Prefer command workflows unless debugging.
