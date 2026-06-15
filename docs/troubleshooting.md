# Troubleshooting

## `tbuild.deps.lua` cannot be found

Run commands from the workspace root or pass `--root <path>`.

```bash
tbuild restore --root path/to/project
```

## No toolchain profile is available

Run:

```bash
tbuild sdk detect
tbuild sdk list
```

If detection does not find the expected SDK, register a profile manually with `tbuild sdk add`.

## Dependency restore uses the wrong compiler

Pass an explicit toolchain profile:

```bash
tbuild restore --toolchain windows-msvc-x64
```

To save a local preference:

```bash
tbuild sdk select windows-msvc-x64
```

## A dependency builds but CMake cannot link it

Check the dependency `artifacts` declaration. At minimum, verify:

- `include_dirs` points to exported headers.
- `libs` or `lib_files` names the libraries actually produced by the dependency.
- `lib_dirs` points to directories that contain the libraries.
- Debug/release library names are configured if they differ.

Then rerun:

```bash
tbuild restore --export-only <package>
tbuild generate
```

## A package cache is stale

Use targeted clean first:

```bash
tbuild clean <package> --artifacts --sources
```

Use full clean only when necessary:

```bash
tbuild clean --all
```

## Lockfile mismatch

If you want exact reproducibility, use:

```bash
tbuild restore --locked
```

If you intentionally want a new resolved commit or source state, use:

```bash
tbuild update <package>
tbuild restore
```
