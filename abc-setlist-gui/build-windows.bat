@echo off
REM Build script for Windows (MSVC or MinGW)

echo ================================================
echo ABC Setlist GUI - Windows Build Script
echo ================================================
echo.

REM Check if dependencies are set up
if not exist deps\glfw-3.3.8 (
    echo Dependencies not found. Running setup-dependencies.bat...
    call setup-dependencies.bat
    if errorlevel 1 (
        echo ERROR: Failed to setup dependencies.
        pause
        exit /b 1
    )
    echo.
) else (
    echo Dependencies already set up.
    echo.
)

REM Check if CMake is available
cmake --version >nul 2>&1
if errorlevel 1 (
    echo ERROR: CMake not found. Please install CMake 3.14 or higher.
    echo Download from: https://cmake.org/download/
    pause
    exit /b 1
)

echo CMake found!
echo.

REM Create build directory
if not exist build mkdir build
cd build

REM Configure with CMake
echo Configuring project...
cmake ..
if errorlevel 1 (
    echo.
    echo ERROR: CMake configuration failed.
    echo Make sure you have a C++ compiler installed:
    echo   - Visual Studio 2017+ with C++ tools, OR
    echo   - MinGW-w64
    echo.
    pause
    exit /b 1
)

echo.
echo Building project (Release)...
cmake --build . --config Release
if errorlevel 1 (
    echo.
    echo ERROR: Build failed.
    pause
    exit /b 1
)

echo.
echo ================================================
echo Build completed successfully!
echo ================================================
echo.
echo Executable location:
echo   Release\abc-setlist-gui.exe
echo.
echo To run the application:
echo   cd build\Release
echo   abc-setlist-gui.exe
echo.
echo Or simply double-click: build\Release\abc-setlist-gui.exe
echo.
pause
