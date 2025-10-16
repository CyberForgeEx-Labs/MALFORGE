#include <windows.h>
#include <stdio.h>
#include <string.h>


BOOL CheckDebuggerWithGetTickCount() {
    printf("[*] Testing debugger detection with GetTickCount()...\n");
    
    DWORD startTime = GetTickCount();
    
    // Dummy operations that should execute very quickly
    volatile int sum = 0;
    for (int i = 0; i < 100; i++) {
        sum += i;
        sum -= i;
    }
    
    DWORD endTime = GetTickCount();
    DWORD elapsedTime = endTime - startTime;
    
    printf("[*] Elapsed time: %lu ms\n", elapsedTime);
    
    // Under normal execution: 0-1ms
    // Under debugger with breakpoints: 5-100+ ms depending on number of breakpoints
    if (elapsedTime > 2) {
        printf("[-] Significant delay detected (%lu ms) - possible debugger\n", elapsedTime);
        return TRUE;
    }
    
    printf("[+] Normal execution speed detected\n");
    return FALSE;
}

BOOL CheckDebuggerWithGetTickCount64() {
    printf("[*] Testing debugger detection with GetTickCount64()...\n");
    
    ULONGLONG startTime = GetTickCount64();
    
    // More intensive dummy operations
    volatile long long sum = 0;
    for (int i = 0; i < 1000; i++) {
        sum += (long long)i * i;
        sum -= (long long)i * i;
    }
    
    ULONGLONG endTime = GetTickCount64();
    ULONGLONG elapsedTime = endTime - startTime;
    
    printf("[*] Elapsed time: %llu ms\n", elapsedTime);
    
    // Similar thresholds as GetTickCount but with 64-bit precision
    if (elapsedTime > 3) {
        printf("[-] Significant delay detected (%llu ms) - possible debugger\n", elapsedTime);
        return TRUE;
    }
    
    printf("[+] Normal execution speed detected\n");
    return FALSE;
}

int main() {
    printf("Timing-Based Anti-Debugger Detection Tool via GetTickCount API\n");
    
    BOOL detectedDebugger = FALSE;
    
    // Test 1: GetTickCount
    printf("TEST 1: GetTickCount \n");
    if (CheckDebuggerWithGetTickCount()) {
        detectedDebugger = TRUE;
    }
    
    printf("\n");
    
    // Test 2: GetTickCount64
    printf("TEST 2: GetTickCount64 \n");
    if (CheckDebuggerWithGetTickCount64()) {
        detectedDebugger = TRUE;
    }
    
    printf("\nFinal Results \n");
    if (detectedDebugger) {
        printf("[+] Debugger/suspicious timing detected\n");
        printf("[*] Execution showed timing anomalies consistent with debugging\n");
        return 1;
    } else {
        printf("[+] No debugger detected - normal execution timing\n");
        printf("[*] All timing measurements are within expected ranges\n");
        return 0;
    }
}