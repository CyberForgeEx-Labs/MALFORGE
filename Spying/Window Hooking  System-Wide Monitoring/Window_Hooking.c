#include <windows.h>
#include <stdio.h>
#include <psapi.h>
#include <tlhelp32.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "psapi.lib")

// Global variables for hooks
HHOOK g_hKeyboardHook = NULL;
HHOOK g_hMsgHook = NULL;
HWINEVENTHOOK g_hWinEventHook = NULL;
FILE* g_logFile = NULL;
HANDLE g_hStopEvent = NULL;

// Function prototypes
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam);
void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd,
                          LONG idObject, LONG idChild, DWORD idEventThread, DWORD dwmsEventTime);
void EnableDebugPrivilege();
DWORD GetProcessIdFromHwnd(HWND hwnd);
void LogToFile(const char* format, ...);
void LogKeyInfo(DWORD vkCode, DWORD scanCode, BOOL isKeyDown);
const char* VirtualKeyCodeToString(UINT vkCode);

// Message types for WH_GETMESSAGE hook
const char* GetMessageTypeStr(UINT msg) {
    switch(msg) {
        case WM_KEYDOWN: return "WM_KEYDOWN";
        case WM_KEYUP: return "WM_KEYUP";
        case WM_CHAR: return "WM_CHAR";
        case WM_MOUSEMOVE: return "WM_MOUSEMOVE";
        case WM_LBUTTONDOWN: return "WM_LBUTTONDOWN";
        case WM_LBUTTONUP: return "WM_LBUTTONDOWN";
        case WM_RBUTTONDOWN: return "WM_RBUTTONDOWN";
        case WM_RBUTTONUP: return "WM_RBUTTONUP";
        case WM_MBUTTONDOWN: return "WM_MBUTTONDOWN";
        case WM_MBUTTONUP: return "WM_MBUTTONUP";
        case WM_MOUSEWHEEL: return "WM_MOUSEWHEEL";
        case WM_QUIT: return "WM_QUIT";
        default: return "OTHER";
    }
}

// Low-level keyboard hook procedure - this works system-wide without DLL injection
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
        
        // Ignore injected events (to avoid logging our own hooks)
        if (p->flags & LLKHF_INJECTED) {
            return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
        }
        
        BOOL isKeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
        
        // Log key information
        LogKeyInfo(p->vkCode, p->scanCode, isKeyDown);
        
        // Check for Ctrl+Alt+S to stop monitoring (secret exit combo)
        static BOOL ctrlPressed = FALSE;
        static BOOL altPressed = FALSE;
        static BOOL sPressed = FALSE;
        
        if (p->vkCode == VK_CONTROL || p->vkCode == VK_LCONTROL || p->vkCode == VK_RCONTROL) {
            ctrlPressed = isKeyDown;
        }
        else if (p->vkCode == VK_MENU || p->vkCode == VK_LMENU || p->vkCode == VK_RMENU) {
            altPressed = isKeyDown;
        }
        else if (p->vkCode == 'S') {
            sPressed = isKeyDown;
            
            // If Ctrl+Alt+S is pressed, signal to stop
            if (ctrlPressed && altPressed && sPressed && isKeyDown) {
                LogToFile("[!] Ctrl+Alt+S detected - stopping monitoring\n");
                SetEvent(g_hStopEvent);
                return 1; // Block the key combination from reaching other applications
            }
        }
    }
    
    // Always call next hook in chain
    return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
}

