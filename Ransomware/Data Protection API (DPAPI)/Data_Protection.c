#include <windows.h>
#include <wincrypt.h>
#include <stdio.h>
#include <string.h>

#pragma comment(lib, "crypt32.lib")

// Helper function to print bytes in hexadecimal
void PrintHex(const char *label, PBYTE pbData, DWORD dwDataLen) {
    printf("%s", label);
    for (DWORD i = 0; i < dwDataLen; i++) {
        printf("%02X ", pbData[i]);
        if ((i + 1) % 16 == 0) printf("\n");
    }
    if (dwDataLen % 16 != 0) printf("\n");
}

// Basic CryptProtectData usage.
void Demo_BasicProtection() {
    printf("\n[+] Basic Data Protection implementation.\n");

    // Data to protect
    char szPassword[] = "1MySecretPassword123!";
    DATA_BLOB dataIn;
    DATA_BLOB dataOut;
    
    dataIn.pbData = (BYTE *)szPassword;
    dataIn.cbData = strlen(szPassword) + 1; // include null terminator.
    
    printf("[+] Original data: %s\n", szPassword);
    printf("[+] Data length: %ld bytes\n\n", dataIn.cbData);
    
    // Protect the data
    printf("[*] Encrypting data with CryptProtectData...\n");
    if (!CryptProtectData(
        &dataIn,              // Data to protect
        L"My Secret Data",    // Description (optional)
        NULL,                 // Optional entropy (additional password)
        NULL,                 // Reserved
        NULL,                 // Prompt struct (for UI)
        0,                    // Flags
        &dataOut              // Output encrypted data
    )) {
        printf("[-] CryptProtectData failed: %ld\n", GetLastError());
        return;
    }
    
    printf("[+] Data encrypted successfully!\n");
    printf("[+] Encrypted data length: %ld bytes\n", dataOut.cbData);
    printf("[+] Encrypted data (first 64 bytes):\n");
    PrintHex("    ", dataOut.pbData, min(64, dataOut.cbData));
    printf("\n");
    
    // Unprotect the data.
    DATA_BLOB dataDecrypted;
    LPWSTR pDescription = NULL;
    
    printf("[*] Decrypting data with CryptUnprotectData...\n");
    if (!CryptUnprotectData(
        &dataOut,             // Encrypted data.
        &pDescription,        // Description (retrieved)
        NULL,                 // Optional entropy.
        NULL,                 // Reserved.
        NULL,                 // Prompt struct.
        0,                    // Flags.
        &dataDecrypted        // Decrypted output.
    )) {
        printf("[-] CryptUnprotectData failed: %ld\n", GetLastError());
        LocalFree(dataOut.pbData);
        return;
    }
    
    printf("[+] Data decrypted successfully!\n");
    printf("[+] Description: %S\n", pDescription);
    printf("[+] Decrypted data: %s\n", (char *)dataDecrypted.pbData);
    printf("[+] Match: %s\n\n", strcmp((char *)dataDecrypted.pbData, szPassword) == 0 ? "YES" : "NO");
    
    // Cleanup
    LocalFree(dataOut.pbData);
    LocalFree(dataDecrypted.pbData);
    LocalFree(pDescription);
}

