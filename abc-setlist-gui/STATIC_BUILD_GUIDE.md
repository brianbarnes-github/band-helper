# Static Build Guide for ABC Setlist Manager

This guide explains how to build a **fully static executable** that doesn't require deploying separate library files.

## What Has Been Modified

The `CMakeLists.txt` has been updated with the following changes:

1. **Added BUILD_STATIC option** (enabled by default)
2. **Static linking flags** for both Windows and Linux
3. **GLFW built as static library** instead of shared
4. **All dependencies statically linked** into the executable

## Build Options

### Option 1: Static Build on WSL/Linux (Recommended for Development)

This creates a Linux static executable you can test on WSL.

**Step 1: Install Required Dependencies**

Run this command first:
```bash
sudo apt-get update && sudo apt-get install -y \
    libx11-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev
```

**Step 2: Build Static Executable**

```bash
cd /home/brian/band-helper/abc-setlist-gui
rm -rf build
mkdir build
cd build
cmake .. -DBUILD_STATIC=ON -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

**Step 3: Verify Static Linking**

Check that the executable doesn't depend on many shared libraries:
```bash
ldd abc-setlist-gui
```

You should see minimal dependencies - most libraries should be statically linked. Only system libraries like `libc`, `libdl`, `libpthread`, and `libGL` will remain (these are always dynamic on Linux).

**Step 4: Test the Executable**

```bash
./abc-setlist-gui
```

### Option 2: Static Build on Windows (Recommended for Distribution)

This creates a Windows .exe that can run without DLL files.

**Prerequisites:**
- MinGW-w64 installed (recommended) OR Visual Studio
- CMake 3.14+

**Step 2a: Using MinGW-w64 on Windows**

Open Windows Command Prompt or PowerShell:

```cmd
cd \\wsl$\Ubuntu\home\brian\band-helper\abc-setlist-gui
rmdir /s /q build
mkdir build
cd build
cmake -G "MinGW Makefiles" .. -DBUILD_STATIC=ON -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

The static executable will be at: `build\abc-setlist-gui.exe`

**Step 2b: Using Visual Studio (MSVC) on Windows**

Open "x64 Native Tools Command Prompt for VS":

```cmd
cd \\wsl$\Ubuntu\home\brian\band-helper\abc-setlist-gui
rmdir /s /q build
mkdir build
cd build
cmake .. -DBUILD_STATIC=ON
cmake --build . --config Release
```

The static executable will be at: `build\Release\abc-setlist-gui.exe`

**Step 3: Verify No DLL Dependencies**

Use Dependency Walker or:
```cmd
dumpbin /dependents abc-setlist-gui.exe
```

You should only see system DLLs like:
- `KERNEL32.dll`
- `USER32.dll`
- `GDI32.dll`
- `OPENGL32.dll`

No GLFW DLL or other library DLLs should be listed.

## Technical Details

### What Gets Statically Linked

✅ **GLFW** - windowing library
✅ **Dear ImGui** - GUI framework (already compiled into executable)
✅ **C++ Standard Library** - libstdc++ or MSVCRT
✅ **GCC Runtime** - libgcc (on MinGW)

### What Cannot Be Statically Linked

Some libraries are always dynamically linked:

**On Linux:**
- `libGL.so` - OpenGL driver (hardware-dependent)
- `libX11.so` - X Window System
- `libc.so` - C standard library
- `libdl.so`, `libpthread.so` - system libraries

**On Windows:**
- `OPENGL32.dll` - OpenGL driver (hardware-dependent)
- `KERNEL32.dll`, `USER32.dll` - Windows system DLLs

These are present on all systems and don't need to be deployed.

## Deployment

### Linux Deployment

The executable can be copied to any Linux system with:
- OpenGL drivers installed
- X11 display server
- Same or newer glibc version

```bash
# Copy executable to target system
cp abc-setlist-gui /path/to/deploy/

# Run on target system
./abc-setlist-gui
```

### Windows Deployment

The executable can be copied to any Windows system with:
- OpenGL support (virtually all modern Windows)
- No additional DLLs needed!

```cmd
REM Copy executable to target system
copy abc-setlist-gui.exe C:\MyPrograms\

REM Run on target system
C:\MyPrograms\abc-setlist-gui.exe
```

## Troubleshooting

### "Cannot find libstdc++.a" on Linux

Install static C++ library:
```bash
sudo apt-get install libstdc++-13-dev
```

### "Cannot find -lstdc++" on Windows MinGW

Make sure you're using MinGW-w64, not MinGW-w32. MinGW-w64 includes static libraries.

### Executable is Large

This is normal for static builds. The executable includes:
- All GLFW code
- All ImGui code
- C++ standard library
- Application code

Typical size: 2-5 MB (vs <1 MB for dynamic builds + DLLs)

### Still Shows DLL Dependencies on Windows

If using MSVC, you may need to change the runtime library setting:
```cmake
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
```

Add this to CMakeLists.txt after the project() line.

## Disabling Static Build

To go back to dynamic linking:

```bash
cmake .. -DBUILD_STATIC=OFF
```

Or edit CMakeLists.txt:
```cmake
option(BUILD_STATIC "Build static executable" OFF)
```

## Summary

With these changes, you now have:
- ✅ **Single executable file** - no separate DLL/SO files needed
- ✅ **Portable** - copy to any compatible system and run
- ✅ **No library deployment** - all dependencies built-in
- ✅ **Same functionality** - works exactly like dynamic build

The only system requirements are:
- Windows: OpenGL support (standard on all Windows)
- Linux: OpenGL drivers and X11 (standard on desktop Linux)
