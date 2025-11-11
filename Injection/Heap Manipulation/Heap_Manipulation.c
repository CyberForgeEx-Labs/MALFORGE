#include <windows.h>
#include <stdio.h>

/*
 * Function pointer type definitions for native API functions from ntdll.dll
 */
typedef PVOID(NTAPI* pRtlCreateHeap)(
    ULONG Flags,
    PVOID HeapBase,
    SIZE_T ReserveSize,
    SIZE_T CommitSize,
    PVOID Lock,
    PVOID Parameters
);

typedef BOOLEAN(NTAPI* pRtlDestroyHeap)(
    PVOID HeapHandle
);

/*
 * Demonstration structure
 */
typedef struct _DEMO_DATA {
    DWORD id;
    char name[64];
    DWORD value;
} DEMO_DATA, *PDEMO_DATA;

void PrintSeparator() {
    printf("\n========================================\n");
}

void PrintHeader(const char* title) {
    PrintSeparator();
    printf("%s\n", title);
    PrintSeparator();
}

void Demo_HeapCreate() {
    PrintHeader("HeapCreate - Creating Private Heap");

    HANDLE hHeap = HeapCreate(0, 4096, 1024 * 1024);

    if (hHeap != NULL) {
        printf("[+] Private heap created successfully\n");
        printf("    Heap Handle: 0x%p\n", hHeap);
        printf("    Initial Size: 4096 bytes (4KB)\n");
        printf("    Maximum Size: 1048576 bytes (1MB)\n");

        PDEMO_DATA pData = (PDEMO_DATA)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(DEMO_DATA));
        
        if (pData) {
            printf("[+] Allocated memory from private heap\n");
            printf("    Address: 0x%p\n", pData);
            printf("    Size: %zu bytes\n", sizeof(DEMO_DATA));

            pData->id = 1001;
            strcpy_s(pData->name, sizeof(pData->name), "Private Heap Demo");
            pData->value = 12345;

            printf("    Data stored: ID=%u, Name='%s', Value=%u\n",
                   pData->id, pData->name, pData->value);

            HeapFree(hHeap, 0, pData);
            printf("[+] Memory freed\n");
        }

        if (HeapDestroy(hHeap)) {
            printf("[+] Private heap destroyed\n");
        }
    } else {
        printf("[-] Failed to create heap. Error: %u\n", GetLastError());
    }
}

void Demo_HeapAlloc() {
    PrintHeader("HeapAlloc - Allocating Heap Memory");

    HANDLE hHeap = GetProcessHeap();
    printf("[+] Using process default heap: 0x%p\n", hHeap);

    printf("\n--- Allocation with HEAP_ZERO_MEMORY ---\n");
    PVOID pZeroMem = HeapAlloc(hHeap, HEAP_ZERO_MEMORY, 256);
    
    if (pZeroMem) {
        printf("[+] Allocated 256 bytes (zero-initialized)\n");
        printf("    Address: 0x%p\n", pZeroMem);
        
        BOOL isZeroed = TRUE;
        for (int i = 0; i < 10; i++) {
            if (((BYTE*)pZeroMem)[i] != 0) {
                isZeroed = FALSE;
                break;
            }
        }
        printf("    First 10 bytes verified: %s\n", 
               isZeroed ? "All zeros" : "Not zeroed");

        HeapFree(hHeap, 0, pZeroMem);
    }

    printf("\n--- Allocation without flags ---\n");
    PVOID pRegMem = HeapAlloc(hHeap, 0, 512);
    
    if (pRegMem) {
        printf("[+] Allocated 512 bytes (uninitialized)\n");
        printf("    Address: 0x%p\n", pRegMem);
        
        strcpy_s((char*)pRegMem, 512, "HeapAlloc demonstration data");
        printf("    Data written: '%s'\n", (char*)pRegMem);

        HeapFree(hHeap, 0, pRegMem);
        printf("[+] Memory freed\n");
    }

    printf("\n--- Multiple Allocations ---\n");
    PVOID allocations[5];
    
    for (int i = 0; i < 5; i++) {
        allocations[i] = HeapAlloc(hHeap, 0, (i + 1) * 128);
        
        if (allocations[i]) {
            printf("[+] Allocation %d: 0x%p (%d bytes)\n",
                   i + 1, allocations[i], (i + 1) * 128);
        }
    }

    for (int i = 0; i < 5; i++) {
        if (allocations[i]) {
            HeapFree(hHeap, 0, allocations[i]);
        }
    }
    printf("[+] All allocations freed\n");
}

