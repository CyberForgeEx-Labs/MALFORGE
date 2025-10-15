#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <string.h>

// Common debugger and analysis tool process names
const char* SUSPICIOUS_PROCESSES[] = {
    "ollydbg.exe",
    "x64dbg.exe",
    "x32dbg.exe",
    "windbg.exe",
    "ida.exe",
    "ida64.exe",
    "idaq.exe",
    "idaq64.exe",
    "idaw.exe",
    "idaw64.exe",
    "idag.exe",
    "idag64.exe",
    "radare2.exe",
    "r2.exe",
    "ghidra.exe",
    "processhacker.exe",
    "procexp.exe",
    "procexp64.exe",
    "procmon.exe",
    "procmon64.exe",
    "wireshark.exe",
    "fiddler.exe",
    "httpdebugger.exe",
    "dbgview.exe",
    "importrec.exe",
    "lordpe.exe",
    "sysinspector.exe",
    "regshot.exe",
    "apimonitor.exe",
    "pestudio.exe",
    "die.exe",
    "scylla.exe",
    NULL
};

// Convert string to lowercase for case-insensitive comparison
void toLowerCase(char* str) {
    for(int i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

// Check if process name is in suspicious list
int isSuspiciousProcess(const char* processName) {
    char lowerName[MAX_PATH];
    strncpy(lowerName, processName, sizeof(lowerName) - 1);
    lowerName[sizeof(lowerName) - 1] = '\0';
    toLowerCase(lowerName);
    
    for(int i = 0; SUSPICIOUS_PROCESSES[i] != NULL; i++) {
        if(strcmp(lowerName, SUSPICIOUS_PROCESSES[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int main() {
    HANDLE hSnapshot;
    PROCESSENTRY32 pe32;
    int foundCount = 0;
    int totalProcesses = 0;
    
    printf("[*] Taking snapshot of running processes...\n\n");
    
    // Take a snapshot of all processes
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    
    if(hSnapshot == INVALID_HANDLE_VALUE) {
        printf("[-] Failed to create process snapshot (Error: %lu)\n", GetLastError());
        return 1;
    }
    
    pe32.dwSize = sizeof(PROCESSENTRY32);
    
    // Retrieve information about the first process
    if(!Process32First(hSnapshot, &pe32)) {
        printf("[-] Failed to retrieve process information (Error: %lu)\n", GetLastError());
        CloseHandle(hSnapshot);
        return 1;
    }
    
    printf("[+] Enumerating processes for analysis tools...\n\n");
    
    // Enumerate all processes
    do {
        totalProcesses++;
        
        if(isSuspiciousProcess(pe32.szExeFile)) {
            printf("[!] DEBUGGER DETECTED: %s (PID: %lu)\n", pe32.szExeFile, pe32.th32ProcessID);
            foundCount++;
        }
        
    } while(Process32Next(hSnapshot, &pe32));
    
    CloseHandle(hSnapshot);
    
    printf("\nDetection Summary \n");
    printf("[+] Total Processes Scanned: %d\n", totalProcesses);
    
    if(foundCount > 0) {
        printf("[!] WARNING: %d analysis tool(s) detected!\n", foundCount);
        printf("[!] Execution environment appears to be monitored\n");
    } else {
        printf("[+] No suspicious processes detected\n");
        printf("[+] Environment appears clean\n");
    }
    
    printf("\n[*] Monitored Tools: Debuggers, Disassemblers, Process Monitors, Network Sniffers\n");
    
    return 0;
}