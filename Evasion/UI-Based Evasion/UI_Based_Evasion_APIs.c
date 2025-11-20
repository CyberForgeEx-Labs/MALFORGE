#include <windows.h>
#include <stdio.h>

// Global variables
HWND ghWnd = NULL; // handle to the window.
int gHotkeyPressed = 0;

// Forward declaration.(fpt)
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// CreateWindowExA Demonstration.
HWND CreateMainWindow(HINSTANCE hInstance)
{
    // Register a window class.
    WNDCLASSA wc = {0};
    wc.lpfnWndProc = WindowProc;           // Callback function for window messages
    wc.hInstance = hInstance;
    wc.lpszClassName = "WindowClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.style = CS_VREDRAW | CS_HREDRAW; // windows size change style altration.

    if (!RegisterClassA(&wc))
    {
        printf("[-] Window class registration failed!\n");
        return NULL;
    }

    // Create a window using CreateWindowExA
    // Extended styles allow for more control over window appearance
    HWND hwnd = CreateWindowExA(
        WS_EX_OVERLAPPEDWINDOW,       // Extended style: standard overlapped window with borders.
        "WindowClass",            // Class name (must match registered class)
        "Window Demo",           // Window title (appears in title bar)
        WS_OVERLAPPEDWINDOW,          // Standard window style
        100,                        // X position (pixels from left)
        100,                           // Y position (pixels from top)
        600,                        // Width (pixels)
        400,                        // Height (pixels)
        NULL,                   // Parent window (NULL = top-level window)
        NULL,               // Menu (NULL = use class menu if any)
        hInstance,              // Application instance
        NULL                // Extra creation parameter
    );

    if (!hwnd)
    {
        printf("[-] Window creation failed!\n");
        return NULL;
    }

    printf("[+] Window created successfully: HWND = %p\n", hwnd);
    return hwnd;
}

// RegisterHotKey Demonstration.
void RegisterApplicationHotkeys(HWND hwnd)
{
    // Hotkey 1 : Ctrl+Alt+E - Exit application
    // ID 1 is used to identify this hotkey in WM_HOTKEY messages
    if (RegisterHotKey(hwnd, 1, MOD_CONTROL | MOD_ALT, 'E'))
    {
        printf("[+] Hotkey 1 registered: Ctrl+Alt+E\n");
    }
    else
    {
        printf("[+] Failed to register hotkey 1\n");
    }

    // Hotkey 2: Ctrl+Alt+H - Show help (custom behavior)
    // ID 2 identifies this hotkey
    if (RegisterHotKey(hwnd, 2, MOD_CONTROL | MOD_ALT, 'H'))
    {
        printf("[+] Hotkey 2 registered: Ctrl+Alt+H\n");
    }
    else
    {
        printf("[-] Failed to register hotkey 2\n");
    }

    // Hotkey 3: Windows+D - Another example using Win key
    // Using VK_D for the D key
    if (RegisterHotKey(hwnd, 3, MOD_WIN, VK_F9))
    {
        printf("[+] Hotkey 3 registered: Win+F9\n");
    }
    else
    {
        printf("[-] Failed to register hotkey 3\n");
    }
}

// UnregisterHotKey Demonstration.
void UnregisterApplicationHotkeys(HWND hwnd)
{
    UnregisterHotKey(hwnd, 1);
    UnregisterHotKey(hwnd, 2);
    UnregisterHotKey(hwnd, 3);
    printf("[+] Hotkeys unregistered\n");
}

/**
 * Window Procedure - Handles all messages sent to the window.
 * This is called by Windows whenever an event occurs.
 */
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CREATE:
            printf("[+] Window created message received\n");
            RegisterApplicationHotkeys(hwnd);
            return 0;

        case WM_HOTKEY:
            // wParam contains the hotkey ID (1, 2, 3, etc.)
            gHotkeyPressed = 1;
            printf("[+] Hotkey pressed! ID = %d \n", (int)wParam);
            
            switch (wParam)
            {
                case 1:
                    printf("[+] Hotkey 1 (Ctrl+Alt+E) triggered - Exiting...\n");
                    PostMessage(hwnd, WM_CLOSE, 0, 0);
                    break;
                case 2:
                    printf("[+] Hotkey 2 (Ctrl+Alt+H) triggered - Help requested\n");
                    MessageBoxA(hwnd, 
                        "Hotkey Demo Help\n"
                        "- Ctrl+Alt+E: Exit\n"
                        "- Ctrl+Alt+H: Help\n"
                        "- Win+F9: Custom action",
                        "Help", MB_OK | MB_ICONINFORMATION);
                    break;
                case 3:
                    printf("[+] Hotkey 3 (Win+F9) triggered - Custom action\n");
                    break;
            }
            return 0;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            // Draw text on the window
            TextOutA(hdc, 20, 20, "Educational Hotkey & Window Demo", 35);
            TextOutA(hdc, 20, 60, "Press registered hotkeys:", 26);
            TextOutA(hdc, 40, 90, "Ctrl+Alt+E: Exit", 16);
            TextOutA(hdc, 40, 120, "Ctrl+Alt+H: Help", 16);
            TextOutA(hdc, 40, 150, "Win+F9: Custom action", 21);
            TextOutA(hdc, 20, 200, "Check console for hotkey events", 30);
            
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_CLOSE:
            printf("[+] Close message received\n");
            UnregisterApplicationHotkeys(hwnd);
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            printf("[+] Window destroyed\n");
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProcA(hwnd, msg, wParam, lParam);
    }
}


int main()
{
    printf("\n[+] UI Based Evasion APIs Demonstration.\n");

    HINSTANCE hInstance = GetModuleHandleA(NULL);

    // Create the main window
    ghWnd = CreateMainWindow(hInstance);
    if (!ghWnd)
    {
        printf("[-] Failed to create window\n");
        return 1;
    }

    // Make the window visible
    ShowWindow(ghWnd, SW_SHOW);
    UpdateWindow(ghWnd);

    printf("\nWindow is running. Try pressing the hotkeys:\n");
    printf("- Ctrl+Alt+E to exit\n");
    printf("- Ctrl+Alt+H for help\n");
    printf("- Win+F9 for custom action\n\n");

    // Message loop - process Windows messages
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    printf("[+] Application exiting...\n");
    printf("[+] Demonstration Completed\n");
    return (int)msg.wParam; //report termination.
}
