# Skylight Lua Test
This is a test project for integrating Lua with C++ code. Included documentation is minimal. Functionality is also limited, with a highly limited version of Lua included and only a handful of custom functions to expand upon.

Includes a CMake file. You probably know how to use CMake.

This project uses a custom CMake based Lua build system, omitting a lot of functionality from the original Make build system. This CMakeLists is included, however the rest of the Lua source is not.

Example scripts are provided in `examples`. Put this in `build/{release mode}/scripts`, next to the compiled executable.

## Skylight Lua Library
- `skylight.getTime()` - returns time since program start in milliseconds. Does include time before Lua script(s) ran.
- `skylight.getConfigProperty(name: string)` - returns a string with the property's value from the `config.xml` file. Case sensitive.
- `skylight.setConfigProperty(key:string, value:[string, number, bool])` - set a property in the `config.xml` file. Returns true on success. Will override property (case sensitive) if found, will create new property otherwise. Saves file immediately.

## Callbacks
- `onModLoad` - called when the lua script is ran. Like most Lua integrations, the script is ran from top to bottom once (excluding functions unless called) before the C++ side calls any functions. The Skylight library is ready to use immediately outside of any functions.
- `onTick(delta: number)` - Called every tick (in this demo, 5 times with a constant delta of `0.016`)

## Other
- `table.length(tbl)` - returns rawlen of table
- `require(module)` - custom `require` implementation that will only search for **Lua** files in the `scripts` directory. Will not load packages from luarocks or C files. Can probably do relative directories?

Technology credits:
- [Lua 5.4.7](https://lua.org) (Lua scripting)
- [SCL](https://github.com/MerianBerry/SCL) (XML processing)