# Package metadata

## `package(name)`

Declares the root package name.

```lua
package("MyProject")
```

## `version(value)`

Declares the root package version.

```lua
version("0.1.0")
```

## `default_config(value)`

Sets the default build configuration.

```lua
default_config("debug")
default_config("release")
```

## `default_platform(value)`

Sets the default target platform.

```lua
default_platform("host")
default_platform("windows")
default_platform("linux")
default_platform("macos")
default_platform("android")
```

## `default_arch(value)`

Sets the default target architecture.

```lua
default_arch("x64")
default_arch("x86")
default_arch("arm64")
default_arch("arm")
```

## `include(path)`

Includes another manifest fragment relative to the current manifest.

```lua
include("deps/common.lua")
include("deps/graphics.lua")
```

Use includes to keep large dependency manifests readable.