// GetMsg hook procedure - intercepts posted messages (only for current thread)
LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        MSG* pMsg = (MSG*)lParam;
        
        // Only log when removing message from queue (wParam == PM_REMOVE)
        if (wParam == PM_REMOVE) {
            char className[256] = {0};
            char windowText[256] = {0};
            DWORD pid = GetProcessIdFromHwnd(pMsg->hwnd);
            
            // Get window info
            if (pMsg->hwnd) {
                GetClassNameA(pMsg->hwnd, className, sizeof(className)-1);
                GetWindowTextA(pMsg->hwnd, windowText, sizeof(windowText)-1);
            }
            
            LogToFile("[GETMESSAGE] HWND: 0x%p, PID: %lu, Class: %s, Title: %s, Msg: %s (0x%X)\n",
                     pMsg->hwnd, pid, className, windowText, 
                     GetMessageTypeStr(pMsg->message), pMsg->message);
            
            // Log specific message details
            if (pMsg->message == WM_CHAR) {
                LogToFile("  [CHAR] Character: %c (0x%X)\n", (char)pMsg->wParam, pMsg->wParam);
            }
            else if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP) {
                LogToFile("  [KEY] Virtual Key: 0x%X, Repeat: %d\n", 
                         pMsg->wParam, LOWORD(pMsg->lParam));
            }
            else if (pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST) {
                int x = GET_SC_WPARAM(pMsg->lParam);
                int y = GET_SC_WPARAM
                (pMsg->lParam);
                LogToFile("  [MOUSE] Position: (%d, %d)\n", x, y);
            }
        }
    }
    
    return CallNextHookEx(g_hMsgHook, nCode, wParam, lParam);
}

// WinEvent hook callback - for accessibility/shell events
void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd,
                          LONG idObject, LONG idChild, DWORD idEventThread, 
                          DWORD dwmsEventTime) {
    
    // Skip events without valid window or not window object
    if (!hwnd || idObject != OBJID_WINDOW) return;
    
    char className[256] = {0};
    char windowText[256] = {0};
    DWORD pid = GetProcessIdFromHwnd(hwnd);
    
    GetClassNameA(hwnd, className, sizeof(className)-1);
    GetWindowTextA(hwnd, windowText, sizeof(windowText)-1);
    
    const char* eventStr = "UNKNOWN";
    
    // Map common WinEvents to strings
    switch(event) {
        case EVENT_SYSTEM_FOREGROUND:
            eventStr = "EVENT_SYSTEM_FOREGROUND";
            break;
        case EVENT_SYSTEM_MINIMIZESTART:
            eventStr = "EVENT_SYSTEM_MINIMIZESTART";
            break;
        case EVENT_SYSTEM_MINIMIZEEND:
            eventStr = "EVENT_SYSTEM_MINIMIZEEND";
            break;
        case EVENT_OBJECT_CREATE:
            eventStr = "EVENT_OBJECT_CREATE";
            break;
        case EVENT_OBJECT_DESTROY:
            eventStr = "EVENT_OBJECT_DESTROY";
            break;
        case EVENT_OBJECT_SHOW:
            eventStr = "EVENT_OBJECT_SHOW";
            break;
        case EVENT_OBJECT_HIDE:
            eventStr = "EVENT_OBJECT_HIDE";
            break;
        case EVENT_OBJECT_FOCUS:
            eventStr = "EVENT_OBJECT_FOCUS";
            break;
        case EVENT_OBJECT_LOCATIONCHANGE:
            eventStr = "EVENT_OBJECT_LOCATIONCHANGE";
            return;
        case EVENT_SYSTEM_MOVESIZESTART:
            eventStr = "EVENT_SYSTEM_MOVESIZESTART";
            break;
        case EVENT_SYSTEM_MOVESIZEEND:
            eventStr = "EVENT_SYSTEM_MOVESIZEEND";
            break;
        case EVENT_SYSTEM_CAPTURESTART:
            eventStr = "EVENT_SYSTEM_CAPTURESTART";
            break;
        case EVENT_SYSTEM_CAPTUREEND:
            eventStr = "EVENT_SYSTEM_CAPTUREEND";
            break;
        case EVENT_OBJECT_NAMECHANGE:
            eventStr = "EVENT_OBJECT_NAMECHANGE";
            break;
    }
    
    // Log window focus events in more detail
    if (event == EVENT_SYSTEM_FOREGROUND) {
        LogToFile("[WINEVENT] %s - Window activated: '%s' (Class: %s, PID: %lu)\n",
                 eventStr, windowText, className, pid);
    } else {
        LogToFile("[WINEVENT] %s - HWND: 0x%p, Class: %s, Title: '%s', PID: %lu\n",
                 eventStr, hwnd, className, windowText, pid);
    }
}

