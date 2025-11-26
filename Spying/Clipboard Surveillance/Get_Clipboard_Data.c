#include <windows.h>
#include <stdio.h>
#include <time.h>

#define WM_CLIPBOARDUPDATE 0x031D

// Global variables
HWND g_hwndNextViewer = NULL;
HWND g_hwnd = NULL;
int g_clipboardChangeCount = 0;

// Function to get current timestamp
void GetTimestamp(char* buffer, size_t bufferSize) {
    time_t now = time(NULL);
    struct tm* timeinfo = localtime(&now);
    strftime(buffer, bufferSize, "%Y-%m-%d %H:%M:%S", timeinfo);
}

// Function to safely get text from clipboard
BOOL GetClipboardText(char* buffer, size_t bufferSize) {
    HANDLE hData;
    char* pszText;
    BOOL success = FALSE;
    
    if (!OpenClipboard(NULL)) {
        printf("[-] ERROR: Could not open clipboard (0x%08X)\n", GetLastError());
        return FALSE;
    }
    
    hData = GetClipboardData(CF_TEXT);
    if (hData != NULL) {
        pszText = (char*)GlobalLock(hData);
        if (pszText != NULL) {
            strncpy(buffer, pszText, bufferSize - 1);
            buffer[bufferSize - 1] = '\0';
            GlobalUnlock(hData);
            success = TRUE;
        }
    }
    
    CloseClipboard();
    return success;
}

// Function to get Unicode text from clipboard
BOOL GetClipboardUnicodeText(wchar_t* buffer, size_t bufferSize) {
    HANDLE hData;
    wchar_t* pszText;
    BOOL success = FALSE;
    
    if (!OpenClipboard(NULL)) {
        return FALSE;
    }
    
    hData = GetClipboardData(CF_UNICODETEXT);
    if (hData != NULL) {
        pszText = (wchar_t*)GlobalLock(hData);
        if (pszText != NULL) {
            wcsncpy(buffer, pszText, bufferSize - 1);
            buffer[bufferSize - 1] = L'\0';
            GlobalUnlock(hData);
            success = TRUE;
        }
    }
    
    CloseClipboard();
    return success;
}

// Function to display all available clipboard formats
void DisplayClipboardFormats() {
    printf("\n[+] Available Clipboard Formats.\n");
    
    if (!OpenClipboard(NULL)) {
        printf("[-] ERROR: Could not open clipboard\n");
        return;
    }
    
    UINT format = 0;
    int formatCount = 0;
    
    while ((format = EnumClipboardFormats(format)) != 0) {
        formatCount++;
        char formatName[256] = {0};
        
        // Get format name
        if (GetClipboardFormatName(format, formatName, sizeof(formatName)) == 0) {
            // Standard format - use predefined names
            switch (format) {
                case CF_TEXT: strcpy(formatName, "CF_TEXT"); break;
                case CF_BITMAP: strcpy(formatName, "CF_BITMAP"); break;
                case CF_METAFILEPICT: strcpy(formatName, "CF_METAFILEPICT"); break;
                case CF_SYLK: strcpy(formatName, "CF_SYLK"); break;
                case CF_DIF: strcpy(formatName, "CF_DIF"); break;
                case CF_TIFF: strcpy(formatName, "CF_TIFF"); break;
                case CF_OEMTEXT: strcpy(formatName, "CF_OEMTEXT"); break;
                case CF_DIB: strcpy(formatName, "CF_DIB"); break;
                case CF_PALETTE: strcpy(formatName, "CF_PALETTE"); break;
                case CF_PENDATA: strcpy(formatName, "CF_PENDATA"); break;
                case CF_RIFF: strcpy(formatName, "CF_RIFF"); break;
                case CF_WAVE: strcpy(formatName, "CF_WAVE"); break;
                case CF_UNICODETEXT: strcpy(formatName, "CF_UNICODETEXT"); break;
                case CF_ENHMETAFILE: strcpy(formatName, "CF_ENHMETAFILE"); break;
                case CF_HDROP: strcpy(formatName, "CF_HDROP"); break;
                case CF_LOCALE: strcpy(formatName, "CF_LOCALE"); break;
                case CF_DIBV5: strcpy(formatName, "CF_DIBV5"); break;
                default: sprintf(formatName, "Unknown (0x%04X)", format);
            }
        }
        
        printf("  [%d] Format ID: 0x%04X - %s\n", formatCount, format, formatName);
    }
    
    if (formatCount == 0) {
        printf("[-] (Clipboard is empty)\n");
    }
    
    CloseClipboard();
}

