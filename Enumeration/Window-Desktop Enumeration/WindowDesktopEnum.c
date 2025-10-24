#include <windows.h>
#include <stdio.h>
#include <psapi.h>

#pragma comment(lib, "user32.lib")

// Structure to hold window information
typedef struct {
    HWND hwnd;
    DWORD processId;
    char title[256];
    char className[256];
    BOOL isVisible;
    BOOL isEnabled;
    RECT rect;
} WindowInfo;

// Callback data structure for EnumWindows
typedef struct {
    WindowInfo* windows;
    int count;
    int maxCount;
} EnumWindowsData;

//EnumWindows - Enumerate All Top-Level Windows
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    EnumWindowsData* data = (EnumWindowsData*)lParam;
    
    if (data->count >= data->maxCount) {
        return FALSE; // Stop enumeration if buffer is full
    }
    
    WindowInfo* info = &data->windows[data->count];
    info->hwnd = hwnd;
    
    // Get window title
    GetWindowTextA(hwnd, info->title, sizeof(info->title));
    
    // Get window class name
    GetClassNameA(hwnd, info->className, sizeof(info->className));
    
    // Get process ID
    GetWindowThreadProcessId(hwnd, &info->processId);
    
    // Get visibility and enabled state
    info->isVisible = IsWindowVisible(hwnd);
    info->isEnabled = IsWindowEnabled(hwnd);
    
    // Get window rectangle
    GetWindowRect(hwnd, &info->rect);
    
    data->count++;
    return TRUE; // Continue enumeration
}

void DemonstrateEnumWindows() {
    printf("\nEnumWindows - All Top-Level Windows\n\n");
    
    const int MAX_WINDOWS = 1000;
    WindowInfo* windows = (WindowInfo*)malloc(MAX_WINDOWS * sizeof(WindowInfo));
    
    if (!windows) {
        printf("[-] Memory allocation failed!\n");
        return;
    }
    
    EnumWindowsData data = {0};
    data.windows = windows;
    data.count = 0;
    data.maxCount = MAX_WINDOWS;
    
    // Enumerate all top-level windows
    if (EnumWindows(EnumWindowsProc, (LPARAM)&data)) {
        printf("[+] Successfully enumerated %d top-level windows\n\n", data.count);
        
        printf("%-10s %-8s %-8s %-50s %-20s\n", 
               "HWND", "PID", "Visible", "Window Title", "Class Name");
        printf("%-10s %-8s %-8s %-50s %-20s\n", 
               "----------", "--------", "--------", 
               "--------------------------------------------------", 
               "--------------------");
        
        // Display first 20 windows
        int displayCount = (data.count > 20) ? 20 : data.count;
        for (int i = 0; i < displayCount; i++) {
            WindowInfo* info = &windows[i];
            
            // Only show windows with titles or interesting class names
            if (strlen(info->title) > 0 || strstr(info->className, "Window") != NULL) {
                printf("0x%08p %-8lu %-8s %-50.50s %-20.20s\n",
                       info->hwnd,
                       info->processId,
                       info->isVisible ? "Yes" : "No",
                       strlen(info->title) > 0 ? info->title : "(No Title)",
                       info->className);
            }
        }
        
        if (data.count > 20) {
            printf("\n... and %d more windows\n", data.count - 20);
        }
        
        // Statistics
        int visibleCount = 0;
        int withTitleCount = 0;
        for (int i = 0; i < data.count; i++) {
            if (windows[i].isVisible) visibleCount++;
            if (strlen(windows[i].title) > 0) withTitleCount++;
        }
        
        printf("\nStatistics:\n");
        printf("  Total Windows: %d\n", data.count);
        printf("  Visible Windows: %d\n", visibleCount);
        printf("  Windows with Title: %d\n", withTitleCount);
        printf("  Hidden Windows: %d\n", data.count - visibleCount);
    } else {
        printf("[-] EnumWindows failed or was stopped early\n");
    }
    
    free(windows);
}

// 2. EnumDesktopWindows - Enumerate Windows on Specific Desktop
BOOL CALLBACK EnumDesktopWindowsProc(HWND hwnd, LPARAM lParam) {
    EnumWindowsData* data = (EnumWindowsData*)lParam;
    
    if (data->count >= data->maxCount) {
        return FALSE;
    }
    
    WindowInfo* info = &data->windows[data->count];
    info->hwnd = hwnd;
    
    // Get window information
    GetWindowTextA(hwnd, info->title, sizeof(info->title));
    GetClassNameA(hwnd, info->className, sizeof(info->className));
    GetWindowThreadProcessId(hwnd, &info->processId);
    info->isVisible = IsWindowVisible(hwnd);
    info->isEnabled = IsWindowEnabled(hwnd);
    GetWindowRect(hwnd, &info->rect);
    
    data->count++;
    return TRUE;
}

