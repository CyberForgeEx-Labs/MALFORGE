#include <windows.h>
#include <wincrypt.h>
#include <stdio.h>
#include <string.h>

#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "rpcrt4.lib")

// Function to create a test file with sample content.
BOOL CreateTestFile(const char* filename, const char* content) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("[-] Error: Could not create file %s\n", filename);
        return FALSE;
    }
    
    fprintf(file, "%s", content);
    fclose(file);
    
    printf("[+] Test file created: %s\n", filename);
    return TRUE;
}

// Function to read file content and display it.
void DisplayFileContent(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("[-] Error: Could not open file %s\n", filename);
        return;
    }
    
    char buffer[512];
    printf("File content: ");
    if (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("%s\n", buffer);
    }
    fclose(file);
}

// Function to get file encryption status.
void GetFileEncryptionStatus(const char* filename) {
    DWORD status;
    
    if (FileEncryptionStatusA(filename, &status)) {
        printf("[+] File encryption status for %s:\n", filename);
        
        if (status & FILE_ENCRYPTABLE) {
            printf("  - File is encryptable\n");
        } else {
            printf("  - File is NOT encryptable\n");
        }
        
        if (status & FILE_READ_ONLY) {
            printf("  - File is read-only\n");
        }
        
        if (status & FILE_DIR_DISALLOWED) {
            printf("  - Directory encryption is disallowed\n");
        }
    } else {
        printf("[-] Error getting encryption status: %ld\n", GetLastError());
    }
}

// Encrypt File
BOOL EncryptFileExample(const char* filename) {
    printf("\n[+] ENCRYPT FILE EXAMPLE \n");
    printf("Target file: %s\n", filename);
    
    // Check if file is encryptable
    GetFileEncryptionStatus(filename);
    
    printf("\n[+] Attempting to encrypt file...\n");
    
    // Try to encrypt the file
    if (EncryptFileA(filename)) {
        printf("[+] SUCCESS: File encrypted successfully!\n");
        return TRUE;
    } else {
        DWORD error = GetLastError();
        printf("[-] Error encrypting file: %ld (0x%X)\n", error, error);
        
        // Display error interpretation
        switch (error) {
            case ERROR_NOT_SUPPORTED:
                printf("  - EFS not supported on this system or drive\n");
                printf("  - Ensure file is on NTFS partition\n");
                break;
            case ERROR_FILE_NOT_FOUND:
                printf("  - File not found\n");
                break;
            case ERROR_SHARING_VIOLATION:
                printf("  - File is in use by another process\n");
                break;
            case ERROR_ACCESS_DENIED:
                printf("  - Access denied. Need administrator privileges or proper EFS rights\n");
                break;
            default:
                printf("  - Unknown error occurred\n");
        }
        return FALSE;
    }
}

// Decrypt File
BOOL DecryptFileExample(const char* filename) {
    printf("\n[+] DECRYPT FILE EXAMPLE\n");
    printf(" >  Target file: %s\n", filename);
    
    printf("[+] Attempting to decrypt file...\n");
    
    // Try to decrypt the file
    if (DecryptFileA(filename, 0)) {
        printf("[+] SUCCESS: File decrypted successfully!\n");
        printf("[+] File encryption removed\n");
        return TRUE;
    } else {
        DWORD error = GetLastError();
        printf("[+] Error decrypting file: %ld (0x%X)\n", error, error);
        
        // Display error interpretation
        switch (error) {
            case ERROR_NOT_SUPPORTED:
                printf("  - File is not encrypted or EFS not supported\n");
                break;
            case ERROR_FILE_NOT_FOUND:
                printf("  - File not found\n");
                break;
            case ERROR_SHARING_VIOLATION:
                printf("  - File is in use by another process\n");
                break;
            case ERROR_ACCESS_DENIED:
                printf("  - Access denied. You may not have decryption rights\n");
                break;
            default:
                printf("  - Unknown error occurred\n");
        }
        return FALSE;
    }
}

// Function to demonstrate file operations
void DemonstrationWithFileOps(const char* filename) {
    printf("\n[+] COMPLETE WORKFLOW \n");
    
    // Create a test file
    printf("\n[Step 1] Creating test file...\n");
    if (!CreateTestFile(filename, "Confidential Data - This file will be encrypted")) {
        printf("[-] Failed to create test file\n");
        return;
    }
    
    printf("\n[Step 2] Displaying original content...\n");
    DisplayFileContent(filename);
    
    printf("\n[Step 3] Checking file encryption status...\n");
    GetFileEncryptionStatus(filename);
    
    printf("\n[Step 4] Attempting to encrypt file...\n");
    BOOL encrypted = EncryptFileExample(filename);
    
    if (encrypted) {
        printf("\n[Step 5] File is now encrypted\n");
        printf("  - Content cannot be read without proper EFS credentials\n");
        
        printf("\n[Step 6] Attempting to decrypt file...\n");
        if (DecryptFileExample(filename)) {
            printf("\n[Step 7] File decrypted successfully\n");
            DisplayFileContent(filename);
        }
    } else {
        printf("\n[Step 5] Encryption failed - skipping decrypt step\n");
    }
    
}


int main()
{
    printf("\n[+] File Encryption APIs Demonstration.\n");
    
    const char* testFilename = "encrypted_test.txt";
    
    // Run the complete workflow
    DemonstrationWithFileOps(testFilename);
    
    
    // Cleanup
    printf("\n[+] Attempting to delete test file...\n");
    if (remove(testFilename) == 0) {
        printf("[+] Test file deleted successfully\n");
    } else {
        printf("[-] Could not delete test file (may still be in use)\n");

    }
        
    printf("[+] DEMONSTRATION COMPLETE\n");

    return 0;
}