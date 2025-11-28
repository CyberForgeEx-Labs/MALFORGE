#include <windows.h>
#include <stdio.h>
#include <psapi.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "psapi.lib")

// Function to get window title.
void GetWindowTitle(HWND hwnd, char* title, int maxLen) {
    if (GetWindowTextA(hwnd, title, maxLen) == 0) {
        strcpy_s(title, maxLen, "[No Title]");
    }
}

// Function to get process name from window handle.
void GetProcessName(HWND hwnd, char* procName, int maxLen) {
    DWORD processId;
    GetWindowThreadProcessId(hwnd, &processId);
    
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (hProcess) {
        GetModuleBaseNameA(hProcess, NULL, procName, maxLen);
        CloseHandle(hProcess);
    } else {
        strcpy_s(procName, maxLen, "[Unknown]");
    }
}

// Basic GetForegroundWindow usage.
void Demo_GetForegroundWindow() {
    printf("\n[+] GetForegroundWindow \n");
    printf("[+] Monitoring active window for 10 seconds...\n");
    printf("[+] Switch between different windows to see changes.\n\n");
    
    HWND lastWindow = NULL;
    DWORD startTime = GetTickCount();
    
    while (GetTickCount() - startTime < 10000) {
        HWND currentWindow = GetForegroundWindow();
        
        // Only print when window changes
        if (currentWindow != lastWindow && currentWindow != NULL) {
            char title[256];
            char procName[256];
            
            GetWindowTitle(currentWindow, title, sizeof(title));
            GetProcessName(currentWindow, procName, sizeof(procName));
            
            printf("[%lu ms] Active Window Changed:\n", GetTickCount() - startTime);
            printf("  Handle: 0x%p\n", currentWindow);
            printf("  Title: %s\n", title);
            printf("  Process: %s\n\n", procName);
            
            lastWindow = currentWindow;
        }
        
        Sleep(100); // Poll every 100ms
    }
}

// AttachThreadInput usage.
void Demo_AttachThreadInput() {
    printf("\n[+] AttachThreadInput \n");
    printf("[+] Demonstrating thread input attachment...\n\n");
    
    // Get current thread ID
    DWORD currentThreadId = GetCurrentThreadId();
    
    // Get foreground window and its thread
    HWND foregroundWnd = GetForegroundWindow();
    if (foregroundWnd == NULL) {
        printf("[-] No foreground window found.\n");
        return;
    }
    
    DWORD foregroundThreadId = GetWindowThreadProcessId(foregroundWnd, NULL);
    
    char title[256];
    GetWindowTitle(foregroundWnd, title, sizeof(title));
    
    printf("[+] Current Thread ID: %lu\n", currentThreadId);
    printf("[+] Foreground Window: %s\n", title);
    printf("[+] Foreground Thread ID: %lu\n\n", foregroundThreadId);
    
    if (currentThreadId == foregroundThreadId) {
        printf("[+] Note: Current thread is already the foreground thread.\n");
        return;
    }
    
    // Attach to foreground thread's input
    printf("[+] Attaching to foreground window's input queue...\n");
    BOOL attached = AttachThreadInput(currentThreadId, foregroundThreadId, TRUE);
    
    if (attached) {
        printf("[+] Successfully attached!\n");
        printf("[+]  Now sharing input queue with foreground window.\n");
        printf("[+]  This allows synchronous input processing.\n\n");
        
        // Demonstrate shared input state.
        printf("[+] Checking keyboard state (shared with foreground):\n");
        SHORT capsState = GetKeyState(VK_CAPITAL);
        printf("[+]  CAPS LOCK: %s\n", (capsState & 1) ? "ON" : "OFF");
        
        // Detach after demonstration.
        Sleep(2000);
        printf("\n[+] Detaching from input queue...\n");
        AttachThreadInput(currentThreadId, foregroundThreadId, FALSE);
        printf("[+] Detached successfully.\n");
    } else {
        printf("[-] Failed to Detach (Error: %lu)\n", GetLastError());
    }
}

