#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <dwmapi.h>

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "dwmapi.lib")

// Define PrintWindow flags if not available in older SDK
#ifndef PW_CLIENTONLY
#define PW_CLIENTONLY 0x00000001
#endif

#ifndef PW_RENDERFULLCONTENT
#define PW_RENDERFULLCONTENT 0x00000002
#endif

// Function to get timestamp for filenames
void GetTimestamp(char* buffer, size_t bufferSize) {
    time_t now = time(NULL);
    struct tm* timeinfo = localtime(&now);
    strftime(buffer, bufferSize, "%Y%m%d_%H%M%S", timeinfo);
}

// Function to save bitmap to file
BOOL SaveBitmapToFile(HBITMAP hBitmap, const char* filename) {
    BITMAP bitmap;
    GetObject(hBitmap, sizeof(BITMAP), &bitmap);
    
    BITMAPFILEHEADER bfHeader;
    BITMAPINFOHEADER biHeader;
    
    biHeader.biSize = sizeof(BITMAPINFOHEADER);
    biHeader.biWidth = bitmap.bmWidth;
    biHeader.biHeight = bitmap.bmHeight;
    biHeader.biPlanes = 1;
    biHeader.biBitCount = 24;
    biHeader.biCompression = BI_RGB;
    biHeader.biSizeImage = 0;
    biHeader.biXPelsPerMeter = 0;
    biHeader.biYPelsPerMeter = 0;
    biHeader.biClrUsed = 0;
    biHeader.biClrImportant = 0;
    
    DWORD dwBmpSize = ((bitmap.bmWidth * biHeader.biBitCount + 31) / 32) * 4 * bitmap.bmHeight;
    
    bfHeader.bfType = 0x4D42;
    bfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bfHeader.bfSize = bfHeader.bfOffBits + dwBmpSize;
    bfHeader.bfReserved1 = 0;
    bfHeader.bfReserved2 = 0;
    
    HANDLE hFile = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 
                              FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("[-] ERROR: Could not create file (0x%08X)\n", GetLastError());
        return FALSE;
    }
    
    DWORD dwWritten;
    WriteFile(hFile, &bfHeader, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
    WriteFile(hFile, &biHeader, sizeof(BITMAPINFOHEADER), &dwWritten, NULL);
    
    HDC hDC = GetDC(NULL);
    char* lpbitmap = (char*)malloc(dwBmpSize);
    GetDIBits(hDC, hBitmap, 0, bitmap.bmHeight, lpbitmap, 
              (BITMAPINFO*)&biHeader, DIB_RGB_COLORS);
    
    WriteFile(hFile, lpbitmap, dwBmpSize, &dwWritten, NULL);
    
    free(lpbitmap);
    ReleaseDC(NULL, hDC);
    CloseHandle(hFile);
    
    return TRUE;
}

// Function to demonstrate GetDC and BitBlt for full screen capture
void CaptureFullScreen() {
    printf("\n[+] FULL SCREEN CAPTURE (GetDC + BitBlt)\n\n");
    
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    
    printf("[+] Screen Resolution: %d x %d pixels\n\n", screenWidth, screenHeight);
    
    printf("[+] GetDC - Getting screen device context...\n");
    HDC hdcScreen = GetDC(NULL);
    if (hdcScreen == NULL) {
        printf("[-]   ERROR: Failed to get screen DC (0x%08X)\n", GetLastError());
        return;
    }
    printf("[+]   SUCCESS: Screen DC obtained (Handle: 0x%p)\n\n", hdcScreen);
    
    printf("[+] Creating compatible DC and bitmap...\n");
    HDC hdcMemory = CreateCompatibleDC(hdcScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, screenWidth, screenHeight);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMemory, hBitmap);
    printf("[+]   SUCCESS: Memory DC and bitmap created\n\n");
    
    printf("[+] BitBlt - Copying screen content...\n");
    printf("[+]   Source: Screen DC (0,0)\n");
    printf("[+]   Destination: Memory DC (0,0)\n");
    printf("[+]   Size: %d x %d\n", screenWidth, screenHeight);
    printf("[+]   Operation: SRCCOPY\n");
    
    BOOL result = BitBlt(hdcMemory, 0, 0, screenWidth, screenHeight, 
                         hdcScreen, 0, 0, SRCCOPY);
    if (result) {
        printf("[+] SUCCESS: Screen captured\n\n");
        
        char filename[256];
        char timestamp[64];
        GetTimestamp(timestamp, sizeof(timestamp));
        sprintf(filename, "Fullscreen_%s.bmp", timestamp);
        
        printf("[+] Saving screenshot...\n");
        if (SaveBitmapToFile(hBitmap, filename)) {
            printf("[+]  SUCCESS: Saved to '%s'\n", filename);
            printf("[+]  File size: ~%d KB\n", (screenWidth * screenHeight * 3) / 1024);
        }
    } else {
        printf("[-]  ERROR: BitBlt failed (0x%08X)\n", GetLastError());
    }
    
    SelectObject(hdcMemory, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcMemory);
    ReleaseDC(NULL, hdcScreen);
    
    printf("\n[+] Cleanup: All resources released\n");
}

