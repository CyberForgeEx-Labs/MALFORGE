#include <windows.h>
#include <stdio.h>
#include <winternl.h>

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "ntdll.lib")

// Define RtlGetVersion if not available
typedef NTSTATUS (WINAPI *RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

// Helper function to get processor relationship type name
const char* GetProcessorRelationshipName(LOGICAL_PROCESSOR_RELATIONSHIP relationship) {
    switch (relationship) {
        case RelationProcessorCore: return "Processor Core";
        case RelationNumaNode: return "NUMA Node";
        case RelationCache: return "Cache";
        case RelationProcessorPackage: return "Processor Package";
        case RelationGroup: return "Processor Group";
        default: return "Unknown";
    }
}

// Helper function to get cache type name
const char* GetCacheTypeName(PROCESSOR_CACHE_TYPE cacheType) {
    switch (cacheType) {
        case CacheUnified: return "Unified";
        case CacheInstruction: return "Instruction";
        case CacheData: return "Data";
        case CacheTrace: return "Trace";
        default: return "Unknown";
    }
}

// GetLogicalProcessorInformation - Get Processor Information
void DemonstrateGetLogicalProcessorInformation() {
    printf("\nGetLogicalProcessorInformation - Processor Info\n\n");
    
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
    DWORD bufferSize = 0;
    
    // Get required buffer size
    GetLogicalProcessorInformation(buffer, &bufferSize);
    
    buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(bufferSize);
    if (!buffer) {
        printf("[-] Memory allocation failed!\n");
        return;
    }
    
    if (GetLogicalProcessorInformation(buffer, &bufferSize)) {
        printf("[+] Successfully retrieved processor information\n\n");
        
        DWORD byteOffset = 0;
        PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = buffer;
        
        int coreCount = 0;
        int logicalProcessorCount = 0;
        int numaNodeCount = 0;
        int packageCount = 0;
        int l1CacheCount = 0, l2CacheCount = 0, l3CacheCount = 0;
        
        while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= bufferSize) {
            switch (ptr->Relationship) {
                case RelationProcessorCore:
                    coreCount++;
                    // count logical processors for this core
                    //count bit by bit
                    {
                        ULONG_PTR mask = ptr->ProcessorMask;
                        while (mask) {
                            if (mask & 1) logicalProcessorCount++;
                            mask >>= 1;
                        }
                    }
                    printf("Processor Core #%d\n", coreCount);
                    printf("  Processor Mask: 0x%IX\n", ptr->ProcessorMask);
                    printf("  Flags: 0x%02X\n\n", ptr->ProcessorCore.Flags);
                    break;
                    
                case RelationNumaNode:
                    numaNodeCount++;
                    break;
                    
                case RelationCache:
                    if (ptr->Cache.Level == 1) l1CacheCount++;
                    else if (ptr->Cache.Level == 2) l2CacheCount++;
                    else if (ptr->Cache.Level == 3) l3CacheCount++;
                    break;
                    
                case RelationProcessorPackage:
                    packageCount++;
                    break;
            }
            
            byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
            ptr++;
        }
        
        printf("\nProcessor Summary:\n");
        printf("  Physical Cores: %d\n", coreCount);
        printf("  Logical Processors: %d\n", logicalProcessorCount);
        printf("  Processor Packages: %d\n", packageCount);
        printf("  NUMA Nodes: %d\n", numaNodeCount);
        printf("  L1 Caches: %d\n", l1CacheCount);
        printf("  L2 Caches: %d\n", l2CacheCount);
        printf("  L3 Caches: %d\n", l3CacheCount);
        printf("  Hyper-Threading: %s\n", 
               (logicalProcessorCount > coreCount) ? "Enabled" : "Disabled");
    } else {
        printf("[-] GetLogicalProcessorInformation failed (Error: %lu)\n", GetLastError());
    }
    
    free(buffer);
}

