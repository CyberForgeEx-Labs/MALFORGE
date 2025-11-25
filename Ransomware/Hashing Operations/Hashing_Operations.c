#include <windows.h>
#include <wincrypt.h>
#include <stdio.h>

#pragma comment(lib, "advapi32.lib")

// Function to convert hash bytes to hex string
void BytesToHex(BYTE* bytes, DWORD len, char* output) {
    for (DWORD i = 0; i < len; i++) {
        sprintf(&output[i * 2], "%02x", bytes[i]);
    }
    output[len * 2] = '\0';
}

// Function to demonstrate hashing operations.
BOOL DemonstrateHashing(const char* data) {
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    BYTE hashBuffer[32]; // SHA-256 produces 32 bytes
    DWORD hashLen = sizeof(hashBuffer);
    DWORD algId = 0;
    DWORD paramLen = sizeof(DWORD);
    char hexHash[65];
    BOOL success = FALSE;

    printf("\n[+] Cryptography Hashing Demonstration.\n\n");
    printf("[+] Data to hash: %s\n\n", data);

    // Acquire cryptographic context
    printf("[+] CryptAcquireContext - Acquiring crypto provider...\n");
    if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, 
                             CRYPT_VERIFYCONTEXT)) {
        printf("[-]   ERROR: Failed to acquire context (0x%08X)\n", GetLastError());
        goto cleanup;
    }
    printf("[+]   SUCCESS: Crypto provider acquired\n\n");

    // Create hash object (SHA-256)
    printf("[+] CryptCreateHash - Creating SHA-256 hash object...\n");
    if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
        printf("[-]   ERROR: Failed to create hash (0x%08X)\n", GetLastError());
        goto cleanup;
    }
    printf("[+]   SUCCESS: Hash object created\n\n");

    // Hash the data.
    printf("[+] CryptHashData - Hashing data...\n");
    if (!CryptHashData(hHash, (BYTE*)data, strlen(data), 0)) {
        printf("[-]   ERROR: Failed to hash data (0x%08X)\n", GetLastError());
        goto cleanup;
    }
    printf("[+]   SUCCESS: Data hashed\n\n");

    // Get hash parameters (algorithm ID)
    printf("[+] CryptGetHashParam - Getting hash algorithm ID...\n");
    if (!CryptGetHashParam(hHash, HP_ALGID, (BYTE*)&algId, &paramLen, 0)) {
        printf("[-]   ERROR: Failed to get algorithm ID (0x%08X)\n", GetLastError());
        goto cleanup;
    }
    printf("[+]   SUCCESS: Algorithm ID = 0x%04X (", algId);
    switch (algId) {
        case CALG_SHA_256: printf("SHA-256"); break;
        case CALG_SHA1: printf("SHA-1"); break;
        case CALG_MD5: printf("MD5"); break;
        default: printf("Unknown");
    }
    printf(")\n\n");

    // Get hash size.
    printf("[+] CryptGetHashParam - Getting hash size...\n");
    paramLen = sizeof(DWORD);
    DWORD hashSize = 0;
    if (!CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE*)&hashSize, &paramLen, 0)) {
        printf("[-]   ERROR: Failed to get hash size (0x%08X)\n", GetLastError());
        goto cleanup;
    }
    printf("[+]   SUCCESS: Hash size = %d bytes\n\n", hashSize);

    // Get the actual hash value.
    printf("[+] CryptGetHashParam - Retrieving hash value...\n");
    hashLen = hashSize;
    if (!CryptGetHashParam(hHash, HP_HASHVAL, hashBuffer, &hashLen, 0)) {
        printf("[-]   ERROR: Failed to get hash value (0x%08X)\n", GetLastError());
        goto cleanup;
    }
    
    BytesToHex(hashBuffer, hashLen, hexHash);
    printf("[+]   SUCCESS: Hash retrieved\n");
    printf("[+]   Hash (hex): %s\n\n", hexHash);

    success = TRUE;

cleanup:
    // Cleanup
    printf("[+] Cleanup...\n");
    if (hHash) {
        CryptDestroyHash(hHash);
        printf("   - Hash object destroyed.\n");
    }
    if (hProv) {
        CryptReleaseContext(hProv, 0);
        printf("   - Crypto context released.\n");
    }
    
    printf("\n[+] Demonstration %s\n", success ? "SUCCESSFUL" : "FAILED");
    return success;
}

// Demonstrate hashing with different algorithms
void DemonstrateMultipleAlgorithms(const char* data) {
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    
    ALG_ID algorithms[] = {CALG_MD5, CALG_SHA1, CALG_SHA_256};
    const char* algNames[] = {"MD5", "SHA-1", "SHA-256"};
    int algCount = 3;

    printf("\n[+] Hashing with Multiple Algorithms.\n\n");
    printf("[+] Data: %s\n\n", data);

    if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, 
                             CRYPT_VERIFYCONTEXT)) {
        printf("[-] Failed to acquire context\n");
        return;
    }

    for (int i = 0; i < algCount; i++) {
        BYTE hashBuffer[64];
        DWORD hashLen = sizeof(hashBuffer);
        char hexHash[129];

        if (CryptCreateHash(hProv, algorithms[i], 0, 0, &hHash)) {
            if (CryptHashData(hHash, (BYTE*)data, strlen(data), 0)) {
                if (CryptGetHashParam(hHash, HP_HASHVAL, hashBuffer, &hashLen, 0)) {
                    BytesToHex(hashBuffer, hashLen, hexHash);
                    printf("%s: %s\n", algNames[i], hexHash);
                }
            }
            CryptDestroyHash(hHash);
        }
    }

    CryptReleaseContext(hProv, 0);
    printf("\n");
}

int main() {
    const char* testData = "Hello, Cryptography!";
    
    // Demonstrate basic hashing operations
    DemonstrateHashing(testData);
    
    // Demonstrate multiple hash algorithms
    DemonstrateMultipleAlgorithms(testData);
    
    printf("\nPress Enter to exit...");
    getchar();
    
    return 0;
}