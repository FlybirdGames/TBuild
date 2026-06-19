# Local overrides

Local overrides redirect dependency sources to local working directories.

## Command-line override

```bash
tpkg restore --override fmt=../fmt-fork
```

## Persistent local override

```bash
tpkg override set fmt ../fmt-fork
tpkg override list
tpkg override remove fmt
tpkg override clear
```

Overrides are local development state and should not be committed.

## Manifest override table

A manifest may also describe overrides when a project needs a controlled development layout.

```lua
dependency_overrides({
    fmt = "../fmt-fork"
})
```

Prefer command or local override files for day-to-day development so the main manifest remains reproducible.
