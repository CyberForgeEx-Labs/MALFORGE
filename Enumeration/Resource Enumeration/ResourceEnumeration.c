#include <windows.h>
#include <stdio.h>
#include <wininet.h>

#pragma comment(lib, "wininet.lib")

// Function prototypes
void EnumerateResourceTypes(const wchar_t* modulePath);
void EnumerateResourceTypesEx(const wchar_t* modulePath);
void EnumerateUrlCacheEntries();

// Callback function for EnumResourceTypesA
BOOL CALLBACK EnumResTypeProc(HMODULE hModule, LPSTR lpType, LONG_PTR lParam) {
    if (IS_INTRESOURCE(lpType)) {
        printf("  Resource Type ID: %d\n", (int)(ULONG_PTR)lpType);
    } else {
        printf("  Resource Type: %s\n", lpType);
    }
    return TRUE; // Continue enumeration
}

// Callback function for EnumResourceTypesExA
BOOL CALLBACK EnumResTypeProcEx(HMODULE hModule, LPSTR lpType, LONG_PTR lParam) {
    if (IS_INTRESOURCE(lpType)) {
        printf("  [Ex] Resource Type ID: %d\n", (int)(ULONG_PTR)lpType);
    } else {
        printf("  [Ex] Resource Type: %s\n", lpType);
    }
    return TRUE; // Continue enumeration
}

// EnumResourceTypesA - Enumerates resource types in a module
void EnumerateResourceTypes(const wchar_t* modulePath) {
    printf("\n[+] Enumerating Resource Types using EnumResourceTypesA\n");
    
    HMODULE hModule = LoadLibraryExW(modulePath, NULL, LOAD_LIBRARY_AS_DATAFILE);
    if (hModule == NULL) {
        printf("[-] Failed to load module: %ls (Error: %lu)\n", modulePath, GetLastError());
        return;
    }
    
    printf("[*] Module loaded: %ls\n", modulePath);
    printf("[*] Enumerating resource types...\n\n");
    
    if (!EnumResourceTypesA(hModule, EnumResTypeProc, 0)) {
        printf("[-] EnumResourceTypesA failed (Error: %lu)\n", GetLastError());
    } else {
        printf("\n[+] Resource type enumeration completed successfully\n");
    }
    
    FreeLibrary(hModule);
}

// 2. EnumResourceTypesExA - Extended resource type enumeration
void EnumerateResourceTypesEx(const wchar_t* modulePath) {
    printf("\n[+] Enumerating Resource Types using EnumResourceTypesExA\n");
    
    HMODULE hModule = LoadLibraryExW(modulePath, NULL, LOAD_LIBRARY_AS_DATAFILE);
    //extracting the message and resources from module.
    if (hModule == NULL) {
        printf("[-] Failed to load module: %ls (Error: %lu)\n", modulePath, GetLastError());
        return;
    }
    
    printf("[*] Module loaded: %ls\n", modulePath);
    printf("[*] Enumerating resource types (Extended)...\n\n");
    
    // EnumResourceTypesExA allows for language-specific enumeration
    // Using MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL) for all languages
    if (!EnumResourceTypesExA(hModule, EnumResTypeProcEx, 0, 0, 0)) {
        printf("[-] EnumResourceTypesExA failed (Error: %lu)\n", GetLastError());
    } else {
        printf("\n[+] Extended resource type enumeration completed successfully\n");
    }
    
    FreeLibrary(hModule);
}

