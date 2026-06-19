<!--
 * @Author: cubevlmu khfahqp@gmail.com
 * @LastEditors: cubevlmu khfahqp@gmail.com
 * Copyright (c) 2026 by FlybirdGames, All Rights Reserved. 
-->
# basic-cmake

Minimal CMake project using Toolkit Package Manager generated dependency integration.

```bash
tpkg restore --root examples/basic-cmake --toolchain <id>
tpkg generate --root examples/basic-cmake --toolchain <id>
cmake -S examples/basic-cmake -B build/basic-cmake -G Ninja
cmake --build build/basic-cmake
```