// Using Entropy for Additional Security
void Demo_WithEntropy() {
    printf("\n[+] Protection with Entropy\n");
    
    
    // Data to protect
    char szCreditCard[] = "4532-1234-5678-9010";
    DATA_BLOB dataIn;
    DATA_BLOB dataOut;
    DATA_BLOB entropy;
    
    // Additional entropy (like a salt or additional password)
    BYTE entropyData[] = "Secret2024";
    entropy.pbData = entropyData;
    entropy.cbData = sizeof(entropyData);
    
    dataIn.pbData = (BYTE *)szCreditCard;
    dataIn.cbData = strlen(szCreditCard) + 1;
    
    printf("[+] Original data: %s\n", szCreditCard);
    printf("[+] Using entropy: %s\n\n", entropyData);
    
    // Protect with entropy
    printf("[*] Encrypting with entropy...\n");
    if (!CryptProtectData(
        &dataIn,
        L"Credit Card Number",
        &entropy,              // Additional entropy here.
        NULL,
        NULL,
        0,
        &dataOut
    )) {
        printf("[-] CryptProtectData failed: %ld\n", GetLastError());
        return;
    }
    
    printf("[+] Data encrypted with entropy!\n");
    printf("[+] Encrypted length: %ld bytes\n\n", dataOut.cbData);
    
    // Try to decrypt WITHOUT entropy (should fail)
    printf("[*] Attempting to decrypt WITHOUT entropy...\n");
    DATA_BLOB dataDecrypted;
    if (!CryptUnprotectData(
        &dataOut,
        NULL,
        NULL,                  // No entropy provided
        NULL,
        NULL,
        0,
        &dataDecrypted
    )) {
        printf("[+] Decryption failed as expected: %ld\n", GetLastError());
        printf("[+] This proves entropy is required!\n\n");
    } else {
        printf("[-] Unexpected: Decryption succeeded without entropy\n\n");
        LocalFree(dataDecrypted.pbData);
    }
    
    // Decrypt WITH correct entropy.
    printf("[*] Decrypting WITH correct entropy...\n");
    if (!CryptUnprotectData(
        &dataOut,
        NULL,
        &entropy,              // Correct entropy provided.
        NULL,
        NULL,
        0,
        &dataDecrypted
    )) {
        printf("[-] CryptUnprotectData failed: %ld\n", GetLastError());
        LocalFree(dataOut.pbData);
        return;
    }
    
    printf("[+] Decryption successful!\n");
    printf("[+] Decrypted data: %s\n", (char *)dataDecrypted.pbData);
    printf("[+] Match: %s\n\n", strcmp((char *)dataDecrypted.pbData, szCreditCard) == 0 ? "YES" : "NO");
    
    // Cleanup
    LocalFree(dataOut.pbData);
    LocalFree(dataDecrypted.pbData);
}

// Machine-Specific vs User-Specific Protection.
void Demo_ProtectionScope() {
    printf("\n[+] Protection Scope\n");
    
    char szData[] = "Sensitive Application Data";
    DATA_BLOB dataIn;
    DATA_BLOB dataOut_User;
    DATA_BLOB dataOut_Machine;
    
    dataIn.pbData = (BYTE *)szData;
    dataIn.cbData = strlen(szData) + 1;
    
    printf("[+] Original data: %s\n\n", szData);
    
    // Protect for current user only (default)
    printf("[*] Protecting for CURRENT USER only...\n");
    if (!CryptProtectData(
        &dataIn,
        L"User-Specific Data",
        NULL,
        NULL,
        NULL,
        0,                     // Default: current user only
        &dataOut_User
    )) {
        printf("[-] User-specific protection failed: %ld\n", GetLastError());
        return;
    }
    printf("[+] Protected for current user\n");
    printf("[+] Encrypted length: %ld bytes\n\n", dataOut_User.cbData);
    
    // Protect for local machine (all users)
    printf("[*] Protecting for LOCAL MACHINE (all users)...\n");
    if (!CryptProtectData(
        &dataIn,
        L"Machine-Specific Data",
        NULL,
        NULL,
        NULL,
        CRYPTPROTECT_LOCAL_MACHINE,  // Any user on this machine with valid creds
        &dataOut_Machine
    )) {
        printf("[-] Machine-specific protection failed: %ld\n", GetLastError());
        LocalFree(dataOut_User.pbData);
        return;
    }

    printf("[+] Protected for all users on this machine\n");
    printf("[+] Encrypted length: %ld bytes\n\n", dataOut_Machine.cbData);
    
    // Decrypt both.
    DATA_BLOB dataDecrypted;
    
    printf("[*] Decrypting user-specific data...\n");
    if (!CryptUnprotectData(&dataOut_User, NULL, NULL, NULL, NULL, 0, &dataDecrypted)) {
        printf("[-] Failed: %ld\n", GetLastError());
    } else {
        printf("[+] Success: %s\n\n", (char *)dataDecrypted.pbData);
        LocalFree(dataDecrypted.pbData);
    }
    
    printf("[*] Decrypting machine-specific data...\n");
    if (!CryptUnprotectData(&dataOut_Machine, NULL, NULL, NULL, NULL, 0, &dataDecrypted)) {
        printf("[-] Failed: %ld\n", GetLastError());
    } else {
        printf("[+] Success: %s\n\n", (char *)dataDecrypted.pbData);
        LocalFree(dataDecrypted.pbData);
    }
    
    // Cleanup
    LocalFree(dataOut_User.pbData);
    LocalFree(dataOut_Machine.pbData);
}

