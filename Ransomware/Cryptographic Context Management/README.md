<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Ransomware/Cryptographic%20Context%20Management/Crypto_Context.c">Cryptographic Context Management Demonstration</a>
</h1>

## Overview
This program demonstrates **complete, production-style usage** of the legacy Microsoft Cryptographic API (CryptoAPI / CAPI) to perform **password-based AES-256 encryption and decryption** of in-memory data.

It showcases best-practice patterns used in real Windows software for:
- Secure password-to-key derivation
- Full round-trip encryption → decryption
- Proper buffer sizing and length handling
- Robust error handling with fallback keyset creation
- Clean resource management

This is the exact methodology historically used by many Windows applications for protecting configuration files, credentials, and local data at rest.

---
## Core APIs Demonstrated
| API                  | Purpose                                      | Use Case                                      |
|----------------------|----------------------------------------------|-----------------------------------------------|
| `CryptAcquireContext` | Acquire CSP context (with keyset creation fallback) | Initialize CryptoAPI with persistent support |
| `CryptCreateHash`     | Create SHA-256 hash object                   | Prepare for password hashing                  |
| `CryptHashData`       | Hash password bytes                          | Input material for key derivation             |
| `CryptDeriveKey`      | Derive AES-256 key from password hash        | Password-Based Encryption (PBE)               |
| `CryptEncrypt`        | Perform AES encryption with PKCS7 padding    | Protect sensitive data                        |
| `CryptDecrypt`        | Decrypt previously encrypted data            | Recover original plaintext                    |
| `CryptDestroyKey` / `CryptDestroyHash` / `CryptReleaseContext` | Securely release cryptographic objects | Prevent resource leaks                        |

---
## 1. [CryptAcquireContext with Keyset Fallback](https://github.com/CyberForgeEx/MALFORGE/blob/main/Ransomware/Cryptographic%20Context%20Management/Crypto_Context.c#L37)
**Description:** Acquires a cryptographic context with intelligent handling of missing key containers.  
**Key Features:**
- Uses `MS_ENH_RSA_AES_PROV` + `PROV_RSA_AES` for modern AES support
- Handles `NTE_BAD_KEYSET` by creating a new keyset via `CRYPT_NEWKEYSET`
- Enables persistent key storage capability (optional for real apps)

**Use Cases:**
- First step in any serious CryptoAPI-based encryption tool
- Required when applications need to reuse derived keys across sessions

---
## 2. [Password-Based AES-256 Key Derivation]()
**Description:** Converts a user password into a strong 256-bit AES key via SHA-256 hashing.  
**Key Features:**
- Single-iteration SHA-256 → AES-256 (legacy PBE method)
- No salt in this example (real apps should add salt + iterations via custom PBKDF1/PBKDF2)

**Use Cases:**
- Local file encryption utilities
- Legacy credential storage systems
- Simple encrypted databases

---
## 3. [CryptEncrypt - Full In-Place Encryption](https://github.com/CyberForgeEx/MALFORGE/blob/main/Ransomware/Cryptographic%20Context%20Management/Crypto_Context.c#L65)
**Description:** Encrypts data using the derived AES-256 key with automatic PKCS7 padding.  
**Key Features:**
- `TRUE` in final block parameter enables padding
- Buffer must be large enough for padding expansion
- Length updated via pointer parameter

**Use Cases:**
- Encrypting strings, files, or binary blobs
- Secure storage of secrets in memory or disk

---
## 4. [CryptDecrypt - Full Decryption Cycle](https://github.com/CyberForgeEx/MALFORGE/blob/main/Ransomware/Cryptographic%20Context%20Management/Crypto_Context.c#L18)
**Description:** Reverses encryption and removes PKCS7 padding.  
**Key Features:**
- Automatically strips padding
- Restores exact original plaintext and length
- Same key and settings as encryption

**Use Cases:**
- Reading back encrypted configuration
- Secure credential retrieval
- Decrypting user-provided encrypted files

---
## Program Workflow
1. Acquire cryptographic context (with fallback for missing keyset)
2. Create SHA-256 hash object
3. Hash the password string
4. Derive AES-256 encryption key from hash
5. Copy plaintext into buffer
6. **Encrypt** data in-place using `CryptEncrypt`
7. **Decrypt** data immediately using `CryptDecrypt`
8. Null-terminate and display recovered plaintext
9. Clean up all cryptographic objects

Confirms full round-trip integrity.

---
## Compilation Instructions
### Visual Studio / cl.exe
```bash
cl.exe crypt_context.c /link advapi32.lib
```

### MinGW / GCC
```bash
gcc crypt_context.c -o crypto_encrypt_decrypt.exe -ladvapi32
```

### Run (No admin rights required)
```cmd
crypto_encrypt_decrypt.exe
```

---
## Required Libraries
- `advapi32.lib` – Core CryptoAPI functions

> Note: This is **legacy CryptoAPI (CAPI)**. Modern Windows applications should use **CNG (BCrypt)** with `BCryptDeriveKey` + `BCryptEncrypt`/`BCryptDecrypt` for better security and performance.

---
## System Requirements
- **OS:** Windows XP SP3 or later (including Windows 11)
- **Provider:** `MS_ENH_RSA_AES_PROV` available since XP SP3 / Vista+
- **Architecture:** x86 or x64

---
## Error Handling
Comprehensive error checking with `GetLastError()` for:
- Context acquisition failures
- Missing or corrupt key containers
- Hash creation and data feeding issues
- Key derivation problems
- Encryption/decryption failures
- Graceful cleanup even on error

---
## API References
- [CryptAcquireContext](https://learn.microsoft.com/en-us/windows/win32/api/wincrypt/nf-wincrypt-cryptacquirecontexta)
- [CryptCreateHash](https://learn.microsoft.com/en-us/windows/win32/api/wincrypt/nf-wincrypt-cryptcreatehash)
- [CryptHashData](https://learn.microsoft.com/en-us/windows/win32/api/wincrypt/nf-wincrypt-crypthashdata)
- [CryptDeriveKey](https://learn.microsoft.com/en-us/windows/win32/api/wincrypt/nf-wincrypt-cryptderivekey)
- [CryptEncrypt](https://learn.microsoft.com/en-us/windows/win32/api/wincrypt/nf-wincrypt-cryptencrypt)
- [CryptDecrypt](https://learn.microsoft.com/en-us/windows/win32/api/wincrypt/nf-wincrypt-cryptdecrypt)

---
<p align="right">
  Last Updated: November 2025
</p>