// GetLogicalProcessorInformationEx - Extended Processor Information
void DemonstrateGetLogicalProcessorInformationEx() {
    printf("\nGetLogicalProcessorInformationEx - Extended Info\n");
    
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX buffer = NULL;
    DWORD bufferSize = 0;
    
    // Get required buffer size
    GetLogicalProcessorInformationEx(RelationAll, buffer, &bufferSize);
    
    buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)malloc(bufferSize);
    if (!buffer) {
        printf("[-] Memory allocation failed!\n");
        return;
    }
    
    if (GetLogicalProcessorInformationEx(RelationAll, buffer, &bufferSize)) {
        printf("[+] Successfully retrieved extended processor information\n\n");
        
        DWORD byteOffset = 0;
        PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX ptr = buffer;
        
        int coreCount = 0;
        int packageCount = 0;
        
        printf("%-25s %-15s %-30s\n", "Relationship Type", "Efficiency", "Details");
        printf("%-25s %-15s %-30s\n", 
               "-------------------------", 
               "---------------", 
               "------------------------------");
        
        while (byteOffset < bufferSize) {
            const char* relType = GetProcessorRelationshipName(ptr->Relationship);
            
            switch (ptr->Relationship) {
                case RelationProcessorCore:
                    coreCount++;
                    printf("%-25s %-15s Core #%d, Flags: 0x%02X\n",
                           relType,
                           (ptr->Processor.EfficiencyClass > 0) ? "High" : "Standard",
                           coreCount,
                           ptr->Processor.Flags);
                    break;
                    
                case RelationProcessorPackage:
                    packageCount++;
                    printf("%-25s %-15s Package #%d\n",
                           relType,
                           "N/A",
                           packageCount);
                    break;
                    
                case RelationCache:
                    printf("%-25s %-15s L%d %s, %lu KB\n",
                           relType,
                           "N/A",
                           ptr->Cache.Level,
                           GetCacheTypeName(ptr->Cache.Type),
                           ptr->Cache.CacheSize / 1024);
                    break;
                    
                case RelationNumaNode:
                    printf("%-25s %-15s Node #%lu\n",
                           relType,
                           "N/A",
                           ptr->NumaNode.NodeNumber);
                    break;
                    
                case RelationGroup:
                    printf("%-25s %-15s Max Groups: %d\n",
                           relType,
                           "N/A",
                           ptr->Group.MaximumGroupCount);
                    break;
            }
            
            byteOffset += ptr->Size;
            ptr = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)(((BYTE*)ptr) + ptr->Size);
        }
        
        printf("\nExtended Summary:\n");
        printf("  Total Processor Cores: %d\n", coreCount);
        printf("  Total Processor Packages: %d\n", packageCount);
    } else {
        printf("[-] GetLogicalProcessorInformationEx failed (Error: %lu)\n", GetLastError());
    }
    
    free(buffer);
}

// GetNativeSystemInfo - Native System Information
void DemonstrateGetNativeSystemInfo() {
    printf("\nGetNativeSystemInfo - Native System Info\n");
    
    SYSTEM_INFO sysInfo;
    GetNativeSystemInfo(&sysInfo);
    
    printf("[+] Native System Information:\n\n");
    
    // Processor architecture
    const char* archName = "Unknown";
    switch (sysInfo.wProcessorArchitecture) {                   
        case PROCESSOR_ARCHITECTURE_AMD64:
            archName = "x64 (AMD64)";
            break;
        case PROCESSOR_ARCHITECTURE_ARM:
            archName = "ARM";
            break;
        case PROCESSOR_ARCHITECTURE_ARM64:
            archName = "ARM64";
            break;
        case PROCESSOR_ARCHITECTURE_INTEL:
            archName = "x86 (Intel)";
            break;
        case PROCESSOR_ARCHITECTURE_IA64:
            archName = "Intel Itanium";
            break;
    }
    
    printf("Processor Architecture:\n");
    printf("  Architecture: %s\n", archName);
    printf("  Architecture ID: %u\n", sysInfo.wProcessorArchitecture);
    printf("  Number of Processors: %lu\n", sysInfo.dwNumberOfProcessors);
    printf("  Processor Type: %lu\n", sysInfo.dwProcessorType);
    printf("  Processor Level: %u\n", sysInfo.wProcessorLevel);
    printf("  Processor Revision: %u\n\n", sysInfo.wProcessorRevision);
    
    printf("Memory Information:\n");
    printf("  Page Size: %lu bytes (%lu KB)\n", 
           sysInfo.dwPageSize, sysInfo.dwPageSize / 1024);
    printf("  Allocation Granularity: %lu bytes (%lu KB)\n",
           sysInfo.dwAllocationGranularity, sysInfo.dwAllocationGranularity / 1024);
    printf("  Minimum Application Address: 0x%p\n", sysInfo.lpMinimumApplicationAddress);
    printf("  Maximum Application Address: 0x%p\n\n", sysInfo.lpMaximumApplicationAddress);
    
    printf("Active Processor Mask: 0x%IX\n", sysInfo.dwActiveProcessorMask);
}

