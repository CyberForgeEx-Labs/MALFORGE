#include <windows.h>
#include <stdio.h>
#include <sddl.h>

#define INFO_BUFFER_SIZE 256
#define SID_BUFFER_SIZE 256

// Function prototypes
void PrintSeparator(const char* title);
void DemoGetUserName();
void DemoGetComputerName();
void DemoLookupAccountName();
void DemoLookupPrivilegeValue();
const char* GetSidTypeString(SID_NAME_USE sidType);
const char* GetPrivilegeName(LUID luid);

int main() {
    printf("\nUSER/ACCOUNT ENUMERATION API DEMONSTRATION\n");
    printf("============================================\n\n");

    // Demonstrate GetUserNameA
    DemoGetUserName();

    // Demonstrate GetComputerNameA
    DemoGetComputerName();

    // Demonstrate LookupAccountNameA
    DemoLookupAccountName();

    // Demonstrate LookupPrivilegeValueA
    DemoLookupPrivilegeValue();

    printf("\nENUMERATION COMPLETE\n");

    return 0;
}

void PrintSeparator(const char* title) {
    printf("%s\n", title);
    printf("--------------------\n");
}

// GetUserNameA Demonstration Retrieves the username of the current thread/process
void DemoGetUserName() {
    PrintSeparator("\nGetUserNameA - Current User Information");
    
    char username[INFO_BUFFER_SIZE];
    DWORD usernameLen = INFO_BUFFER_SIZE;
    
    if (GetUserNameA(username, &usernameLen)) {
        printf("Current User Name: %s\n", username);
        printf("Username Length: %lu characters\n", usernameLen - 1); // -1 for null terminator
        
        // Additional context
        printf("\nContext:\n");
        printf("  - This is the name of the user account running this process\n");
        printf("  - Format is typically: DOMAIN\\Username or just Username\n");
        printf("  - Retrieved from current thread's access token\n");
    } else {
        printf("Error: Failed to get username (Error Code: %lu)\n", GetLastError());
    }
}


// GetComputerNameA Demonstration - Retrieves the NetBIOS name of the local computer
void DemoGetComputerName() {
    PrintSeparator("\nGetComputerNameA - Computer Identification");
    
    char computerName[INFO_BUFFER_SIZE];
    DWORD computerNameLen = INFO_BUFFER_SIZE;
    
    if (GetComputerNameA(computerName, &computerNameLen)) {
        printf("Computer Name (NetBIOS): %s\n", computerName);
        printf("Name Length: %lu characters\n", computerNameLen);
        
        // Get additional computer name formats
        char dnsHostname[INFO_BUFFER_SIZE];
        DWORD dnsLen = INFO_BUFFER_SIZE;
        
        if (GetComputerNameExA(ComputerNameDnsHostname, dnsHostname, &dnsLen)) {
            printf("DNS Hostname: %s\n", dnsHostname);
        }
        
        char dnsDomain[INFO_BUFFER_SIZE];
        dnsLen = INFO_BUFFER_SIZE;
        if (GetComputerNameExA(ComputerNameDnsDomain, dnsDomain, &dnsLen)) {
            printf("DNS Domain: %s\n", dnsDomain);
        }
        
        char fqdn[INFO_BUFFER_SIZE];
        dnsLen = INFO_BUFFER_SIZE;
        if (GetComputerNameExA(ComputerNameDnsFullyQualified, fqdn, &dnsLen)) {
            printf("Fully Qualified Domain Name: %s\n", fqdn);
        }
        
        printf("\nContext:\n");
        printf("  - NetBIOS name is limited to 15 characters\n");
        printf("  - Used for network identification and file sharing\n");
        printf("  - Can be different from DNS hostname in some configurations\n");
    } else {
        printf("Error: Failed to get computer name (Error Code: %lu)\n", GetLastError());
    }
}

