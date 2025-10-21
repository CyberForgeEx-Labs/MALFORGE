#include <windows.h>
#include <stdio.h>
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

// function prototypes
void FindFirstFile_FindNextFile();
void GetWindowsDirectory();
void GetSystemDirectory();
void PathFileExists();
void GetLogicalDrivess();

int main() 
{

    printf("File System Enumeration API Usage\n");

    FindFirstFile_FindNextFile();
    GetWindowsDirectory();
    GetSystemDirectory();
    PathFileExists();
    GetLogicalDrivess();

    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}

// FindFirstFileA + FindNextFileA
// Purpose: List all files in a directory
void FindFirstFile_FindNextFile() {
    printf("\nFindFirstFileA + FindNextFileA \n\n");
    
    WIN32_FIND_DATAA findData;
    HANDLE hFind;
    int count = 0;
    
    // Search for all files in C:\Windows (default)
    printf("Listing files in C:\\Windows (first 10 items):\n\n");
    
    hFind = FindFirstFileA("C:\\Windows\\*", &findData);
    
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("ERROR: FindFirstFileA failed!\n");
        return;
    }
    
    // Print header
    printf("%-30s %10s\n", "Name", "Type");
    
    // Loop through all files
    do {
        if (count >= 10) break;  // Only show first 10
        
        // Check if it's a directory or file
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
        {
            printf("%-30s %10s\n", findData.cFileName, "[DIR]");
        } 
        else 
        {
            printf("%-30s %10s\n", findData.cFileName, "[FILE]");
        }
        count++;
        
    } while (FindNextFileA(hFind, &findData));
    
    FindClose(hFind);
    
    printf("\nListed %d items\n", count);
}

// GetWindowsDirectoryA
// Purpose: Get the Windows installation directory path
void GetWindowsDirectory() {
    printf("\nGetWindowsDirectoryA \n\n");
    
    char windowsDir[MAX_PATH];
    
    // Get Windows directory
    UINT length = GetWindowsDirectoryA(windowsDir, MAX_PATH);
    
    if (length == 0) {
        printf("ERROR: GetWindowsDirectoryA failed!\n");
        return;
    }
    
    printf("Windows Directory: %s\n", windowsDir);
    printf("Path Length: %u characters\n", length);
}

// GetSystemDirectoryA
// Purpose: Get the System32 directory path
void GetSystemDirectory() {
    printf("\nGetSystemDirectoryA \n\n");
    
    char systemDir[MAX_PATH];
    
    // Get System32 directory
    UINT length = GetSystemDirectoryA(systemDir, MAX_PATH);
    
    if (length == 0) {
        printf("ERROR: GetSystemDirectoryA failed!\n");
        return;
    }
    
    printf("System Directory: %s\n", systemDir);
    printf("Path Length: %u characters\n", length);
}

// PathFileExistsA
// Purpose: Check if files or directories exist
void PathFileExists() {
    printf("\nPathFileExistsA \n\n");
    
    // Array of paths to test
    const char* testPaths[] = {
        "C:\\Windows\\System32\\notepad.exe",
        "C:\\Windows\\System32\\kernel32.dll",
        "C:\\Windows",
        "C:\\Program Files"
    };
    
    printf("%-45s %s\n", "Path", "Exists?");
   
    // Check each path
    for (int i = 0; i < 4; i++) {
        BOOL exists = PathFileExistsA(testPaths[i]);
        printf("%-45s %s\n", testPaths[i], exists ? "[YES]" : "[NO]");
    }
}

// GetLogicalDrives
// Purpose: List all available drives (A:, C:, D:, etc.)
void GetLogicalDrivess() {
    printf("\nGetLogicalDrives\n\n");
    
    // Get bitmask of all logical drives
    DWORD drivesMask = GetLogicalDrives();
    
    if (drivesMask == 0) {
        printf("ERROR: GetLogicalDrives failed!\n");
        return;
    }
    
    printf("Drive Bitmask: 0x%08X\n\n", drivesMask);
    printf("%-10s %-15s\n", "Drive", "Type");
    printf("-------------------------\n");
    
    // Check each possible drive letter (A to Z)
    for (int i = 0; i < 26; i++) {
        // Check if this drive exists (bit is set)
        if (drivesMask & (1 << i)) {
            char driveLetter[4];
            sprintf(driveLetter, "%c:\\", 'A' + i);
            
            // Get drive type
            UINT driveType = GetDriveTypeA(driveLetter);
            const char* typeStr = "Unknown";
            
            if (driveType == DRIVE_FIXED)        typeStr = "Hard Disk";
            else if (driveType == DRIVE_REMOVABLE)  typeStr = "Removable";
            else if (driveType == DRIVE_REMOTE)     typeStr = "Network";
            else if (driveType == DRIVE_CDROM)      typeStr = "CD-ROM";
            else if (driveType == DRIVE_RAMDISK)    typeStr = "RAM Disk";
            
            printf("%-10s %-15s\n", driveLetter, typeStr);
        }
    }
}