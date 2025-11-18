#include <stdio.h>
#include <winternl.h>  // For NTSTATUS and NtDelayExecution
#include <mmsystem.h>  // For timeGetTime
#include <winsock2.h>  // For select and Winsock
#include <windows.h>
#include <iphlpapi.h> 
#include <icmpapi.h> // For IcmpSendEcho

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

// Typedef for NtDelayExecution
typedef NTSTATUS (NTAPI *pNtDelayExecution)(
    BOOLEAN Alertable,
    PLARGE_INTEGER DelayInterval
);

// Callback for CreateTimerQueueTimer
VOID CALLBACK TimerQueueCallback(PVOID lpParameter, BOOLEAN TimerOrWaitFired) {
    printf("[SUCCESS] CreateTimerQueueTimer: Callback executed after 1 second delay.\n");
}

// Callback for SetTimer
VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    printf("[SUCCESS] SetTimer: Timer elapsed after 1 second.\n");
    KillTimer(NULL, idEvent);
}

// Function to demonstrate NtDelayExecution
BOOL DemonstrateNtDelayExecution()
{
    printf("\n[+] Demonstrating NtDelayExecution\n");
    HMODULE ntdll = LoadLibrary("ntdll.dll");
    pNtDelayExecution NtDelayExecution = (pNtDelayExecution)GetProcAddress(ntdll, "NtDelayExecution");
    LARGE_INTEGER delay;
    delay.QuadPart = -10000000LL;  // 1 second
    NtDelayExecution(FALSE, &delay);
    printf("[SUCCESS] NtDelayExecution: Delayed by 1 second.\n");
    FreeLibrary(ntdll);
    return TRUE;
}

// Function to demonstrate SetWaitableTimer and CreateWaitableTimer
BOOL DemonstrateSetWaitableTimer()
{
    printf("\n[+] Demonstrating SetWaitableTimer and  CreateWaitableTimer\n");
    HANDLE hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
    LARGE_INTEGER dueTime;
    dueTime.QuadPart = -10000000LL;  // 1 second
    SetWaitableTimer(hTimer, &dueTime, 0, NULL, NULL, FALSE);
    WaitForSingleObject(hTimer, INFINITE);
    printf("[SUCCESS] SetWaitableTimer: Waited for 1 second.\n");
    CloseHandle(hTimer);
    return TRUE;
}

// Function to demonstrate CreateTimerQueueTimer
BOOL DemonstrateCreateTimerQueueTimer()
{
    printf("\n[+] Demonstrating CreateTimerQueueTimer\n");
    HANDLE hTimerQueue = CreateTimerQueue();
    HANDLE hTimer;
    CreateTimerQueueTimer(&hTimer, hTimerQueue, TimerQueueCallback, NULL, 1000, 0, 0);
    Sleep(1500);  // Wait for callback
    DeleteTimerQueueTimer(hTimerQueue, hTimer, NULL);
    DeleteTimerQueue(hTimerQueue);
    printf("[INFO] CreateTimerQueueTimer demonstration complete.\n");
    return TRUE;
}

// Function to demonstrate SetTimer
BOOL DemonstrateSetTimer()
{
    printf("\n[+] Demonstrating SetTimer\n");
    UINT_PTR timerId = SetTimer(NULL, 0, 1000, TimerProc);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (msg.message == WM_TIMER) {
            break;
        }
    }
    printf("[INFO] SetTimer demonstration complete.\n");
    return TRUE;
}

// Function to demonstrate select
BOOL DemonstrateSelect()
{
    printf("\n[+] Demonstrating select\n");
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    fd_set dummy;
    FD_ZERO(&dummy);
    select(0, NULL, NULL, NULL, &timeout);
    printf("[SUCCESS] select: Delayed for 1 second.\n");
    return TRUE;
}

// Function to demonstrate IcmpSendEcho
BOOL DemonstrateIcmpSendEcho()
{
    printf("\n[+] Demonstrating IcmpSendEcho\n");
    HANDLE hIcmp = IcmpCreateFile();
    IPAddr destAddr = inet_addr("8.8.8.8");
    char sendData[32] = "Echo request";
    DWORD replySize = sizeof(ICMP_ECHO_REPLY) + sizeof(sendData) + 8;
    void* replyBuffer = malloc(replySize);
    IcmpSendEcho(hIcmp, destAddr, sendData, sizeof(sendData), NULL, replyBuffer, replySize, 1000);
    printf("[SUCCESS] IcmpSendEcho: Echo reply received after up to 1 second.\n");
    free(replyBuffer);
    IcmpCloseHandle(hIcmp);
    return TRUE;
}

// Function to demonstrate timeGetTime
BOOL DemonstrateTimeGetTime()
{
    printf("\n[+] Demonstrating timeGetTime\n");
    DWORD startTime = timeGetTime();
    DWORD delayMs = 1000;
    while (timeGetTime() - startTime < delayMs) {
        // Busy loop
    }
    printf("[SUCCESS] timeGetTime: Busy-waited for 1 second.\n");
    return TRUE;
}

int main(int argc, char* argv[])
{
    printf("\n[+] Time-Based Evasion / Sandbox Detection Demonstration\n");

    // Initialize Winsock
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Demonstrate each API
    DemonstrateNtDelayExecution();
    DemonstrateSetWaitableTimer();
    DemonstrateCreateTimerQueueTimer();
    DemonstrateSetTimer();
    DemonstrateSelect();
    DemonstrateIcmpSendEcho();
    DemonstrateTimeGetTime();

    WSACleanup();

    printf("\n Demonstration Complete.\n");
    printf("\nPress Enter to exit...");
    getchar();
    
    return 0;
}