//Focus change detection.
void Demo_FocusChangeDetection() {
    printf("\n[+] Focus Change Detection\n");
    printf("[+] Detecting focus changes for 10 seconds...\n");
    printf("[+] Click on different windows to trigger detection.\n\n");
    
    HWND lastFocus = NULL;
    DWORD startTime = GetTickCount();
    int changeCount = 0;
    
    while (GetTickCount() - startTime < 10000) {
        HWND currentFocus = GetForegroundWindow();
        
        if (currentFocus != lastFocus && currentFocus != NULL) {
            changeCount++;
            char title[256];
            GetWindowTitle(currentFocus, title, sizeof(title));
            
            DWORD threadId = GetWindowThreadProcessId(currentFocus, NULL);
            
            printf("[+] [Change #%d] Focus switched to:\n", changeCount);
            printf("[+]  Window: %s\n", title);
            printf("[+]  Thread ID: %lu\n", threadId);
            printf("[+]  Time: %lu ms\n\n", GetTickCount() - startTime);
            
            lastFocus = currentFocus;
        }
        
        Sleep(50);
    }
    
    printf("[+] Total focus changes detected: %d\n", changeCount);
}

// Advanced - Monitor specific window focus
void Demo_MonitorSpecificWindow() {
    printf("\n[+] Monitor Specific Window.\n");
    
    // Find a window by partial title (e.g., "Notepad")
    printf("[+] Enter partial window title to monitor (e.g., 'Notepad'): ");
    char searchTitle[256];
    if (fgets(searchTitle, sizeof(searchTitle), stdin)) {
        // Remove newline
        searchTitle[strcspn(searchTitle, "\n")] = 0;
        
        printf("\n[+] Searching for windows containing '%s'...\n", searchTitle);
        
        HWND targetWindow = FindWindowA(NULL, NULL);
        BOOL found = FALSE;
        
        // Search through all windows.
        while (targetWindow != NULL) {
            char title[256];
            GetWindowTitle(targetWindow, title, sizeof(title));
            
            if (strstr(title, searchTitle) != NULL && IsWindowVisible(targetWindow)) {
                printf("[+] Found: %s\n", title);
                found = TRUE;
                break;
            }
            
            targetWindow = GetWindow(targetWindow, GW_HWNDNEXT);
        }
        
        if (!found) {
            printf("[-] No matching window found.\n");
            return;
        }
        
        printf("\n[+] Monitoring for 10 seconds...\n");
        DWORD startTime = GetTickCount();
        DWORD focusTime = 0;
        BOOL wasFocused = FALSE;
        
        while (GetTickCount() - startTime < 10000) {
            HWND current = GetForegroundWindow();
            BOOL isFocused = (current == targetWindow);
            
            if (isFocused && !wasFocused) {
                printf("[%lu ms] Window gained focus\n", GetTickCount() - startTime);
            } else if (!isFocused && wasFocused) {
                printf("[%lu ms] Window lost focus\n", GetTickCount() - startTime);
            }
            
            if (isFocused) {
                focusTime += 50;
            }
            
            wasFocused = isFocused;
            Sleep(50);
        }
        
        printf("\n[+] Statistics:\n");
        printf("[+]  Total focus time: %lu ms (%.1f%%)\n", 
               focusTime, (focusTime / 10000.0) * 100);
    }
}

int main() 
{
    printf("\n\n[+] Window & Focus Tracking API Demonstration.\n");
    
    // Run demos
    Demo_GetForegroundWindow();
    
    printf("\n[*] Press Enter to continue to next demo...");
    getchar();
    
    Demo_AttachThreadInput();
    
    printf("\n[*] Press Enter to continue to next demo...");
    getchar();
    
    Demo_FocusChangeDetection();
    
    printf("\n[*] Press Enter to continue to next demo...");
    getchar();
    
    Demo_MonitorSpecificWindow();
    
    printf("\n[+] All Demos Complete.\n");
    printf("Press Enter to exit...");
    getchar();
    
    return 0;
}