void Demo_HeapReAlloc() {
    PrintHeader("HeapReAlloc - Reallocating Heap Memory");

    HANDLE hHeap = GetProcessHeap();
    
    SIZE_T initialSize = 128;
    char* pBuffer = (char*)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, initialSize);
    
    if (pBuffer) {
        printf("[+] Initial allocation: 0x%p (%zu bytes)\n", pBuffer, initialSize);
        
        strcpy_s(pBuffer, initialSize, "Initial data in buffer");
        printf("    Content: '%s'\n", pBuffer);

        SIZE_T newSize = 512;
        printf("\n--- Reallocating to larger size ---\n");
        char* pNewBuffer = (char*)HeapReAlloc(hHeap, HEAP_ZERO_MEMORY, pBuffer, newSize);
        
        if (pNewBuffer) {
            printf("[+] Reallocated: 0x%p (%zu bytes)\n", pNewBuffer, newSize);
            printf("    Content preserved: '%s'\n", pNewBuffer);
            printf("    Address %s\n", (pNewBuffer == pBuffer) ? "unchanged" : "changed");

            strcat_s(pNewBuffer, newSize, " - Extended with more content!");
            printf("    New content: '%s'\n", pNewBuffer);

            SIZE_T smallerSize = 64;
            printf("\n--- Reallocating to smaller size ---\n");
            char* pSmaller = (char*)HeapReAlloc(hHeap, 0, pNewBuffer, smallerSize);
            
            if (pSmaller) {
                printf("[+] Reallocated: 0x%p (%zu bytes)\n", pSmaller, smallerSize);
                printf("    Content (truncated): '%s'\n", pSmaller);

                HeapFree(hHeap, 0, pSmaller);
                printf("[+] Memory freed\n");
            }
        }
    } else {
        printf("[-] Initial allocation failed\n");
    }
}

void Demo_GetProcessHeap() {
    PrintHeader("GetProcessHeap - Getting Default Heap");

    HANDLE hDefaultHeap = GetProcessHeap();
    
    if (hDefaultHeap) {
        printf("[+] Default process heap retrieved\n");
        printf("    Heap Handle: 0x%p\n", hDefaultHeap);

        ULONG heapInfo = 0;
        if (HeapQueryInformation(hDefaultHeap, HeapCompatibilityInformation,
                &heapInfo, sizeof(heapInfo), NULL)) {
            printf("    Heap Type: %s\n", 
                   heapInfo == 0 ? "Standard" : 
                   heapInfo == 1 ? "LAL (Low Fragmentation)" : "Unknown");
        }

        printf("\n--- Using Default Heap ---\n");
        PDEMO_DATA pData = (PDEMO_DATA)HeapAlloc(hDefaultHeap, HEAP_ZERO_MEMORY, sizeof(DEMO_DATA));
        
        if (pData) {
            printf("[+] Allocated from default heap: 0x%p\n", pData);
            
            pData->id = 2001;
            strcpy_s(pData->name, sizeof(pData->name), "Default Heap Data");
            pData->value = 98765;

            printf("    Data: ID=%u, Name='%s', Value=%u\n",
                   pData->id, pData->name, pData->value);

            HeapFree(hDefaultHeap, 0, pData);
            printf("[+] Memory freed\n");
        }
    } else {
        printf("[-] Failed to get default heap\n");
    }
}

