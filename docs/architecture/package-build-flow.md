# Package build flow

Package builders convert restored sources into artifact directories.

## Common inputs

- Dependency description.
- Source directory.
- Build workspace directory.
- Install/export directory.
- Active config.
- Toolchain profile.
- Dependency artifact metadata.

## Builders

| Builder | Responsibility |
| --- | --- |
| Header-only | Export include directories |
| Prebuilt | Validate and expose existing binaries |
| CMake | Configure, build, install/export CMake packages |
| Make | Run make-based builds |
| ConfigureMake | Run configure, make, install |
| Custom | Run configured command lists |

## Refactor policy

The package builder refactor should not change command lines, artifact paths, or variable expansion semantics unless a separate behavior-change task is created.
