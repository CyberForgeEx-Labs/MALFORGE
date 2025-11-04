#include <windows.h>
#include <stdio.h>
#include <psapi.h>

// Using VT for processing the output
void EnableVirtualTerminalProcessing() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING; // flipping single bit ON
    SetConsoleMode(hOut, dwMode);
}

// Demonstrate ReadProcessMemory
void DemoReadProcessMemory() {
    printf("\nReadProcessMemory - Reading Memory from Current Process\n");
    
    // Allocate some memory in our own process to read
    const char* testData = "Hello, World!";
    printf("[*] Test data address: 0x%p\n", (void*)testData);
    printf("[*] Test data content: %s\n\n", testData);
    
    // Get handle to current process
    HANDLE hProcess = GetCurrentProcess();
    
    // Buffer to read into
    char buffer[256] = {0};
    SIZE_T bytesRead = 0;
    
    // Read memory from our own process
    BOOL result = ReadProcessMemory(
        hProcess,
        (LPCVOID)testData,
        buffer,
        strlen(testData) + 1,
        &bytesRead
    );
    
    if (result) {
        printf("[+] ReadProcessMemory succeeded!\n");
        printf("    Bytes Read: %zu\n", bytesRead);
        printf("    Content: \"%s\"\n", buffer);
        
        // Display hex dump
        printf("\n    Hex Dump:\n    ");
        for (SIZE_T i = 0; i < bytesRead; i++) {
            printf("%02X ", (unsigned char)buffer[i]);
            if ((i + 1) % 8 == 0) printf("\n    ");
        }
        printf("\n");
    } else {
        printf("[-] ReadProcessMemory failed! Error: %lu\n", GetLastError());
    }
    
    printf("\n");
}

// Demonstrate ReadFile
void DemoReadFile(const char* filename) {
    printf("\nReadFile - Reading Data from File\n");
    
    // Create a test file if no filename provided
    char tempFile[MAX_PATH];
    BOOL cleanupRequired = FALSE;
    
    if (filename == NULL) {
        GetTempPathA(MAX_PATH, tempFile);
        strcat_s(tempFile, MAX_PATH, "test_readfile.txt"); //appending name into test path
        filename = tempFile;
        cleanupRequired = TRUE;
        
        // Create test file
        HANDLE hWrite = CreateFileA(
            filename,
            GENERIC_WRITE,
            0,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );
        
        if (hWrite != INVALID_HANDLE_VALUE) {
            const char* testContent = "This is a test file for ReadFile demonstration.\r\nLine 2\r\nLine 3";
            DWORD written;
            WriteFile(hWrite, testContent, (DWORD)strlen(testContent), &written, NULL);
            CloseHandle(hWrite);
        }
    }
    
    printf("[*] Target file: %s\n", filename);
    
    // Open file for reading
    HANDLE hFile = CreateFileA(
        filename,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("[-] Failed to open file! Error: %lu\n\n", GetLastError());
        return;
    }
    
    // Get file size
    DWORD fileSize = GetFileSize(hFile, NULL);
    printf("[+] File opened successfully!\n");
    printf("    File Size: %lu bytes\n\n", fileSize);
    
    // Read file content
    char* fileBuffer = (char*)malloc(fileSize + 1);
    DWORD bytesRead = 0;
    
    if (ReadFile(hFile, fileBuffer, fileSize, &bytesRead, NULL)) {
        fileBuffer[bytesRead] = '\0';
        printf("[+] ReadFile succeeded!\n");
        printf("    Bytes Read: %lu\n", bytesRead);
        printf("\n    File Content:\n");
        printf("    -------------\n");
        printf("    %s\n", fileBuffer);
        printf("    -------------\n");
    } else {
        printf("[-] ReadFile failed! Error: %lu\n", GetLastError());
    }
    
    free(fileBuffer);
    CloseHandle(hFile);
    
    // Cleanup temp file
    if (cleanupRequired) {
        DeleteFileA(filename);
    }
    
    printf("\n");
}