// Function to capture window using PrintWindow API (DWM-compatible)
void CaptureActiveWindow() {
    printf("\n\n[+] ACTIVE WINDOW CAPTURE (PrintWindow - DWM Compatible)\n\n");
    
    HWND hwnd = GetForegroundWindow();
    if (hwnd == NULL) {
        printf("[-] ERROR: No active window found\n");
        return;
    }
    
    char windowTitle[256];
    GetWindowText(hwnd, windowTitle, sizeof(windowTitle));
    printf("[+] Target Window: '%s'\n", windowTitle);
    
    RECT rect;
    GetWindowRect(hwnd, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    printf("[+] Window Size: %d x %d pixels\n", width, height);
    printf("[+] Window Position: (%d, %d)\n\n", rect.left, rect.top);
    
    // Use screen DC for compatibility
    HDC hdcScreen = GetDC(NULL);
    HDC hdcMemory = CreateCompatibleDC(hdcScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, width, height);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMemory, hBitmap);
    
    printf("[+] PrintWindow - Capturing window content (DWM-safe)...\n");
    
    // PrintWindow is the recommended method for DWM-enabled systems
    BOOL result = PrintWindow(hwnd, hdcMemory, PW_RENDERFULLCONTENT);
    
    if (result) {
        printf("[+]  SUCCESS: Window captured using PrintWindow\n\n");
        
        char filename[256];
        char timestamp[64];
        GetTimestamp(timestamp, sizeof(timestamp));
        sprintf(filename, "Window_%s.bmp", timestamp);
        
        printf("[+] Saving screenshot...\n");
        if (SaveBitmapToFile(hBitmap, filename)) {
            printf("[+]   SUCCESS: Saved to '%s'\n", filename);
        }
    } else {
        printf("[-]  PrintWindow failed, trying fallback method...\n");
        
        // Fallback: Capture from screen coordinates
        HDC hdcScreen2 = GetDC(NULL);
        BOOL fallbackResult = BitBlt(hdcMemory, 0, 0, width, height, 
                                     hdcScreen2, rect.left, rect.top, SRCCOPY);
        ReleaseDC(NULL, hdcScreen2);
        
        if (fallbackResult) {
            printf("[+]   SUCCESS: Window captured using screen coordinates\n\n");
            
            char filename[256];
            char timestamp[64];
            GetTimestamp(timestamp, sizeof(timestamp));
            sprintf(filename, "Window_%s.bmp", timestamp);
            
            if (SaveBitmapToFile(hBitmap, filename)) {
                printf("[+]   SUCCESS: Saved to '%s'\n", filename);
            }
        } else {
            printf("[-]  ERROR: Both methods failed (0x%08X)\n", GetLastError());
        }
    }
    
    SelectObject(hdcMemory, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcMemory);
    ReleaseDC(NULL, hdcScreen);
    
    printf("\n[+] Cleanup: All resources released\n");
}

