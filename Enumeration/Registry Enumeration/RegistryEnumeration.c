#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum buffer sizes
#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383
#define MAX_VALUE_DATA 32768

// Function prototypes
void demo_RegEnumKeyExA(void);
void demo_RegEnumValueA(void);
void demo_RegEnumKeyA(void);
void demo_RegQueryInfoKeyA(void);
void demo_RegQueryMultipleValuesA(void);
void demo_RegQueryValueExA(void);
void demo_GetCurrentHwProfileA(void);
void printRegistryValueType(DWORD type);
void printSeparator(void);

int main(void)
{

    printf("\n[*] Registry Enumeration API Demonstrations\n\n");

    // Demo 1: RegEnumKeyExA
    demo_RegEnumKeyExA();
    printSeparator();

    // Demo 2: RegEnumValueA
    demo_RegEnumValueA();
    printSeparator();

    // Demo 3: RegEnumKeyA
    demo_RegEnumKeyA();
    printSeparator();

    // Demo 4: RegQueryInfoKeyA
    demo_RegQueryInfoKeyA();
    printSeparator();

    // Demo 5: RegQueryMultipleValuesA
    demo_RegQueryMultipleValuesA();
    printSeparator();

    // Demo 6: RegQueryValueExA
    demo_RegQueryValueExA();
    printSeparator();

    // Demo 7: GetCurrentHwProfileA
    demo_GetCurrentHwProfileA();

    printf("\n[+] All demonstrations completed successfully!\n");
    printf("\nPress Enter to exit...");
    getchar();

    return 0;
}

void printSeparator(void)
{
    printf("\n================================================================\n\n");
}

void printRegistryValueType(DWORD type)
{
    switch (type) {
        case REG_SZ:
            printf("REG_SZ (String)");
            break;
        case REG_EXPAND_SZ:
            printf("REG_EXPAND_SZ (Expandable String)");
            break;
        case REG_BINARY:
            printf("REG_BINARY (Binary)");
            break;
        case REG_DWORD:
            printf("REG_DWORD (32-bit Number)");
            break;
        case REG_QWORD:
            printf("REG_QWORD (64-bit Number)");
            break;
        case REG_MULTI_SZ:
            printf("REG_MULTI_SZ (Multi-String)");
            break;
        case REG_NONE:
            printf("REG_NONE");
            break;
        default:
            printf("Unknown Type (0x%X)", type);
            break;
    }
}

// RegEnumKeyExA - Enumerates subkeys with extended information
void demo_RegEnumKeyExA(void)
{
    printf("[1] RegEnumKeyExA - Enumerate Subkeys with Extended Info\n");
    printf("    Target: HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\n\n");

    HKEY hKey;
    LONG result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft", 0, KEY_READ, &hKey);

    if (result != ERROR_SUCCESS) {
        printf("[!] Failed to open registry key. Error: %ld\n", result);
        return;
    }

    printf("%-5s %-40s %-20s\n", "Index", "Subkey Name", "Last Write Time");
    printf("------------------------------------------------------------------------\n");

    DWORD index = 0;
    char subkeyName[MAX_KEY_LENGTH];
    DWORD subkeyNameSize;
    FILETIME lastWriteTime;
    int displayCount = 0;

    while (displayCount < 10) {  // Show first 10 subkeys
        subkeyNameSize = MAX_KEY_LENGTH;
        result = RegEnumKeyExA(hKey, index, subkeyName, &subkeyNameSize, NULL, NULL, NULL, &lastWriteTime);

        if (result == ERROR_SUCCESS) {
            SYSTEMTIME st;
            FileTimeToSystemTime(&lastWriteTime, &st);

            printf("%-5lu %-40s %04d-%02d-%02d %02d:%02d\n",
                   index, subkeyName, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);
            displayCount++;
        } else if (result == ERROR_NO_MORE_ITEMS) {
            break;
        } else {
            printf("[!] Enumeration error at index %lu\n", index);
            break;
        }
        index++;
    }

    printf("\n[+] Successfully enumerated %d subkeys\n", displayCount);
    RegCloseKey(hKey);
}