// GetSystemDefaultLangID - System Default Language
void DemonstrateGetSystemDefaultLangID() {
    printf("\nGetSystemDefaultLangID - Default Language\n");
    
    LANGID langId = GetSystemDefaultLangID();
    LANGID userLangId = GetUserDefaultLangID();
    
    printf("[+] Language Information:\n\n");
    printf("System Default Language ID: 0x%04X\n", langId);
    printf("  Primary Language ID: 0x%04X\n", PRIMARYLANGID(langId));
    printf("  Sub-language ID: 0x%04X\n\n", SUBLANGID(langId));
    
    printf("User Default Language ID: 0x%04X\n", userLangId);
    printf("  Primary Language ID: 0x%04X\n", PRIMARYLANGID(userLangId));
    printf("  Sub-language ID: 0x%04X\n\n", SUBLANGID(userLangId));
    
    // Get language name
    char langName[256];
    char countryName[256];
    
    if (GetLocaleInfoA(MAKELCID(langId, SORT_DEFAULT), LOCALE_SENGLANGUAGE, 
                       langName, sizeof(langName))) {
        printf("System Language Name: %s\n", langName);
    }
    
    if (GetLocaleInfoA(MAKELCID(langId, SORT_DEFAULT), LOCALE_SENGCOUNTRY, 
                       countryName, sizeof(countryName))) {
        printf("System Country Name: %s\n", countryName);
    }
}

// GetVersionExA & RtlGetVersion - OS Version Information
void DemonstrateVersionAPIs() {
    printf("\nGetVersionExA & RtlGetVersion - OS Version\n\n");
    
    // Method 1: GetVersionExA 
    OSVERSIONINFOEXA osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEXA));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA);
    
#pragma warning(push)
#pragma warning(disable: 4996) // Disable deprecation warning
    if (GetVersionExA((OSVERSIONINFOA*)&osvi)) {
        printf("[+] GetVersionExA Results:\n");
        printf("  Major Version: %lu\n", osvi.dwMajorVersion);
        printf("  Minor Version: %lu\n", osvi.dwMinorVersion);
        printf("  Build Number: %lu\n", osvi.dwBuildNumber);
        printf("  Platform ID: %lu\n", osvi.dwPlatformId);
        printf("  Service Pack: %s\n", osvi.szCSDVersion);
        printf("  Suite Mask: 0x%04X\n", osvi.wSuiteMask);
        printf("  Product Type: %u\n\n", osvi.wProductType);
    }
#pragma warning(pop)
    
    // Method 2: RtlGetVersion
    HMODULE ntdll = GetModuleHandleA("ntdll.dll");
    if (ntdll) {
        RtlGetVersionPtr RtlGetVersion = (RtlGetVersionPtr)GetProcAddress(ntdll, "RtlGetVersion");
        
        if (RtlGetVersion) {
            RTL_OSVERSIONINFOW rovi;
            ZeroMemory(&rovi, sizeof(RTL_OSVERSIONINFOW));
            rovi.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);
            
            if (RtlGetVersion(&rovi) == 0) {
                printf("[+] RtlGetVersion Results (Accurate):\n");
                printf("  Major Version: %lu\n", rovi.dwMajorVersion);
                printf("  Minor Version: %lu\n", rovi.dwMinorVersion);
                printf("  Build Number: %lu\n\n", rovi.dwBuildNumber);
                
                // Determine Windows version
                printf("Windows Version: ");
                if (rovi.dwMajorVersion == 10 && rovi.dwBuildNumber >= 22000) {
                    printf("Windows 11");
                } else if (rovi.dwMajorVersion == 10) {
                    printf("Windows 10");
                } else if (rovi.dwMajorVersion == 6 && rovi.dwMinorVersion == 3) {
                    printf("Windows 8.1");
                } else if (rovi.dwMajorVersion == 6 && rovi.dwMinorVersion == 2) {
                    printf("Windows 8");
                } else if (rovi.dwMajorVersion == 6 && rovi.dwMinorVersion == 1) {
                    printf("Windows 7");
                } else {
                    printf("Unknown Windows Version");
                }
                printf(" (Build %lu)\n", rovi.dwBuildNumber);
            }
        }
    }
}

