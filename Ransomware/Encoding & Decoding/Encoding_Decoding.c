#include <windows.h>
#include <wincrypt.h>
#include <stdio.h>
#include <string.h>

#pragma comment(lib, "crypt32.lib")

// Function to print binary data as hex
void PrintHex(const char* label, BYTE* data, DWORD len) {
    printf("%s", label);
    for (DWORD i = 0; i < len; i++) {
        printf("%02X ", data[i]);
        if ((i + 1) % 16 == 0) printf("\n%*s", (int)strlen(label), "");
    }
    printf("\n");
}

// Demonstrate Base64 encoding/decoding.
void DemonstrateBase64() {
    printf("\n[+] BASE64 ENCODING/DECODING.\n\n");
    
    // Original binary data
    BYTE originalData[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 
                          0x72, 0x6C, 0x64, 0x21}; // "Hello World!" in hex.
    DWORD dataLen = sizeof(originalData);
    
    printf("[+] Original Data: \"%s\"\n", originalData);
    PrintHex("[+] Binary (hex):  ", originalData, dataLen);
    printf("\n");
    
    // ENCODING: Binary to Base64 String.
    printf("[+] CryptBinaryToString - Encoding to Base64...\n");
    
    DWORD base64Len = 0;
    // First call: get required buffer size
    if (!CryptBinaryToString(originalData, dataLen, 
                             CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, 
                             NULL, &base64Len)) {
        printf("[-]   ERROR: Failed to get buffer size (0x%08X)\n", GetLastError());
        return;
    }
    
    printf("[+]   Required buffer size: %d characters\n", base64Len);
    
    // Allocate buffer and encode
    char* base64String = (char*)malloc(base64Len * sizeof(char));
    if (!CryptBinaryToString(originalData, dataLen, 
                             CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, 
                             base64String, &base64Len)) {
        printf("[-]   ERROR: Failed to encode (0x%08X)\n", GetLastError());
        free(base64String);
        return;
    }
    
    printf("[+]   SUCCESS: Encoded to Base64\n");
    printf("[+]   Base64 string: %s\n\n", base64String);
    
    // DECODING: Base64 String to Binary
    printf("[+] CryptStringToBinary - Decoding from Base64...\n");
    
    DWORD decodedLen = 0;
    DWORD skipChars = 0;
    DWORD flags = 0;
    
    // First call: get required buffer size.
    if (!CryptStringToBinary(base64String, 0, CRYPT_STRING_BASE64,
                             NULL, &decodedLen, &skipChars, &flags)) {
        printf("[-]   ERROR: Failed to get buffer size (0x%08X)\n", GetLastError());
        free(base64String);
        return;
    }
    
    printf("[+]   Required buffer size: %d bytes\n", decodedLen);
    printf("[+]   Characters skipped: %d\n", skipChars);
    
    // Allocate buffer and decode
    BYTE* decodedData = (BYTE*)malloc(decodedLen);
    if (!CryptStringToBinary(base64String, 0, CRYPT_STRING_BASE64,
                             decodedData, &decodedLen, &skipChars, &flags)) {
        printf("[-]   ERROR: Failed to decode (0x%08X)\n", GetLastError());
        free(base64String);
        free(decodedData);
        return;
    }
    
    printf("[+]   SUCCESS: Decoded from Base64\n");
    printf("[+]   Decoded Data: \"%s\"\n", decodedData);
    PrintHex("[+]   Binary (hex):  ", decodedData, decodedLen);
    
    // Verify data integrity
    if (dataLen == decodedLen && memcmp(originalData, decodedData, dataLen) == 0) {
        printf("\n   [+] VERIFICATION: Original and decoded data match!\n");
    } else {
        printf("\n   [-] VERIFICATION: Data mismatch!\n");
    }
    
    free(base64String);
    free(decodedData);
}

