@echo off
setlocal

REM Parameters:
REM   %1 = Source DLL path (complete path including file name, directory name, and drive letter)
REM   %2 = Target AppX directory (directory path)

if "%~1"=="" (
    echo [CopyDllToAppx][ERROR] Source DLL path is not specified.
    exit /b 1
)

if "%~2"=="" (
    echo [CopyDllToAppx][ERROR] Target AppX directory is not specified.
    exit /b 1
)

set "SRC_DLL=%~1"
set "APPX_DIR=%~2"

echo [CopyDllToAppx] SRC_DLL = "%SRC_DLL%"
echo [CopyDllToAppx] APPX_DIR = "%APPX_DIR%"

REM   Check whether the DLL exists. If not, report an error and terminate the build
if not exist "%SRC_DLL%" (
    echo [CopyDllToAppx][ERROR] "%SRC_DLL%" not found.
    exit /b 1
)

REM   Ensure that the target directory exists
if not exist "%APPX_DIR%" (
    mkdir "%APPX_DIR%"
    if errorlevel 1 (
        echo [CopyDllToAppx][ERROR] Failed to create directory "%APPX_DIR%".
        exit /b 1
    )
)

REM   Copy DLL, terminate the build if failed
xcopy "%SRC_DLL%" "%APPX_DIR%\" /Y /I
if errorlevel 1 (
    echo [CopyDllToAppx][ERROR] Failed to copy "%SRC_DLL%" to "%APPX_DIR%".
    exit /b 1
)

echo [CopyDllToAppx] Done.
endlocal
exit /b 0