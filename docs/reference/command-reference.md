# Command reference

## Global options

```bash
tpkg [--root <path>] <command> [options]
```

| Option | Meaning |
| --- | --- |
| `--root <path>` | Workspace root containing `tpkg.lua` |
| `--help` | Show help |

## `doctor`

Check the local Toolkit Package Manager runtime.

```bash
tpkg doctor
tpkg doctor --sdk
```

`--sdk` prints detailed SDK and toolchain diagnostics.

## `load`

Load and validate `tpkg.lua`.

```bash
tpkg load
```

Use this before restore when checking DSL syntax and model validation.

## `dump-model`

Print the loaded dependency model as JSON.

```bash
tpkg dump-model
```

## `deps`

Print dependencies declared in the manifest.

```bash
tpkg deps
```

## `generate`

Generate CMake dependency integration files.

```bash
tpkg generate [options]
```

| Option | Meaning |
| --- | --- |
| `--toolchain <id>` | Preferred local toolchain profile ID |
| `--config <debug|release>` | Build config |
| `--output <dir>` | Output directory; default `.tpkg/generated/cmake` |
| `--override <name=path>` | Override dependency source |

## `restore`

Prepare package cache, build/export dependency artifacts, and update lock state.

```bash
tpkg restore [package] [options]
```

| Option | Meaning |
| --- | --- |
| `package` | Optional dependency name to restore only one dependency |
| `--toolchain <id>` | Preferred local toolchain profile ID |
| `--config <debug|release>` | Build config |
| `--override <name=path>` | Override dependency source |
| `--locked` | Use lockfile entries without updating the lockfile |
| `--build-only` | Build package workspace without exporting artifacts |
| `--export-only` | Export artifacts from an existing package build workspace |
| `--rebuild` | Remove the package build workspace before building |

## `update`

Update package lock entries.

```bash
tpkg update [package] [options]
```

| Option | Meaning |
| --- | --- |
| `package` | Optional package to update |
| `--toolchain <id>` | Preferred local toolchain profile ID |
| `--config <debug|release>` | Build config |
| `--override <name=path>` | Override dependency source |
| `--all` | Update all non-overridden packages |

## `packages`

Print restored package cache and artifact status.

```bash
tpkg packages
tpkg packages --verbose
```

| Option | Meaning |
| --- | --- |
| `--override <name=path>` | Override dependency source |
| `--verbose` | Print build hash and validation details |

### `packages gc`

Garbage collect unreferenced package caches.

```bash
tpkg packages gc [options]
```

| Option | Meaning |
| --- | --- |
| `--apply` | Delete unreferenced artifacts instead of dry run |
| `--package <name>` | Only consider artifacts for one package |
| `--builds` | Also delete stale package build caches |
| `--sources` | Also delete stale package source caches |
| `--toolchain <id>` | Toolchain profile ID for build cache retention |
| `--config <debug|release>` | Config for build cache retention |

## `tree`

Display dependency tree.

```bash
tpkg tree
```

## `clean`

Clean dependency cache and generated integration files.

```bash
tpkg clean [package] [options]
```

| Option | Meaning |
| --- | --- |
| `package` | Optional dependency to clean |
| `--all` | Remove the whole `.tpkg` directory |
| `--artifacts` | Remove `.tpkg/artifacts` |
| `--sources` | Remove `.tpkg/packages` source cache |
| `--lock` | Remove `tpkg.lock.toml` |

## `override`

Manage local dependency overrides.

```bash
tpkg override list
tpkg override set <name> <path>
tpkg override remove <name>
tpkg override clear
```

## `sdk`

Detect and inspect local SDK/toolchains.

### `sdk detect`

```bash
tpkg sdk detect
```

Detect SDK/toolchains and write `.tpkg/toolchains/host.toml`.

### `sdk list`

```bash
tpkg sdk list [--refresh] [--source auto|project-user|workspace|global-user]
```

### `sdk show`

```bash
tpkg sdk show <id>
```

### `sdk add`

```bash
tpkg sdk add <id> [options]
```

Common options:

```text
--source project-user|global-user
--force
--platform <name>
--compiler-kind <name>
--sdk-kind <name>
--host-arch <arch>
--target-arch <arch>
--target-triple <triple>
--cc <path>
--cxx <path>
--linker <path>
--archiver <path>
--rc <path>
--mt <path>
--strip <path>
--ranlib <path>
--ninja <path>
--git <path>
--include <path>
--lib <path>
--bin <path>
--sysroot <path>
--android-sdk <path>
--android-ndk <path>
--android-api <level>
--android-abi <abi>
--jdk <path>
--adb <path>
--java <path>
--javac <path>
--gradle <path>
--env KEY=VALUE
```

### `sdk remove`

```bash
tpkg sdk remove <id> [--source project-user|global-user]
```

### Other SDK commands

```bash
tpkg sdk doctor
tpkg sdk dump
tpkg sdk select <id>
tpkg sdk clear
```