// Function to get clipboard content details.
void GetClipboardDetails() {
    char timestamp[64];
    GetTimestamp(timestamp, sizeof(timestamp));
    
    printf("[+] CLIPBOARD CHANGE DETECTED [#%d] - %s\n\n", ++g_clipboardChangeCount, timestamp);
    
    // Display available formats
    DisplayClipboardFormats();
    
    // Try to get text content
    char textBuffer[4096] = {0};
    wchar_t unicodeBuffer[4096] = {0};
    
    printf("\n[+] Clipboard Content.\n");
    
    // Try CF_UNICODETEXT first
    if (GetClipboardUnicodeText(unicodeBuffer, sizeof(unicodeBuffer) / sizeof(wchar_t))) {
        printf("[+] Type: Unicode Text\n");
        printf("[+] Content: %ls\n", unicodeBuffer);
        printf("[+] Length: %d characters\n", (int)wcslen(unicodeBuffer));
    }
    // Try CF_TEXT
    else if (GetClipboardText(textBuffer, sizeof(textBuffer))) {
        printf("[+] Type: ANSI Text\n");
        printf("[+] Content: %s\n", textBuffer);
        printf("[+] Length: %d characters\n", (int)strlen(textBuffer));
    }
    else {
        printf("[+] Type: Non-text data (binary/image/file)\n");
        
        // Check if it's a bitmap
        if (OpenClipboard(NULL)) {
            if (GetClipboardData(CF_BITMAP) != NULL) {
                printf("[+] Content: Bitmap image detected\n");
            }
            else if (GetClipboardData(CF_HDROP) != NULL) {
                printf("[+] Content: File(s) detected\n");
            }
            else {
                printf("[-] Content: Unknown binary data\n");
            }
            CloseClipboard();
        }
    }
    
    printf("\n");
}

// Window procedure to handle clipboard messages.
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
    switch (uMsg) {
        case WM_CREATE:
            // Register for clipboard format change notifications.
            if (!AddClipboardFormatListener(hwnd)) 
            {
                printf("[-] ERROR: Failed to register clipboard listener (0x%08X)\n", GetLastError());
            } else 
            {
                printf("[+] SUCCESS: Clipboard listener registered\n");
            }
            break;
            
        case WM_CLIPBOARDUPDATE:
            // Clipboard content has changed
            GetClipboardDetails();
            break;
            
        case WM_DESTROY:
            RemoveClipboardFormatListener(hwnd);
            PostQuitMessage(0);
            break;
            
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

// Function to demonstrate one-time clipboard reading
void DemonstrateClipboardReading() {
    printf("\n[+] One-Time Clipboard Content Reading Demonstration.\n");
    
    GetClipboardDetails();
}

// Function to start clipboard monitoring
void StartClipboardMonitoring() {
    printf("\n[+] Real-Time Clipboard Monitoring Active\n");
    printf("\n[+] Monitoring clipboard changes... (Press Ctrl+C to stop)\n");
    printf("[+] Copy any text, image, or file to see the clipboard content.\n");
    
    // Register window class
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "ClipboardMonitor";
    
    if (!RegisterClass(&wc)) {
        printf("[+] ERROR: Failed to register window class (0x%08X)\n", GetLastError());
        return;
    }
    
    // Create invisible window for receiving messages
    g_hwnd = CreateWindowEx(
        0,
        "ClipboardMonitor",
        "Clipboard Monitor",
        0,
        0, 0, 0, 0,
        HWND_MESSAGE,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );
    
    if (g_hwnd == NULL) {
        printf("[-] ERROR: Failed to create window (0x%08X)\n", GetLastError());
        return;
    }
    
    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    UnregisterClass("ClipboardMonitor", GetModuleHandle(NULL));
}

// Main menu
void ShowMenu() {
    printf("\n [+] Windows Clipboard Surveillance Demonstration.\n");
    printf("\n[+] Select an option:\n");
    printf("   [1] Read current clipboard content (one-time)\n");
    printf("   [2] Start real-time clipboard monitoring\n");
    printf("   [3] Exit\n");
    printf("\n[+] Choice: ");
}

int main() {
    int choice;
    
    while (1) {
        ShowMenu();
        
        if (scanf("%d", &choice) != 1) {
            // Clear input buffer
            while (getchar() != '\n');
            printf("\n[-] Invalid input! Please enter a number.\n");
            Sleep(1000);
            continue;
        }
        
        // Clear input buffer
        while (getchar() != '\n');
        
        switch (choice) {
            case 1:
                DemonstrateClipboardReading();
                printf("\nPress Enter to continue...");
                getchar();
                break;
                
            case 2:
                StartClipboardMonitoring();
                break;
                
            case 3:
                printf("\n[+] Exiting...\n");
                return 0;
                
            default:
                printf("\n[-] Invalid choice! Please select 1, 2, or 3.\n");
                Sleep(1000);
        }
    }
    
    return 0;
}