//  RegEnumValueA - Enumerates values in a registry key
void demo_RegEnumValueA(void)
{
    printf("[2] RegEnumValueA - Enumerate Registry Values\n");
    printf("    Target: HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\n\n");

    HKEY hKey;
    LONG result = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                                  "SOFTWARE\\Microsoft\\Windows\\CurrentVersion",
                                  0, KEY_READ, &hKey);

    if (result != ERROR_SUCCESS) {
        printf("[!] Failed to open registry key. Error: %ld\n", result);
        return;
    }

    printf("%-5s %-35s %-30s %-10s\n", "Index", "Value Name", "Type", "Data Size");
    printf("--------------------------------------------------------------------------------\n");

    DWORD index = 0;
    char valueName[MAX_VALUE_NAME];
    DWORD valueNameSize;
    DWORD valueType;
    DWORD dataSize;
    int displayCount = 0;

    while (displayCount < 10) {  // Show first 10 values
        valueNameSize = MAX_VALUE_NAME;
        dataSize = 0;

        result = RegEnumValueA(hKey, index, valueName, &valueNameSize, NULL, &valueType, NULL, &dataSize);

        if (result == ERROR_SUCCESS) {
            printf("%-5lu %-35s ", index, valueName);
            printRegistryValueType(valueType);

            // Calculate padding for alignment
            int typeLen = 0;
            switch (valueType) {
                case REG_SZ: typeLen = 16; break;
                case REG_EXPAND_SZ: typeLen = 32; break;
                case REG_BINARY: typeLen = 18; break;
                case REG_DWORD: typeLen = 24; break;
                case REG_QWORD: typeLen = 24; break;
                case REG_MULTI_SZ: typeLen = 25; break;
                default: typeLen = 20; break;
            }

            int padding = 30 - typeLen;
            if (padding > 0) {
                printf("%*s", padding, "");
            }

            printf(" %lu bytes\n", dataSize);
            displayCount++;
        } else if (result == ERROR_NO_MORE_ITEMS) {
            break;
        } else {
            printf("[!] Enumeration error at index %lu\n", index);
            break;
        }
        index++;
    }

    printf("\n[+] Successfully enumerated %d values\n", displayCount);
    RegCloseKey(hKey);
}

// RegEnumKeyA  Enumerates subkeys (older API) 
void demo_RegEnumKeyA(void)
{
    printf("[3] RegEnumKeyA - Enumerate Subkeys (Legacy API)\n");
    printf("    Target: HKEY_CURRENT_USER\\Software\n\n");

    HKEY hKey;
    LONG result = RegOpenKeyA(HKEY_CURRENT_USER, "Software", &hKey);

    if (result != ERROR_SUCCESS) {
        printf("[!] Failed to open registry key. Error: %ld\n", result);
        return;
    }

    printf("%-5s %-50s\n", "Index", "Subkey Name");
    printf("-------------------------------------------\n");

    DWORD index = 0;
    char subkeyName[MAX_KEY_LENGTH];
    int displayCount = 0;

    while (displayCount < 15) {  // Show first 15 subkeys
        result = RegEnumKeyA(hKey, index, subkeyName, MAX_KEY_LENGTH);

        if (result == ERROR_SUCCESS) {
            printf("%-5lu %s\n", index, subkeyName);
            displayCount++;
        } else if (result == ERROR_NO_MORE_ITEMS) {
            break;
        } else {
            printf("[!] Enumeration error at index %lu\n", index);
            break;
        }
        index++;
    }

    printf("\n[+] Successfully enumerated %d subkeys using legacy API\n", displayCount);
    RegCloseKey(hKey);
}

