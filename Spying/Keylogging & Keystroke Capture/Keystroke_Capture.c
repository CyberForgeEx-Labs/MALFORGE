#include <windows.h>
#include <stdio.h>
#include <stdbool.h>

// Returns the state of a virtual key at the time of the call.
void demo_GetAsyncKeyState(void) {
    printf("\n[+] GetAsyncKeyState Demonstration\n");
    printf("[+] Press ESC to exit, or any key to test...\n\n");
    
    while (true) {
        // Check if ESC key is pressed
        // 0x8000 - refers that key was down(being pressing) high order bit. 
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            printf(" - > ESC pressed - exiting demo\n");
            break;
        }
        
        // Check for some common keys
        if (GetAsyncKeyState('A') & 0x8000) {
            printf(" - > Key 'A' is currently pressed\n");
            Sleep(200); // Debounce
        }
        
        if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
            printf(" - > SPACE is currently pressed\n");
            Sleep(200);
        }
        
        if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
            printf(" - > ENTER is currently pressed\n");
            Sleep(200);
        }
        
        Sleep(50); // Small delay to reduce CPU usage
    }
}

// Gets the status of all 256 virtual keys
void demo_GetKeyboardState(void) {
    printf("\n[+] GetKeyboardState Demonstration\n");
    printf("[+] Press keys and watch their states (Press ESC to exit)\n\n");
    
    BYTE keyState[256];
    int count = 0;
    bool lastStates[256] = {false};
    
    while (count++ < 100) { // Run for limited iterations
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) break;
        
        // First, populate keyState with async states for demo purposes
        // In a GUI app with message loop, GetKeyboardState would work directly
        for (int i = 0; i < 256; i++) {
            keyState[i] = (GetAsyncKeyState(i) & 0x8000) ? 0x80 : 0x00;
        }
        
        bool somethingPressed = false;
        
        // Check modifier keys
        if (keyState[VK_SHIFT] & 0x80 || keyState[VK_CONTROL] & 0x80 || 
            keyState[VK_MENU] & 0x80) {
            printf("Modifiers: ");
            if (keyState[VK_SHIFT] & 0x80) printf("[SHIFT] ");
            if (keyState[VK_CONTROL] & 0x80) printf("[CTRL] ");
            if (keyState[VK_MENU] & 0x80) printf("[ALT] ");
            somethingPressed = true;
        }
        
        // Check letter keys
        for (int i = 'A'; i <= 'Z'; i++) {
            if ((keyState[i] & 0x80) && !lastStates[i]) {
                if (!somethingPressed) printf("Keys: ");
                printf("[%c] ", i);
                somethingPressed = true;
            }
            lastStates[i] = (keyState[i] & 0x80);
        }
        
        // Check number keys
        for (int i = '0'; i <= '9'; i++) {
            if ((keyState[i] & 0x80) && !lastStates[i]) {
                if (!somethingPressed) printf("Keys: ");
                printf("[%c] ", i);
                somethingPressed = true;
            }
            lastStates[i] = (keyState[i] & 0x80);
        }
        
        // Check special keys
        if ((keyState[VK_SPACE] & 0x80) && !lastStates[VK_SPACE]) {
            if (!somethingPressed) printf("Keys: ");
            printf("[SPACE] ");
            somethingPressed = true;
        }
        lastStates[VK_SPACE] = (keyState[VK_SPACE] & 0x80);
        
        if ((keyState[VK_RETURN] & 0x80) && !lastStates[VK_RETURN]) {
            if (!somethingPressed) printf("Keys: ");
            printf("[ENTER] ");
            somethingPressed = true;
        }
        lastStates[VK_RETURN] = (keyState[VK_RETURN] & 0x80);
        
        if (somethingPressed) {
            printf("\n");
        }
        
        Sleep(50);
    }
    
}

// Gets the state of a specific virtual key
void demo_GetKeyState(void) {
    printf("\n[+] GetKeyState Demonstration.\n");
    printf("[+] Toggle CAPS LOCK and NUM LOCK to see their states\n");
    printf("[+] Press ESC to exit\n\n");
    
    bool lastCaps = false, lastNum = false;
    
    while (true) {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) break;
        
        // GetKeyState returns toggle state in low-order bit
        // to check the key was tapped.
        bool capsLock = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
        bool numLock = (GetKeyState(VK_NUMLOCK) & 0x0001) != 0;
        
        if (capsLock != lastCaps || numLock != lastNum) {
            printf("CAPS LOCK: %s | NUM LOCK: %s\n", 
                   capsLock ? "ON " : "OFF",
                   numLock ? "ON " : "OFF");
            lastCaps = capsLock;
            lastNum = numLock;
        }
        
        Sleep(100);
    }
}

