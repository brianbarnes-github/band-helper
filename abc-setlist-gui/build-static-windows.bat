@echo off
REM Static Build Script for Windows
REM This script builds a fully static ABC Setlist Manager executable

echo ========================================
echo ABC Setlist Manager - Static Build
echo ========================================
echo.

REM Check for CMake
where cmake >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] CMake not found!
    echo Please install CMake from https://cmake.org/download/
    echo Make sure to add CMake to your PATH during installation.
    pause
    exit /b 1
)
echo [OK] CMake found

REM Check for MinGW
where g++ >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo [OK] MinGW g++ compiler found
    set COMPILER=MinGW
    goto :build_mingw
)

REM Check for MSVC
where cl >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo [OK] MSVC compiler found
    set COMPILER=MSVC
    goto :build_msvc
)

echo [ERROR] No compiler found!
echo.
echo Please install one of the following:
echo   1. MinGW-w64: https://winlibs.com/ (recommended - easier setup)
echo   2. Visual Studio with "Desktop development with C++" workload
echo.
echo If you installed MinGW or Visual Studio, make sure the compiler is in your PATH.
echo For Visual Studio, run this script from "x64 Native Tools Command Prompt for VS"
pause
exit /b 1

:build_mingw
echo.
echo ========================================
echo Building with MinGW (Static)
echo ========================================
echo.

REM Check and setup local dependencies first
if not exist deps\glfw-3.3.8 (
    echo Setting up local dependencies...
    call setup-dependencies.bat
    if %ERRORLEVEL% NEQ 0 (
        echo [ERROR] Failed to setup dependencies!
        pause
        exit /b 1
    )
) else (
    echo [OK] Local dependencies found
)

echo.

REM Clean old build
if exist build (
    echo Cleaning old build directory...
    rmdir /s /q build
)

REM Create build directory
mkdir build
cd build

echo.
echo [Step 1/3] Configuring CMake for static build...
echo Configuration: Static linking enabled, using local dependencies
cmake -G "MinGW Makefiles" .. -DBUILD_STATIC=ON -DCMAKE_BUILD_TYPE=Release -DUSE_LOCAL_DEPS=ON
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] CMake configuration failed!
    echo.
    echo Troubleshooting tips:
    echo - Make sure MinGW bin directory is in PATH
    echo - Try running: where g++
    echo - Ensure CMake can find g++
    cd ..
    pause
    exit /b 1
)

echo.
echo [Step 2/3] Building static executable...
echo This may take a few minutes on first build (downloading dependencies)...
mingw32-make -j4
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Build failed!
    echo Check the error messages above for details.
    cd ..
    pause
    exit /b 1
)

cd ..
goto :success

:build_msvc
echo.
echo ========================================
echo Building with MSVC (Static)
echo ========================================
echo.

REM Check and setup local dependencies first
if not exist deps\glfw-3.3.8 (
    echo Setting up local dependencies...
    call setup-dependencies.bat
    if %ERRORLEVEL% NEQ 0 (
        echo [ERROR] Failed to setup dependencies!
        pause
        exit /b 1
    )
) else (
    echo [OK] Local dependencies found
)

echo.

REM Clean old build
if exist build (
    echo Cleaning old build directory...
    rmdir /s /q build
)

REM Create build directory
mkdir build
cd build

echo.
echo [Step 1/3] Configuring CMake for static build...
echo Configuration: Static linking enabled, using local dependencies
cmake .. -DBUILD_STATIC=ON -DUSE_LOCAL_DEPS=ON
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] CMake configuration failed!
    cd ..
    pause
    exit /b 1
)

echo.
echo [Step 2/3] Building executable...
echo This may take a few minutes...
cmake --build . --config Release -- /maxcpucount
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Build failed!
    cd ..
    pause
    exit /b 1
)

cd ..
goto :success

:success
echo.
echo ========================================
echo [SUCCESS] Build Complete!
echo ========================================
echo.

if "%COMPILER%"=="MinGW" (
    echo Your static executable is at:
    echo   build\abc-setlist-gui.exe
    echo.
    echo [Step 3/3] Verifying static linking...
    echo.
    if exist build\abc-setlist-gui.exe (
        echo Executable size:
        dir build\abc-setlist-gui.exe | find "abc-setlist-gui.exe"
        echo.
        echo To run the application:
        echo   build\abc-setlist-gui.exe
        echo.
        echo You can copy this single .exe file to any Windows PC and run it!
        echo No DLLs needed (except standard Windows system DLLs).
    )
) else (
    echo Your static executable is at:
    echo   build\Release\abc-setlist-gui.exe
    echo.
    echo [Step 3/3] Verifying static linking...
    echo.
    if exist build\Release\abc-setlist-gui.exe (
        echo Executable size:
        dir build\Release\abc-setlist-gui.exe | find "abc-setlist-gui.exe"
        echo.
        echo To run the application:
        echo   build\Release\abc-setlist-gui.exe
        echo.
        echo You can copy this single .exe file to any Windows PC and run it!
        echo No DLLs needed (except standard Windows system DLLs).
    )
)

echo.
echo ========================================
echo Next Steps:
echo ========================================
echo 1. Test the executable by running it
echo 2. Copy the .exe to your desired location
echo 3. Distribute without any DLL files!
echo.
pause
