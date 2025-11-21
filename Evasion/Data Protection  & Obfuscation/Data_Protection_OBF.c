#include <windows.h>
#include <dpapi.h>
#include <stdio.h>
#include <string.h>

#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "rpcrt4.lib")

// Function to print data as hex
void PrintHex(const BYTE* data, DWORD size, const char* label) {
    printf("%s: ", label);
    for (DWORD i = 0; i < size; i++) {
        printf("%02X ", data[i]);
    }
    printf("\n");
}

// Encrypt data using CryptProtectData
BOOL EncryptDataExample(const char* plaintext) {
    DATA_BLOB dataIn, dataOut;
    
    // Initialize input data
    dataIn.pbData = (BYTE*)plaintext;
    dataIn.cbData = (DWORD)strlen(plaintext) + 1;
    
    printf("\n[*] CryptProtectData Example.\n");
    printf("[+] Original text: %s\n", plaintext);
    PrintHex(dataIn.pbData, dataIn.cbData, "Original data (hex)");
    
    // Encrypt the data
    if (!CryptProtectData(&dataIn, L"Data Protection Example", NULL, NULL, NULL, 0, &dataOut)) {
        printf("[-] Error encrypting data: %ld\n", GetLastError());
        return FALSE;
    }
    
    printf("[+] Encryption successful!\n");
    PrintHex(dataOut.pbData, dataOut.cbData, "Encrypted data (hex)");
    
    // Decrypt the data
    DATA_BLOB decryptedData;
    LPWSTR descriptionOut = NULL;
    
    if (!CryptUnprotectData(&dataOut, &descriptionOut, NULL, NULL, NULL, 0, &decryptedData)) {
        printf("[-] Error decrypting data: %ld\n", GetLastError());
        LocalFree(dataOut.pbData);
        return FALSE;
    }
    
    printf("[+] Decryption successful!\n");
    printf("[+] Decrypted text: %s\n", (char*)decryptedData.pbData);
    
    // Cleanup
    LocalFree(dataOut.pbData);
    LocalFree(decryptedData.pbData);
    LocalFree(descriptionOut);
    
    return TRUE;
}

// Generate UUID from string using UuidFromStringA
BOOL UuidFromStringExample(const char* uuidString) {
    UUID uuid;
    RPC_STATUS status;
    
    printf("\n[*] UuidFromStringA Example.\n");
    printf("Input UUID string: %s\n", uuidString);
    
    // Convert string to UUID
    status = UuidFromStringA((unsigned char*)uuidString, &uuid);
    
    if (status == RPC_S_OK) {
        printf("[+] UUID conversion successful!\n");
        printf("[+] UUID binary representation:\n");
        printf("    Data1: 0x%08X\n", uuid.Data1);
        printf("    Data2: 0x%04X\n", uuid.Data2);
        printf("    Data3: 0x%04X\n", uuid.Data3);
        printf("    Data4: ");
        for (int i = 0; i < 8; i++) {
            printf("%02X ", uuid.Data4[i]);
        }
        printf("\n");
        return TRUE;
    } else if (status == RPC_S_INVALID_STRING_UUID) {
        printf("[-] Error: Invalid UUID string format\n");
        return FALSE;
    } else {
        printf("[-] Error converting UUID: %ld\n", status);
        return FALSE;
    }
}

// Generate a new UUID
void GenerateUuidExample() {
    UUID uuid;
    RPC_STATUS status;
    unsigned char* uuidString = NULL;
    
    printf("\n[*] Generate New UUID Example.\n");
    
    status = UuidCreate(&uuid);
    
    if (status == RPC_S_OK) {
        printf("[+] UUID created successfully!\n");
        
        status = UuidToStringA(&uuid, &uuidString);
        if (status == RPC_S_OK) {
            printf("[+] Generated UUID: %s\n", (char*)uuidString);
            RpcStringFreeA(&uuidString);
        }
    } else {
        printf("[-] Error creating UUID: %ld\n", status);
    }
}

int main() 
{
    printf("\n[+] Data Protection & Obfuscation APIs Demonstration.\n");
    
    // Encrypt/Decrypt data.
    EncryptDataExample("Hello, World! This is sensitive data.");
    
    // Convert UUID string to UUID structure.
    UuidFromStringExample("550e8400-e29b-41d4-a716-446655440000");
    
    // Generate new UUID.
    GenerateUuidExample();
    
    // Invalid UUID string (error handling).
    printf("\n[+] Testing invalid UUID.\n");
    UuidFromStringExample("invalid-uuid-string");
    
    printf("[+] Demonstration Complete\n");
    
    return 0;
}