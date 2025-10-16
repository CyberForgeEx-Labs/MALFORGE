#include <stdio.h>
#include <windows.h> 

// Define a structure to hold the information needed for each check
typedef struct {
    const char *windowClass;
    const char *windowTitle;
    const char *toolName;
} DebuggerCheck;

// List of common debugger and analysis tools to check for.
// We use the exact class names and titles mentioned in the request.
DebuggerCheck known_tools[] = {
    {"OLLYDBG", NULL, "OllyDbg"},
    {NULL, "x64dbg", "x64dbg"},
    {NULL, "x32dbg", "x32dbg"},
    {"WinDbgFrameClass", "WinDbg", "WinDbg"},
    {"ID", "Immunity Debugger", "Immunity Debugger"},
    {"ProcessHacker", NULL, "Process Hacker"},
    {NULL, "Process Monitor", "Process Monitor"},
    {NULL, "Process Explorer", "Process Explorer"},
    {NULL, "Wireshark", "Wireshark"},
};

// Function to perform the anti-debugging check
void check_for_analysis_tools() {
    printf("Analysis via FindWindowA\n\n");

    int detected_count = 0;
    int total_checks = sizeof(known_tools) / sizeof(DebuggerCheck);

    // Loop through the list of known debugger/tool combinations
    for (int i = 0; i < total_checks; i++) {
        DebuggerCheck current = known_tools[i];

        // Call the Windows API function FindWindowA.
        // It returns a non-NULL handle (HWND) if a matching window is found.
        HWND hwnd = FindWindowA(
            current.windowClass,
            current.windowTitle  
        );

        // Check if a handle was returned
        if (hwnd != NULL) {
            printf("[!] DETECTED: %s window found! (Handle: 0x%p)\n", current.toolName, hwnd);
            detected_count++;
        }
    }

    if (detected_count > 0) {
        printf("[!] Anti-Debugging Flag Raised: %d analysis tools detected.\n", detected_count);
    } else {
        printf("[+] All basic checks passed. No known tools detected.\n");
    }
}

int main() {
    check_for_analysis_tools();
    return 0;
}
