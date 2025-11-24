#include <windows.h>
#include <wincrypt.h>
#include <stdio.h>
#include <string.h>

#pragma comment(lib, "advapi32.lib")

// Function to encrypt data using CryptEncrypt.
BOOL EncryptData(HCRYPTKEY hKey, PBYTE pbData, DWORD *pdwDataLen, DWORD dwBufLen) {
    if (!CryptEncrypt(hKey, 0, TRUE, 0, pbData, pdwDataLen, dwBufLen)) {
        printf("[-] Error during CryptEncrypt: %ld\n", GetLastError());
        return FALSE;
    }
    return TRUE;
}

// Function to decrypt data using CryptDecrypt.
BOOL DecryptData(HCRYPTKEY hKey, PBYTE pbData, DWORD *pdwDataLen) {
    if (!CryptDecrypt(hKey, 0, TRUE, 0, pbData, pdwDataLen)) {
        printf("[-] Error during CryptDecrypt: %ld\n", GetLastError());
        return FALSE;
    }
    return TRUE;
}

int main() 
{
    HCRYPTPROV hCryptProv = 0;
    HCRYPTHASH hHash = 0;
    HCRYPTKEY hKey = 0;
    
    BYTE pbData[1024];
    DWORD dwDataLen = 0;
    char szPassword[] = "7N3:Y20w1!4{sB^{"; // Random Secure Password
    
    // Acquire cryptographic context
    if (!CryptAcquireContext(&hCryptProv, NULL, MS_ENH_RSA_AES_PROV, PROV_RSA_AES, 0)) {
        if (GetLastError() == NTE_BAD_KEYSET) {
            if (!CryptAcquireContext(&hCryptProv, NULL, MS_ENH_RSA_AES_PROV, PROV_RSA_AES, CRYPT_NEWKEYSET)) {
                printf("[-] Error acquiring context: %ld\n", GetLastError());
                return 1;
            }
        } else {
            printf("[-] Error acquiring context: %ld\n", GetLastError());
            return 1;
        }
    }
    printf("[+] Cryptographic context acquired.\n");
    
    // Create hash object for key derivation.
    if (!CryptCreateHash(hCryptProv, CALG_SHA_256, 0, 0, &hHash)) {
        printf("[-] Error creating hash: %ld\n", GetLastError());
        goto cleanup;
    }
    printf("[+] Hash object created.\n");
    
    // Hash the password
    if (!CryptHashData(hHash, (BYTE *)szPassword, strlen(szPassword), 0)) {
        printf("[-] Error hashing data: %ld\n", GetLastError());
        goto cleanup;
    }
    printf("[+] Password hashed.\n");
    
    // Derive key from hash
    if (!CryptDeriveKey(hCryptProv, CALG_AES_256, hHash, 0, &hKey)) {
        printf("[-] Error deriving key: %ld\n", GetLastError());
        goto cleanup;
    }
    printf("[+] Key derived.\n");
    
    // Prepare data to encrypt
    strcpy_s((char *)pbData, sizeof(pbData), "Hello, World! This is sensitive data.");
    dwDataLen = strlen((char *)pbData);
    printf("[+] Original data: %s\n", pbData);
    printf("[+] Data length: %ld bytes\n\n", dwDataLen);
    
    // Encrypt the data
    DWORD dwBufLen = sizeof(pbData);
    printf("\nENCRYPTION\n");
    if (!EncryptData(hKey, pbData, &dwDataLen, dwBufLen)) {
        goto cleanup;
    }
    printf("[+] Data encrypted successfully.\n");
    printf("[+] Encrypted data length: %ld bytes\n\n", dwDataLen);
    
    // Decrypt the data
    printf("\nDECRYPTION\n");
    if (!DecryptData(hKey, pbData, &dwDataLen)) {
        goto cleanup;
    }
    printf("\n[+] Data decrypted successfully.\n");
    pbData[dwDataLen] = '\0';
    printf("[+] Decrypted data: %s\n", pbData);
    printf("[+] Decrypted data length: %ld bytes\n", dwDataLen);
    
cleanup:
    // Clean up
    if (hKey) {
        CryptDestroyKey(hKey);
    }
    if (hHash) {
        CryptDestroyHash(hHash);
    }
    if (hCryptProv) {
        CryptReleaseContext(hCryptProv, 0);
    }
    
    printf("\n[+] Cleanup completed.\n");
    return 0;
}