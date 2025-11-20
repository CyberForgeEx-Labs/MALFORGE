#include <windows.h>
#include <stdio.h>

// Callback function for EnumSystemLocalesA
BOOL CALLBACK LocaleEnumProc(LPSTR lpLocaleString) {
    static int count = 0;
    LCID lcid;
    char countryName[128];
    char languageName[128];
    
    // Convert locale string to LCID
    sscanf(lpLocaleString, "%x", &lcid);
    
    // Get locale information
    GetLocaleInfoA(lcid, LOCALE_SENGLANGUAGE, languageName, sizeof(languageName));
    GetLocaleInfoA(lcid, LOCALE_SENGCOUNTRY, countryName, sizeof(countryName));
    
    printf("  [%2d] Locale: %s - %s (%s)\n", 
           ++count, languageName, countryName, lpLocaleString);
    
    // Only show first 10 for brevity
    if (count >= 10) {
        printf("  ... (showing first 10 only)\n");
        return FALSE; // Stop enumeration
    }
    
    return TRUE; // Continue enumeration.
}

int main() 
{
    char buffer[256];
    DWORD result;
    
    printf("\n=== Windows Environment Manipulation API Demonstration ===\n\n");
    
    // Reading Environment Variables
    printf("[1] Reading existing environment variables\n");
    
    // Read some common environment variables
    const char* commonVars[] = {"PATH", "TEMP", "USERNAME", "COMPUTERNAME"};
    
    for (int i = 0; i < 4; i++) {
        result = GetEnvironmentVariableA(commonVars[i], buffer, sizeof(buffer));
        if (result > 0) {
            // Truncate PATH for readability
            if (strcmp(commonVars[i], "PATH") == 0 && result > 80) {
                buffer[80] = '\0';
                printf("  %s = %s...\n", commonVars[i], buffer);
            } else {
                printf("  %s = %s\n", commonVars[i], buffer);
            }
        } else {
            printf("  %s = (not set)\n", commonVars[i]);
        }
    }
    printf("\n");
    
    //  SetEnvironmentVariableA - Set variables
    printf("[2] Demonstrating SetEnvironmentVariableA API\n");
    
    // Set a custom application variable.
    const char* varName = "MY_APP_CONFIG";
    const char* varValue = "debug_mode=true;log_level=verbose";
    
    if (SetEnvironmentVariableA(varName, varValue)) {
        printf("[+] SetEnvironmentVariableA: Variable set successfully\n");
        printf("    Name:  %s\n", varName);
        printf("    Value: %s\n", varValue);
        
        // Verify by reading it back
        result = GetEnvironmentVariableA(varName, buffer, sizeof(buffer));
        if (result > 0) {
            printf("    Verified: %s\n", buffer);
        }
    } else {
        printf("[-] SetEnvironmentVariableA failed. Error: %lu\n", GetLastError());
    }
    printf("\n");
    
    //Modify PATH temporarily (common in installers)
    printf("Example: Temporarily modifying PATH variable\n");
    
    result = GetEnvironmentVariableA("PATH", buffer, sizeof(buffer));
    if (result > 0 && result < sizeof(buffer) - 50) {
        char newPath[512];
        snprintf(newPath, sizeof(newPath), "%s;C:\\MyApp\\bin", buffer);
        
        if (SetEnvironmentVariableA("PATH", newPath)) {
            printf("[+] PATH updated with custom directory\n");
            printf("    Added: C:\\MyApp\\bin\n");
        }
    }
    printf("\n");
    
    // Set locale preference.
    printf("Example: Setting locale preference\n");
    if (SetEnvironmentVariableA("LANG", "en_US.UTF-8")) {
        printf("[+] Locale environment variable set\n");
        printf("    LANG = en_US.UTF-8\n");
    }
    printf("\n");
    
    // Delete an environment variable
    printf("Example: Removing a variable (pass NULL as value)\n");
    if (SetEnvironmentVariableA("MY_APP_CONFIG", NULL)) {
        printf("[+] Variable MY_APP_CONFIG removed\n");
    }
    printf("\n");
    
    // EnumSystemLocalesA - Enumerate locales
    printf("[3] Demonstrating EnumSystemLocalesA API\n");
    
    printf("Available system locales:\n");
    if (EnumSystemLocalesA(LocaleEnumProc, LCID_SUPPORTED)) {
        printf("\n[+] EnumSystemLocalesA: Enumeration successful\n");
    } else {
        printf("\n[-] EnumSystemLocalesA failed. Error: %lu\n", GetLastError());
    }
    printf("\n");
    
    // Get current locale info.
    printf("Current system locale information:\n");
    LCID currentLCID = GetSystemDefaultLCID();
    
    GetLocaleInfoA(currentLCID, LOCALE_SENGLANGUAGE, buffer, sizeof(buffer));
    printf("  Language: %s\n", buffer);
    
    GetLocaleInfoA(currentLCID, LOCALE_SENGCOUNTRY, buffer, sizeof(buffer));
    printf("  Country:  %s\n", buffer);
    
    GetLocaleInfoA(currentLCID, LOCALE_SSHORTDATE, buffer, sizeof(buffer));
    printf("  Date format: %s\n", buffer);
    
    GetLocaleInfoA(currentLCID, LOCALE_SCURRENCY, buffer, sizeof(buffer));
    printf("  Currency symbol: %s\n", buffer);
    
    printf("\nDemonstration Complete\n");
    
    return 0;
}