// Enable debug privilege for process enumeration
void EnableDebugPrivilege() {
    HANDLE hToken;
    TOKEN_PRIVILEGES tp;
    LUID luid;
    
    if (OpenProcessToken(GetCurrentProcess(), 
                        TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) {
            tp.PrivilegeCount = 1;
            tp.Privileges[0].Luid = luid;
            tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
            
            AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
        }
        CloseHandle(hToken);
    }
}

// Get process ID from window handle
DWORD GetProcessIdFromHwnd(HWND hwnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    return pid;
}

// Convert virtual key code to string
const char* VirtualKeyCodeToString(UINT vkCode) {
    switch(vkCode) {
        case VK_LBUTTON: return "Left Mouse Button";
        case VK_RBUTTON: return "Right Mouse Button";
        case VK_MBUTTON: return "Middle Mouse Button";
        case VK_BACK: return "Backspace";
        case VK_TAB: return "Tab";
        case VK_RETURN: return "Enter";
        case VK_SHIFT: return "Shift";
        case VK_CONTROL: return "Ctrl";
        case VK_MENU: return "Alt";
        case VK_PAUSE: return "Pause";
        case VK_CAPITAL: return "Caps Lock";
        case VK_ESCAPE: return "Escape";
        case VK_SPACE: return "Space";
        case VK_PRIOR: return "Page Up";
        case VK_NEXT: return "Page Down";
        case VK_END: return "End";
        case VK_HOME: return "Home";
        case VK_LEFT: return "Left Arrow";
        case VK_UP: return "Up Arrow";
        case VK_RIGHT: return "Right Arrow";
        case VK_DOWN: return "Down Arrow";
        case VK_INSERT: return "Insert";
        case VK_DELETE: return "Delete";
        case VK_LWIN: return "Left Windows";
        case VK_RWIN: return "Right Windows";
        case VK_APPS: return "Application";
        case VK_NUMPAD0: return "Numpad 0";
        case VK_NUMPAD1: return "Numpad 1";
        case VK_NUMPAD2: return "Numpad 2";
        case VK_NUMPAD3: return "Numpad 3";
        case VK_NUMPAD4: return "Numpad 4";
        case VK_NUMPAD5: return "Numpad 5";
        case VK_NUMPAD6: return "Numpad 6";
        case VK_NUMPAD7: return "Numpad 7";
        case VK_NUMPAD8: return "Numpad 8";
        case VK_NUMPAD9: return "Numpad 9";
        case VK_MULTIPLY: return "Numpad *";
        case VK_ADD: return "Numpad +";
        case VK_SEPARATOR: return "Separator";
        case VK_SUBTRACT: return "Numpad -";
        case VK_DECIMAL: return "Numpad .";
        case VK_DIVIDE: return "Numpad /";
        case VK_F1: return "F1";
        case VK_F2: return "F2";
        case VK_F3: return "F3";
        case VK_F4: return "F4";
        case VK_F5: return "F5";
        case VK_F6: return "F6";
        case VK_F7: return "F7";
        case VK_F8: return "F8";
        case VK_F9: return "F9";
        case VK_F10: return "F10";
        case VK_F11: return "F11";
        case VK_F12: return "F12";
        case VK_NUMLOCK: return "Num Lock";
        case VK_SCROLL: return "Scroll Lock";
        case VK_LSHIFT: return "Left Shift";
        case VK_RSHIFT: return "Right Shift";
        case VK_LCONTROL: return "Left Ctrl";
        case VK_RCONTROL: return "Right Ctrl";
        case VK_LMENU: return "Left Alt";
        case VK_RMENU: return "Right Alt";
        case VK_VOLUME_MUTE: return "Volume Mute";
        case VK_VOLUME_DOWN: return "Volume Down";
        case VK_VOLUME_UP: return "Volume Up";
        case VK_MEDIA_NEXT_TRACK: return "Next Track";
        case VK_MEDIA_PREV_TRACK: return "Previous Track";
        case VK_MEDIA_PLAY_PAUSE: return "Play/Pause";
        default:
            // Check if it's a letter
            if (vkCode >= 'A' && vkCode <= 'Z') {
                static char letter[2] = {0};
                letter[0] = (char)vkCode;
                return letter;
            }
            // Check if it's a number
            else if (vkCode >= '0' && vkCode <= '9') {
                static char number[2] = {0};
                number[0] = (char)vkCode;
                return number;
            }
            return "Unknown Key";
    }
}

