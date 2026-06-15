# Command reference

## Global options

```bash
tbuild [--root <path>] <command> [options]
```

| Option | Meaning |
| --- | --- |
| `--root <path>` | Workspace root containing `tbuild.deps.lua` |
| `--help` | Show help |

## `doctor`

Check the local ToolkitBuild runtime.

```bash
tbuild doctor
tbuild doctor --sdk
```

`--sdk` prints detailed SDK and toolchain diagnostics.

## `load`

Load and validate `tbuild.deps.lua`.

```bash
tbuild load
```

Use this before restore when checking DSL syntax and model validation.

## `dump-model`

Print the loaded dependency model as JSON.

```bash
tbuild dump-model
```

## `deps`

Print dependencies declared in the manifest.

```bash
tbuild deps
```

## `generate`

Generate CMake dependency integration files.

```bash
tbuild generate [options]
```

| Option | Meaning |
| --- | --- |
| `--toolchain <id>` | Preferred local toolchain profile ID |
| `--config <debug|release>` | Build config |
| `--output <dir>` | Output directory; default `.tbuild/generated/cmake` |
| `--override <name=path>` | Override dependency source |

## `restore`

Prepare package cache, build/export dependency artifacts, and update lock state.

```bash
tbuild restore [package] [options]
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
tbuild update [package] [options]
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
tbuild packages
tbuild packages --verbose
```

| Option | Meaning |
| --- | --- |
| `--override <name=path>` | Override dependency source |
| `--verbose` | Print build hash and validation details |

### `packages gc`

Garbage collect unreferenced package caches.

```bash
tbuild packages gc [options]
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
tbuild tree
```

## `clean`

Clean dependency cache and generated integration files.

```bash
tbuild clean [package] [options]
```

| Option | Meaning |
| --- | --- |
| `package` | Optional dependency to clean |
| `--all` | Remove the whole `.tbuild` directory |
| `--artifacts` | Remove `.tbuild/artifacts` |
| `--sources` | Remove `.tbuild/packages` source cache |
| `--lock` | Remove `tbuild.lock.toml` |

## `override`

Manage local dependency overrides.

```bash
tbuild override list
tbuild override set <name> <path>
tbuild override remove <name>
tbuild override clear
```

## `sdk`

Detect and inspect local SDK/toolchains.

### `sdk detect`

```bash
tbuild sdk detect
```

Detect SDK/toolchains and write `.tbuild/toolchains/host.toml`.

### `sdk list`

```bash
tbuild sdk list [--refresh] [--source auto|project-user|workspace|global-user]
```

### `sdk show`

```bash
tbuild sdk show <id>
```

### `sdk add`

```bash
tbuild sdk add <id> [options]
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
tbuild sdk remove <id> [--source project-user|global-user]
```

### Other SDK commands

```bash
tbuild sdk doctor
tbuild sdk dump
tbuild sdk select <id>
tbuild sdk clear
```