// RegQueryInfoKeyA - Queries detailed registry key information
void demo_RegQueryInfoKeyA(void)
{
    printf("[4] RegQueryInfoKeyA - Query Registry Key Information\n");
    printf("    Target: HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\n\n");

    HKEY hKey;
    LONG result = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                                  "SOFTWARE\\Microsoft\\Windows\\CurrentVersion",
                                  0, KEY_READ, &hKey);

    if (result != ERROR_SUCCESS) {
        printf("[!] Failed to open registry key. Error: %ld\n", result);
        return;
    }

    char className[MAX_PATH];
    DWORD classNameSize = MAX_PATH;
    DWORD numSubKeys = 0;
    DWORD maxSubKeyLen = 0;
    DWORD maxClassLen = 0;
    DWORD numValues = 0;
    DWORD maxValueNameLen = 0;
    DWORD maxValueLen = 0;
    DWORD securityDescriptor = 0;
    FILETIME lastWriteTime;

    result = RegQueryInfoKeyA(hKey, className, &classNameSize, NULL,
                              &numSubKeys, &maxSubKeyLen, &maxClassLen,
                              &numValues, &maxValueNameLen, &maxValueLen,
                              &securityDescriptor, &lastWriteTime);

    if (result == ERROR_SUCCESS) {
        SYSTEMTIME st;
        FileTimeToSystemTime(&lastWriteTime, &st);

        printf("[Key Information]\n");
        printf("  Class Name:              %s\n", className);
        printf("  Number of Subkeys:       %lu\n", numSubKeys);
        printf("  Max Subkey Length:       %lu\n", maxSubKeyLen);
        printf("  Max Class Length:        %lu\n", maxClassLen);
        printf("  Number of Values:        %lu\n", numValues);
        printf("  Max Value Name Length:   %lu\n", maxValueNameLen);
        printf("  Max Value Data Length:   %lu bytes\n", maxValueLen);
        printf("  Security Descriptor:     %lu bytes\n", securityDescriptor);
        printf("  Last Write Time:         %04d-%02d-%02d %02d:%02d:%02d\n",
               st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

        printf("\n[+] Successfully queried key information\n");
    } else {
        printf("[!] Failed to query key information. Error: %ld\n", result);
    }

    RegCloseKey(hKey);
}

// RegQueryMultipleValuesA - Queries multiple registry values at once
void demo_RegQueryMultipleValuesA(void)
{
    printf("[5] RegQueryMultipleValuesA - Query Multiple Values Simultaneously\n");
    printf("    Target: HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\n\n");

    HKEY hKey;
    LONG result = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                                  "SOFTWARE\\Microsoft\\Windows\\CurrentVersion",
                                  0, KEY_READ, &hKey);

    if (result != ERROR_SUCCESS) {
        printf("[!] Failed to open registry key. Error: %ld\n", result);
        return;
    }

    // Define multiple values to query
    VALENTA valueEntries[3] = {0};
    valueEntries[0].ve_valuename = "ProgramFilesDir";
    valueEntries[1].ve_valuename = "CommonFilesDir";
    valueEntries[2].ve_valuename = "DevicePath";

    DWORD bufferSize = MAX_VALUE_DATA;
    char* buffer = (char*)malloc(bufferSize);

    if (buffer == NULL) {
        printf("[!] Failed to allocate memory\n");
        RegCloseKey(hKey);
        return;
    }

    result = RegQueryMultipleValuesA(hKey, valueEntries, 3, buffer, &bufferSize);

    if (result == ERROR_SUCCESS) {
        printf("[Multiple Values Query Results]\n");

        for (int i = 0; i < 3; i++) {
            printf("\n  Value Name: %s\n", valueEntries[i].ve_valuename);
            printf("  Type:       ");
            printRegistryValueType(valueEntries[i].ve_type);
            printf("\n");

            if (valueEntries[i].ve_valuelen > 0 && valueEntries[i].ve_valueptr != 0) {
                if (valueEntries[i].ve_type == REG_SZ || valueEntries[i].ve_type == REG_EXPAND_SZ) {
                    printf("  Data:       %s\n", (char*)(buffer + (valueEntries[i].ve_valueptr - (DWORD_PTR)buffer)));
                } else {
                    printf("  Data Size:  %lu bytes\n", valueEntries[i].ve_valuelen);
                }
            } else {
                printf("  Data:       (No data)\n");
            }
        }

        printf("\n[+] Successfully queried multiple values\n");
    } else {
        printf("[!] Failed to query multiple values. Error: %ld\n", result);
    }

    free(buffer);
    RegCloseKey(hKey);
}

