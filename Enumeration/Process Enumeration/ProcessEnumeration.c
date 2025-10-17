#include <windows.h>
#include <psapi.h>
#include <stdio.h>

#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "kernel32.lib")

/**
 * Get process name from process ID
 * Using OpenProcess and GetModuleFileNameEx
 */
BOOL GetProcessNameByID(DWORD dwPID, char *pszProcessName, DWORD size) {
    HANDLE hProcess = OpenProcess(
        PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
        FALSE,
        dwPID
    );
    
    if (hProcess == NULL) {
        return FALSE;
    }
    
    BOOL result = GetModuleFileNameEx(hProcess, NULL, (LPSTR)pszProcessName, size);
    
    CloseHandle(hProcess);
    return result;
}

/**
 * Enumerate all running processes using EnumProcesses
 * Returns count of processes found
 */
DWORD EnumerateAllProcesses(void) {
    printf("\nEnumProcesses with GetProcessImageFileName\n\n");
    
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    
    // Get all process IDs
    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
        printf("EnumProcesses failed\n");
        return 0;
    }
    
    // Calculate number of processes
    cProcesses = cbNeeded / sizeof(DWORD);
    printf("Found %lu processes:\n", cProcesses);
    printf("PID      | Process Name\n");
    printf("----------------------------------------\n");
    
    // Enumerate each process
    for (DWORD i = 0; i < cProcesses; i++) {
        if (aProcesses[i] != 0) {
            char szProcessName[MAX_PATH] = "Unknown";
            
            // Get process name
            if (GetProcessNameByID(aProcesses[i], szProcessName, sizeof(szProcessName))) {
                // Extract just the filename from path
                char *pszFileName = strrchr(szProcessName, '\\');
                if (pszFileName) {
                    pszFileName++;
                } else {
                    pszFileName = szProcessName;
                }
                printf("%-8lu | %s\n", aProcesses[i], pszFileName);
            } else {
                printf("%-8lu | [Access Denied]\n", aProcesses[i]);
            }
        }
    }
    
    return cProcesses;
}

/**
 * Get memory information for a process
 * Using GetProcessMemoryInfo
 */
void GetProcessMemoryStats(DWORD dwPID) {
    printf("\nGetProcessMemoryInfo\n\n");
    
    HANDLE hProcess = OpenProcess(
        PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
        FALSE,
        dwPID
    );
    
    if (hProcess == NULL) {
        printf("Failed to open process %lu\n", dwPID);
        return;
    }
    
    PROCESS_MEMORY_COUNTERS pmc;
    
    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
        printf("Process ID: %lu\n", dwPID);
        printf("Working Set Size: %lu KB\n", pmc.WorkingSetSize / 1024);
        printf("Peak Working Set: %lu KB\n", pmc.PeakWorkingSetSize / 1024);
        printf("Page File Usage: %lu KB\n", pmc.PagefileUsage / 1024);
        printf("Peak Page File: %lu KB\n", pmc.PeakPagefileUsage / 1024);
    } else {
        printf("GetProcessMemoryInfo failed\n");
    }
    
    CloseHandle(hProcess);
}

/**
 * Get process times information
 * Using GetProcessTimes
 */
void GetProcessTimesInfo(DWORD dwPID) {
    printf("\nGetProcessTimes\n\n");
    
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPID);
    
    if (hProcess == NULL) {
        printf("Failed to open process %lu\n", dwPID);
        return;
    }
    
    FILETIME ftCreation, ftExit, ftKernel, ftUser;
    
    if (GetProcessTimes(hProcess, &ftCreation, &ftExit, &ftKernel, &ftUser)) {
        printf("Process ID: %lu\n", dwPID);
        printf("Kernel Time: %lu ms\n", ftKernel.dwLowDateTime / 10000);
        printf("User Time: %lu ms\n", ftUser.dwLowDateTime / 10000);
        printf("Process Creation Time: %08lx:%08lx\n", ftCreation.dwHighDateTime, ftCreation.dwLowDateTime);
    } else {
        printf("GetProcessTimes failed\n");
    }
    
    CloseHandle(hProcess);
}

/**
 * Get process priority and other information
 * Using GetPriorityClass
 */