// FindFirstUrlCacheEntryA + FindNextUrlCacheEntryA
// Enumerates Internet Explorer/WinINet URL cache entries
void EnumerateUrlCacheEntries() {
    printf("\n[+] Enumerating URL Cache Entries\n");
    
    LPINTERNET_CACHE_ENTRY_INFOA lpCacheEntry = NULL;
    DWORD dwEntrySize = 0;
    HANDLE hFind = NULL;
    int entryCount = 0;
    
    // First call to get required buffer size
    hFind = FindFirstUrlCacheEntryA(NULL, NULL, &dwEntrySize);
    
    if (hFind == NULL && GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
        // Allocate buffer
        lpCacheEntry = (LPINTERNET_CACHE_ENTRY_INFOA)malloc(dwEntrySize);
        if (lpCacheEntry == NULL) {
            printf("[-] Memory allocation failed\n");
            return;
        }
        
        // Get first entry
        hFind = FindFirstUrlCacheEntryA(NULL, lpCacheEntry, &dwEntrySize);
    }
    
    if (hFind == NULL) {
        printf("[-] FindFirstUrlCacheEntryA failed (Error: %lu)\n", GetLastError());
        if (GetLastError() == ERROR_NO_MORE_ITEMS) {
            printf("[*] No cache entries found\n");
        }
        if (lpCacheEntry) free(lpCacheEntry);
        return;
    }
    
    printf("[*] Enumerating Internet cache entries...\n\n");
    
    // Process first entry
    do {
        entryCount++;
        printf("Entry #%d:\n", entryCount);
        printf("  URL: %s\n", lpCacheEntry->lpszSourceUrlName ? lpCacheEntry->lpszSourceUrlName : "N/A");
        
        if (lpCacheEntry->lpszLocalFileName) {
            printf("  Local File: %s\n", lpCacheEntry->lpszLocalFileName);
        }
        
        printf("  Cache Entry Type: 0x%08X\n", lpCacheEntry->CacheEntryType);
        printf("  Size: %lu bytes\n", lpCacheEntry->dwSizeLow);
        
        // Format last modified time
        FILETIME ft = lpCacheEntry->LastModifiedTime;
        SYSTEMTIME st;
        if (FileTimeToSystemTime(&ft, &st)) {
            printf("  Last Modified: %02d/%02d/%04d %02d:%02d:%02d\n", 
                   st.wMonth, st.wDay, st.wYear, st.wHour, st.wMinute, st.wSecond);
        }
        
        printf("\n");
        
        // Get next entry
        dwEntrySize = 0;
        if (!FindNextUrlCacheEntryA(hFind, NULL, &dwEntrySize)) {
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
                free(lpCacheEntry);
                lpCacheEntry = (LPINTERNET_CACHE_ENTRY_INFOA)malloc(dwEntrySize);
                if (lpCacheEntry == NULL) {
                    printf("[-] Memory allocation failed\n");
                    break;
                }
                
                if (!FindNextUrlCacheEntryA(hFind, lpCacheEntry, &dwEntrySize)) {
                    if (GetLastError() != ERROR_NO_MORE_ITEMS) {
                        printf("[-] FindNextUrlCacheEntryA failed (Error: %lu)\n", GetLastError());
                    }
                    break;
                }
            } else {
                if (GetLastError() != ERROR_NO_MORE_ITEMS) {
                    printf("[-] FindNextUrlCacheEntryA failed (Error: %lu)\n", GetLastError());
                }
                break;
            }
        }
        
        // Limit output for demonstration
        if (entryCount >= 5) {
            printf("[*] Showing first 5 entries only...\n");
            break;
        }
        
    } while (TRUE);
    
    printf("\n[+] Total cache entries processed: %d\n", entryCount);
    
    // Cleanup
    if (hFind) FindCloseUrlCache(hFind);
    if (lpCacheEntry) free(lpCacheEntry);
}

int main(int argc, char* argv[]) 
{
    printf("\nWindows Resource Enumeration API Demonstration\n");
    
    wchar_t modulePath[MAX_PATH];
    
    // Allow user to specify a module path, default to system32\kernel32.dll
    if (argc > 1) {
        //character convertion
        MultiByteToWideChar(CP_ACP, 0, argv[1], -1, modulePath, MAX_PATH);
    } else {
        // Use kernel32.dll as default
        GetSystemDirectoryW(modulePath, MAX_PATH);
        //appending string
        wcscat_s(modulePath, MAX_PATH, L"\\kernel32.dll");
    }
    
    EnumerateResourceTypes(modulePath);
    
    EnumerateResourceTypesEx(modulePath);
    
    EnumerateUrlCacheEntries();
    
    printf("\nEnumeration Complete\n");
    printf("\nPress Enter to exit...");
    getchar();
    
    return 0;
}