// RegQueryValueExA - Queries specific registry value data
void demo_RegQueryValueExA(void)
{
    printf("[6] RegQueryValueExA - Query Specific Registry Value\n");
    printf("    Target: HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\n\n");

    HKEY hKey;
    LONG result = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                                  "SOFTWARE\\Microsoft\\Windows\\CurrentVersion",
                                  0, KEY_READ, &hKey);

    if (result != ERROR_SUCCESS) {
        printf("[!] Failed to open registry key. Error: %ld\n", result);
        return;
    }

    // Query multiple specific values
    const char* valueNames[] = {"ProgramFilesDir", "CommonFilesDir", "ProgramW6432Dir"};
    int numValues = 3;

    printf("[Query Results]\n");

    for (int i = 0; i < numValues; i++) {
        DWORD dataType;
        BYTE data[MAX_VALUE_DATA];
        DWORD dataSize = MAX_VALUE_DATA;

        result = RegQueryValueExA(hKey, valueNames[i], NULL, &dataType, data, &dataSize);

        printf("\n  Value: %s\n", valueNames[i]);

        if (result == ERROR_SUCCESS) {
            printf("  Type:  ");
            printRegistryValueType(dataType);
            printf("\n");

            if (dataType == REG_SZ || dataType == REG_EXPAND_SZ) {
                printf("  Data:  %s\n", (char*)data);
            } else if (dataType == REG_DWORD) {
                printf("  Data:  0x%08X (%lu)\n", *(DWORD*)data, *(DWORD*)data);
            } else {
                printf("  Size:  %lu bytes\n", dataSize);
            }
            printf("  [+] Success\n");
        } else if (result == ERROR_FILE_NOT_FOUND) {
            printf("  [!] Value not found\n");
        } else {
            printf("  [!] Query failed. Error: %ld\n", result);
        }
    }

    printf("\n[+] Value query demonstration completed\n");
    RegCloseKey(hKey);
}

// GetCurrentHwProfileA - Gets current hardware profile information
void demo_GetCurrentHwProfileA(void)
{
    printf("[7] GetCurrentHwProfileA - Get Current Hardware Profile\n");
    printf("    Retrieves information about the current hardware profile\n\n");

    HW_PROFILE_INFOA hwProfileInfo;

    if (GetCurrentHwProfileA(&hwProfileInfo)) {
        printf("[Hardware Profile Information]\n");
        printf("  Dock Info:        0x%08lX\n", hwProfileInfo.dwDockInfo);

        // Interpret dock info
        if (hwProfileInfo.dwDockInfo & DOCKINFO_UNDOCKED)
            printf("  Docking State:    Undocked\n");
        else if (hwProfileInfo.dwDockInfo & DOCKINFO_DOCKED)
            printf("  Docking State:    Docked\n");
        else
            printf("  Docking State:    Unknown\n");

        printf("  Profile GUID:     %s\n", hwProfileInfo.szHwProfileGuid);
        printf("  Profile Name:     %s\n", hwProfileInfo.szHwProfileName);

        printf("\n[+] Successfully retrieved hardware profile information\n");
    } else {
        DWORD error = GetLastError();
        printf("[!] Failed to get hardware profile. Error: %lu\n", error);
    }
}
