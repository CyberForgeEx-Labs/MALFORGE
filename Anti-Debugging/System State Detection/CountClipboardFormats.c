#include <windows.h>
#include <stdio.h>

BOOL CheckSuspiciousClipboardState() {
    int formatCount = CountClipboardFormats();
    
    // Debug information
    printf("[*] Clipboard format count: %d\n", formatCount);
    
    // Suspicious conditions for sandbox/virtual environment detection
    if (formatCount == 0) {
        // Empty clipboard - common in fresh sandbox environments
        printf("[+] Empty clipboard detected (common in sandboxes)\n");
        return TRUE;
    }
    else if (formatCount > 20) {
        // Unusually high number of formats - might indicate analysis tools
        printf("[+] High clipboard format count: %d (possible analysis tools)\n", formatCount);
        return TRUE;
    }
    else if (formatCount == 1) {
        // Single format - often just CF_TEXT in basic environments
        printf("[+] Single clipboard format detected (typical in minimal environments)\n");
        return TRUE;
    }
    
    printf("[-] Normal clipboard state detected\n");
    return FALSE;
}

int main() {
    printf("Clipboard State Detection Tool\n");
    
    if (OpenClipboard(NULL)) {
        BOOL isSuspicious = CheckSuspiciousClipboardState();
        CloseClipboard();
        
        if (isSuspicious) {
            printf("\n[+] Suspicious system state detected - possible sandbox/virtual environment\n");
            return 1;
        } else {
            printf("[-] Normal system state detected\n");
            return 0;
        }
    } else {
        printf("[-] Failed to open clipboard (Error: %lu)\n", GetLastError());
        return -1;
    }
}