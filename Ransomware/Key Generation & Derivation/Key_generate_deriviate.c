#include <windows.h>
#include <wincrypt.h>
#include <stdio.h>
#include <string.h>

#pragma comment(lib, "advapi32.lib")

// Helper function to print bytes in hexadecimal.
void PrintHex(const char *label, PBYTE pbData, DWORD dwDataLen) {
    printf("%s", label);
    for (DWORD i = 0; i < dwDataLen; i++) {
        printf("%02X ", pbData[i]);
        if ((i + 1) % 16 == 0) printf("\n");
    }
    if (dwDataLen % 16 != 0) printf("\n");
}

int main() 
{
    HCRYPTPROV hCryptProv = 0;
    HCRYPTHASH hHash = 0;
    HCRYPTKEY hKey = 0;
    BYTE pbRandomData[32];
    DWORD dwDataLen = 0;
    char szPassword[] = "randomsecurepass";
    
    printf("[+] Windows Crypto API - Key Generation & Derivation Demonstration\n");
    
    // Acquire Cryptographic Context
    printf("[1] Acquiring cryptographic context...\n");
    if (!CryptAcquireContext(&hCryptProv, NULL, MS_ENH_RSA_AES_PROV, 
                            PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
        printf("[-] Error acquiring context: %ld\n", GetLastError());
        return 1;
    }
    printf("[+] Cryptographic context acquired successfully.\n\n");
    
    // CryptGenRandom - Generate Random Data
    printf("[2] CryptGenRandom - Generating random bytes...\n");
    
    if (!CryptGenRandom(hCryptProv, sizeof(pbRandomData), pbRandomData)) {
        printf("[-] Error generating random data: %ld\n", GetLastError());
        goto cleanup;
    }
    printf("[+] Generated %d random bytes:\n", sizeof(pbRandomData));
    PrintHex("    ", pbRandomData, sizeof(pbRandomData));
    printf("\n");
    
    // CryptDeriveKey - Derive Key from Password
    printf("[3] CryptDeriveKey - Deriving key from password...\n");
    printf("    Password: %s\n", szPassword);
    
    // Create hash object
    if (!CryptCreateHash(hCryptProv, CALG_SHA_256, 0, 0, &hHash)) {
        printf("[-] Error creating hash: %ld\n", GetLastError());
        goto cleanup;
    }
    
    // Hash the password
    if (!CryptHashData(hHash, (BYTE *)szPassword, strlen(szPassword), 0)) {
        printf("[-] Error hashing password: %ld\n", GetLastError());
        goto cleanup;
    }
    
    // Derive AES-256 key from the hash
    if (!CryptDeriveKey(hCryptProv, CALG_AES_256, hHash, 0, &hKey)) {
        printf("[-] Error deriving key: %ld\n", GetLastError());
        goto cleanup;
    }
    printf("[+] AES-256 key derived successfully from password.\n\n");
    
    // CryptGetHashParam - Get Hash Information.
    printf("[4] CryptGetHashParam - Getting hash information...\n");
    
    // Get hash size
    DWORD dwHashSize = 0;
    DWORD dwHashSizeLen = sizeof(DWORD);
    if (!CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE *)&dwHashSize, &dwHashSizeLen, 0)) {
        printf("[-] Error getting hash size: %ld\n", GetLastError());
        goto cleanup;
    }
    printf("[+] Hash size: %ld bytes\n", dwHashSize);
    
    // Get hash value
    BYTE pbHashValue[64];
    DWORD dwHashValueLen = sizeof(pbHashValue);
    if (!CryptGetHashParam(hHash, HP_HASHVAL, pbHashValue, &dwHashValueLen, 0)) {
        printf("[-] Error getting hash value: %ld\n", GetLastError());
        goto cleanup;
    }
    printf("[+] Hash value (%ld bytes):\n", dwHashValueLen);
    PrintHex("    ", pbHashValue, dwHashValueLen);
    
    // Get algorithm ID
    ALG_ID algId = 0;
    DWORD dwAlgIdLen = sizeof(ALG_ID);
    if (!CryptGetHashParam(hHash, HP_ALGID, (BYTE *)&algId, &dwAlgIdLen, 0)) {
        printf("[-] Error getting algorithm ID: %ld\n", GetLastError());
        goto cleanup;
    }
    printf("[+] Algorithm ID: 0x%X ", algId);
    switch(algId) {
        case CALG_SHA_256: printf("(SHA-256)\n\n"); break;
        case CALG_SHA1: printf("(SHA-1)\n\n"); break;
        case CALG_MD5: printf("(MD5)\n\n"); break;
        default: printf("(Unknown)\n\n"); break;
    }
    
    // CryptSetKeyParam - Configure Key Parameters
    printf("[5] CryptSetKeyParam - Setting key parameters...\n");
    
    // Set encryption mode to CBC (Cipher Block Chaining)
    DWORD dwMode = CRYPT_MODE_CBC;
    if (!CryptSetKeyParam(hKey, KP_MODE, (BYTE *)&dwMode, 0)) {
        printf("[-] Error setting encryption mode: %ld\n", GetLastError());
        goto cleanup;
    }
    printf("[+] Encryption mode set to CBC (Cipher Block Chaining).\n");
    
    // Generate and set initialization vector (IV)
    BYTE pbIV[16]; // AES block size is 16 bytes
    if (!CryptGenRandom(hCryptProv, sizeof(pbIV), pbIV)) {
        printf("[-] Error generating IV: %ld\n", GetLastError());
        goto cleanup;
    }
    
    if (!CryptSetKeyParam(hKey, KP_IV, pbIV, 0)) {
        printf("[-] Error setting IV: %ld\n", GetLastError());
        goto cleanup;
    }
    printf("[+] Initialization Vector (IV) set:\n");
    PrintHex("    ", pbIV, sizeof(pbIV));
    
    // Set padding mode
    DWORD dwPaddingMode = PKCS5_PADDING;
    if (!CryptSetKeyParam(hKey, KP_PADDING, (BYTE *)&dwPaddingMode, 0)) {
        printf("[-] Error setting padding mode: %ld\n", GetLastError());
        goto cleanup;
    }
    printf("[+] Padding mode set to PKCS5.\n\n");
    
    // Demonstrate Key Usage - Encrypt Data.
    printf("[6] Using the derived key to encrypt data...\n");
    
    BYTE pbTestData[1024];
    strcpy_s((char *)pbTestData, sizeof(pbTestData), "This is test data encrypted with derived key!");
    DWORD dwTestDataLen = strlen((char *)pbTestData);
    
    printf("    Original data: %s\n", pbTestData);
    printf("    Original length: %ld bytes\n", dwTestDataLen);
    
    // Encrypt
    DWORD dwBufLen = sizeof(pbTestData);
    if (!CryptEncrypt(hKey, 0, TRUE, 0, pbTestData, &dwTestDataLen, dwBufLen)) {
        printf("[-] Error encrypting data: %ld\n", GetLastError());
        goto cleanup;
    }
    printf("[+] Data encrypted successfully.\n");
    printf("    Encrypted length: %ld bytes\n", dwTestDataLen);
    printf("    Encrypted data (first 32 bytes):\n");
    PrintHex("    ", pbTestData, min(32, dwTestDataLen));
    printf("\n");
    
    // CryptDestroyKey - Cleanup Key
    printf("[7] CryptDestroyKey - Destroying key...\n");
    
    if (hKey) {
        if (!CryptDestroyKey(hKey)) {
            printf("[-] Error destroying key: %ld\n", GetLastError());
        } else {
            printf("[+] Key destroyed successfully.\n");
            hKey = 0; // Mark as destroyed
        }
    }
    printf("\n");
    
    // Random Key Generation
    printf("[8] Bonus: Generating a random session key...\n");
    printf("    Purpose: Create key without password derivation\n");
    
    HCRYPTKEY hSessionKey = 0;
    if (!CryptGenKey(hCryptProv, CALG_AES_128, CRYPT_EXPORTABLE, &hSessionKey)) {
        printf("[-] Error generating session key: %ld\n", GetLastError());
        goto cleanup;
    }
    printf("[+] Random AES-128 session key generated successfully.\n");
    
    // Get key length
    DWORD dwKeyLen = 0;
    DWORD dwKeyLenSize = sizeof(DWORD);
    if (!CryptGetKeyParam(hSessionKey, KP_KEYLEN, (BYTE *)&dwKeyLen, &dwKeyLenSize, 0)) {
        printf("[-] Error getting key length: %ld\n", GetLastError());
    } else {
        printf("[+] Key length: %ld bits\n", dwKeyLen);
    }
    
    // Destroy session key
    if (hSessionKey) {
        CryptDestroyKey(hSessionKey);
        printf("[+] Session key destroyed.\n\n");
    }
    
    printf("[+] Demonstration Completed!\n");

cleanup:
    // Clean up resources
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