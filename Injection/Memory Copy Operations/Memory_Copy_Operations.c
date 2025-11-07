#include <windows.h>
#include <stdio.h>

// Function prototypes for demonstrations
void Demo_RtlMoveMemory();
void Demo_RtlCopyMemory();
void Demo_WriteProcessMemory();

int main() 
{
    printf("\nWindows Memory Copy Operations Demonstration\n");

    Demo_RtlMoveMemory();
    Demo_RtlCopyMemory();
    Demo_WriteProcessMemory();

    printf("\n[*] All demonstrations completed.\n");
    return 0;
}

// RtlMoveMemory Demonstration
void Demo_RtlMoveMemory() {
    printf("\nRtlMoveMemory - Moving Memory Block\n");

    // Source and destination buffers
    char source[64] = "Original data in source buffer";
    char dest[64] = {0};

    printf("[*] Before RtlMoveMemory:\n");
    printf("    Source: \"%s\"\n", source);
    printf("    Dest:   \"%s\"\n", dest);

    // Move memory from source to destination
    RtlMoveMemory(dest, source, strlen(source) + 1);

    printf("\n[+] RtlMoveMemory completed!\n");
    printf("    Bytes moved: %zu\n", strlen(source) + 1);
    printf("\n[*] After RtlMoveMemory:\n");
    printf("    Source: \"%s\"\n", source);
    printf("    Dest:   \"%s\"\n", dest);

    // Demonstrate overlapping move
    char overlap[64] = "ABCDEFGHIJKLMNOP";
    printf("\n[*] Overlapping move demonstration:\n");
    printf("    Before: \"%s\"\n", overlap);
    
    // Move within same buffer (overlapping)
    RtlMoveMemory(overlap + 4, overlap, 8);
    
    printf("    After:  \"%s\"\n", overlap);
    printf("    (Moved first 8 bytes to offset 4)\n\n");
}

// RtlCopyMemory Demonstration
void Demo_RtlCopyMemory() {
    printf("\nRtlCopyMemory - Copying Memory Block\n");

    // Create source buffer with data
    unsigned char source[32];
    for (int i = 0; i < 32; i++) {
        source[i] = i * 8;
    }

    // Destination buffer
    unsigned char dest[32] = {0};

    printf("[*] Before RtlCopyMemory:\n");
    printf("    Source (hex): ");
    for (int i = 0; i < 16; i++) {
        printf("%02X ", source[i]);
    }


    printf("    Dest (hex):   ");
    for (int i = 0; i < 16; i++) {
        printf("%02X ", dest[i]);
    }

    // Copy memory
    RtlCopyMemory(dest, source, 32);

    printf("\n[+] RtlCopyMemory completed!\n");
    printf("    Bytes copied: 32\n");
    printf("\n[*] After RtlCopyMemory:\n");
    printf("    Dest (hex):   ");
    for (int i = 0; i < 16; i++) {
        printf("%02X ", dest[i]);
    }
    printf("...\n\n");
}

// WriteProcessMemory Demonstration
void Demo_WriteProcessMemory() {
    printf("\nWriteProcessMemory - Writing Data to Remote Process\n");

    STARTUPINFOA si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(si);

    // Create suspended process
    printf("[*] Creating target process (suspended)...\n");
    if (!CreateProcessA(
        "C:\\Windows\\System32\\notepad.exe",
        NULL, NULL, NULL, FALSE,
        CREATE_SUSPENDED,
        NULL, NULL, &si, &pi)) {
        printf("[-] Failed to create process. Error: %lu\n\n", GetLastError());
        return;
    }

    printf("[+] Process created (PID: %lu)\n", pi.dwProcessId);

    // Allocate memory in remote process
    SIZE_T allocSize = 4096;
    LPVOID remoteBuffer = VirtualAllocEx(
        pi.hProcess,
        NULL,
        allocSize,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );

    if (!remoteBuffer) {
        printf("[-] VirtualAllocEx failed. Error: %lu\n", GetLastError());
        TerminateProcess(pi.hProcess, 0);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return;
    }

    printf("[+] Remote memory allocated at: 0x%p\n", remoteBuffer);

    // Data to write
    char data[64] = "Hello from WriteProcessMemory!";
    SIZE_T bytesWritten = 0;

    printf("[*] Writing %zu bytes to remote process...\n", strlen(data) + 1);
    printf("    Data: \"%s\"\n", data);

    // Write to remote process
    if (!WriteProcessMemory(
        pi.hProcess,
        remoteBuffer,
        data,
        strlen(data) + 1,
        &bytesWritten)) {
        printf("[-] WriteProcessMemory failed. Error: %lu\n", GetLastError());
    } else {
        printf("[+] Write successful!\n");
        printf("    Bytes written: %zu\n", bytesWritten);
        printf("    Target address: 0x%p\n", remoteBuffer);

        // Verify by reading back
        char readBuffer[64] = {0};
        SIZE_T bytesRead = 0;
        
        if (ReadProcessMemory(
            pi.hProcess,
            remoteBuffer,
            readBuffer,
            bytesWritten,
            &bytesRead)) {
            printf("\n[*] Verification read:\n");
            printf("    Bytes read: %zu\n", bytesRead);
            printf("    Data read back: \"%s\"\n", readBuffer);
            printf("[+] Data integrity verified!\n");
        }
    }

    // Cleanup
    printf("\n[*] Cleaning up...\n");
    VirtualFreeEx(pi.hProcess, remoteBuffer, 0, MEM_RELEASE);
    TerminateProcess(pi.hProcess, 0);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    printf("[+] Cleanup completed.\n\n");
}