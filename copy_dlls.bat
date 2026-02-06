@echo off
if "%1"=="" (
    echo Usage: copy_dlls.bat ^<build_directory^>
    echo Example: copy_dlls.bat build.64
    exit /b 1
)

set BUILD_DIR=%1
set BASE_DIR=%~dp0..\..

echo BUILD_DIR: %BUILD_DIR% BASE_DIR: %BASE_DIR%
echo Copying VKD3D DLLs from %BUILD_DIR%...

:: Define source paths (default Meson build layout)
set D3D12_DLL=%BUILD_DIR%\libs\d3d12\d3d12.dll
set D3D12CORE_DLL=%BUILD_DIR%\libs\d3d12core\d3d12core.dll

:: Fallbacks for installed/package layouts
if not exist "%D3D12_DLL%" set D3D12_DLL=%BUILD_DIR%\d3d12.dll
if not exist "%D3D12CORE_DLL%" set D3D12CORE_DLL=%BUILD_DIR%\d3d12core.dll

if not exist "%D3D12_DLL%" set D3D12_DLL=%BUILD_DIR%\x64\d3d12.dll
if not exist "%D3D12CORE_DLL%" set D3D12CORE_DLL=%BUILD_DIR%\x64\d3d12core.dll

if not exist "%D3D12_DLL%" set D3D12_DLL=%BUILD_DIR%\bin64\d3d12.dll
if not exist "%D3D12CORE_DLL%" set D3D12CORE_DLL=%BUILD_DIR%\bin64\d3d12core.dll

if not exist "%D3D12_DLL%" (
    echo Error: could not find d3d12.dll in %BUILD_DIR%
    exit /b 1
)
if not exist "%D3D12CORE_DLL%" (
    echo Error: could not find d3d12core.dll in %BUILD_DIR%
    exit /b 1
)

:: Check if Release directory exists and copy
if exist "%BASE_DIR%\build\Release" (
    echo Copying to Release directory...
    copy /Y "%D3D12_DLL%" "%BASE_DIR%\build\Release\RemoteGPUD3D12.dll" 2>nul
    copy /Y "%D3D12CORE_DLL%" "%BASE_DIR%\build\Release\RemoteGPUD3D12Core.dll" 2>nul
    if errorlevel 1 (
        echo Warning: Some files could not be copied to Release directory
    ) else (
        echo Successfully copied DLLs to Release directory
    )
) else (
    echo Release directory does not exist, skipping...
)

:: Check if Debug directory exists and copy
if exist "%BASE_DIR%\build\Debug" (
    echo Copying to Debug directory...
    copy /Y "%D3D12_DLL%" "%BASE_DIR%\build\Debug\RemoteGPUD3D12.dll" 2>nul
    copy /Y "%D3D12CORE_DLL%" "%BASE_DIR%\build\Debug\RemoteGPUD3D12Core.dll" 2>nul
    if errorlevel 1 (
        echo Warning: Some files could not be copied to Debug directory
    ) else (
        echo Successfully copied DLLs to Debug directory
    )
) else (
    echo Debug directory does not exist, skipping...
)

:: Copy all DLLs to driver directory with original names
echo Copying all DLLs to driver directory...
if not exist "%BASE_DIR%\driver\windows\third_party\vkd3d\bin64" (
    mkdir "%BASE_DIR%\driver\windows\third_party\vkd3d\bin64"
)
copy /Y "%D3D12_DLL%" "%BASE_DIR%\driver\windows\third_party\vkd3d\bin64\" 2>nul
copy /Y "%D3D12CORE_DLL%" "%BASE_DIR%\driver\windows\third_party\vkd3d\bin64\" 2>nul
if errorlevel 1 (
    echo Warning: Some files could not be copied to driver directory
) else (
    echo Successfully copied all DLLs to driver directory
)

echo Done!
