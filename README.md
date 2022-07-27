# Description
An implementation of free-form Board focused on text posts and their connections. Made with C++ and [Dear ImGui](https://github.com/ocornut/imgui).

# Build instructions

Boards, Boards, Boards uses [Premake](https://github.com/premake/premake-core) to handle solution files. Run VS2022.bat to get an .sln file targeting Visual Studio 2022 or use the command line to run Premake passing the [desired target](https://premake.github.io/docs/Using-Premake/) as a parameter:

Code:
```Bash
$ cd premake/
$ premake5.exe --file=../premake5.lua <desired-target>
```

# Dependencies

For ease of use, all the dependencies files are already included in the repository.

| Library  | License |
| ------------- | ------------- |
| [Dear ImGui](https://github.com/ocornut/imgui)  | [MIT](https://github.com/ocornut/imgui/blob/master/LICENSE.txt)  |
| [Lua](http://www.lua.org/home.html)  | [MIT](http://www.lua.org/license.html)  |
| [LuaBinaries](http://luabinaries.sourceforge.net/)  | [Compatible with GPL](http://luabinaries.sourceforge.net/license.html)  |
| [Sol2](https://github.com/ThePhD/sol2/)  | [MIT](https://github.com/ThePhD/sol2/blob/develop/LICENSE.txt)  |
| [Premake](https://github.com/premake/premake-core)  | [BSD-3-Clause](https://github.com/premake/premake-core/blob/master/LICENSE.txt)  |
| [Catch2](https://github.com/catchorg/Catch2/)  | [BSL-1.0](https://github.com/catchorg/Catch2/blob/devel/LICENSE.txt)  |
| [SDL2](https://www.libsdl.org/index.php)  | [Zlib](https://www.libsdl.org/license.php)  |
| [Serpent](https://github.com/pkulchenko/serpent) | [MIT](https://github.com/pkulchenko/serpent/blob/master/LICENSE)  |
| [NativeFileDialog](https://github.com/mlabbe/nativefiledialog)  | [Zlib](https://github.com/mlabbe/nativefiledialog/blob/master/LICENSE)  |