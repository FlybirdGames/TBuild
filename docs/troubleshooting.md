# Troubleshooting

## `tpkg.lua` cannot be found

Run commands from the workspace root or pass `--root <path>`.

```bash
tpkg restore --root path/to/project
```

## No toolchain profile is available

Run:

```bash
tpkg sdk detect
tpkg sdk list
```

If detection does not find the expected SDK, register a profile manually with `tpkg sdk add`.

## Dependency restore uses the wrong compiler

Pass an explicit toolchain profile:

```bash
tpkg restore --toolchain windows-msvc-x64
```

To save a local preference:

```bash
tpkg sdk select windows-msvc-x64
```

## A dependency builds but CMake cannot link it

Check the dependency `artifacts` declaration. At minimum, verify:

- `include_dirs` points to exported headers.
- `libs` or `lib_files` names the libraries actually produced by the dependency.
- `lib_dirs` points to directories that contain the libraries.
- Debug/release library names are configured if they differ.

Then rerun:

```bash
tpkg restore --export-only <package>
tpkg generate
```

## A package cache is stale

Use targeted clean first:

```bash
tpkg clean <package> --artifacts --sources
```

Use full clean only when necessary:

```bash
tpkg clean --all
```

## Lockfile mismatch

If you want exact reproducibility, use:

```bash
tpkg restore --locked
```

If you intentionally want a new resolved commit or source state, use:

```bash
tpkg update <package>
tpkg restore
```
