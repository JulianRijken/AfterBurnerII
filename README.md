# GameDevEngine2

Reimplementation of the DAE Programming 2 engine, aiming to make ports as frictionless as possible.

This allows porting existing games to Linux and web, and potentially other platforms. Click [here](https://allpurposem.at/tdyd.html) to see an example game!

## Porting
Please read through the template in `src`. It has some tips for things such as window scaling that might be useful.

Start by copying your game files to the `src` folder, and resources to the `resources` folder. Keep the original `main.cpp` and simply change the window title & size!

Try to build it. If coming from MSVC (Microsoft Visual Studio's compiler), **some build errors are expected** as this compiler behaves slightly differently. Solving these varies per project, but here are some tips:

- templates must be implemented in the header file.
- certain newer C++ features are not fully implemented by clang/emscripten (web compiler), so you will need to switch to the `fmt` library if you use `std::format`, or `std::algorithm` instead of `std::ranges`.
- paths in POSIX are cAsE SeNsItIvE, so files won't be found if the casing does not match!
- certain C math functions that MSVC puts in the global scope, such as `abs`, need to be included with `#include <cmath>`, and require specifying the namespace (e.g. `std::abs`).

## Installing Linux in 1 minute
Linux is required for game development. If you are using Microsoft Windows and cannot dual boot, you can use the Microsoft Windows Subsystem for Linux, which runs Linux (at degraded performance) under Microsoft Windows. It is installable by opening a PowerShell prompt and typing `wsl --install`. You will be prompted to reboot, and can then access an install of Ubuntu Linux.

## Linux setup
Make sure you have the basic development packages installed (`git`, `cmake`, and any C++ compiler).

Arch Linux:
```sh
sudo pacman -S base-devel
```
Ubuntu:
```sh
sudo apt update && sudo apt install git g++ cmake lbzip2
```

Make sure you have the dependencies installed (`SDL2`, `SDL2_image`, `SDL2_mixer`, and `SDL2_ttf`).

Arch Linux:
```sh
sudo pacman -S sdl2 sdl2_image sdl2_mixer sdl2_ttf
```
Ubuntu:
```sh
sudo apt install libsdl2-dev libsdl2_image-dev libsdl2_mixer-dev libsdl2_ttf-dev
```

## Building

Clone this repository with `git clone https://git.allpurposem.at/mat/GameDevEngine2`.<br>

You can customize the binary's name, as well as the aspect ratio (for web), by changing the variables at the top of the `CMakeLists.txt` file.

### Linux
This project uses CMake, so you can use your preferred method of buildin CMake projects. I have included a script `build_linux.sh` which automates an out-of-tree build and outputs into `build/linux`. You will be prompted whether to run the game at the end.

### Web
Make sure you have the Emscripten SDK installed. This is done differently depending on your distribution. You can follow the official instructions [here](https://emscripten.org/docs/getting_started/downloads.html). If you are using Arch Linux, you can also install it with the following command:
```sh
sudo pacman -S emscripten
```

Then, you can use the `build_web.sh` script to build your game. The build script will prompt you whether to run the game at the end, which opens a web browser to the game.

NOTE: You cannot run the game just by double-clicking the HTML file! You must either use `emrun` or start a HTTP server serving the right folder (e.g. `python -m http.server`).

The resources folder will be packaged into it, so for redistribution only the `.data`, `.html`, `.js`, and `.wasm` files should be required.