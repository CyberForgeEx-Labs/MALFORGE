#include <windows.h>
#include <stdio.h>
#include <string.h>

// Common sandbox/analysis environment indicators
const char* SUSPICIOUS_COMPUTER_NAMES[] = {
    "SANDBOX",
    "MALWARE",
    "VIRUS",
    "MALTEST",
    "SAMPLE",
    "ANALYSIS",
    "CUCKOO",
    "VMWARE",
    "VBOX",
    NULL
};

const char* SUSPICIOUS_USER_NAMES[] = {
    "JOHN DOE",
    "MALWARE",
    "SANDBOX",
    "VIRUS",
    "CURRENTUSER",
    "SAMPLE",
    "ADMIN",
    "TESTER",
    "MALTEST",
    NULL
};

// Function to convert string to uppercase for case-insensitive comparison
void toUpperCase(char* str) {
    for(int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

// Check if name contains any suspicious keywords
int containsSuspiciousKeyword(const char* name, const char** keywords) {
    char upperName[256];
    strncpy(upperName, name, sizeof(upperName) - 1);
    upperName[sizeof(upperName) - 1] = '\0';
    toUpperCase(upperName);
    
    for(int i = 0; keywords[i] != NULL; i++) {
        if(strstr(upperName, keywords[i]) != NULL) {
            return 1;
        }
    }
    return 0;
}

int main() {
    char computerName[MAX_COMPUTERNAME_LENGTH + 1];
    char userName[256];
    DWORD computerNameSize = sizeof(computerName);
    DWORD userNameSize = sizeof(userName);
    
    printf("Sandbox Detection Predefined Names  \n\n");
    
    // Get Computer Name
    if(GetComputerNameA(computerName, &computerNameSize)) {
        printf("[+] Computer Name: %s\n", computerName);
        
        if(containsSuspiciousKeyword(computerName, SUSPICIOUS_COMPUTER_NAMES)) {
            printf("[!] WARNING: Suspicious computer name detected!\n");
            printf("[!] Possible sandbox/analysis environment\n");
        } else {
            printf("[+] Computer name appears normal\n");
        }
    } else {
        printf("[-] Failed to get computer name (Error: %lu)\n", GetLastError());
    }
    
    printf("\n");
    
    // Get User Name
    if(GetUserNameA(userName, &userNameSize)) {
        printf("[+] User Name: %s\n", userName);
        
        if(containsSuspiciousKeyword(userName, SUSPICIOUS_USER_NAMES)) {
            printf("[!] WARNING: Suspicious user name detected!\n");
            printf("[!] Possible sandbox/analysis environment\n");
        } else {
            printf("[+] User name appears normal\n");
        }
    } else {
        printf("[-] Failed to get user name (Error: %lu)\n", GetLastError());
    }
    
    printf("\nKnown Suspicious Indicators:\n");
    printf("Computer Names: SANDBOX, MALWARE, VIRUS, MALTEST, SAMPLE, ANALYSIS, CUCKOO\n");
    printf("User Names: JOHN DOE, MALWARE, SANDBOX, VIRUS, CURRENTUSER, SAMPLE\n");
    
    return 0;
}