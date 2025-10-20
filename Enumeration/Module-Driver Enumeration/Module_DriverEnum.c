#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <string.h>

#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "kernel32.lib")

#define MAX_MODULES 256
#define MAX_NAME_LEN 260

// EnumDeviceDrivers - Enumerate all device drivers
void EnumDeviceDriversExample() {
    printf("\nEnumDeviceDrivers Example\n\n");
    
    LPVOID drivers[MAX_MODULES];
    DWORD cbNeeded = 0;
    
    // First call to get the size needed
    if (!EnumDeviceDrivers(drivers, sizeof(drivers), &cbNeeded)) {
        printf("EnumDeviceDrivers failed: %lu\n", GetLastError());
        return;
    }
    
    if (cbNeeded == 0) {
        printf("No device drivers found or cbNeeded is 0\n");
        return;
    }
    
    DWORD driverCount = cbNeeded / sizeof(LPVOID);
    
    // Ensure we don't exceed array bounds
    if (driverCount > MAX_MODULES) {
        driverCount = MAX_MODULES;
    }
    
    printf("Total Device Drivers: %lu\n\n", driverCount);
    printf("Driver Address      | Driver Name\n");
    printf("----------------------------------------\n");
    
    // Use a set to track unique drivers and avoid duplicates
    int uniqueCount = 0;
    for (DWORD i = 0; i < driverCount; i++) {
        // Skip null pointers
        if (drivers[i] == NULL) {
            continue;
        }
        
        char driverName[MAX_NAME_LEN] = {0};
        char driverPath[MAX_NAME_LEN] = {0};
        
        // Get base name
        if (GetDeviceDriverBaseNameA(drivers[i], driverName, sizeof(driverName))) {
            // Get full path
            GetDeviceDriverFileNameA(drivers[i], driverPath, sizeof(driverPath));
            
            if (strlen(driverName) > 0) {
                printf("0x%016p | %-30s | %s\n", 
                       drivers[i], 
                       driverName,
                       strlen(driverPath) > 0 ? driverPath : "N/A");
                uniqueCount++;
            }
        }
    }
    
    printf("\nUnique Drivers Enumerated: %d\n", uniqueCount);
}


// EnumProcessModulesEx - Extended module enumeration (32/64-bit support)
void EnumProcessModulesExExample(DWORD processID) {
    printf("\nEnumProcessModulesEx Example (PID: %lu)\n\n", processID);
    
    HANDLE hProcess = OpenProcess(
        PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
        FALSE,
        processID
    );
    
    if (hProcess == NULL) {
        printf("OpenProcess failed: %lu\n", GetLastError());
        return;
    }
    
    HMODULE modules[MAX_MODULES];
    DWORD cbNeeded;
    
    // LIST_MODULES_ALL = 0x03 (both 32-bit and 64-bit modules)
    if (!EnumProcessModulesEx(hProcess, modules, sizeof(modules), &cbNeeded, LIST_MODULES_ALL)) {
        printf("EnumProcessModulesEx failed: %lu\n", GetLastError());
        CloseHandle(hProcess);
        return;
    }
    
    DWORD moduleCount = cbNeeded / sizeof(HMODULE);
    printf("Total Modules (All Architectures): %lu\n\n", moduleCount);
    printf("Module Base     | Module Name\n");
    printf("----------------------------------------\n");
    
    for (DWORD i = 0; i < moduleCount && i < MAX_MODULES; i++) {
        char moduleName[MAX_NAME_LEN];
        
        if (GetModuleFileNameExA(hProcess, modules[i], moduleName, sizeof(moduleName))) {
            printf("0x%p | %s\n", modules[i], moduleName);
        }
    }
    
    CloseHandle(hProcess);
}