// Demonstrate VirtualQueryEx
void DemoVirtualQueryEx() {
    printf("\nVirtualQueryEx - Querying Virtual Memory Information\n");
    
    // Allocate some memory to query
    SIZE_T allocSize = 4096;
    LPVOID pMemory = VirtualAlloc(NULL, allocSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    
    if (pMemory == NULL) {
        printf("[-] VirtualAlloc failed! Error: %lu\n\n", GetLastError());
        return;
    }
    
    printf("[*] Allocated memory at: 0x%p\n", pMemory);
    printf("[*] Allocation size: %zu bytes\n\n", allocSize);
    
    // Query the memory region
    MEMORY_BASIC_INFORMATION mbi = {0};
    SIZE_T result = VirtualQueryEx(
        GetCurrentProcess(),
        pMemory,
        &mbi,
        sizeof(mbi)
    );
    
    if (result > 0) {
        printf("[+] VirtualQueryEx succeeded!\n");
        printf("    Bytes Returned: %zu\n\n", result);
        
        printf("    MEMORY_BASIC_INFORMATION:\n");
        printf("    -------------------------\n");
        printf("    BaseAddress:       0x%p\n", mbi.BaseAddress);
        printf("    AllocationBase:    0x%p\n", mbi.AllocationBase);
        printf("    AllocationProtect: 0x%08lX ", mbi.AllocationProtect);
        
        // Decode protection flags according to allocation
        if (mbi.AllocationProtect & PAGE_READWRITE) printf("(PAGE_READWRITE)");
        else if (mbi.AllocationProtect & PAGE_READONLY) printf("(PAGE_READONLY)");
        else if (mbi.AllocationProtect & PAGE_EXECUTE_READ) printf("(PAGE_EXECUTE_READ)");
        printf("\n");
        
        printf("    RegionSize:        %zu bytes\n", mbi.RegionSize);
        printf("    State:             0x%08lX ", mbi.State);
        
        // Decode state current memory region
        if (mbi.State == MEM_COMMIT) printf("(MEM_COMMIT)");
        else if (mbi.State == MEM_RESERVE) printf("(MEM_RESERVE)");
        else if (mbi.State == MEM_FREE) printf("(MEM_FREE)");
        printf("\n");
        
        printf("    Protect:           0x%08lX ", mbi.Protect);
        if (mbi.Protect & PAGE_READWRITE) printf("(PAGE_READWRITE)");
        printf("\n");
        
        printf("    Type:              0x%08lX ", mbi.Type);
        if (mbi.Type == MEM_PRIVATE) printf("(MEM_PRIVATE)");
        else if (mbi.Type == MEM_MAPPED) printf("(MEM_MAPPED)");
        else if (mbi.Type == MEM_IMAGE) printf("(MEM_IMAGE)");
        printf("\n");
    } else {
        printf("[-] VirtualQueryEx failed! Error: %lu\n", GetLastError());
    }
    
    // free
    VirtualFree(pMemory, 0, MEM_RELEASE);
    printf("\n");
}

// Demonstrate GetFileAttributesA
void DemoGetFileAttributesA(const char* filename) {
    printf("\nGetFileAttributesA - Getting File Attributes\n");
    
    // Use current executable if no filename provided
    char exePath[MAX_PATH];
    if (filename == NULL) {
        GetModuleFileNameA(NULL, exePath, MAX_PATH);
        filename = exePath;
    }
    
    printf("[*] Target file: %s\n", filename);
    
    // Get file attributes
    DWORD attributes = GetFileAttributesA(filename);
    
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        printf("[-] GetFileAttributesA failed! Error: %lu\n", GetLastError());
        printf("    (File may not exist or is inaccessible)\n\n");
        return;
    }
    
    printf("[+] GetFileAttributesA succeeded!\n");
    printf("    Attributes Value: 0x%08lX\n\n", attributes);
    
    printf("    File Attributes:\n");
    printf("    ----------------\n");
    
    if (attributes & FILE_ATTRIBUTE_ARCHIVE)
        printf("    [X] ARCHIVE        - File should be archived\n");
    else
        printf("    [ ] ARCHIVE\n");
    
    if (attributes & FILE_ATTRIBUTE_COMPRESSED)
        printf("    [X] COMPRESSED     - File is compressed\n");
    else
        printf("    [ ] COMPRESSED\n");
    
    if (attributes & FILE_ATTRIBUTE_DIRECTORY)
        printf("    [X] DIRECTORY      - File is a directory\n");
    else
        printf("    [ ] DIRECTORY\n");
    
    if (attributes & FILE_ATTRIBUTE_ENCRYPTED)
        printf("    [X] ENCRYPTED      - File is encrypted\n");
    else
        printf("    [ ] ENCRYPTED\n");
    
    if (attributes & FILE_ATTRIBUTE_HIDDEN)
        printf("    [X] HIDDEN         - File is hidden\n");
    else
        printf("    [ ] HIDDEN\n");
    
    if (attributes & FILE_ATTRIBUTE_NORMAL)
        printf("    [X] NORMAL         - No other attributes set\n");
    else
        printf("    [ ] NORMAL\n");
    
    if (attributes & FILE_ATTRIBUTE_READONLY)
        printf("    [X] READONLY       - File is read-only\n");
    else
        printf("    [ ] READONLY\n");
    
    if (attributes & FILE_ATTRIBUTE_SYSTEM)
        printf("    [X] SYSTEM         - File is a system file\n");
    else
        printf("    [ ] SYSTEM\n");
    
    if (attributes & FILE_ATTRIBUTE_TEMPORARY)
        printf("    [X] TEMPORARY      - File is temporary\n");
    else
        printf("    [ ] TEMPORARY\n");
    
    printf("\n");
}