// IsWoW64Process - Check WOW64 Status
void DemonstrateIsWoW64Process() {
    printf("\nIsWoW64Process - WOW64 Status Check\n");
    
    BOOL isWow64 = FALSE;
    HANDLE hProcess = GetCurrentProcess();
    
    if (IsWow64Process(hProcess, &isWow64)) {
        printf("[+] WOW64 Status Check\n\n");
        printf("Current Process Running Under WOW64: %s\n", isWow64 ? "Yes" : "No");
        
        if (isWow64) {
            printf("\nExplanation:\n");
            printf("  - This is a 32-bit process running on 64-bit Windows\n");
            printf("  - WOW64 (Windows 32-bit on Windows 64-bit) subsystem is active\n");
            printf("  - File system and registry are redirected\n");
        } else {
            printf("\nExplanation:\n");
            printf("  - This is a native process for this architecture\n");
            printf("  - Either 64-bit process on 64-bit Windows\n");
            printf("  - Or 32-bit process on 32-bit Windows\n");
        }
        
        // Additional architecture info
        SYSTEM_INFO sysInfo;
        GetNativeSystemInfo(&sysInfo);
        
        printf("\nProcess Architecture Details:\n");
        printf("  Native System Architecture: ");
        switch (sysInfo.wProcessorArchitecture) {
            case PROCESSOR_ARCHITECTURE_AMD64:
                printf("x64 (64-bit)\n");
                break;
            case PROCESSOR_ARCHITECTURE_INTEL:
                printf("x86 (32-bit)\n");
                break;
            case PROCESSOR_ARCHITECTURE_ARM64:
                printf("ARM64 (64-bit)\n");
                break;
            default:
                printf("Unknown\n");
        }
        
        #ifdef _WIN64
        printf("  Process Type: 64-bit\n");
        #else
        printf("  Process Type: 32-bit\n");
        #endif
    } else {
        printf("[-] IsWoW64Process failed (Error: %lu)\n", GetLastError());
    }
}

// GetDriveTypeA - Drive Type Information
void DemonstrateGetDriveTypeA() {
    printf(" \nGetDriveTypeA - Drive Type Enumeration\n");
    
    printf("[+] Scanning system drives...\n\n");
    
    printf("%-6s %-20s %-40s\n", "Drive", "Type", "Description");
    printf("%-6s %-20s %-40s\n", 
           "------", 
           "--------------------", 
           "----------------------------------------");
    
    char drive[4] = "A:\\";
    int driveCount = 0;
    
    for (char letter = 'A'; letter <= 'Z'; letter++) {
        drive[0] = letter;
        
        UINT driveType = GetDriveTypeA(drive);
        
        if (driveType != DRIVE_NO_ROOT_DIR) {
            driveCount++;
            const char* typeName = "Unknown";
            const char* description = "";
            
            switch (driveType) {
                case DRIVE_UNKNOWN:
                    typeName = "Unknown";
                    description = "Drive type cannot be determined";
                    break;
                case DRIVE_NO_ROOT_DIR:
                    typeName = "No Root Directory";
                    description = "Root path is invalid";
                    break;
                case DRIVE_REMOVABLE:
                    typeName = "Removable";
                    description = "Removable media (USB, floppy, etc.)";
                    break;
                case DRIVE_FIXED:
                    typeName = "Fixed";
                    description = "Fixed hard disk";
                    break;
                case DRIVE_REMOTE:
                    typeName = "Network";
                    description = "Network drive";
                    break;
                case DRIVE_CDROM:
                    typeName = "CD-ROM";
                    description = "CD-ROM or DVD drive";
                    break;
                case DRIVE_RAMDISK:
                    typeName = "RAM Disk";
                    description = "RAM disk";
                    break;
            }
            
            printf("%-6s %-20s %-40s\n", drive, typeName, description);
            
            // Getting additional volume information for fixed drives
            if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
                ULARGE_INTEGER freeBytesAvailable, totalBytes, totalFreeBytes;
                if (GetDiskFreeSpaceExA(drive, &freeBytesAvailable, &totalBytes, &totalFreeBytes)) {
                    double totalGB = totalBytes.QuadPart / (1024.0 * 1024.0 * 1024.0);
                    double freeGB = totalFreeBytes.QuadPart / (1024.0 * 1024.0 * 1024.0);
                    printf("       Size: %.2f GB, Free: %.2f GB (%.1f%% used)\n",
                           totalGB, freeGB, 
                           ((totalGB - freeGB) / totalGB) * 100.0);
                }
            }
        }
    }
    
    printf("\nDrive Summary:\n");
    printf("  Total Drives Found: %d\n", driveCount);
}