//LookupAccountNameA Demonstration
//Retrieves the SID and domain for a given account name
void DemoLookupAccountName() {
    PrintSeparator("\nLookupAccountNameA - Account SID Lookup");
    
    // Get current username to lookup
    char username[INFO_BUFFER_SIZE];
    DWORD usernameLen = INFO_BUFFER_SIZE;
    
    if (!GetUserNameA(username, &usernameLen)) {
        printf("Error: Failed to get current username\n");
        return;
    }
    
    printf("Looking up account: %s\n\n", username);
    
    // Prepare buffers for SID and domain
    BYTE sid[SID_BUFFER_SIZE];
    DWORD sidSize = SID_BUFFER_SIZE;
    char domainName[INFO_BUFFER_SIZE];
    DWORD domainSize = INFO_BUFFER_SIZE;
    SID_NAME_USE sidType;
    
    if (LookupAccountNameA(NULL, username, sid, &sidSize, domainName, &domainSize, &sidType)) {
        printf("Account Lookup Successful:\n\n");
        
        // Convert SID to string
        LPSTR sidString = NULL;
        if (ConvertSidToStringSidA((PSID)sid, &sidString)) {
            printf("Security Identifier (SID):\n");
            printf("  String Format: %s\n", sidString);
            printf("  Binary Size: %lu bytes\n", sidSize);
            LocalFree(sidString);
        }
        
        printf("\nDomain Information:\n");
        printf("  Domain Name: %s\n", domainName);
        printf("  Domain Length: %lu characters\n", domainSize);
        
        printf("\nAccount Type:\n");
        printf("  SID Type : %s\n", GetSidTypeString(sidType));
        printf("  Type Value: %d\n", sidType);
        
        // Get additional SID information
        PSID_IDENTIFIER_AUTHORITY psia = GetSidIdentifierAuthority((PSID)sid);
        printf("\nSID Components:\n");
        printf("  Identifier Authority: %u\n", psia->Value[5]); // represent the top-level authority of a Security Identifier (SID) in big-endian byte order.
        
        DWORD subAuthCount = *GetSidSubAuthorityCount((PSID)sid);
        printf("  Sub-Authority Count: %lu\n", subAuthCount);
        
        printf("  Sub-Authorities: ");
        for (DWORD i = 0; i < subAuthCount; i++) {
            DWORD subAuth = *GetSidSubAuthority((PSID)sid, i);
            printf("%lu", subAuth);
            if (i < subAuthCount - 1) printf("-");
        }
        printf("\n");
        
        printf("\nContext:\n");
        printf("  - SIDs are unique identifiers for security principals\n");
        printf("  - Format: S-R-I-S-S-S... (Revision-Authority-SubAuthorities)\n");
        printf("  - Used internally by Windows for access control\n");
        
    } else {
        DWORD error = GetLastError();
        printf("Error: Failed to lookup account (Error Code: %lu)\n", error);
        
        if (error == ERROR_NONE_MAPPED) {
            printf("Reason: Account name could not be mapped to a SID\n");
        } else if (error == ERROR_INSUFFICIENT_BUFFER) {
            printf("Reason: Buffer too small (Required SID: %lu, Domain: %lu)\n", 
                   sidSize, domainSize);
        }
    }
    
    // Demonstrate lookup of well-known accounts
    printf("\n\nWell-Known Account Lookups\n");
    printf("-------------------------------\n");
    
    const char* wellKnownAccounts[] = {
        "SYSTEM",
        "Administrators",
        "Everyone",
        "NETWORK SERVICE"
    };
    
    for (int i = 0; i < 4; i++) {
        sidSize = SID_BUFFER_SIZE;
        domainSize = INFO_BUFFER_SIZE;
        
        if (LookupAccountNameA(NULL, wellKnownAccounts[i], sid, &sidSize, 
                               domainName, &domainSize, &sidType)) {
            LPSTR sidStr = NULL;
            if (ConvertSidToStringSidA((PSID)sid, &sidStr)) {
                printf("%-20s %s (%s)\n", wellKnownAccounts[i], sidStr, 
                       GetSidTypeString(sidType));
                LocalFree(sidStr);
            }
        }
    }
}

