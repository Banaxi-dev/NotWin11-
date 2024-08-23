# NotWin11

## Overview

**NotWin11** is a utility designed to assess your system's compatibility with Windows 11. It evaluates key hardware and firmware components against Windows 11's requirements and provides detailed feedback. Additionally, it offers options to bypass these requirements during the Windows installation process by generating the necessary registry or batch files.

## Features

### Compatibility Checks
- **CPU**: Verifies that the processor is 64-bit with at least 2 cores.
- **RAM**: Ensures the system has a minimum of 4 GB of RAM.
- **Storage**: Checks for at least 64 GB of available storage space.
- **DirectX**: Confirms that the system has a DirectX 12 compatible GPU.
- **UEFI Mode**: Validates that the system is running in UEFI mode.
- **TPM**: Ensures TPM 2.0 is present and enabled.
- **Secure Boot**: Confirms that Secure Boot is enabled.

### Bypass Options
- **`--cd_skip`**: Generates a `bypass11.reg` file that allows you to bypass the TPM, Secure Boot, CPU, and RAM checks during Windows 11 installation.

  ```reg
  Windows Registry Editor Version 5.00

  [HKEY_LOCAL_MACHINE\SYSTEM\Setup\LabConfig]
  "BypassSecureBootCheck"=dword:00000001
  "BypassTPMCheck"=dword:00000001
  "BypassCPUCheck"=dword:00000001
  "BypassRAMCheck"=dword:00000001


- **`--install_skip`**: Generates a `bypass11.bat` file that allows you to bypass the TPM, Secure Boot, CPU, and RAM checks during Windows 11 upgrade.


```batch
@echo off
setlocal

set /p drive=Enter the drive letter (e.g. C): 

if not exist %drive%:\ (
    echo The drive %drive% does not exist.
    exit /b 1
)

%drive%:
cd \ 

if not exist setup.exe (
    echo The file setup.exe was not found.
    exit /b 1
)

setup.exe /product server

endlocal
pause

**Full Changelog**: https://github.com/Banaxi-dev/NotWin11-/commits/Win11
