# Build Notes for ABC Setlist GUI

## Current Environment: WSL (Windows Subsystem for Linux)

You are currently on WSL, which requires additional setup to build GUI applications.

## Option 1: Build on Native Windows (Recommended)

The easiest way to build this Windows GUI application is to use native Windows tools:

### Using Visual Studio (MSVC)

1. Install **Visual Studio 2019 or later** with "Desktop development with C++" workload
2. Open "x64 Native Tools Command Prompt for VS"
3. Navigate to this directory
4. Build:

```cmd
cd abc-setlist-gui
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

The executable will be at: `build\Release\abc-setlist-gui.exe`

### Using MinGW-w64 on Windows

1. Install **MinGW-w64** from [winlibs.com](https://winlibs.com/)
2. Add MinGW bin directory to PATH
3. Open Windows Command Prompt or PowerShell
4. Build:

```cmd
cd abc-setlist-gui
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
cmake --build .
```

The executable will be at: `build\abc-setlist-gui.exe`

## Option 2: Build on WSL with X11 Support

If you want to build and test on WSL:

### Install Required Packages

```bash
sudo apt-get update
sudo apt-get install -y \
    libx11-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev
```

### Setup Dependencies First

```bash
sudo apt-get update
sudo apt-get install -y \
    libx11-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev
```

### Build

```bash
cd /home/brian/band-helper/abc-setlist-gui
rm -rf build  # Clean any old build
mkdir build
cd build
cmake ..
cmake --build .
```

### Run with X Server

You'll need an X server running on Windows (like VcXsrv or WSLg on Windows 11):

```bash
export DISPLAY=:0
./abc-setlist-gui
```

**Note:** The Linux build will NOT produce a Windows .exe file. It creates a Linux binary.

## Option 3: Cross-Compile for Windows from WSL

This is more complex and requires setting up a MinGW cross-compiler:

### Install MinGW Cross-Compiler

```bash
sudo apt-get install -y mingw-w64 cmake
```

### Create a CMake Toolchain File

Create `cmake/windows-toolchain.cmake`:

```cmake
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
```

### Cross-Compile

```bash
cd /home/brian/band-helper/abc-setlist-gui
mkdir -p build-windows
cd build-windows
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/windows-toolchain.cmake ..
cmake --build .
```

This may require additional Windows libraries and is more complex to set up.

## Recommended Workflow

**For Windows GUI Development:**

1. **Develop in WSL** - Edit code in WSL using your preferred editor
2. **Build on Windows** - Access the same files from Windows (they're in `\\wsl$\Ubuntu\home\brian\band-helper\abc-setlist-gui`)
3. **Run on Windows** - Execute the .exe from Windows Explorer

**Quick Test on Windows:**

From Windows PowerShell or Command Prompt:

```powershell
cd \\wsl$\Ubuntu\home\brian\band-helper\abc-setlist-gui
mkdir build
cd build
cmake ..
cmake --build . --config Release
.\Release\abc-setlist-gui.exe
```

## Current Status

The code is complete and ready to build. The only blocker is the build environment setup. Choose the option that best fits your workflow:

- **Option 1** (Native Windows): Best for final builds and distribution
- **Option 2** (WSL with X11): Good for development if you prefer Linux tools
- **Option 3** (Cross-compile): Advanced, best if you need automated builds

All three options will produce a working application!
