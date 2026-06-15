# Toolchain detection

Toolchain detection discovers compilers, SDKs, and helper tools.

## Responsibilities

- Detect host desktop toolchains.
- Detect Android SDK/NDK profiles when configured or discoverable.
- Serialize discovered profiles.
- Merge auto, workspace, project-user, and global-user profiles.
- Provide diagnostics when required tools are missing.

## Target split

- `WindowsToolchainDetector.cpp`: MSVC, clang-cl, MinGW, Windows SDK.
- `AndroidToolchainDetector.cpp`: Android SDK, NDK, ABI/API, JDK/ADB/Gradle tools.
- `UnixToolchainDetector.cpp`: Linux/macOS clang/gcc-style toolchains.
- `ToolchainProbe.cpp`: shared low-level path, environment, executable, and version probing.

## Invariants

- Existing profile IDs should remain stable.
- User-registered profiles must not be overwritten unless `--force` or a matching command explicitly does so.
- Detection should degrade gracefully when optional SDKs are missing.
