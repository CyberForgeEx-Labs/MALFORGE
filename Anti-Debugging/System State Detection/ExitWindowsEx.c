#include <windows.h>
#include <stdio.h>

BOOL CheckSystemShutdownBehavior() {
    printf("[-] Testing system shutdown behavior...\n");
    
    // Attempt to shutdown the system
    BOOL result = ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, SHTDN_REASON_MAJOR_OTHER);
    
    DWORD lastError = GetLastError();
    
    printf("[*] ExitWindowsEx result: %s\n", result ? "TRUE" : "FALSE");
    printf("[*] Last error code: %lu\n", lastError);
    
    // Analysis of results
    if (result == TRUE) {
        // Shutdown was initiated - unlikely in sandboxes
        printf("[+] Shutdown initiated successfully (unexpected in restricted environments)\n");
        return TRUE;
    }
    else {
        // Shutdown failed - analyze error codes
        switch (lastError) {
            case ERROR_SHUTDOWN_IN_PROGRESS:
                printf("[+] Shutdown already in progress\n");
                break;
            case ERROR_ACCESS_DENIED:
                printf("[+] Access denied - expected in user mode without privileges\n");
                break;
            case ERROR_FAIL_NOACTION_REBOOT:
                printf("[+] No action taken - possible sandbox behavior\n");
                return TRUE;
            case ERROR_INVALID_PARAMETER:
                printf("[-] Invalid parameters provided\n");
                break;
            case 0:
                printf("[-] No error code but call failed - suspicious behavior\n");
                return TRUE;
            default:
                printf("[*] Shutdown failed with error: %lu\n", lastError);
                break;
        }
    }
    
    return FALSE;
}

BOOL CheckRebootBehavior() {
    // Attempt to reboot the system
    BOOL result = ExitWindowsEx(EWX_REBOOT | EWX_FORCEIFHUNG, SHTDN_REASON_MAJOR_SOFTWARE);
    
    DWORD lastError = GetLastError();
    
    printf("[+] Reboot attempt result: %s\n", result ? "TRUE" : "FALSE");
    printf("[+] Last error code: %lu\n", lastError);
    
    // Sandboxes often have specific behaviors for reboot attempts
    if (result == FALSE && lastError == 0) {
        printf("[+] Silent failure - common in analysis environments\n");
        return TRUE;
    }
    
    return FALSE;
}

BOOL CheckLogoffBehavior() {
    printf("[*] Testing user logoff behavior...\n");
    
    // Attempt to logoff current user
    BOOL result = ExitWindowsEx(EWX_LOGOFF, 0);
    
    DWORD lastError = GetLastError();
    
    printf("[+] Logoff attempt result: %s\n", result ? "TRUE" : "FALSE");
    printf("[+] Last error code: %lu\n", lastError);
    
    // Analysis environments may handle logoff differently
    if (result == TRUE) {
        printf("[+] Logoff initiated - unexpected in automated analysis\n");
        return TRUE;
    }
    
    return FALSE;
}

int main() {
    printf("System State Shutdown Behavior Detection\n");
    
    // Check current privileges
    HANDLE hToken;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        printf("[*] Process token opened successfully\n");
        CloseHandle(hToken);
    } else {
        printf("[*] Running with standard user privileges\n");
    }
    
    BOOL detectedSuspicious = FALSE;
    
    // Test different shutdown behaviors
    printf("\n1. Testing shutdown behavior:\n");
    if (CheckSystemShutdownBehavior()) {
        detectedSuspicious = TRUE;
    }
    
    printf("\n2. Testing reboot behavior:\n");
    if (CheckRebootBehavior()) {
        detectedSuspicious = TRUE;
    }
    
    printf("\n3. Testing logoff behavior:\n");
    if (CheckLogoffBehavior()) {
        detectedSuspicious = TRUE;
    }
    
    // Final analysis
    printf("\nRESULTS \n");
    if (detectedSuspicious) {
        printf("[+] Suspicious system behavior detected - possible analysis environment\n");
        printf("[*] Analysis environments often handle shutdown calls differently\n");
        return 1;
    } else {
        printf("[+] Normal system behavior detected\n");
        printf("[*] Shutdown calls behaved as expected for current privileges\n");
        return 0;
    }
}