# Windows Static Build - Step by Step Instructions

Follow these steps to build a static Windows executable from your WSL environment.

## Prerequisites Check

Before starting, you need:
- ✅ **CMake** (version 3.14 or higher)
- ✅ **A C++ compiler** (MinGW-w64 OR Visual Studio)

## Step 1: Install Required Software (if needed)

### Option A: Install MinGW-w64 (Recommended - Simpler)

1. **Download MinGW-w64**:
   - Go to: https://winlibs.com/
   - Download the latest "Win64" version with UCRT runtime
   - Example: `winlibs-x86_64-posix-seh-gcc-13.2.0-mingw-w64ucrt-11.0.1-r5.zip`

2. **Extract to a simple path**:
   - Extract to `C:\mingw64\` (or similar)
   - Final structure: `C:\mingw64\bin\g++.exe`

3. **Add to PATH**:
   - Search Windows for "Environment Variables"
   - Edit "Path" in System Variables
   - Add: `C:\mingw64\bin`
   - Click OK

4. **Verify installation**:
   - Open Command Prompt (cmd)
   - Type: `g++ --version`
   - Should show GCC version info

### Option B: Install Visual Studio (Alternative)

1. Download Visual Studio Community (free) from: https://visualstudio.microsoft.com/
2. During installation, select "Desktop development with C++"
3. Install and restart

### Install CMake (if not already installed)

1. **Download CMake**:
   - Go to: https://cmake.org/download/
   - Download the Windows installer (.msi)
   - Example: `cmake-3.28.0-windows-x86_64.msi`

2. **Install CMake**:
   - Run the installer
   - **Important**: Check "Add CMake to system PATH for all users"
   - Complete installation

3. **Verify installation**:
   - Open new Command Prompt
   - Type: `cmake --version`
   - Should show CMake version

## Step 2: Access Your Project Files from Windows

Your WSL files are accessible from Windows at this path:

```
\\wsl$\Ubuntu\home\brian\band-helper\abc-setlist-gui
```

**Two ways to access:**

### Method 1: File Explorer
1. Open File Explorer
2. In the address bar, type: `\\wsl$\Ubuntu\home\brian\band-helper\abc-setlist-gui`
3. Press Enter
4. You should see your project files

### Method 2: Map as Network Drive (Optional)
1. In File Explorer, right-click "This PC"
2. Click "Map network drive"
3. Choose a drive letter (e.g., Z:)
4. Enter path: `\\wsl$\Ubuntu\home\brian\band-helper\abc-setlist-gui`
5. Now you can access as `Z:\`

## Step 3: Build the Static Executable

### Using the Build Script (Easiest)

1. **Open Command Prompt or PowerShell** (NOT WSL):
   - Press `Win + R`
   - Type: `cmd`
   - Press Enter

2. **Navigate to your project**:
   ```cmd
   cd \\wsl$\Ubuntu\home\brian\band-helper\abc-setlist-gui
   ```

3. **Run the build script**:
   ```cmd
   build-static-windows.bat
   ```

4. **Wait for build to complete**:
   - Script will check for tools
   - Download dependencies via CMake (first time only)
   - Compile all source files
   - Create static executable

### Manual Build (Alternative)

If you prefer to build manually:

**For MinGW:**
```cmd
cd \\wsl$\Ubuntu\home\brian\band-helper\abc-setlist-gui
rmdir /s /q build
mkdir build
cd build
cmake -G "MinGW Makefiles" .. -DBUILD_STATIC=ON -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

**For Visual Studio** (from "x64 Native Tools Command Prompt for VS"):
```cmd
cd \\wsl$\Ubuntu\home\brian\band-helper\abc-setlist-gui
rmdir /s /q build
mkdir build
cd build
cmake .. -DBUILD_STATIC=ON
cmake --build . --config Release
```

## Step 4: Locate Your Executable

After successful build:

**MinGW builds:**
- Location: `build\abc-setlist-gui.exe`

**Visual Studio builds:**
- Location: `build\Release\abc-setlist-gui.exe`

## Step 5: Test the Executable

1. **Run from command prompt**:
   ```cmd
   cd \\wsl$\Ubuntu\home\brian\band-helper\abc-setlist-gui
   build\abc-setlist-gui.exe
   ```

2. **Or double-click** the .exe in File Explorer

3. **The application should open** with the GUI

## Step 6: Verify Static Build (Optional)

To confirm all libraries are statically linked:

1. **Check file size**:
   - Static build: ~2-5 MB
   - Dynamic build: <1 MB (but needs DLLs)

2. **Test on another PC**:
   - Copy just the .exe to another Windows computer
   - Run it - should work without any DLLs!

## Common Issues and Solutions

### Issue: "CMake not found"
**Solution**:
- Make sure CMake is installed
- Restart Command Prompt after installation
- CMake must be in PATH

### Issue: "g++ not found" or "cl not found"
**Solution**:
- Install MinGW or Visual Studio
- Add MinGW to PATH
- For VS, use "x64 Native Tools Command Prompt"

### Issue: "Access denied" to WSL path
**Solution**:
- Make sure WSL is running
- Try: `wsl --list --running`
- Start WSL if needed: `wsl`

### Issue: Build fails with "cannot find -lstdc++"
**Solution**:
- Make sure you're using MinGW-w64, not old MinGW
- Re-download from winlibs.com

### Issue: Downloads fail during CMake
**Solution**:
- Check internet connection
- CMake downloads GLFW and ImGui on first build
- If behind proxy, configure CMake proxy settings

## Where Are the Dependencies?

The first time you build, CMake will download:
- **GLFW** 3.3.8 (windowing library)
- **Dear ImGui** 1.90.1 (GUI framework)

These are stored in:
```
build\_deps\
```

Subsequent builds will be much faster since dependencies are cached.

## Distribution

Once built, you can:

1. **Copy the single .exe file** to any location
2. **Distribute to other Windows PCs** - no installation needed
3. **No DLLs required** - everything is built in!

The only requirement is:
- Windows 7 or later
- OpenGL support (virtually all Windows PCs have this)

## Quick Reference

| Task | MinGW Command | MSVC Command |
|------|---------------|--------------|
| Configure | `cmake -G "MinGW Makefiles" .. -DBUILD_STATIC=ON` | `cmake .. -DBUILD_STATIC=ON` |
| Build | `cmake --build .` | `cmake --build . --config Release` |
| Output | `build\abc-setlist-gui.exe` | `build\Release\abc-setlist-gui.exe` |
| Clean | `rmdir /s /q build` | `rmdir /s /q build` |

## Summary

✅ Install CMake and MinGW (or Visual Studio)
✅ Access WSL files from Windows
✅ Run `build-static-windows.bat`
✅ Find executable in `build\` directory
✅ Distribute single .exe file - no DLLs needed!

---

**Need Help?**
- Check build output for specific error messages
- Make sure all prerequisites are installed
- Verify PATH includes CMake and compiler
