#include <windows.h>
#include <stdio.h>
#include <time.h>

// Detect if sleep was skipped.
void AntiDebug_SleepTimingCheck() 
{
    printf("\n[*] Sleep timing verification...\n");
    
    DWORD startTick = GetTickCount();
    
    // Sleep for 3 seconds
    Sleep(3000);
    
    DWORD endTick = GetTickCount();
    DWORD elapsed = endTick - startTick;
    
    printf("[*] Expected: 3000ms, Actual: %lums\n", elapsed);
    
    // If sleep returned too quickly, debugger may have patched it,
    // 100ms tolerance
    if (elapsed < 2900) 
    {  
        printf("[!] DEBUGGER DETECTED!\n");
        printf("[!] Sleep call was bypassed or patched.\n");
        printf("[!] Elapsed time too short: %lums\n", elapsed);
        ExitProcess(1);
    }
    
    printf("[+] Sleep timing check passed.\n");
}

// Multiple short sleeps are harder to patch
void AntiDebug_MultiSleep() 
{
    printf("\n[*] Multi-sleep anti-debug...\n");
    
    DWORD totalStart = GetTickCount();
    DWORD expectedTotal = 0;
    
    // 10 sleeps of 500ms each
    for (int i = 0; i < 10; i++) 
    {
        DWORD start = GetTickCount();
        Sleep(500);
        DWORD end = GetTickCount();
        
        expectedTotal += 500;
        
        DWORD actual = end - start;
        // 50ms tolerance per sleep
        if (actual < 450) 
        {
            printf("[!] DEBUGGER DETECTED at iteration %d!\n", i);
            printf("[!] Sleep %d bypassed (%lums instead of 500ms)\n", i, actual);
            ExitProcess(1);
        }
    }
    
    DWORD totalEnd = GetTickCount();
    DWORD totalElapsed = totalEnd - totalStart;
    
    printf("[*] Total expected: %lums, actual: %lums\n", 
           expectedTotal, totalElapsed);
    
    if (totalElapsed < expectedTotal - 500) 
    {
        printf("[!] DEBUGGER DETECTED!\n");
        printf("[!] Multiple sleeps were bypassed.\n");
        ExitProcess(1);
    }
    
    printf("[+] Multi-sleep check passed.\n");
}

// SleepEx with alertable wait
void AntiDebug_SleepEx() 
{
    printf("\n[*] SleepEx alertable wait test...\n");
    
    // SleepEx with alertable = TRUE
    // Returns early if APC is queued.
    DWORD result = SleepEx(2000, TRUE);
    
    if (result == WAIT_IO_COMPLETION) 
    {
        printf("[!] WARNING: SleepEx interrupted by APC!\n");
        printf("[!] Possible debugger interference.\n");
    }
    
    printf("[+] SleepEx check completed.\n");
}

int main() 
{
    AntiDebug_SleepTimingCheck();
    AntiDebug_MultiSleep();
    AntiDebug_SleepEx();
    printf("\n[+] All sleep-based checks passed!\n");
    return 0;
}
