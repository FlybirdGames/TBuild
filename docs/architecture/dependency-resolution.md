# Dependency resolution

Dependency resolution converts manifest dependencies into ordered restore work.

## Intended stages

1. Load the root manifest model.
2. Apply local and command-line overrides.
3. Build the dependency graph.
4. Detect missing, duplicate, or cyclic dependencies.
5. Resolve source state from lockfile or remote source.
6. Fetch or reuse source cache.
7. Build/export package artifacts.
8. Update lockfile when allowed.
9. Generate dependency artifact metadata for CMake export.

## Invariants

- A dependency name identifies one logical package inside a manifest.
- Restore order must respect transitive dependencies.
- `--locked` must not silently change lockfile state.
- Local overrides should be visible in diagnostics.
- Artifact identity must not change during a refactor unless explicitly planned.