// Module32First - Get first module from process snapshot
void Module32FirstExample(DWORD processID) {
    printf("\nModule32First/Module32Next Example (PID: %lu)\n\n", processID);
    
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processID);
    
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        printf("CreateToolhelp32Snapshot failed: %lu\n", GetLastError());
        return;
    }
    
    MODULEENTRY32 moduleEntry;
    moduleEntry.dwSize = sizeof(MODULEENTRY32);
    
    printf("Module Base     | Module Size | Module Name\n");
    printf("--------------------------------------------------------------------\n");
    
    // Module32Next - Get subsequent modules
    if (Module32First(hSnapshot, &moduleEntry)) {
        do {
            printf("0x%p | %10lu | %s\n",
                   moduleEntry.modBaseAddr,
                   moduleEntry.modBaseSize,
                   moduleEntry.szModule);
        } while (Module32Next(hSnapshot, &moduleEntry));
    } else {
        printf("Module32First failed: %lu\n", GetLastError());
    }
    
    CloseHandle(hSnapshot);
}

// GetModuleBaseNameA - Get module base name
void GetModuleBaseNameExample(DWORD processID) {
    printf("\nGetModuleBaseNameA Example (PID: %lu)\n\n", processID);
    
    HANDLE hProcess = OpenProcess(
        PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
        FALSE,
        processID
    );
    
    if (hProcess == NULL) {
        printf("OpenProcess failed: %lu\n", GetLastError());
        return;
    }
    
    HMODULE modules[MAX_MODULES];
    DWORD cbNeeded;
    
    if (!EnumProcessModules(hProcess, modules, sizeof(modules), &cbNeeded)) {
        printf("EnumProcessModules failed: %lu\n", GetLastError());
        CloseHandle(hProcess);
        return;
    }
    
    DWORD moduleCount = cbNeeded / sizeof(HMODULE);
    printf("First 5 Module Base Names:\n");
    printf("Module Base Name\n");
    printf("----------------------------------------\n");
    
    for (DWORD i = 0; i < moduleCount && i < 5; i++) {
        char baseName[MAX_NAME_LEN];
        
        if (GetModuleBaseNameA(hProcess, modules[i], baseName, sizeof(baseName))) {
            printf("%s\n", baseName);
        }
    }
    
    CloseHandle(hProcess);
}

// Comprehensive module analysis function
void ComprehensiveModuleAnalysis(DWORD processID) {
    printf("\nComprehensive Module Analysis (PID: %lu) \n\n", processID);
    
    HANDLE hProcess = OpenProcess(
        PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
        FALSE,
        processID
    );
    
    if (hProcess == NULL) {
        printf("OpenProcess failed: %lu\n", GetLastError());
        return;
    }
    
    HMODULE modules[MAX_MODULES];
    DWORD cbNeeded;
    
    if (!EnumProcessModules(hProcess, modules, sizeof(modules), &cbNeeded)) {
        printf("EnumProcessModules failed: %lu\n", GetLastError());
        CloseHandle(hProcess);
        return;
    }
    
    DWORD moduleCount = cbNeeded / sizeof(HMODULE);
    printf("Total Modules: %lu\n\n", moduleCount);
    printf("%-8s | %-15s | %-12s | %-40s\n", "Index", "Base Address", "Size", "Module Name");
    printf("--------------------------------------------------------------------\n");
    
    for (DWORD i = 0; i < moduleCount && i < MAX_MODULES; i++) {
        char moduleName[MAX_NAME_LEN];
        char baseName[MAX_NAME_LEN];
        
        GetModuleFileNameExA(hProcess, modules[i], moduleName, sizeof(moduleName));
        GetModuleBaseNameA(hProcess, modules[i], baseName, sizeof(baseName));
        
        MODULEINFO modInfo;
        if (GetModuleInformation(hProcess, modules[i], &modInfo, sizeof(modInfo))) {
            printf("%-8lu | 0x%-13p | %-10lu | %s\n",
                   i,
                   modInfo.lpBaseOfDll,
                   modInfo.SizeOfImage,
                   baseName);
        }
    }
    
    CloseHandle(hProcess);
}

int main() 
{
    printf("Module and Driver Enumeration API Examples\n");
    
    // Get current process ID for demonstration
    DWORD currentPID = GetCurrentProcessId();
    
    // Enumerate device drivers
    EnumDeviceDriversExample();
    
    // Extended module enumeration
    EnumProcessModulesExExample(currentPID);
    
    // Module snapshot enumeration
    Module32FirstExample(currentPID);
    
    // Get module base names
    GetModuleBaseNameExample(currentPID);
    
    // Comprehensive analysis
    ComprehensiveModuleAnalysis(currentPID);
    
    return 0;
}