// Converts scan codes to key names.
void demo_GetKeynameTextA(void) {
    printf("\n[+] GetKeynameTextA Demonstration.\n");
    printf("[+] Showing names for common scan codes:\n\n");
    
    // Array of scan codes to test
    struct {
        LONG scanCode;
        const char* desc;
    } codes[] = {
        {0x001E0001, "A key"},
        {0x00300001, "B key"},
        {0x001C0001, "Enter"},
        {0x00390001, "Space"},
        {0x00010001, "Escape"},
        {0x003B0001, "F1"},
        {0x00440001, "F10"},
        {0x00480001, "Up Arrow"},
        {0x004B0001, "Left Arrow"}
    };
    
    char keyName[256];

    int length = sizeof(codes) / sizeof(codes[0]);
    
    for (int i = 0; i < length; i++) {
        int len = GetKeyNameTextA(codes[i].scanCode, keyName, sizeof(keyName));
        if (len > 0) {
            printf("%-15s -> '%s'\n", codes[i].desc, keyName);
        }
    }
}

// Maps virtual key codes to scan codes and vice versa.
void demo_MapVirtualKeyA(void) {
    printf("\n[+] MapVirtualKeyA Demonstration.\n\n");
    
    // MAPVK_VK_TO_VSC (0) - Virtual key to scan code
    printf("[+] Virtual Key to Scan Code:\n");
    printf("  >> VK_RETURN (0x0D) -> Scan Code: 0x%08X\n", MapVirtualKeyA(VK_RETURN, MAPVK_VK_TO_VSC));
    printf("  >> VK_SPACE  (0x20) -> Scan Code: 0x%08X\n", MapVirtualKeyA(VK_SPACE, MAPVK_VK_TO_VSC));
    printf("  >> 'A'       (0x41) -> Scan Code: 0x%08X\n\n", MapVirtualKeyA('A', MAPVK_VK_TO_VSC));
    
    // MAPVK_VSC_TO_VK (1) - Scan code to virtual key
    printf("[+] Scan Code to Virtual Key:\n");
    printf(" >> Scan 0x1C -> Virtual Key: 0x%08X (ENTER)\n", MapVirtualKeyA(0x1C, MAPVK_VSC_TO_VK));
    printf(" >> Scan 0x39 -> Virtual Key: 0x%08X (SPACE)\n", MapVirtualKeyA(0x39, MAPVK_VSC_TO_VK));
    printf(" >> Scan 0x1E -> Virtual Key: 0x%08X ('A')\n\n", MapVirtualKeyA(0x1E, MAPVK_VSC_TO_VK));
    
    // MAPVK_VK_TO_CHAR (2) - Virtual key to character
    printf("[+] Virtual Key to Character:\n");
    printf(" >> 'A' -> Character: '%c'\n", (char)MapVirtualKeyA('A', MAPVK_VK_TO_CHAR));
    printf(" >> '1' -> Character: '%c'\n", (char)MapVirtualKeyA('1', MAPVK_VK_TO_CHAR));
}

// Maps keys with specific keyboard layout
void demo_MapVirtualKeyExA(void) {
    printf("\n[+] MapVirtualKeyExA Demonstration.\n");
    
    HKL layout = GetKeyboardLayout(0);
    printf("[+] Current keyboard layout: 0x%p\n\n", layout);
    
    printf("[+] Mapping with current layout:\n");
    printf("[+] VK_A -> Scan Code: 0x%02X\n", MapVirtualKeyExA('A', MAPVK_VK_TO_VSC, layout));
    printf("[+] VK_RETURN -> Scan Code: 0x%02X\n", MapVirtualKeyExA(VK_RETURN, MAPVK_VK_TO_VSC, layout));
}

//  Main Menu
int main(void) {
    printf("[+] Windows Keyboard API Demonstrations\n");
    printf("====================================\n\n");
    
    printf("[+] Select a demo:\n");
    printf("1. GetAsyncKeyState\n");
    printf("2. GetKeyboardState\n");
    printf("3. GetKeyState\n");
    printf("4. GetKeynameTextA\n");
    printf("5. MapVirtualKeyA\n");
    printf("6. MapVirtualKeyExA\n");
    printf("0. Exit\n\n");
    printf("[+] Choice: ");
    
    int choice;
    scanf("%d", &choice);
    
    switch (choice) {
        case 1: demo_GetAsyncKeyState(); break;
        case 2: demo_GetKeyboardState(); break;
        case 3: demo_GetKeyState(); break;
        case 4: demo_GetKeynameTextA(); break;
        case 5: demo_MapVirtualKeyA(); break;
        case 6: demo_MapVirtualKeyExA(); break;
        case 0: printf("Exiting...\n"); break;
        default: printf("Invalid choice\n");
    }
    
    return 0;
}