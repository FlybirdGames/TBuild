# Coding style

## General rules

- Use C++20.
- Prefer clear ownership and value types.
- Keep headers minimal.
- Avoid large static utility classes for domain logic.
- Split by responsibility, not by arbitrary helper buckets.
- Keep user-facing behavior stable during refactors.

## Module rules

- `model/`: plain data structures and validation only.
- `cli/`: command parsing, user output, command orchestration.
- `script/`: Lua binding and manifest parsing.
- `resolve/`: dependency graph and restore orchestration.
- `package/`: source cache, build adapters, artifact materialization.
- `toolchain/`: detection, registry, serialization.

## Error handling

Use diagnostics for user-facing failures. Include enough context: package name, source path, command, toolchain ID, and config when relevant.

## Refactor policy

When refactoring, preserve:

- CLI names and options.
- DSL fields.
- Lockfile format.
- Artifact ID rules.
- Cache layout.
- Generated CMake behavior.