void GetProcessPriorityInfo(DWORD dwPID) {
    printf("\nGetPriorityClass of the Process\n\n");
    
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPID);
    
    if (hProcess == NULL) {
        printf("Failed to open process %lu\n", dwPID);
        return;
    }
    
    DWORD dwPriorityClass = GetPriorityClass(hProcess);
    
    if (dwPriorityClass != 0) {
        const char *pszPriority = "Unknown";
        
        switch (dwPriorityClass) {
            case REALTIME_PRIORITY_CLASS:
                pszPriority = "Real-time";
                break;
            case HIGH_PRIORITY_CLASS:
                pszPriority = "High";
                break;
            case ABOVE_NORMAL_PRIORITY_CLASS:
                pszPriority = "Above Normal";
                break;
            case NORMAL_PRIORITY_CLASS:
                pszPriority = "Normal";
                break;
            case BELOW_NORMAL_PRIORITY_CLASS:
                pszPriority = "Below Normal";
                break;
            case IDLE_PRIORITY_CLASS:
                pszPriority = "Idle";
                break;
        }
        
        printf("Process ID: %lu\n", dwPID);
        printf("Priority Class: %s (0x%lx)\n", pszPriority, dwPriorityClass);
    } else {
        printf("GetPriorityClass failed\n");
    }
    
    CloseHandle(hProcess);
}

/**
 * Enumerate modules loaded by a process
 * Using EnumProcessModules and GetModuleBaseName
 */
void EnumerateProcessModules(DWORD dwPID) {
    printf("\nEnumerate Process Modules\n\n");
    
    HANDLE hProcess = OpenProcess(
        PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
        FALSE,
        dwPID
    );
    
    if (hProcess == NULL) {
        printf("Failed to open process %lu\n", dwPID);
        return;
    }
    
    HMODULE hMods[1024];
    DWORD cbNeeded;
    
    if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded)) {
        DWORD moduleCount = cbNeeded / sizeof(HMODULE);
        printf("Process ID: %lu loaded %lu modules:\n", dwPID, moduleCount);
        printf("Module Name\n");
        printf("----------------------------------------\n");
        
        for (DWORD i = 0; i < moduleCount && i < 10; i++) {
            char szModName[MAX_PATH];
            
            if (GetModuleBaseName(hProcess, hMods[i], szModName, sizeof(szModName))) {
                printf("%s\n", szModName);
            }
        }
        
        if (moduleCount > 10) {
            printf("... and %lu more modules\n", moduleCount - 10);
        }
    } else {
        printf("EnumProcessModules failed\n");
    }
    
    CloseHandle(hProcess);
}

/**
 * Check if process is still running
 * Using OpenProcess with minimal access
 */
BOOL IsProcessRunning(DWORD dwPID) {
    printf("\nCheck Process is Running\n\n");
    
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwPID);
    
    if (hProcess == NULL) {
        printf("Process %lu: Not Running\n", dwPID);
        return FALSE;
    }
    
    DWORD dwExitCode = 0;
    BOOL bRunning = GetExitCodeProcess(hProcess, &dwExitCode) && (dwExitCode == STILL_ACTIVE);
    
    printf("Process %lu: %s\n", dwPID, bRunning ? "Running" : "Terminated");
    
    if (!bRunning && dwExitCode != STILL_ACTIVE) {
        printf("Exit Code: %lu\n", dwExitCode);
    }
    
    CloseHandle(hProcess);
    return bRunning;
}

/**
 * Find specific process by name
 */
DWORD FindProcessByName(const char *processName) {
    printf("\nFinding Process: %s\n", processName);
    
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    
    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
        printf("EnumProcesses failed\n");
        return 0;
    }
    
    cProcesses = cbNeeded / sizeof(DWORD);
    
    for (DWORD i = 0; i < cProcesses; i++) {
        if (aProcesses[i] != 0) {
            char szProcessName[MAX_PATH] = "Unknown";
            
            if (GetProcessNameByID(aProcesses[i], szProcessName, sizeof(szProcessName))) {
                char *pszFileName = strrchr(szProcessName, '\\');
                if (pszFileName) {
                    pszFileName++;
                } else {
                    pszFileName = szProcessName;
                }
                
                if (_stricmp(pszFileName, processName) == 0) {
                    printf("Found: %s (PID: %lu)\n", pszFileName, aProcesses[i]);
                    return aProcesses[i];
                }
            }
        }
    }
    
    printf("Process '%s' not found\n", processName);
    return 0;
}

int main(void) 
{
    printf("Windows Process Enumeration API Implementation\n");
   
    // EnumProcesses - List all processes
    DWORD processCount = EnumerateAllProcesses();
    
    // Find explorer.exe and get detailed info
    DWORD dwExplorerPID = FindProcessByName("explorer.exe");
    
    if (dwExplorerPID != 0) {
        // Get memory information
        GetProcessMemoryStats(dwExplorerPID);
        
        // Get process times
        GetProcessTimesInfo(dwExplorerPID);
        
        // Get priority class
        GetProcessPriorityInfo(dwExplorerPID);
        
        // Enumerate loaded modules
        EnumerateProcessModules(dwExplorerPID);
        
        // Check if process is running
        IsProcessRunning(dwExplorerPID);
    }
    
    // Check current process
    DWORD dwCurrentPID = GetCurrentProcessId();
    printf("\nCurrent Process: %lu\n", dwCurrentPID);
    GetProcessMemoryStats(dwCurrentPID);
    
    return 0;
}
