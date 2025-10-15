#include <windows.h>
#include <stdio.h>

BOOL DetectVMByCPUCores() {
    SYSTEM_INFO sysInfo;
    DWORD processorCount = 0;
    
    // Get system information
    GetSystemInfo(&sysInfo);
    processorCount = sysInfo.dwNumberOfProcessors;
    
    printf("[*] Processor Count: %d\n", processorCount);
    
    // VMs/Sandboxes have fewer than 3 cores
    if (processorCount < 3) {
        printf("[!] VM/Sandbox Detected: Low core count (%d)\n", processorCount);
        return TRUE;
    }
    
    printf("[+] Physical machine likely (Core count: %d)\n", processorCount);
    return FALSE;
}

// Advanced method using GetLogicalProcessorInformation
BOOL DetectVMAdvanced() {
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
    DWORD returnLength = 0;
    DWORD logicalProcessorCount = 0;
    DWORD physicalCoreCount = 0;
    
    // Get buffer size
    GetLogicalProcessorInformation(NULL, &returnLength);
    buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(returnLength);
    
    if (!GetLogicalProcessorInformation(buffer, &returnLength)) {
        printf("[-] GetLogicalProcessorInformation failed\n");
        free(buffer);
        return FALSE;
    }
    
    DWORD byteOffset = 0;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = buffer;
    
    while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength) {
        if (ptr->Relationship == RelationProcessorCore) {
            physicalCoreCount++;
            // Count logical processors (threads) per core
            DWORD threads = 0;
            for (DWORD bit = 0; bit < sizeof(ULONG_PTR) * 8; bit++) {
                if (ptr->ProcessorMask & ((ULONG_PTR)1 << bit)) {
                    threads++;
                }
            }
            logicalProcessorCount += threads;
        }
        byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
        ptr++;
    }
    
    printf("[*] Physical Cores: %d\n", physicalCoreCount);
    printf("[*] Logical Processors: %d\n", logicalProcessorCount);
    
    free(buffer);
    
    // Detection logic
    if (physicalCoreCount < 2) {
        printf("[!] VM/Sandbox Detected: Low physical core count\n");
        return TRUE;
    }
    
    return FALSE;
}

int main() {
    printf("VM Detection via CPU Cores.\n\n");
    
    if (DetectVMByCPUCores()) {
        printf("\n[!] Analysis environment detected!\n");
    } else {
        printf("\n[+] Normal execution environment\n");
    }
    
    printf("\nAdvanced Detection \n");
    DetectVMAdvanced();
    
    return 0;
}