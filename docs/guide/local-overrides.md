# Local overrides

Local overrides redirect dependency sources to local working directories.

## Command-line override

```bash
tbuild restore --override fmt=../fmt-fork
```

## Persistent local override

```bash
tbuild override set fmt ../fmt-fork
tbuild override list
tbuild override remove fmt
tbuild override clear
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