// Demonstrate SearchPathA
void DemoSearchPathA() {
    printf("\nSearchPathA - Searching for Files in System Paths\n");
    
    // Test with common system executables
    const char* testFiles[] = {
        "notepad.exe",
        "cmd.exe",
        "kernel32.dll",
        "user32.dll"
    };
    
    printf("[*] Searching for common system files...\n\n");

    int len = sizeof(testFiles) / sizeof(testFiles[0]);
    
    for (int i = 0; i < len; i++) {
        char foundPath[MAX_PATH] = {0};
        char* filePart = NULL;
        
        printf("  Searching for: %s\n", testFiles[i]);
        
        DWORD result = SearchPathA(
            NULL,              // Search in standard locations
            testFiles[i],      // Filename to search for
            NULL,              // No extension to append
            MAX_PATH,          // Size of buffer
            foundPath,         // Buffer for found path
            &filePart          // Pointer to filename part
        );
        
        if (result > 0 && result < MAX_PATH) {
            printf("  [+] Found!\n");
            printf("      Full Path:  %s\n", foundPath);
            printf("      File Part:  %s\n", filePart);
            printf("      Path Len:   %lu characters\n", result);
        } else if (result >= MAX_PATH) {
            printf("  [-] Path too long (requires %lu chars)\n", result);
        } else {
            printf("  [-] Not found (Error: %lu)\n", GetLastError());
        }
        printf("\n");
    }
    
    // Demonstrate with custom search path
    printf("[*] Searching in custom path (Windows directory)...\n\n");
    
    char windowsDir[MAX_PATH];
    GetWindowsDirectoryA(windowsDir, MAX_PATH);
    
    char foundPath[MAX_PATH] = {0};
    char* filePart = NULL;
    
    DWORD result = SearchPathA(
        windowsDir,
        "explorer.exe",
        NULL,
        MAX_PATH,
        foundPath,
        &filePart
    );
    
    if (result > 0) {
        printf("  [+] Found explorer.exe!\n");
        printf("      Search Path: %s\n", windowsDir);
        printf("      Found Path:  %s\n", foundPath);
    } else {
        printf("  [-] Not found in specified path\n");
    }
    
    printf("\n");
}

int main(int argc, char* argv[]) 
{
    EnableVirtualTerminalProcessing();
    
    printf("\nMemory and File Attribute Query API Demonstration. \n\n");
    
    const char* testFile = (argc > 1) ? argv[1] : NULL;
    
    // demonstrations
    DemoReadProcessMemory();
    DemoReadFile(testFile);
    DemoVirtualQueryEx();
    DemoGetFileAttributesA(testFile);
    DemoSearchPathA();
    
    printf("\n[*] All demonstrations completed!\n");
    
    return 0;
}