//Scaled capture with proper DC handling
void CaptureAndScale() {
    printf("\n\n[+] SCALED CAPTURE (GetDC + StretchBlt)\n\n");
    
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    
    int scaledWidth = screenWidth / 2;
    int scaledHeight = screenHeight / 2;
    
    printf("[+] Original Resolution: %d x %d\n", screenWidth, screenHeight);
    printf("[+] Scaled Resolution: %d x %d (50%%)\n\n", scaledWidth, scaledHeight);
    
    printf("[+] GetDC - Getting screen device context...\n");
    HDC hdcScreen = GetDC(NULL);
    printf("[+]   SUCCESS: Screen DC obtained\n\n");
    
    printf("[+] Creating scaled bitmap...\n");
    HDC hdcMemory = CreateCompatibleDC(hdcScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, scaledWidth, scaledHeight);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMemory, hBitmap);
    printf("[+]  SUCCESS: Scaled bitmap created\n\n");
    
    printf("[+] StretchBlt - Copying and scaling screen content...\n");
    printf("[+]   Source: %d x %d\n", screenWidth, screenHeight);
    printf("[+]   Destination: %d x %d\n", scaledWidth, scaledHeight);
    printf("[+]   Stretch Mode: HALFTONE (high quality)\n");
    
    SetStretchBltMode(hdcMemory, HALFTONE);
    SetBrushOrgEx(hdcMemory, 0, 0, NULL);
    
    BOOL result = StretchBlt(hdcMemory, 0, 0, scaledWidth, scaledHeight,
                             hdcScreen, 0, 0, screenWidth, screenHeight,
                             SRCCOPY);
    
    if (result) {
        printf("[+]   SUCCESS: Screen captured and scaled\n\n");
        
        char filename[256];
        char timestamp[64];
        GetTimestamp(timestamp, sizeof(timestamp));
        sprintf(filename, "Scaled_%s.bmp", timestamp);
        
        printf("[+] Saving scaled screenshot...\n");
        if (SaveBitmapToFile(hBitmap, filename)) {
            printf("[+]  SUCCESS: Saved to '%s'\n", filename);
            printf("[+]   File size: ~%d KB (vs ~%d KB original)\n", 
                   (scaledWidth * scaledHeight * 3) / 1024,
                   (screenWidth * screenHeight * 3) / 1024);
        }
    } else {
        printf("[+]  ERROR: StretchBlt failed (0x%08X)\n", GetLastError());
    }
    
    SelectObject(hdcMemory, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcMemory);
    ReleaseDC(NULL, hdcScreen);
    
    printf("\n   Cleanup: All resources released\n");
}

// Advanced capture using screen coordinates
void CaptureWithGetDCEx() {
    printf("\n\n[+] ADVANCED CAPTURE (Screen Region Method)\n\n");
    
    HWND hwnd = GetForegroundWindow();
    if (hwnd == NULL) {
        printf("[-] ERROR: No active window found.\n");
        return;
    }
    
    char windowTitle[256];
    GetWindowText(hwnd, windowTitle, sizeof(windowTitle));
    printf("[+] Target Window: '%s'\n\n", windowTitle);
    
    RECT rect;
    GetWindowRect(hwnd, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    
    printf("[+] Capturing window region from screen...\n");
    HDC hdcScreen = GetDC(NULL);
    HDC hdcMemory = CreateCompatibleDC(hdcScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, width, height);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMemory, hBitmap);
    
    printf("[+] BitBlt - Capturing from screen coordinates...\n");
    if (BitBlt(hdcMemory, 0, 0, width, height, hdcScreen, rect.left, rect.top, SRCCOPY)) {
        printf("[+]   SUCCESS: Window region captured\n\n");
        
        char filename[256];
        char timestamp[64];
        GetTimestamp(timestamp, sizeof(timestamp));
        sprintf(filename, "Advanced_%s.bmp", timestamp);
        
        if (SaveBitmapToFile(hBitmap, filename)) {
            printf("[+] Screenshot saved to '%s'\n", filename);
        }
    } else {
        printf("[-]   ERROR: Capture failed (0x%08X)\n", GetLastError());
    }
    
    SelectObject(hdcMemory, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcMemory);
    ReleaseDC(NULL, hdcScreen);
    
    printf("\n[+]  Cleanup: All resources released\n");
}