void Demo_GetProcessHeaps() {
    PrintHeader("GetProcessHeaps - Enumerating Process Heaps");

    DWORD numHeaps = GetProcessHeaps(0, NULL);
    printf("[+] Number of heaps in process: %u\n", numHeaps);

    if (numHeaps > 0) {
        PHANDLE pHeaps = (PHANDLE)malloc(numHeaps * sizeof(HANDLE));
        
        if (pHeaps) {
            DWORD retrieved = GetProcessHeaps(numHeaps, pHeaps);
            printf("[+] Retrieved %u heap handles\n", retrieved);

            printf("\n--- Heap Information ---\n");
            for (DWORD i = 0; i < retrieved; i++) {
                printf("\nHeap #%u:\n", i + 1);
                printf("  Handle: 0x%p\n", pHeaps[i]);
                
                if (pHeaps[i] == GetProcessHeap()) {
                    printf("  Type: Default Process Heap\n");
                } else {
                    printf("  Type: Private Heap\n");
                }
            }

            free(pHeaps);
        } else {
            printf("[-] Failed to allocate memory for heap array\n");
        }
    }

    printf("\n--- Creating Additional Heaps ---\n");
    HANDLE hNewHeap1 = HeapCreate(0, 8192, 0);
    HANDLE hNewHeap2 = HeapCreate(0, 16384, 0);

    if (hNewHeap1 && hNewHeap2) {
        printf("[+] Created 2 new heaps\n");
        
        numHeaps = GetProcessHeaps(0, NULL);
        printf("[+] Total heaps now: %u\n", numHeaps);

        HeapDestroy(hNewHeap1);
        HeapDestroy(hNewHeap2);
        printf("[+] New heaps destroyed\n");
    }
}

void Demo_RtlCreateHeap() {
    PrintHeader("RtlCreateHeap - Native Heap Creation");

    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    
    if (!hNtdll) {
        printf("[-] Failed to get ntdll.dll handle\n");
        return;
    }

    pRtlCreateHeap RtlCreateHeap = 
        (pRtlCreateHeap)GetProcAddress(hNtdll, "RtlCreateHeap");
    pRtlDestroyHeap RtlDestroyHeap = 
        (pRtlDestroyHeap)GetProcAddress(hNtdll, "RtlDestroyHeap");

    if (RtlCreateHeap && RtlDestroyHeap) {
        printf("[+] Native API functions loaded\n");

        PVOID hNativeHeap = RtlCreateHeap(HEAP_GROWABLE, NULL, 8192, 4096, NULL, NULL);

        if (hNativeHeap) {
            printf("[+] Native heap created successfully\n");
            printf("    Heap Handle: 0x%p\n", hNativeHeap);
            printf("    Reserve Size: 8192 bytes (8KB)\n");
            printf("    Commit Size: 4096 bytes (4KB)\n");
            printf("    Flags: HEAP_GROWABLE (0x%08X)\n", HEAP_GROWABLE);

            printf("\n--- Using Native Heap ---\n");
            PVOID pMem = HeapAlloc(hNativeHeap, HEAP_ZERO_MEMORY, 1024);
            
            if (pMem) {
                printf("[+] Allocated 1024 bytes from native heap\n");
                printf("    Address: 0x%p\n", pMem);

                strcpy_s((char*)pMem, 1024, "Data in native heap");
                printf("    Content: '%s'\n", (char*)pMem);

                HeapFree(hNativeHeap, 0, pMem);
                printf("[+] Memory freed\n");
            }

            if (RtlDestroyHeap(hNativeHeap)) {
                printf("[+] Native heap destroyed\n");
            }
        } else {
            printf("[-] Failed to create native heap\n");
        }
    } else {
        printf("[-] Failed to load native API functions\n");
    }
}