// Log detailed keyboard information
void LogKeyInfo(DWORD vkCode, DWORD scanCode, BOOL isKeyDown) {
    const char* keyName = VirtualKeyCodeToString(vkCode);
    const char* action = isKeyDown ? "DOWN" : "UP";
    
    // Get current foreground window info
    HWND fgWindow = GetForegroundWindow();
    char windowTitle[256] = {0};
    char className[256] = {0};
    DWORD pid = 0;
    
    if (fgWindow) {
        GetWindowTextA(fgWindow, windowTitle, sizeof(windowTitle)-1);
        GetClassNameA(fgWindow, className, sizeof(className)-1);
        pid = GetProcessIdFromHwnd(fgWindow);
    }
    
    // Get modifier key states
    BOOL shiftPressed = GetAsyncKeyState(VK_SHIFT) & 0x8000;
    BOOL ctrlPressed = GetAsyncKeyState(VK_CONTROL) & 0x8000;
    BOOL altPressed = GetAsyncKeyState(VK_MENU) & 0x8000;
    
    LogToFile("[KEYBOARD] Key %s: %s (VK: 0x%02X, Scan: 0x%04X) [Window: '%s', Class: %s, PID: %lu] ",
             action, keyName, vkCode, scanCode, windowTitle, className, pid);
    
    // Show modifier state
    if (shiftPressed || ctrlPressed || altPressed) {
        LogToFile("[Modifiers: ");
        if (shiftPressed) LogToFile("SHIFT ");
        if (ctrlPressed) LogToFile("CTRL ");
        if (altPressed) LogToFile("ALT ");
        LogToFile("]");
    }
    LogToFile("\n");
}

