#include <windows.h>
#include <stdio.h>

// Helper function to print file times
void PrintFileTime(const char* label, FILETIME* ft) {
    SYSTEMTIME st;
    FileTimeToSystemTime(ft, &st);
    printf("%s: %04d-%02d-%02d %02d:%02d:%02d\n", 
           label, st.wYear, st.wMonth, st.wDay, 
           st.wHour, st.wMinute, st.wSecond);
}

int main() 
{
    const char* testFile = "test_file.txt";
    const char* mappedFile = "mapped_file.dat";
    HANDLE hFile;
    DWORD bytesWritten;
    FILETIME ftCreate, ftAccess, ftWrite;
    DWORD attributes;
    
    printf("=== Windows File System API Demonstration ===\n\n");
    
    // CreateFile & Write Sample Data
    printf("[1] Creating test file...\n");
    hFile = CreateFile(testFile, GENERIC_WRITE, 0, NULL, 
                       CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("Failed to create file. Error: %lu\n", GetLastError());
        return 1;
    }
    
    const char* data = "This is test data for demonstration.\n";
    WriteFile(hFile, data, strlen(data), &bytesWritten, NULL);
    CloseHandle(hFile);
    printf("[+] File created: %s\n\n", testFile);
    
    // SetFileTime - Modify timestamps
    printf("[2] Demonstrating SetFileTime API\n");
    
    hFile = CreateFile(testFile, GENERIC_READ | GENERIC_WRITE, 
                       0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("Failed to open file. Error: %lu\n", GetLastError());
        return 1;
    }
    
    // Get current file times
    GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite);
    printf("Current times:\n");
    PrintFileTime("  Created ", &ftCreate);
    PrintFileTime("  Accessed", &ftAccess);
    PrintFileTime("  Modified", &ftWrite);
    
    // Set custom timestamp (e.g., January 1, 2024)
    SYSTEMTIME newTime = {2024, 1, 0, 1, 12, 0, 0, 0};
    FILETIME newFileTime;
    SystemTimeToFileTime(&newTime, &newFileTime);
    
    // File time alter.
    if (SetFileTime(hFile, &newFileTime, &newFileTime, &newFileTime)) {
        printf("\n[+] SetFileTime: Successfully updated timestamps\n");
        
        // Verify the change
        GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite);
        printf("New times:\n");
        PrintFileTime("  Created ", &ftCreate);
    } else {
        printf("[-] SetFileTime failed. Error: %lu\n", GetLastError());
    }
    CloseHandle(hFile);
    printf("\n");
    
    // SetFileAttributesA - Modify attributes
    printf("[3] Demonstrating SetFileAttributesA API\n");
    
    // Get current attributes
    attributes = GetFileAttributesA(testFile);
    printf("Current attributes: 0x%X\n", attributes);
    
    // Set file as HIDDEN and READ-ONLY
    if (SetFileAttributesA(testFile, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY)) {
        printf("[+] SetFileAttributesA: File is now HIDDEN and READ-ONLY\n");
        
        attributes = GetFileAttributesA(testFile);
        printf("New attributes: 0x%X\n", attributes);
        if (attributes & FILE_ATTRIBUTE_HIDDEN)
            printf("  - FILE_ATTRIBUTE_HIDDEN\n");
        if (attributes & FILE_ATTRIBUTE_READONLY)
            printf("  - FILE_ATTRIBUTE_READONLY\n");
    } else {
        printf("[-] SetFileAttributesA failed. Error: %lu\n", GetLastError());
    }
    printf("\n");
    
    // CreateFileMappingA - Memory-mapped files
    printf("[4] Demonstrating CreateFileMappingA API\n");
    
    // Create a file for mapping
    hFile = CreateFile(mappedFile, GENERIC_READ | GENERIC_WRITE, 
                       0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("Failed to create mapped file. Error: %lu\n", GetLastError());
    } else {
        // Write some data
        const char* mappedData = "Data for memory mapping demonstration";
        WriteFile(hFile, mappedData, strlen(mappedData), &bytesWritten, NULL);
        
        // Create file mapping between disk and memory.
        HANDLE hMapping = CreateFileMappingA(hFile, NULL, PAGE_READWRITE, 
                                             0, 0, NULL);
        if (hMapping == NULL) {
            printf("[-] CreateFileMappingA failed. Error: %lu\n", GetLastError());
        } else {
            printf("[+] CreateFileMappingA: Mapping created successfully\n");
            
            // Map view of file
            LPVOID pView = MapViewOfFile(hMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
            if (pView != NULL) {
                printf("[+] MapViewOfFile: View mapped at %p\n", pView);
                printf("Content: %.37s\n", (char*)pView);
                
                UnmapViewOfFile(pView);
            }
            CloseHandle(hMapping);
        }
        CloseHandle(hFile);
    }
    printf("\n");
    
    // DeleteFileA - Clean up
    printf("[5] Demonstrating DeleteFileA API\n");
    
    // First remove read-only attribute so we can delete
    SetFileAttributesA(testFile, FILE_ATTRIBUTE_NORMAL);
    
    if (DeleteFileA(testFile)) {
        printf("[+] DeleteFileA: Successfully deleted %s\n", testFile);
    } else {
        printf("[-] DeleteFileA failed. Error: %lu\n", GetLastError());
    }
    
    if (DeleteFileA(mappedFile)) {
        printf("[+] DeleteFileA: Successfully deleted %s\n", mappedFile);
    } else {
        printf("[-] DeleteFileA failed. Error: %lu\n", GetLastError());
    }
    
    printf("\n=== Demonstration Complete ===\n");
    
    return 0;
}