// Protecting Binary Data
void Demo_BinaryData() {
    printf("\n[+] Protecting Binary Data\n");
        
    // Binary data (simulating encryption key)
    BYTE binaryKey[32] = {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
        0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10,
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
        0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00
    };
    
    DATA_BLOB dataIn;
    DATA_BLOB dataOut;
    
    dataIn.pbData = binaryKey;
    dataIn.cbData = sizeof(binaryKey);
    
    printf("[+] Original binary data (32 bytes):\n");
    PrintHex("    ", binaryKey, sizeof(binaryKey));
    printf("\n");
    
    // Protect binary data
    printf("[*] Encrypting binary data...\n");
    if (!CryptProtectData(
        &dataIn,
        L"Encryption Key",
        NULL,
        NULL,
        NULL,
        0,
        &dataOut
    )) {
        printf("[-] CryptProtectData failed: %ld\n", GetLastError());
        return;
    }
    
    printf("[+] Binary data encrypted!\n");
    printf("[+] Encrypted length: %ld bytes\n\n", dataOut.cbData);
    
    // Decrypt binary data
    DATA_BLOB dataDecrypted;
    printf("[*] Decrypting binary data...\n");
    if (!CryptUnprotectData(
        &dataOut,
        NULL,
        NULL,
        NULL,
        NULL,
        0,
        &dataDecrypted
    )) {
        printf("[-] CryptUnprotectData failed: %ld\n", GetLastError());
        LocalFree(dataOut.pbData);
        return;
    }
    
    printf("[+] Binary data decrypted!\n");
    printf("[+] Decrypted binary data:\n");
    PrintHex("    ", dataDecrypted.pbData, dataDecrypted.cbData);
    
    // Verify integrity
    printf("\n[+] Data integrity check: %s\n\n",
           (memcmp(binaryKey, dataDecrypted.pbData, sizeof(binaryKey)) == 0) ? "PASSED" : "FAILED");
    
    // Cleanup
    LocalFree(dataOut.pbData);
    LocalFree(dataDecrypted.pbData);
}

// Audit and UI Flags demonstration.
void Demo_AdvancedFlags() {
    printf("\n[+] Advanced Flags\n");
    
    char szData[] = "Data with audit flag";
    DATA_BLOB dataIn;
    DATA_BLOB dataOut;
    
    dataIn.pbData = (BYTE *)szData;
    dataIn.cbData = strlen(szData) + 1;
    
    printf("[+] Original data: %s\n\n", szData);
    
    // Protect with audit flag
    printf("[*] Protecting with CRYPTPROTECT_AUDIT flag...\n");
    
    if (!CryptProtectData(
        &dataIn,
        L"Audited Data",
        NULL,
        NULL,
        NULL,
        CRYPTPROTECT_AUDIT,    // Generate audit event
        &dataOut
    )) {
        printf("[-] CryptProtectData failed: %ld\n", GetLastError());
        return;
    }
    
    printf("[+] Data protected with audit flag\n");
    printf("[+] Check Windows Event Viewer for audit entries\n\n");
    
    // Decrypt
    DATA_BLOB dataDecrypted;
    if (!CryptUnprotectData(&dataOut, NULL, NULL, NULL, NULL, 0, &dataDecrypted)) {
        printf("[-] CryptUnprotectData failed: %ld\n", GetLastError());
        LocalFree(dataOut.pbData);
        return;
    }
    
    printf("[+] Data decrypted: %s\n\n", (char *)dataDecrypted.pbData);
    
    // Cleanup
    LocalFree(dataOut.pbData);
    LocalFree(dataDecrypted.pbData);
}

int main() 
{
    printf("\n[++] DP API - Demonstration.\n");
    
    Demo_BasicProtection();
    Demo_WithEntropy();
    Demo_ProtectionScope();
    Demo_BinaryData();
    Demo_AdvancedFlags();
    
    printf("[+] Demonstration Completed!\n");
    
    return 0;
}