// System Summary
void SystemSummary() {
    printf("\nSystem Summary\n");
    
    // CPU Information
    SYSTEM_INFO sysInfo;
    GetNativeSystemInfo(&sysInfo);
    
    printf("Processor Summary:\n");
    printf("  Architecture: ");
    switch (sysInfo.wProcessorArchitecture) {
        case PROCESSOR_ARCHITECTURE_AMD64: printf("x64\n"); break;
        case PROCESSOR_ARCHITECTURE_ARM64: printf("ARM64\n"); break;
        case PROCESSOR_ARCHITECTURE_INTEL: printf("x86\n"); break;
        default: printf("Unknown\n");
    }
    printf("  Number of Processors: %lu\n", sysInfo.dwNumberOfProcessors);
    printf("  Page Size: %lu KB\n\n", sysInfo.dwPageSize / 1024);
    
    // Memory Status
    MEMORYSTATUSEX memStatus;
    memStatus.dwLength = sizeof(memStatus);
    if (GlobalMemoryStatusEx(&memStatus)) {
        printf("Memory Summary:\n");
        printf("  Total Physical Memory: %.2f GB\n", 
               memStatus.ullTotalPhys / (1024.0 * 1024.0 * 1024.0));
        printf("  Available Physical Memory: %.2f GB\n",
               memStatus.ullAvailPhys / (1024.0 * 1024.0 * 1024.0));
        printf("  Memory Load: %lu%%\n\n", memStatus.dwMemoryLoad);
    }
    
    // OS Version
    HMODULE ntdll = GetModuleHandleA("ntdll.dll");
    if (ntdll) {
        RtlGetVersionPtr RtlGetVersion = (RtlGetVersionPtr)GetProcAddress(ntdll, "RtlGetVersion");
        if (RtlGetVersion) {
            RTL_OSVERSIONINFOW rovi;
            ZeroMemory(&rovi, sizeof(RTL_OSVERSIONINFOW));
            rovi.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);
            
            if (RtlGetVersion(&rovi) == 0) {
                printf("Operating System:\n");
                printf("  Version: %lu.%lu.%lu\n", rovi.dwMajorVersion, rovi.dwMinorVersion, rovi.dwBuildNumber);
            }
        }
    }
    
    // WOW64 Status
    BOOL isWow64 = FALSE;
    if (IsWow64Process(GetCurrentProcess(), &isWow64)) {
        printf("  Process Type: %s\n", isWow64 ? "32-bit (WOW64)" : "Native");
    }
    
    // Language
    LANGID langId = GetSystemDefaultLangID();
    char langName[256];
    if (GetLocaleInfoA(MAKELCID(langId, SORT_DEFAULT), LOCALE_SENGLANGUAGE, 
                       langName, sizeof(langName))) {
        printf("  System Language: %s\n", langName);
    }
}

int main() {

    printf("\nSystem Information Enumeration\n\n");

    // 1. GetLogicalProcessorInformation
    DemonstrateGetLogicalProcessorInformation();
    
    // 2. GetLogicalProcessorInformationEx
    DemonstrateGetLogicalProcessorInformationEx();
    
    // 3. GetNativeSystemInfo
    DemonstrateGetNativeSystemInfo();
    
    // 4. GetSystemDefaultLangID
    DemonstrateGetSystemDefaultLangID();
    
    // 5. GetVersionExA & RtlGetVersion
    DemonstrateVersionAPIs();

    // 6. IsWoW64Process
    DemonstrateIsWoW64Process();
    
    // 7. GetDriveTypeA
    DemonstrateGetDriveTypeA();
    
    // System Summary
    SystemSummary();
    
    printf("\nEnumeration complete\n");
    printf("Press Enter to exit...");
    getchar();
    
    return 0;
}