#include <windows.h>
#include <stdio.h>

// Hot key identifiers
#define HOTKEY_CTRL_ALT_K 1
#define HOTKEY_CTRL_SHIFT_M 2

// Global variables
HWND g_hwnd = NULL;
int g_mouseClickCount = 0;
int g_keyPressCount = 0;

// Function to register raw input devices
BOOL RegisterRawInputs(HWND hwnd) {
    RAWINPUTDEVICE rid[2];
    
    // Register for mouse raw input
    rid[0].usUsagePage = 0x01;          // HID_USAGE_PAGE_GENERIC
    rid[0].usUsage = 0x02;              // HID_USAGE_GENERIC_MOUSE
    rid[0].dwFlags = RIDEV_INPUTSINK;   // Receive input even when not in foreground
    rid[0].hwndTarget = hwnd;
    
    // Register for keyboard raw input
    rid[1].usUsagePage = 0x01;          // HID_USAGE_PAGE_GENERIC
    rid[1].usUsage = 0x06;              // HID_USAGE_GENERIC_KEYBOARD
    rid[1].dwFlags = RIDEV_INPUTSINK;   // Receive input even when not in foreground
    rid[1].hwndTarget = hwnd;
    
    if (!RegisterRawInputDevices(rid, 2, sizeof(RAWINPUTDEVICE))) {
        printf("[-] Failed to register raw input devices. Error: %lu\n", GetLastError());
        return FALSE;
    }
    
    printf("[+] Successfully registered raw input devices!\n");
    return TRUE;
}

// Function to process raw mouse input
void ProcessRawMouse(RAWMOUSE* mouse) {
    // Check button flags
    if (mouse->usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) {
        g_mouseClickCount++;
        printf("[+] Left mouse button DOWN (Total clicks: %d)\n", g_mouseClickCount);
    } 
    
    if (mouse->usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) {
        printf("[+] Right mouse button DOWN\n");
    }
    
    if (mouse->usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN) {
        printf("[+] Middle mouse button DOWN\n");
    }
    
    // Mouse movement (relative coordinates)
    if (mouse->lLastX != 0 || mouse->lLastY != 0) {
        printf("[+] Mouse moved: X=%ld, Y=%ld\n", mouse->lLastX, mouse->lLastY);
    }
    
    // Mouse wheel
    if (mouse->usButtonFlags & RI_MOUSE_WHEEL) {
        SHORT wheelDelta = (SHORT)mouse->usButtonData;
        printf("[+] Mouse wheel: %d\n", wheelDelta);
    }
}

// Function to process raw keyboard input
void ProcessRawKeyboard(RAWKEYBOARD* keyboard) {
    // Check if key is pressed (not released)
    if (!(keyboard->Flags & RI_KEY_BREAK)) {
        g_keyPressCount++;
        
        // Get the virtual key code
        USHORT vkey = keyboard->VKey;
        
        // Get scan code
        USHORT scanCode = keyboard->MakeCode;
        
        printf("[+] Key pressed: VKey=0x%02X, ScanCode=0x%02X (Total: %d)\n", 
               vkey, scanCode, g_keyPressCount);
        
        // Example: Detect specific keys
        if (vkey == VK_ESCAPE) {
            printf("[+] ESC key detected! Press Ctrl+Alt+K to see statistics.\n");
        }
    }
}

// Window procedure to handle messages
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
            // Register hot keys suitable global constants.
            if (!RegisterHotKey(hwnd, HOTKEY_CTRL_ALT_K, MOD_CONTROL | MOD_ALT, 'K')) {
                printf("[-] Failed to register Ctrl+Alt+K hotkey\n");
            } else {
                printf("[+] Registered hotkey: Ctrl+Alt+K\n");
            }
            
            if (!RegisterHotKey(hwnd, HOTKEY_CTRL_SHIFT_M, MOD_CONTROL | MOD_SHIFT, 'M')) {
                printf("[-] Failed to register Ctrl+Shift+M hotkey\n");
            } else {
                printf("[+] Registered hotkey: Ctrl+Shift+M\n");
            }
            break;
            
        case WM_INPUT: {
            UINT dwSize;
            
            // Get the size of raw input data
            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, 
                          sizeof(RAWINPUTHEADER));
            
            LPBYTE lpb = (LPBYTE)malloc(dwSize);
            if (lpb == NULL) {
                free(lpb);
                return 0;
            }
            
            // Get the actual raw input data
            if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, 
                              sizeof(RAWINPUTHEADER)) != dwSize) {
                printf("[-] GetRawInputData failed\n");
            } else {
                RAWINPUT* raw = (RAWINPUT*)lpb;
                
                // Process based on device type
                if (raw->header.dwType == RIM_TYPEMOUSE) {
                    ProcessRawMouse(&raw->data.mouse);
                } else if (raw->header.dwType == RIM_TYPEKEYBOARD) {
                    ProcessRawKeyboard(&raw->data.keyboard);
                }
            }
            
            free(lpb);
            break;
        }
        
        case WM_HOTKEY:
            // Handle registered hot keys
            if (wParam == HOTKEY_CTRL_ALT_K) {
                printf("\n[+] STATISTICS (Ctrl+Alt+K pressed)\n");
                printf("  -> Total mouse clicks: %d\n", g_mouseClickCount);
                printf("  -> Total key presses: %d\n", g_keyPressCount);
                printf("=====================================\n\n");
            } else if (wParam == HOTKEY_CTRL_SHIFT_M) {
                printf("\n[+] Ctrl+Shift+M pressed - Resetting counters.\n\n");
                g_mouseClickCount = 0;
                g_keyPressCount = 0;
            }
            break;
            
        case WM_DESTROY:
            // Unregister hot keys
            UnregisterHotKey(hwnd, HOTKEY_CTRL_ALT_K);
            UnregisterHotKey(hwnd, HOTKEY_CTRL_SHIFT_M);
            PostQuitMessage(0);
            break;
            
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int main() 
{
    printf("[+] Windows Raw Input and HotKey Demonstrations.\n\n");
    
    // Register window class
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "RawInputDemoClass";
    
    if (!RegisterClassEx(&wc)) {
        printf("[+] Failed to register window class\n");
        return 1;
    }
    
    // Create window (hidden)
    g_hwnd = CreateWindowEx(
        0,
        "RawInputDemoClass",
        "Raw Input Demo",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
        NULL, NULL, GetModuleHandle(NULL), NULL
    );
    
    if (g_hwnd == NULL) {
        printf("[+] Failed to create window\n");
        return 1;
    }
    
    // Register raw input devices
    if (!RegisterRawInputs(g_hwnd)) {
        return 1;
    }
    
    printf("\n[+] Instructions:\n");
    printf("  -> Move your mouse and click buttons to see raw input\n");
    printf("  -> Press any keys to see keyboard input\n");
    printf("  -> Press Ctrl+Alt+K to view statistics\n");
    printf("  -> Press Ctrl+Shift+M to reset counters\n");
    printf("  -> Press Alt+F4 or close this console to exit\n\n");
    
    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return 0;
}