void Demo_GlobalAlloc() {
    PrintHeader("GlobalAlloc - Legacy Global Memory Allocation");

    printf("--- Fixed Memory Allocation (GPTR) ---\n");
    HGLOBAL hFixedMem = GlobalAlloc(GPTR, 512); // handle to the global memory.
    
    if (hFixedMem) {
        printf("[+] Global memory allocated (FIXED)\n");
        printf("    Handle/Pointer: 0x%p\n", hFixedMem);
        printf("    Size: 512 bytes\n");
        printf("    Flags: GPTR (GMEM_FIXED | GMEM_ZEROINIT)\n");

        char* pFixed = (char*)hFixedMem;
        strcpy_s(pFixed, 512, "Fixed global memory data");
        printf("    Content: '%s'\n", pFixed);

        SIZE_T actualSize = GlobalSize(hFixedMem);
        printf("    Actual size: %zu bytes\n", actualSize);

        if (GlobalFree(hFixedMem) == NULL) {
            printf("[+] Fixed memory freed\n");
        }
    }

    printf("\n--- Moveable Memory Allocation (GHND) ---\n");
    HGLOBAL hMoveable = GlobalAlloc(GHND, 1024);
    
    if (hMoveable) {
        printf("[+] Global memory allocated (MOVEABLE)\n");
        printf("    Handle: 0x%p\n", hMoveable);
        printf("    Size: 1024 bytes\n");
        printf("    Flags: GHND (GMEM_MOVEABLE | GMEM_ZEROINIT)\n");

        LPVOID pMoveable = GlobalLock(hMoveable);
        
        if (pMoveable) {
            printf("[+] Memory locked\n");
            printf("    Pointer: 0x%p\n", pMoveable);

            strcpy_s((char*)pMoveable, 1024, 
                     "Moveable global memory - used for clipboard operations");
            printf("    Content: '%s'\n", (char*)pMoveable);

            GlobalUnlock(hMoveable);
            printf("[+] Memory unlocked\n");
        }

        if (GlobalFree(hMoveable) == NULL) {
            printf("[+] Moveable memory freed\n");
        }
    }

    printf("\n--- Reallocating Global Memory ---\n");
    HGLOBAL hRealloc = GlobalAlloc(GPTR, 256);
    
    if (hRealloc) {
        printf("[+] Initial allocation: 0x%p (256 bytes)\n", hRealloc);
        
        strcpy_s((char*)hRealloc, 256, "Initial data");
        
        HGLOBAL hNewRealloc = GlobalReAlloc(hRealloc, 1024, GMEM_MOVEABLE);
        
        if (hNewRealloc) {
            printf("[+] Reallocated: 0x%p (1024 bytes)\n", hNewRealloc);
            printf("    Address %s\n", 
                   (hNewRealloc == hRealloc) ? "unchanged" : "changed");
            
            printf("    Content preserved: '%s'\n", (char*)hNewRealloc);
            
            GlobalFree(hNewRealloc);
            printf("[+] Memory freed\n");
        }
    }

    printf("\n[!] Note: GlobalAlloc is a legacy API. Use HeapAlloc for new code.\n");
}

int main(int argc, char* argv[]) {
    printf("\n HEAP MANIPULATION API DEMONSTRATION SUITE \n");


    if (argc > 1) {
        int demoNum = atoi(argv[1]);
        printf("\n[*] Running Demo %d only...\n", demoNum);
        
        switch (demoNum) {
            case 1: Demo_HeapCreate(); break;
            case 2: Demo_HeapAlloc(); break;
            case 3: Demo_HeapReAlloc(); break;
            case 4: Demo_GetProcessHeap(); break;
            case 5: Demo_GetProcessHeaps(); break;
            case 6: Demo_RtlCreateHeap(); break;
            case 7: Demo_GlobalAlloc(); break;
            default:
                printf("[-] Invalid demo number. Valid range: 1-7\n");
                return 1;
        }
    } else {
        printf("\n[*] Running all demonstrations...\n");
        
        Demo_HeapCreate();
        Demo_HeapAlloc();
        Demo_HeapReAlloc();
        Demo_GetProcessHeap();
        Demo_GetProcessHeaps();
        Demo_RtlCreateHeap();
        Demo_GlobalAlloc();
    }
    
    PrintSeparator();
    
    printf("\nPress Enter to exit...");
    getchar();
    
    return 0;
}