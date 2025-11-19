#include <windows.h>
#include <stdio.h>

int main() 
{
    HMODULE hModule;
    HRSRC hResource;
    HGLOBAL hLoadedResource;
    LPVOID pLockedResource;
    DWORD dwResourceSize;
    
    // Get handle to current module (executable)
    hModule = GetModuleHandle(NULL);
    if (hModule == NULL) {
        printf("[x] Failed to get module handle. Error: %lu\n", GetLastError());
        return 1;
    }
    
    // Find the resource (example: RT_RCDATA type, ID 101)
    // In practice, you'd define this in a .rc resource file
    hResource = FindResource(hModule, MAKEINTRESOURCE(101), RT_RCDATA);
    if (hResource == NULL) {
        printf("[x] Resource not found. Error: %lu\n", GetLastError());
        return 1;
    }
    
    // Load the resource into memory
    hLoadedResource = LoadResource(hModule, hResource);
    if (hLoadedResource == NULL) {
        printf("[x] Failed to load resource. Error: %lu\n", GetLastError());
        return 1;
    }
    
    // Lock the resource to get a pointer to the data
    pLockedResource = LockResource(hLoadedResource);
    if (pLockedResource == NULL) {
        printf("[x] Failed to lock resource.\n");
        return 1;
    }
    
    // Get the size of the resource
    dwResourceSize = SizeofResource(hModule, hResource);
    if (dwResourceSize == 0) {
        printf("[x] Failed to get resource size. Error: %lu\n", GetLastError());
        return 1;
    }
    
    // Use the resource data
    printf("\n [+] Dynamic Code Loading APIs Demonstration. \n");
    printf("[+] Resource loaded successfully!\n");
    printf("[+] Resource size: %lu bytes\n", dwResourceSize);
    
    // Print first 100 bytes if it's text data
    if (dwResourceSize > 0) {
        char* pData = (char*)pLockedResource;
        printf("[+] First bytes of resource:\n");
        for (DWORD i = 0; i < min(100, dwResourceSize); i++) {
            printf("%c", pData[i]);
        }
        printf("\n");
    }
    
    printf("[+] Demonstration Complete\n");

    //No need to free resources loaded this way.
    // They're automatically freed when the module is unloaded.
    
    return 0;
}