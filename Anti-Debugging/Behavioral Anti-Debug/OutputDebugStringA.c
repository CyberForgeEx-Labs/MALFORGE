#include <windows.h>
#include <stdio.h>

void CheckDebugger_OutputDebugString() 
{
    printf("[*] Testing OutputDebugStringA behavior...\n");
    
    // Set unique error code
    SetLastError(0x12345678);  
    
    // Call OutputDebugStringA with a string
    OutputDebugStringA("Anti-Debug Check");
    
    // Get the error code immediately
    DWORD errorCode = GetLastError();
    
    printf("[*] GetLastError() returned: 0x%lX\n", errorCode);
    
    // If debugger is present: error stays as 0x12345678 (unchanged)
    // If no debugger: Windows sets error to ERROR_INVALID_HANDLE (6)
    if (errorCode == 0x12345678) {
        // Error code did NOT change = debugger consumed the message.
        printf("[!] DEBUGGER DETECTED!\n");
        printf("[!] Reason: OutputDebugStringA did not set error (debugger consumed message)\n");
        ExitProcess(1);
    } 
    else if (errorCode == ERROR_INVALID_HANDLE) {
        // Error code changed to 6 = no debugger present
        printf("[+] No debugger detected (ERROR_INVALID_HANDLE received).\n");
    } 
    else {
        printf("[+] No debugger detected (error code: 0x%lX).\n", errorCode);
    }
}

int main() 
{
    CheckDebugger_OutputDebugString();
    printf("\n[+] OutputDebugString check passed!\n");    
    return 0;
}
