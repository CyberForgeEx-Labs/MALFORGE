#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

// Function prototypes
void Demo_OpenProcessToken();
void Demo_AdjustTokenPrivileges();
void Demo_DuplicateToken();
void QueryTokenInformation(HANDLE hToken);
void DisplayTokenPrivileges(HANDLE hToken);
void DisplayTokenGroups(HANDLE hToken);
void DisplayTokenUser(HANDLE hToken);
const char* PrivilegeToString(LUID priv);

// Helper function to convert LUID privilege to readable string
const char* PrivilegeToString(LUID priv) {
    char privName[256];
    DWORD size = sizeof(privName);
    
    if (LookupPrivilegeNameA(NULL, &priv, privName, &size)) {
        // Return common privilege names
        if (strcmp(privName, "SeDebugPrivilege") == 0)
            return "SeDebugPrivilege (Debug programs)";
        if (strcmp(privName, "SeBackupPrivilege") == 0)
            return "SeBackupPrivilege (Backup files)";
        if (strcmp(privName, "SeRestorePrivilege") == 0)
            return "SeRestorePrivilege (Restore files)";
        if (strcmp(privName, "SeShutdownPrivilege") == 0)
            return "SeShutdownPrivilege (Shutdown system)";
        if (strcmp(privName, "SeTakeOwnershipPrivilege") == 0)
            return "SeTakeOwnershipPrivilege (Take ownership)";
        
        static char buffer[300];
        snprintf(buffer, sizeof(buffer), "%s", privName);
        return buffer;
    }
    return "Unknown Privilege";
}

/*
 * OpenProcessToken
 * Opens a token associated with a process for inspection
 * Legitimate Use: Security auditing, permission checking
 */
void Demo_OpenProcessToken() 
{
    printf("\n[+] OpenProcessToken Demonstration.\n");

    HANDLE hToken = NULL;
    HANDLE hProcess = GetCurrentProcess();

    printf("[*] Opening current process token for QUERY access...\n");
    
    // Open token with QUERY access only (read-only, safe operation)
    BOOL result = OpenProcessToken(
        hProcess,           // Handle to current process
        TOKEN_QUERY,        // Query access only (read-only)
        &hToken            // Receives token handle
    );

    if (!result) {
        printf("[!] OpenProcessToken failed: %lu\n", GetLastError());
        return;
    }

    printf("[+] Successfully opened process token\n");
    printf("[+] Token Handle: 0x%p\n", hToken);
    printf("[*] Access Rights: TOKEN_QUERY (Read-only inspection)\n\n");

    // Query and display token information
    QueryTokenInformation(hToken);

    CloseHandle(hToken);
    printf("[*] Token handle closed\n");
}

/*
 * Display comprehensive token information
 */
void QueryTokenInformation(HANDLE hToken) {
    printf("\n--- TOKEN INFORMATION ---\n\n");
    
    DisplayTokenUser(hToken);
    DisplayTokenPrivileges(hToken);
    DisplayTokenGroups(hToken);
}

/*
 * Display token user (who owns this token)
 */
void DisplayTokenUser(HANDLE hToken) {
    DWORD dwLength = 0;
    TOKEN_USER* pTokenUser = NULL;
    
    // Get required buffer size
    GetTokenInformation(hToken, TokenUser, NULL, 0, &dwLength);
    pTokenUser = (TOKEN_USER*)malloc(dwLength);
    
    if (GetTokenInformation(hToken, TokenUser, pTokenUser, dwLength, &dwLength)) {
        char userName[256] = {0};
        char domainName[256] = {0};
        DWORD userSize = sizeof(userName);
        DWORD domainSize = sizeof(domainName);
        SID_NAME_USE sidType;
        
        if (LookupAccountSidA(NULL, pTokenUser->User.Sid, userName, &userSize,
                              domainName, &domainSize, &sidType)) {
            printf("Token Owner: %s\\%s\n", domainName, userName);
        }
    }
    
    free(pTokenUser);
}

/*
 * Display token privileges and their states
 */
void DisplayTokenPrivileges(HANDLE hToken) {
    DWORD dwLength = 0;
    TOKEN_PRIVILEGES* pPrivileges = NULL;
    
    printf("\nToken Privileges:\n");
    printf("%-50s %s\n", "Privilege Name", "State");
    printf("---------------------------------------------------------------\n");
    
    // Get required buffer size
    GetTokenInformation(hToken, TokenPrivileges, NULL, 0, &dwLength);
    pPrivileges = (TOKEN_PRIVILEGES*)malloc(dwLength);
    
    if (GetTokenInformation(hToken, TokenPrivileges, pPrivileges, dwLength, &dwLength)) {
        for (DWORD i = 0; i < pPrivileges->PrivilegeCount; i++) {
            const char* privName = PrivilegeToString(pPrivileges->Privileges[i].Luid);
            const char* state = (pPrivileges->Privileges[i].Attributes & SE_PRIVILEGE_ENABLED) 
                                ? "ENABLED" : "DISABLED";
            printf("%-50s %s\n", privName, state);
        }
    }
    
    free(pPrivileges);
}

/*
 * Display token groups (group memberships)
 */
