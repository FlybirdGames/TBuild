# basic-cmake

Minimal CMake project using ToolkitBuild-generated dependency integration.

```bash
tbuild restore --root examples/basic-cmake --toolchain <id>
tbuild generate --root examples/basic-cmake --toolchain <id>
cmake -S examples/basic-cmake -B build/basic-cmake -G Ninja
cmake --build build/basic-cmake
```