void DemonstrateEnumDesktopWindows() {
    printf("\nEnumDesktopWindows - Desktop Specific Windows\n");
    printf("================================================\n\n");
    
    // Get current desktop handle
    HDESK hDesktop = GetThreadDesktop(GetCurrentThreadId());
    
    if (!hDesktop) {
        printf("[-] Failed to get current desktop handle (Error: %lu)\n", GetLastError());
        return;
    }
    
    // Get desktop name
    char desktopName[256] = {0};
    DWORD nameLen = sizeof(desktopName);
    GetUserObjectInformationA(hDesktop, UOI_NAME, desktopName, nameLen, &nameLen);
    
    printf("[+] Current Desktop: %s\n", desktopName);
    printf("[+] Desktop Handle: 0x%p\n\n", hDesktop);
    
    const int MAX_WINDOWS = 1000;
    WindowInfo* windows = (WindowInfo*)malloc(MAX_WINDOWS * sizeof(WindowInfo));
    
    if (!windows) {
        printf("[-] Memory allocation failed!\n");
        return;
    }
    
    EnumWindowsData data = {0};
    data.windows = windows;
    data.count = 0;
    data.maxCount = MAX_WINDOWS;
    
    // Enumerate windows on current desktop same as before.
    if (EnumDesktopWindows(hDesktop, EnumDesktopWindowsProc, (LPARAM)&data)) {
        printf("[+] Successfully enumerated %d windows on desktop '%s'\n\n", 
               data.count, desktopName);
        
        printf("%-10s %-8s %-8s %-8s %-40s %-20s\n", 
               "HWND", "PID", "Visible", "Enabled", "Window Title", "Class Name");
        printf("%-10s %-8s %-8s %-8s %-40s %-20s\n", 
               "----------", "--------", "--------", "--------",
               "----------------------------------------", 
               "--------------------");
        
        // Display first 25 windows
        int displayCount = (data.count > 25) ? 25 : data.count;
        for (int i = 0; i < displayCount; i++) {
            WindowInfo* info = &windows[i];
            
            printf("0x%08p %-8lu %-8s %-8s %-40.40s %-20.20s\n",
                   info->hwnd,
                   info->processId,
                   info->isVisible ? "Yes" : "No",
                   info->isEnabled ? "Yes" : "No",
                   strlen(info->title) > 0 ? info->title : "(No Title)",
                   info->className);
        }
        
        if (data.count > 25) {
            printf("\n... and %d more windows\n", data.count - 25);
        }
        
        // Detailed statistics
        int visibleCount = 0, enabledCount = 0, withTitleCount = 0;
        for (int i = 0; i < data.count; i++) {
            if (windows[i].isVisible) visibleCount++;
            if (windows[i].isEnabled) enabledCount++;
            if (strlen(windows[i].title) > 0) withTitleCount++;
        }
        
        printf("\nDesktop Window Statistics:\n");
        printf("  Desktop Name: %s\n", desktopName);
        printf("  Total Windows: %d\n", data.count);
        printf("  Visible: %d (%.1f%%)\n", visibleCount, 
               (data.count > 0) ? (visibleCount * 100.0 / data.count) : 0);
        printf("  Enabled: %d (%.1f%%)\n", enabledCount, 
               (data.count > 0) ? (enabledCount * 100.0 / data.count) : 0);
        printf("  With Title: %d (%.1f%%)\n", withTitleCount, 
               (data.count > 0) ? (withTitleCount * 100.0 / data.count) : 0);
    } else {
        printf("[-] EnumDesktopWindows failed (Error: %lu)\n", GetLastError());
    }
    
    free(windows);
}

// Advanced Window Analysis
void AdvancedWindowAnalysis() 
{
    printf("\nAdvanced Window Analysis\n");
   
    const int MAX_WINDOWS = 1000;
    WindowInfo* windows = (WindowInfo*)malloc(MAX_WINDOWS * sizeof(WindowInfo));
    EnumWindowsData data = {windows, 0, MAX_WINDOWS};
    
    EnumWindows(EnumWindowsProc, (LPARAM)&data);
    
    // Analyze window characteristics
    int overlappedWindows = 0;
    int popupWindows = 0;
    int childWindows = 0;
    
    printf("Window Style Analysis:\n\n");
    printf("%-50s %-15s %-10s\n", "Window Title", "Style Type", "Parent");
    printf("%-50s %-15s %-10s\n", 
           "--------------------------------------------------", 
           "---------------", 
           "----------");
    
    int displayCount = 0;
    for (int i = 0; i < data.count && displayCount < 15; i++) {
        HWND hwnd = windows[i].hwnd;
        LONG style = GetWindowLong(hwnd, GWL_STYLE);
        HWND parent = GetParent(hwnd);
        
        const char* styleType = "Unknown";
        if (style & WS_OVERLAPPEDWINDOW) {
            styleType = "Overlapped";
            overlappedWindows++;
        } else if (style & WS_POPUP) {
            styleType = "Popup";
            popupWindows++;
        } else if (style & WS_CHILD) {
            styleType = "Child";
            childWindows++;
        }
        
        if (strlen(windows[i].title) > 0) {
            printf("%-50.50s %-15s %-10s\n",
                   windows[i].title,
                   styleType,
                   parent ? "Yes" : "No");
            displayCount++;
        }
    }
    
    printf("\nWindow Style Statistics:\n");
    printf("  Overlapped Windows: %d\n", overlappedWindows);
    printf("  Popup Windows: %d\n", popupWindows);
    printf("  Child Windows: %d\n", childWindows);
    
    free(windows);
}

//entry point
int main() 
{
    printf("\nWindows API - Window Enumeration Demonstration\n\n");
    
    // Demonstrate EnumWindows
    DemonstrateEnumWindows();
    
    // Demonstrate EnumDesktopWindows
    DemonstrateEnumDesktopWindows();
    
    // Advanced analysis
    AdvancedWindowAnalysis();
    
    printf("\nWindow Enumeration Complete\n");
    printf("Press Enter to exit...");
    getchar();
    
    return 0;
}