// Demonstrate Hexadecimal encoding/decoding
void DemonstrateHexadecimal() {
    printf("\n\n[+] HEXADECIMAL ENCODING/DECODING\n\n");
    
    BYTE originalData[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE, 0xBA, 0xBE};
    DWORD dataLen = sizeof(originalData);
    
    PrintHex("[+] Original Data: ", originalData, dataLen);
    printf("\n");
    
    // ENCODING: Binary to Hex String
    printf("[+] CryptBinaryToString - Encoding to Hexadecimal...\n");
    
    DWORD hexLen = 0;
    if (!CryptBinaryToString(originalData, dataLen, 
                             CRYPT_STRING_HEXRAW | CRYPT_STRING_NOCRLF,
                             NULL, &hexLen)) {
        printf("[-]   ERROR: Failed to get buffer size (0x%08X)\n", GetLastError());
        return;
    }
    
    char* hexString = (char*)malloc(hexLen * sizeof(char));
    if (!CryptBinaryToString(originalData, dataLen, 
                             CRYPT_STRING_HEXRAW | CRYPT_STRING_NOCRLF,
                             hexString, &hexLen)) {
        printf("[-]   ERROR: Failed to encode (0x%08X)\n", GetLastError());
        free(hexString);
        return;
    }
    
    printf("[+]   SUCCESS: Encoded to Hexadecimal\n");
    printf("[+]   Hex string: %s\n\n", hexString);
    
    // DECODING: Hex String to Binary
    printf("[+] CryptStringToBinary - Decoding from Hexadecimal...\n");
    
    DWORD decodedLen = 0;
    if (!CryptStringToBinary(hexString, 0, CRYPT_STRING_HEXRAW,
                             NULL, &decodedLen, NULL, NULL)) {
        printf("[-]   ERROR: Failed to get buffer size (0x%08X)\n", GetLastError());
        free(hexString);
        return;
    }
    
    BYTE* decodedData = (BYTE*)malloc(decodedLen);
    if (!CryptStringToBinary(hexString, 0, CRYPT_STRING_HEXRAW,
                             decodedData, &decodedLen, NULL, NULL)) {
        printf("[-]   ERROR: Failed to decode (0x%08X)\n", GetLastError());
        free(hexString);
        free(decodedData);
        return;
    }
    
    printf("[+]   SUCCESS: Decoded from Hexadecimal\n");
    PrintHex("[+]   Decoded Data:  ", decodedData, decodedLen);
    
    // Verify data integrity
    if (dataLen == decodedLen && memcmp(originalData, decodedData, dataLen) == 0) {
        printf("\n   [+] VERIFICATION: Original and decoded data match!\n");
    } else {
        printf("\n   [-] VERIFICATION: Data mismatch!\n");
    }
    
    free(hexString);
    free(decodedData);
}

// Demonstrate different encoding formats
void DemonstrateAllFormats() {
    printf("\n\n[+] OTHER ENCODING FORMATS \n\n");
    
    BYTE data[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
    DWORD dataLen = sizeof(data);
    
    PrintHex("[+] Original Data: ", data, dataLen);
    printf("\n");
    
    // Array of encoding formats to demonstrate
    struct {
        DWORD format;
        const char* name;
    } formats[] = {
        {CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, "Base64 (no CRLF)"},
        {CRYPT_STRING_BASE64, "Base64 (with CRLF)"},
        {CRYPT_STRING_HEX | CRYPT_STRING_NOCRLF, "Hex (formatted)"},
        {CRYPT_STRING_HEXRAW | CRYPT_STRING_NOCRLF, "Hex (raw)"},
        {CRYPT_STRING_HEXASCII | CRYPT_STRING_NOCRLF, "Hex ASCII"}
    };
    
    int formatCount = sizeof(formats) / sizeof(formats[0]);
    
    for (int i = 0; i < formatCount; i++) {
        DWORD encodedLen = 0;
        
        if (CryptBinaryToString(data, dataLen, formats[i].format, 
                                NULL, &encodedLen)) {
            char* encoded = (char*)malloc(encodedLen * sizeof(char));
            
            if (CryptBinaryToString(data, dataLen, formats[i].format,
                                   encoded, &encodedLen)) {
                printf("%-20s: %s\n", formats[i].name, encoded);
            }
            
            free(encoded);
        }
    }
}

int main() 
{
    printf("[+] Windows Cryptography Encoding/Decoding Demonstration.\n");
    
    // Demonstrate different encoding operations
    DemonstrateBase64();
    DemonstrateHexadecimal();
    DemonstrateAllFormats();
    
    printf("\n\n[+] Demonstration Complete.\n\n");
    printf("Press Enter to exit...");
    getchar();
    
    return 0;
}