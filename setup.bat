@echo off
title TrakKita Setup
color 0B

echo.
echo  ================================================
echo   TrakKita - Dependency Installer
echo   CS103 Group 4
echo  ================================================
echo.

:: ── Step 1: Check for MSYS2 ───────────────────────────────────
echo [1/3] Checking for MSYS2...

if not exist "C:\msys64\usr\bin\pacman.exe" (
    echo.
    echo  [ERROR] MSYS2 is not installed or not found at C:\msys64
    echo.
    echo  Please install MSYS2 first:
    echo    1. Go to https://www.msys2.org
    echo    2. Download and run the installer
    echo    3. Install to the DEFAULT path: C:\msys64
    echo    4. Run this script again after installing
    echo.
    pause
    exit /b 1
)

echo  [OK] MSYS2 found.
echo.

:: ── Step 2: Install GCC and Make via pacman ───────────────────
echo [2/3] Installing GCC and Make...
echo.

"C:\msys64\usr\bin\pacman.exe" -S --noconfirm --needed ^
    mingw-w64-ucrt-x86_64-gcc ^
    mingw-w64-ucrt-x86_64-make

if %errorlevel% neq 0 (
    echo.
    echo  [ERROR] Package installation failed.
    echo  Try opening the MSYS2 UCRT64 terminal and running:
    echo    pacman -Syu
    echo  Then run this script again.
    echo.
    pause
    exit /b 1
)

echo.
echo  [OK] GCC and Make installed.
echo.

:: ── Step 3: Add to PATH if not already there ─────────────────
echo [3/3] Checking PATH...

powershell -Command "$p = [System.Environment]::GetEnvironmentVariable('Path','Machine'); if ($p -notlike '*msys64\ucrt64\bin*') { [System.Environment]::SetEnvironmentVariable('Path', $p + ';C:\msys64\ucrt64\bin', 'Machine'); Write-Host '  [OK] Added C:\msys64\ucrt64\bin to system PATH.' } else { Write-Host '  [OK] Already in PATH. Nothing to add.' }"

echo.

:: ── Done ──────────────────────────────────────────────────────
echo  ================================================
echo   Setup complete!
echo.
echo   IMPORTANT: Restart your terminal or VS Code
echo   before building, so PATH changes take effect.
echo.
echo   Then navigate to this folder and run:
echo     mingw32-make run
echo  ================================================
echo.
pause
