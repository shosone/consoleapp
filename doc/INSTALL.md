# How to build and install

## Unix/Linux/Cygwin

`readline` is dependency library for building sample.

### CMake

```bash
mkdir -p build
cd build
cmake ..
make
make install
```

### autotools

Introduction of autotools is under consideration.

### GnuMake

```bash
make
make install
```

## Windows

### Visual Studio

In Windows, the API provided from `termios.h` is completely different from Win32API' terminal API.

So, we cannot provide prompt API.

When you want prompt API, use cygwin.

[vcpkg](https://github.com/Microsoft/vcpkg) is convenient to build and install `readline`

```ps
cd <vcpkg dir>
.\vcpkg install readline:x86-windows readline:<arch>-windows
```

* `<vcpkg dir>`: The path where you clone vcpkg.
* `<arch>`: Your PC's arch. ex.) `x86`, `x64`

1. Start `cmake-gui.exe`
2. Specify `Where is the source code:` and `where to build the binaries:`.
3. Press `Configure` button.
4. Put a check mark to `Advanced` and specify `Readline_ROOT_DIR` as `<vcpkg dir>/installed/<arch>-windows`
5. Press `Configure` button again.
6. Press `Generate` button.
7. Press `Open Project` button.
8. In solution explorer, `ALL_BUILD` -> `Build`
