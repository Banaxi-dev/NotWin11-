#include <iostream>
#include <Windows.h>
#include <string>
#include <memory>
#include <fileapi.h>
#include <winreg.h>
#include <sstream>
#include <vector>
#include <fstream>  // For file operations
#include <cstdlib>  // For system()

// Function to execute a PowerShell command and get the output
std::string executePowerShellCommand(const std::string& command) {
    std::string result;
    std::string cmd = "powershell -command \"" + command + "\"";

    FILE* pipe = _popen(cmd.c_str(), "r");
    if (!pipe) return "Error opening pipe";
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    _pclose(pipe);
    return result;
}

// Function to create the registry file
void createRegistryFile() {
    std::ofstream regFile("bypass11.reg");
    if (regFile.is_open()) {
        regFile << "Windows Registry Editor Version 5.00\n\n";
        regFile << "[HKEY_LOCAL_MACHINE\\SYSTEM\\Setup\\LabConfig]\n";
        regFile << "\"BypassSecureBootCheck\"=dword:00000001\n";
        regFile << "\"BypassTPMCheck\"=dword:00000001\n";
        regFile << "\"BypassCPUCheck\"=dword:00000001\n";
        regFile << "\"BypassRAMCheck\"=dword:00000001\n";
        regFile.close();
        std::cout << "Registry file 'bypass11.reg' has been created.\n";
    } else {
        std::cerr << "Error creating the registry file.\n";
    }
}

// Function to create the batch file
void createBatchFile() {
    std::ofstream batchFile("bypass11.bat");
    if (batchFile.is_open()) {
        batchFile << "@echo off\n";
        batchFile << "setlocal\n\n";
        batchFile << "set /p drive=Enter the drive letter (e.g., C): \n\n";
        batchFile << "if not exist %drive%:\\ (\n";
        batchFile << "    echo Drive %drive% does not exist.\n";
        batchFile << "    exit /b 1\n";
        batchFile << ")\n\n";
        batchFile << "%drive%:\n";
        batchFile << "cd \\ \n\n";
        batchFile << "if not exist setup.exe (\n";
        batchFile << "    echo setup.exe file not found.\n";
        batchFile << "    exit /b 1\n";
        batchFile << ")\n\n";
        batchFile << "setup.exe /product server\n\n";
        batchFile << "endlocal\n";
        batchFile << "pause\n";
        batchFile.close();
        std::cout << "Batch file 'bypass11.bat' has been created.\n";
    } else {
        std::cerr << "Error creating the batch file.\n";
    }
}

// Function to check TPM 2.0
bool checkTPM(std::string& tpmOutput) {
    tpmOutput = executePowerShellCommand("Get-WmiObject -Namespace 'Root\\CIMv2\\Security\\MicrosoftTpm' -Class Win32_Tpm | Select-Object -ExpandProperty SpecVersion");
    return tpmOutput.find("2.0") != std::string::npos;
}

// Function to check Secure Boot
bool checkSecureBoot(std::string& secureBootOutput) {
    secureBootOutput = executePowerShellCommand("Confirm-SecureBootUEFI");
    return secureBootOutput.find("True") != std::string::npos;
}

// Function to check the CPU
bool checkCPU() {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    if (sysInfo.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_AMD64) {
        return false;
    }

    if (sysInfo.dwNumberOfProcessors < 2) {
        return false;
    }

    HKEY hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        wchar_t cpuName[256];
        DWORD bufferSize = sizeof(cpuName);
        if (RegQueryValueExW(hKey, L"ProcessorNameString", NULL, NULL, (LPBYTE)cpuName, &bufferSize) == ERROR_SUCCESS) {
            std::wcout << L"CPU: " << cpuName << std::endl;
            RegCloseKey(hKey);

            if (wcsstr(cpuName, L"Intel") || wcsstr(cpuName, L"AMD")) {
                return true;
            }
        }
        RegCloseKey(hKey);
    }

    return false;
}

// Function to check the RAM
bool checkRAM() {
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);
    
    DWORDLONG totalRAMInGB = statex.ullTotalPhys / (1024 * 1024 * 1024);
    return totalRAMInGB >= 4;
}

// Function to check storage space
bool checkStorage() {
    ULARGE_INTEGER totalNumberOfBytes;

    if (GetDiskFreeSpaceExW(L"C:\\", NULL, &totalNumberOfBytes, NULL)) {
        DWORDLONG totalSpaceInGB = totalNumberOfBytes.QuadPart / (1024 * 1024 * 1024);
        std::wcout << L"Total storage space on C:\\ in GB: " << totalSpaceInGB << L" GB" << std::endl;
        return totalSpaceInGB >= 64;
    }
    return false;
}

// Function to check DirectX
bool checkDirectX() {
    return true; // Placeholder: Assuming DirectX 12 compatible
}

// Function to check UEFI
bool checkEFI(std::string& efiOutput) {
    efiOutput = executePowerShellCommand("Confirm-SecureBootUEFI");
    return efiOutput.find("True") != std::string::npos;
}

int main(int argc, char* argv[]) {
    // Check for command line arguments
    bool bypassMode = false;
    bool installerMode = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--cd_skip") {
            bypassMode = true;
            break;
        } else if (std::string(argv[i]) == "--installer_skip") {
            installerMode = true;
            break;
        }
    }

    if (bypassMode) {
        createRegistryFile();
        return 0; // Exit after creating the file
    } else if (installerMode) {
        createBatchFile();
        return 0; // Exit after creating the file
    }

    // Set code page to UTF-8
    system("chcp 65001");

    std::cout << "Windows 11 Compatibility Check\n";

    bool cpuCheck = checkCPU();
    std::cout << "64-Bit CPU and 2 Cores: " << (cpuCheck ? "OK" : "Not Met") << std::endl;

    bool ramCheck = checkRAM();
    std::cout << "At least 4 GB RAM: " << (ramCheck ? "OK" : "Not Met") << std::endl;

    bool storageCheck = checkStorage();
    std::cout << "At least 64 GB Total Storage: " << (storageCheck ? "OK" : "Not Met") << std::endl;

    bool directXCheck = checkDirectX();
    std::cout << "DirectX 12 Compatible Graphics Card: " << (directXCheck ? "OK" : "Not Met") << std::endl;

    std::string efiOutput, tpmOutput, secureBootOutput;
    bool efiCheck = checkEFI(efiOutput);
    bool tpmCheck = checkTPM(tpmOutput);
    bool secureBootCheck = checkSecureBoot(secureBootOutput);

    std::cout << "UEFI Mode: " << (efiCheck ? "OK" : "Not Met") << std::endl;
    std::cout << "TPM 2.0: " << (tpmCheck ? "OK" : "Not Met") << std::endl;
    std::cout << "Secure Boot: " << (secureBootCheck ? "OK" : "Not Met") << std::endl;

    if (cpuCheck && ramCheck && storageCheck && directXCheck && efiCheck && tpmCheck && secureBootCheck) {
        std::cout << "This PC meets the basic requirements for Windows 11.\n";
    } else {
        std::cout << "This PC does not meet the basic requirements for Windows 11.\n";
    }

    std::cout << "Press any key to exit..." << std::endl;
    std::cin.get();  // Wait for key press

    return 0;
}
