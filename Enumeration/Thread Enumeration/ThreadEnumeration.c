#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "kernel32.lib")

// Function to enumerate threads using Thread32First/Thread32Next
void EnumerateThreadsBySnapshot(DWORD processId) 
{
    printf("\nThread32First/Thread32Next \n\n");
    printf("Enumerating threads for Process ID: %lu\n", processId);
    
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE) 
    {
        printf("Failed to create snapshot: %lu\n", GetLastError());
        return;
    }
    
    THREADENTRY32 threadEntry;
    threadEntry.dwSize = sizeof(THREADENTRY32);
    
    printf("TID      | PID      | Base Priority | State\n");
    
    if (Thread32First(snapshot, &threadEntry)) 
    {
        do 
        {
            if (threadEntry.th32OwnerProcessID == processId) 
            {
                printf("%-8lu | %-8lu | %-13lu | %s\n",
                    threadEntry.th32ThreadID,
                    threadEntry.th32OwnerProcessID,
                    threadEntry.tpBasePri,
                    "Active");
            }
        } while (Thread32Next(snapshot, &threadEntry));
    } else
    {
        printf("Thread32First failed: %lu\n", GetLastError());
    }
    
    CloseHandle(snapshot);
}

// Function to demonstrate GetProcessIdOfThread
void DemonstrateGetProcessIdOfThread() 
{
    printf("\nGet Process Id Of Thread\n\n");
    
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE) 
    {
        printf("Failed to create snapshot: %lu\n", GetLastError());
        return;
    }
    
    THREADENTRY32 threadEntry;
    threadEntry.dwSize = sizeof(THREADENTRY32);
    
    int count = 0;
    if (Thread32First(snapshot, &threadEntry)) 
    {
        do 
        {
            if (count < 3) 
            {  // Show first 3 threads
                HANDLE threadHandle = OpenThread(
                    THREAD_QUERY_INFORMATION,
                    FALSE,
                    threadEntry.th32ThreadID
                );
                
                if (threadHandle) 
                {
                    DWORD ownerPid = GetProcessIdOfThread(threadHandle);
                    printf("Thread ID: %lu -> Process ID: %lu\n",
                        threadEntry.th32ThreadID, ownerPid);
                    CloseHandle(threadHandle);
                    count++;
                }
            } 
            else 
            {
                break;
            }
        } while (Thread32Next(snapshot, &threadEntry));
    }
    
    CloseHandle(snapshot);
}

// Function to demonstrate GetCurrentThread and GetCurrentThreadId
void DemonstrateCurrentThread() 
{
    printf("\nGetCurrentThread / GetCurrentThreadId \n\n");
    
    HANDLE currentThread = GetCurrentThread();
    DWORD currentThreadId = GetCurrentThreadId();
    
    printf("Current Thread Handle: %p\n", currentThread);
    printf("Current Thread ID: %lu\n", currentThreadId);
    printf("Current Process ID: %lu\n", GetCurrentProcessId());
}

// Function to demonstrate GetThreadId
void DemonstrateGetThreadId() 
{
    printf("\nGetThreadId\n\n");
    
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE) 
    {
        printf("Failed to create snapshot: %lu\n", GetLastError());
        return;
    }
    
    THREADENTRY32 threadEntry;
    threadEntry.dwSize = sizeof(THREADENTRY32);
    
    int count = 0;
    if (Thread32First(snapshot, &threadEntry)) 
    {
        do {
            if (count < 3) 
            {
                HANDLE threadHandle = OpenThread(
                    THREAD_QUERY_INFORMATION,
                    FALSE,
                    threadEntry.th32ThreadID
                );
                
                if (threadHandle) 
                {
                    DWORD tid = GetThreadId(threadHandle);
                    printf("Thread Handle %p -> Thread ID: %lu\n", threadHandle, tid);
                    CloseHandle(threadHandle);
                    count++;
                }
            } 
            else 
            {
                break;
            }
        } while (Thread32Next(snapshot, &threadEntry));
    }
    
    CloseHandle(snapshot);
}

// Function to demonstrate GetThreadInformation
void DemonstrateGetThreadInformation() 
{
    printf("\nGet Thread Information (Base Priority)\n\n");
    
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE) 
    {
        printf("Failed to create snapshot: %lu\n", GetLastError());
        return;
    }
    
    THREADENTRY32 threadEntry;
    threadEntry.dwSize = sizeof(THREADENTRY32);
    
    int count = 0;
    if (Thread32First(snapshot, &threadEntry)) 
    {
        do 
        {
            if (count < 3) 
            {
                printf("Thread ID: %lu\n", threadEntry.th32ThreadID);
                printf("  Base Priority: %lu\n", threadEntry.tpBasePri);
                printf("  Owner Process ID: %lu\n", threadEntry.th32OwnerProcessID);
                printf("  Flags: %lu\n\n", threadEntry.th32ThreadID);
                count++;
            } 
            else 
            {
                break;
            }
        } while (Thread32Next(snapshot, &threadEntry));
    }
    
    CloseHandle(snapshot);
}

int main() 
{
    printf("Windows Thread Enumeration Implementaion\n");
    
    DWORD currentPid = GetCurrentProcessId();
    
    // Demonstrate all thread enumeration APIs
    EnumerateThreadsBySnapshot(currentPid);
    DemonstrateGetProcessIdOfThread();
    DemonstrateCurrentThread();
    DemonstrateGetThreadId();
    DemonstrateGetThreadInformation();
    
    return 0;
}