//BitBlt operations with proper screen DC usage
void DemonstrateBitBltOperations() {
    printf("\n\n[+] BitBlt RASTER OPERATIONS\n\n");
    
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    
    int captureWidth = screenWidth / 4;
    int captureHeight = screenHeight / 4;
    
    HDC hdcScreen = GetDC(NULL);
    
    struct {
        DWORD rop;
        const char* name;
        const char* description;
    } operations[] = {
        {SRCCOPY, "SRCCOPY", "Direct copy"},
        {NOTSRCCOPY, "NOTSRCCOPY", "Inverted copy"},
        {SRCAND, "SRCAND", "Source AND destination"},
        {SRCINVERT, "SRCINVERT", "Source XOR destination"},
        {BLACKNESS, "BLACKNESS", "Fill with black"},
        {WHITENESS, "WHITENESS", "Fill with white"}
    };
    
    int opCount = sizeof(operations) / sizeof(operations[0]);
    
    printf("[+] Demonstrating %d raster operations...\n", opCount);
    printf("[+] Capture area: %d x %d (top-left corner)\n\n", captureWidth, captureHeight);
    
    for (int i = 0; i < opCount; i++) {
        printf("%d. %s - %s\n", i + 1, operations[i].name, operations[i].description);
        
        HDC hdcMemory = CreateCompatibleDC(hdcScreen);
        HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, captureWidth, captureHeight);
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMemory, hBitmap);
        
        BitBlt(hdcMemory, 0, 0, captureWidth, captureHeight,
               hdcScreen, 0, 0, operations[i].rop);
        
        char filename[256];
        char timestamp[64];
        GetTimestamp(timestamp, sizeof(timestamp));
        sprintf(filename, "Bitblt_%s_%s.bmp", operations[i].name, timestamp);
        
        if (SaveBitmapToFile(hBitmap, filename)) {
            printf("[+]   Saved to '%s'\n", filename);
        }
        
        SelectObject(hdcMemory, hOldBitmap);
        DeleteObject(hBitmap);
        DeleteDC(hdcMemory);
    }
    
    ReleaseDC(NULL, hdcScreen);
    printf("\n[+]  All operations completed\n");
}

void ShowMenu() 
{
    printf("\n [+] Windows Screen Capture / Visual \n\n\n");
    printf("[++]Select a demonstration:\n");
    printf("  [1] Full Screen Capture (GetDC + BitBlt)\n");
    printf("  [2] Active Window Capture (PrintWindow - DWM Fixed)\n");
    printf("  [3] Scaled Capture (GetDC + StretchBlt)\n");
    printf("  [4] Advanced Capture (Screen Region)\n");
    printf("  [5] BitBlt Raster Operations Demo\n");
    printf("  [6] Run All Demonstrations\n");
    printf("  [7] Exit\n");
    printf("\nChoice: ");
}

int main() {
    int choice;
    
    printf("[+] All screenshots will be saved in the current directory.\n");
    printf("[+] Make sure you have some windows open for better demonstration!\n");
    
    while (1) {
        ShowMenu();
        
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("\nInvalid input!\n");
            Sleep(1000);
            continue;
        }
        while (getchar() != '\n');
        
        switch (choice) {
            case 1:
                CaptureFullScreen();
                break;
            case 2:
                CaptureActiveWindow();
                break;
            case 3:
                CaptureAndScale();
                break;
            case 4:
                CaptureWithGetDCEx();
                break;
            case 5:
                DemonstrateBitBltOperations();
                break;
            case 6:
                printf("\n[+] Running All Demonstrations.\n");
                CaptureFullScreen();
                Sleep(500);
                CaptureActiveWindow();
                Sleep(500);
                CaptureAndScale();
                Sleep(500);
                CaptureWithGetDCEx();
                Sleep(500);
                DemonstrateBitBltOperations();
                printf("\n[+] All Demonstrations Complete.\n");
                break;
            case 7:
                printf("\n[+] Exiting...\n");
                return 0;
            default:
                printf("\n[-] Invalid choice!\n");
                Sleep(1000);
                continue;
        }
        
        printf("\n\nPress Enter to continue...");
        getchar();
    }
    
    return 0;
}