@echo off
setlocal enabledelayedexpansion

echo ========================================
echo Setting up dependencies for ABC Setlist GUI
echo ========================================
echo.

REM Create deps directory
if not exist deps mkdir deps
cd deps

REM Download GLFW
echo [1/2] Downloading GLFW 3.3.8...
if not exist glfw-3.3.8 (
    curl -L -o glfw.zip https://github.com/glfw/glfw/releases/download/3.3.8/glfw-3.3.8.zip
    if errorlevel 1 (
        echo ERROR: Failed to download GLFW
        pause
        exit /b 1
    )
    echo Extracting GLFW...
    tar -xf glfw.zip
    del glfw.zip
    echo GLFW ready!
) else (
    echo GLFW already exists, skipping...
)

echo.

REM Download ImGui
echo [2/2] Downloading Dear ImGui 1.90.1...
if not exist imgui-1.90.1 (
    curl -L -o imgui.zip https://github.com/ocornut/imgui/archive/refs/tags/v1.90.1.zip
    if errorlevel 1 (
        echo ERROR: Failed to download ImGui
        pause
        exit /b 1
    )
    echo Extracting ImGui...
    tar -xf imgui.zip
    del imgui.zip
    echo ImGui ready!
) else (
    echo ImGui already exists, skipping...
)

cd ..

echo.
echo ========================================
echo Dependencies setup complete!
echo ========================================
echo.
echo Next steps:
echo   1. Run: mkdir build
echo   2. Run: cd build
echo   3. Run: cmake -G "MinGW Makefiles" -DUSE_LOCAL_DEPS=ON ..
echo   4. Run: cmake --build .
echo.
pause
