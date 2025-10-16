#include <windows.h>
#include <stdio.h>
#include <string.h>

BOOL CheckDebuggerWithQueryPerformanceCounter() {
    printf("[*] Testing debugger detection with QueryPerformanceCounter()...\n");
    
    LARGE_INTEGER frequency, startCounter, endCounter;
    
    // Get the frequency of the performance counter
    if (!QueryPerformanceFrequency(&frequency)) {
        printf("[-] QueryPerformanceFrequency failed\n");
        return FALSE;
    }
    
    printf("[*] Performance counter frequency: %lld Hz\n", frequency.QuadPart);
    
    // Start timing
    if (!QueryPerformanceCounter(&startCounter)) {
        printf("[-] QueryPerformanceCounter failed at start\n");
        return FALSE;
    }
    
    // Execute code block that should be very fast
    volatile unsigned long long result = 0;
    for (int i = 0; i < 50; i++) {
        result ^= (unsigned long long)i;
        result |= (unsigned long long)(i * 2);
    }
    
    // End timing
    if (!QueryPerformanceCounter(&endCounter)) {
        printf("[-] QueryPerformanceCounter failed at end\n");
        return FALSE;
    }
    
    // Calculate elapsed time in microseconds
    LONGLONG elapsedCycles = endCounter.QuadPart - startCounter.QuadPart;
    double elapsedMicroseconds = (double)elapsedCycles / (frequency.QuadPart / 1000000.0);
    
    printf("[*] Elapsed cycles: %lld\n", elapsedCycles);
    printf("[*] Elapsed time: %.2f microseconds\n", elapsedMicroseconds);
    
    // Normal execution: <1000 microseconds (1 millisecond)
    // Debugger with breakpoints: >10000 microseconds
    if (elapsedMicroseconds > 5000.0) {
        printf("[-] Excessive delay detected (%.2f µs) - possible debugger/breakpoints\n", elapsedMicroseconds);
        return TRUE;
    }
    
    printf("[+] Normal execution speed detected\n");
    return FALSE;
}

int main() {
    printf("Timing-Based Anti-Debugger Detection Tool via QueryPerformanceCounter API\n");
    
    BOOL detectedDebugger = FALSE;
    
    // Test 3: QueryPerformanceCounter
    printf("QueryPerformanceCounter Result\n");
    if (CheckDebuggerWithQueryPerformanceCounter()) {
        detectedDebugger = TRUE;
    }

    // Final result
    printf("\nFinal Result \n");
    if (detectedDebugger) {
        printf("[*] Debugger/suspicious timing detected\n");
        printf("[+] Execution showed timing anomalies consistent with debugging\n");
        return 1;
    } else {
        printf("[*] No debugger detected - normal execution timing\n");
        printf("[+] All timing measurements are within expected ranges\n");
        return 0;
    }
}