// Logging function
void LogToFile(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    // Get current time
    SYSTEMTIME st;
    GetLocalTime(&st);
    char timestamp[64];
    sprintf(timestamp, "%02d:%02d:%02d.%03d", 
            st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    
    // Print to console with timestamp
    printf("[%s] ", timestamp);
    vprintf(format, args);
    
    // Write to file if open
    if (g_logFile) {
        fprintf(g_logFile, "[%s] ", timestamp);
        vfprintf(g_logFile, format, args);
        fflush(g_logFile);
    }
    
    va_end(args);
}

// Install hooks
BOOL InstallHooks() {
    printf("[+] Installing system-wide hooks...\n");
    
    // Create stop event
    g_hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!g_hStopEvent) {
        printf("[-] Failed to create stop event. Error: %lu\n", GetLastError());
    }
    
    // Open log file
    g_logFile = fopen("hook_monitor.log", "a");
    if (!g_logFile) {
        printf("[*] Warning: Could not open log file, logging to console only.\n");
    }
    
    // Enable debug privilege for better process information
    EnableDebugPrivilege();
    
    // Log start time
    SYSTEMTIME st;
    GetLocalTime(&st);
    LogToFile("\n[+] Hook Monitoring Started at %04d-%02d-%02d %02d:%02d:%02d\n\n",
             st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
    
    // 1. Install WH_KEYBOARD_LL hook (system-wide low-level keyboard hook)
    // This works without DLL injection!
    g_hKeyboardHook = SetWindowsHookExA(WH_KEYBOARD_LL, KeyboardProc, 
                                        GetModuleHandle(NULL), 0);
    if (!g_hKeyboardHook) {
        DWORD err = GetLastError();
        printf("[-] Failed to install keyboard hook. Error: %lu\n", err);
        return FALSE;
    }
    printf("[+] WH_KEYBOARD_LL hook installed successfully (system-wide).\n");
    
    // 2. Install WH_GETMESSAGE hook (only for current thread - for demo)
    // Note: For system-wide WH_GETMESSAGE, you would need a DLL
    g_hMsgHook = SetWindowsHookExA(WH_GETMESSAGE, GetMsgProc, 
                                   NULL, GetCurrentThreadId());
    if (!g_hMsgHook) {
        printf("[-] Failed to install WH_GETMESSAGE hook. Error: %lu\n", GetLastError());
        // Continue anyway
    } else {
        printf("[+] WH_GETMESSAGE hook installed successfully (thread-local).\n");
    }
    
    // 3. Install WinEvent hook (system-wide accessibility events)
    g_hWinEventHook = SetWinEventHook(
        EVENT_MIN,                    // Minimum event
        EVENT_MAX,                    // Maximum event
        NULL,                         // DLL module (NULL = current process)
        WinEventProc,                 // Callback function
        0, 0,                         // Process/thread IDs (0 = all)
        WINEVENT_OUTOFCONTEXT |       // Don't inject into target processes
        WINEVENT_SKIPOWNPROCESS);     // Skip events from this process
    
    if (!g_hWinEventHook) {
        printf("[-] Failed to install WinEvent hook. Error: %lu\n", GetLastError());
        // Don't return FALSE here - keyboard hook is the most important
    } else {
        printf("[+] WinEvent hook installed successfully.\n");
    }
    
    printf("\n[+] All hooks installed successfully!\n");
    printf("[+] Keyboard events will be captured system-wide.\n");
    printf("[+] Press Ctrl+Alt+S to stop monitoring (secret exit combo).\n");
    printf("[+] Or press Enter in this console window.\n\n");
    
    return TRUE;
}

// Remove hooks
void RemoveHooks() {
    printf("\n[+] Removing hooks...\n");
    
    if (g_hKeyboardHook) {
        UnhookWindowsHookEx(g_hKeyboardHook);
        g_hKeyboardHook = NULL;
        printf("[+] WH_KEYBOARD_LL hook removed.\n");
    }
    
    if (g_hMsgHook) {
        UnhookWindowsHookEx(g_hMsgHook);
        g_hMsgHook = NULL;
        printf("[+] WH_GETMESSAGE hook removed.\n");
    }
    
    if (g_hWinEventHook) {
        UnhookWinEvent(g_hWinEventHook);
        g_hWinEventHook = NULL;
        printf("[+] WinEvent hook removed.\n");
    }
    
    if (g_logFile) {
        fclose(g_logFile);
        g_logFile = NULL;
        printf("[+] Log file closed.\n");
    }
    
    if (g_hStopEvent) {
        CloseHandle(g_hStopEvent);
        g_hStopEvent = NULL;
    }
    
    printf("[+] All hooks removed.\n");
}

// Message pump for console application
void RunMessageLoop() {
    MSG msg;
    
    printf("[+] Starting message loop...\n");
    
    // Main message loop
    while (TRUE) {
        // Check if we should stop
        if (WaitForSingleObject(g_hStopEvent, 0) == WAIT_OBJECT_0) {
            printf("[+] Stop event signaled. Exiting...\n");
            break;
        }
        
        // Process messages
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            
            // Check for quit message
            if (msg.message == WM_QUIT) {
                return;
            }
        }
        
        // Small sleep to prevent high CPU usage
        Sleep(10);
    }
}

int main() 
{
    printf("[+] Windows System-Wide Hook Monitor Demostration.\n");
    
    // Install hooks
    if (!InstallHooks()) {
        printf("[-] Failed to install hooks. Exiting.\n");
        return 1;
    }
    
    printf("\n[+] Monitoring active! Start typing in any application...\n");
    printf("[+] Events will be logged to console and hook_monitor.log\n");
    printf("[+] Press Enter in this console to stop...\n\n");
    
    // Start message loop
    RunMessageLoop();
    
    // Alternative: Wait for Enter key (if message loop fails)
    printf("[+] Press Enter to stop monitoring...\n");
    getchar();
    
    // Cleanup
    RemoveHooks();
    
    printf("\n[+] Monitoring stopped.\n");
    printf("[+] Check 'hook_monitor.log' for detailed event history.\n");
    
    return 0;
}