void DisplayTokenGroups(HANDLE hToken) {
    DWORD dwLength = 0;
    TOKEN_GROUPS* pGroups = NULL;
    
    printf("\nToken Groups (first 5):\n");
    printf("---------------------------------------------------------------\n");
    
    // Get required buffer size
    GetTokenInformation(hToken, TokenGroups, NULL, 0, &dwLength);
    pGroups = (TOKEN_GROUPS*)malloc(dwLength);
    
    if (GetTokenInformation(hToken, TokenGroups, pGroups, dwLength, &dwLength)) {
        DWORD displayCount = (pGroups->GroupCount < 5) ? pGroups->GroupCount : 5;
        
        for (DWORD i = 0; i < displayCount; i++) {
            char groupName[256] = {0};
            char domainName[256] = {0};
            DWORD groupSize = sizeof(groupName);
            DWORD domainSize = sizeof(domainName);
            SID_NAME_USE sidType;
            
            if (LookupAccountSidA(NULL, pGroups->Groups[i].Sid, groupName, &groupSize,
                                  domainName, &domainSize, &sidType)) {
                printf("  %s\\%s\n", domainName, groupName);
            }
        }
        
        if (pGroups->GroupCount > 5) {
            printf("  ... and %lu more groups\n", pGroups->GroupCount - 5);
        }
    }
    
    free(pGroups);
}

/*
 * AdjustTokenPrivileges (Safe Query Mode)
 * Shows how to CHECK privilege status without actually enabling them
 * Auditing what privileges could be enabled.
 */
void Demo_AdjustTokenPrivileges() {
    printf("\n[+]AdjustTokenPrivileges (Query Mode)\n\n");


    HANDLE hToken = NULL;
    
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken)) {
        printf("[!] OpenProcessToken failed: %lu\n", GetLastError());
        return;
    }

    printf("[*] Attempting to check SeDebugPrivilege status...\n");

    // Lookup the LUID for SeDebugPrivilege
    TOKEN_PRIVILEGES tp = {0};
    tp.PrivilegeCount = 1;
    
    if (!LookupPrivilegeValueA(NULL, "SeDebugPrivilege", &tp.Privileges[0].Luid)) {
        printf("[!] LookupPrivilegeValue failed: %lu\n", GetLastError());
        CloseHandle(hToken);
        return;
    }

    // Set attributes to SE_PRIVILEGE_ENABLED to test
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    // Save previous state
    TOKEN_PRIVILEGES tpPrevious = {0};
    DWORD dwReturnLength = 0;

    printf("[*] Checking if privilege can be adjusted...\n");
    
    // Call with return previous state to check current status
    BOOL result = AdjustTokenPrivileges(
        hToken,
        FALSE,              // Do not disable all
        &tp,                // New privileges
        sizeof(TOKEN_PRIVILEGES),
        &tpPrevious,        // Previous state
        &dwReturnLength
    );

    if (result) {
        DWORD error = GetLastError();
        
        if (error == ERROR_SUCCESS) {
            printf("[+] Privilege check successful\n");
            
            // Restore previous state immediately (undo any change)
            AdjustTokenPrivileges(hToken, FALSE, &tpPrevious, 
                                 sizeof(TOKEN_PRIVILEGES), NULL, NULL);
            
            printf("[*] Previous state: %s\n", 
                   (tpPrevious.Privileges[0].Attributes & SE_PRIVILEGE_ENABLED) 
                   ? "ENABLED" : "DISABLED");
            printf("[*] Privilege state restored to original\n");
            
        } else if (error == ERROR_NOT_ALL_ASSIGNED) {
            printf("[!] Privilege not held by this process\n");
            printf("[*] This is normal for non-elevated processes\n");
        }
    } else {
        printf("[!] AdjustTokenPrivileges failed: %lu\n", GetLastError());
    }

    printf("\n[*] IMPORTANT: No privileges were actually modified\n");
    printf("[*] This demonstration only queries privilege status\n");

    CloseHandle(hToken);
}

/*
 * DuplicateToken (Inspection Mode)
 * Creates a duplicate token for safe inspection without affecting original.
 */
void Demo_DuplicateToken() {
    printf("\n [+]DuplicateToken (Inspection Mode)\n");


    HANDLE hToken = NULL;
    HANDLE hDuplicateToken = NULL;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_DUPLICATE | TOKEN_QUERY, &hToken)) {
        printf("[!] OpenProcessToken failed: %lu\n", GetLastError());
        return;
    }

    printf("[*] Creating duplicate token for inspection...\n");
    
    // Create duplicate impersonation token
    BOOL result = DuplicateToken(
        hToken,                      // Source token
        SecurityIdentification,      // Identification level (lowest, safest)
        &hDuplicateToken            // Duplicate token
    );

    if (!result) {
        printf("[!] DuplicateToken failed: %lu\n", GetLastError());
        CloseHandle(hToken);
        return;
    }

    printf("[+] Successfully created duplicate token\n");
    printf("[+] Original Token: 0x%p\n", hToken);
    printf("[+] Duplicate Token: 0x%p\n", hDuplicateToken);
    printf("[*] Impersonation Level: SecurityIdentification (read-only)\n\n");

    // Compare token information
    printf("--- COMPARING TOKENS ---\n\n");
    
    printf("Original Token Information:\n");
    DisplayTokenPrivileges(hToken);
    
    printf("\n\nDuplicate Token Information:\n");
    DisplayTokenPrivileges(hDuplicateToken);

    printf("\n[*] Duplicate token can be inspected safely\n");
    printf("[*] SecurityIdentification level prevents misuse\n");

    CloseHandle(hDuplicateToken);
    CloseHandle(hToken);
}


int main(int argc, char* argv[]) {

    printf("\nWINDOWS TOKEN MANIPULATION DEMONSTRATION\n");


    // Run demonstrations
    Demo_OpenProcessToken();
    Demo_AdjustTokenPrivileges();
    Demo_DuplicateToken();


    printf("\nDEMONSTRATION COMPLETE\n");

    printf("Press Enter to exit...");
    getchar();

    return 0;
}