//LookupPrivilegeValueA Demonstration
//Retrieves the LUID for a specified privilege name
void DemoLookupPrivilegeValue() {
    PrintSeparator("\nLookupPrivilegeValueA - Privilege LUID Lookup");
    
    printf("Looking up common Windows privileges\n\n");
    
    // Array of common privilege names
    //Can use "whoami /priv" retrive priv info from cmd.
    const char* privileges[] = {
        SE_DEBUG_NAME,              // SeDebugPrivilege
        SE_SHUTDOWN_NAME,           // SeShutdownPrivilege
        SE_TCB_NAME,                // SeTcbPrivilege
        SE_BACKUP_NAME,             // SeBackupPrivilege
        SE_RESTORE_NAME,            // SeRestorePrivilege
        SE_SECURITY_NAME,           // SeSecurityPrivilege
        SE_SYSTEMTIME_NAME,         // SeSystemtimePrivilege
        SE_LOAD_DRIVER_NAME,        // SeLoadDriverPrivilege
        SE_TAKE_OWNERSHIP_NAME,     // SeTakeOwnershipPrivilege
        SE_INC_BASE_PRIORITY_NAME   // SeIncreaseBasePriorityPrivilege
    };
    
    //alignment setting
    printf("%-35s %-20s %s\n", "Privilege Name", "LUID", "Description");
    printf("%-35s %-20s %s\n", 
           "-----------------------------------", 
           "--------------------",
           "----------------------------------------");
    
    for (int i = 0; i < 10; i++) {
        LUID luid;
        
        if (LookupPrivilegeValueA(NULL, privileges[i], &luid)) {
            printf("%-35s 0x%08X%08X   ", privileges[i], luid.HighPart, luid.LowPart);
            
            // Get privilege display name if possible
            char displayName[256];
            DWORD displayNameSize = sizeof(displayName);
            DWORD langId = 0;
            
            if (LookupPrivilegeDisplayNameA(NULL, privileges[i], displayName, 
                                           &displayNameSize, &langId)) {
                printf("%s\n", displayName);
            } else {
                printf("\n");
            }
        } else {
            printf("%-35s ERROR (Code: %lu)\n", privileges[i], GetLastError());
        }
    }
    
    printf("\n\nLUID Details:\n");
    printf("  - LUID = Locally Unique Identifier\n");
    printf("  - 64-bit value (HighPart + LowPart)\n");
    printf("  - Unique on the local system until reboot\n");
    printf("  - Used to represent privileges in access tokens\n");
    
    // Check current process privileges
    printf("\n\nCurrent Process Token Privileges:\n");
    printf("-------------------------------------\n");
    
    HANDLE hToken;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        DWORD tokenInfoLen = 0;
        GetTokenInformation(hToken, TokenPrivileges, NULL, 0, &tokenInfoLen);
        
        if (tokenInfoLen > 0) {
            PTOKEN_PRIVILEGES pPrivileges = (PTOKEN_PRIVILEGES)malloc(tokenInfoLen);
            
            if (pPrivileges && GetTokenInformation(hToken, TokenPrivileges, 
                                                   pPrivileges, tokenInfoLen, &tokenInfoLen)) {
                printf("Total Privileges: %lu\n\n", pPrivileges->PrivilegeCount);
                printf("%-40s %-10s %s\n", "Privilege", "State", "LUID");
                printf("%-40s %-10s %s\n", 
                       "----------------------------------------", 
                       "----------",
                       "--------------------");
                
                for (DWORD i = 0; i < pPrivileges->PrivilegeCount; i++) {
                    char privName[256];
                    DWORD nameLen = sizeof(privName);
                    
                    if (LookupPrivilegeNameA(NULL, &pPrivileges->Privileges[i].Luid, 
                                            privName, &nameLen)) {
                        const char* state = "Unknown";
                        DWORD attr = pPrivileges->Privileges[i].Attributes;
                        
                        if (attr & SE_PRIVILEGE_ENABLED_BY_DEFAULT)
                            state = "Default";
                        else if (attr & SE_PRIVILEGE_ENABLED)
                            state = "Enabled";
                        else if (attr & SE_PRIVILEGE_REMOVED)
                            state = "Removed";
                        else
                            state = "Disabled";
                        
                        printf("%-40s %-10s 0x%08X%08X\n", 
                               privName, state,
                               pPrivileges->Privileges[i].Luid.HighPart,
                               pPrivileges->Privileges[i].Luid.LowPart);
                    }
                }
                
                free(pPrivileges);
            }
        }
        
        CloseHandle(hToken);
    }
}

/*
 * Helper function to convert SID_NAME_USE enum to string
 */
const char* GetSidTypeString(SID_NAME_USE sidType) {
    switch (sidType) {
        case SidTypeUser:           return "User Account";
        case SidTypeGroup:          return "Group Account";
        case SidTypeDomain:         return "Domain";
        case SidTypeAlias:          return "Alias (Local Group)";
        case SidTypeWellKnownGroup: return "Well-Known Group";
        case SidTypeDeletedAccount: return "Deleted Account";
        case SidTypeInvalid:        return "Invalid SID";
        case SidTypeUnknown:        return "Unknown";
        case SidTypeComputer:       return "Computer Account";
        case SidTypeLabel:          return "Mandatory Integrity Label";
        default:                    return "Undefined";
    }
}

/*
 * Helper function to get human-readable privilege name
 */
const char* GetPrivilegeName(LUID luid) {
    static char name[256];
    DWORD nameLen = sizeof(name);
    
    if (LookupPrivilegeNameA(NULL, &luid, name, &nameLen)) {
        return name;
    }
    return "Unknown";
}