<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Ransomware/Key%20Generation%20%26%20Derivation/Key_generate_deriviate.c">Windows CryptoAPI - Key Derivation & Symmetric Encryption Demonstration</a>
</h1>

## Overview
This program demonstrates **legitimate use of the Microsoft Cryptographic API (CAPI / CryptoAPI)** for secure key generation, password-based key derivation, and symmetric encryption using AES. These techniques are widely used in real-world Windows applications for:

- Secure password-to-key derivation (PBKDF-like behavior using hash-based methods)
- Generating cryptographically strong random data
- Configuring AES encryption parameters (mode, IV, padding)
- Safe resource cleanup of cryptographic objects
- Session key generation and management.

This is the foundation of secure data protection in many Windows software products.

---
## Core APIs Demonstrated
| API                  | Purpose                                      | Use Case                                      |
|----------------------|----------------------------------------------|-----------------------------------------------|
| `CryptAcquireContext` | Acquire cryptographic service provider context | Initialize CryptoAPI usage                    |
| `CryptGenRandom`      | Generate cryptographically secure random bytes | IVs, salts, session keys, nonces              |
| `CryptCreateHash` + `CryptHashData` | Create and feed data into a hash object | Password hashing before key derivation        |
| `CryptDeriveKey`      | Derive a symmetric key from a hash           | Password-Based Key Derivation (legacy method) |
| `CryptSetKeyParam`    | Configure key parameters (mode, IV, padding) | Set CBC mode, custom IV, PKCS5/PKCS7 padding  |
| `CryptEncrypt`        | Perform in-place symmetric encryption       | Encrypt sensitive data in memory or files     |
| `CryptGenKey`         | Generate a random symmetric session key      | Ephemeral keys for secure communication       |
| `CryptGetHashParam`   | Retrieve hash properties and value           | Verify hash algorithm and output              |

---
## 1. [CryptAcquireContext](https://github.com/CyberForgeEx/MALFORGE/blob/main/Ransomware/Key%20Generation%20%26%20Derivation/Key_generate_deriviate.c#L31)
**Description:** Acquires a handle to a cryptographic service provider (CSP).  
**Key Features:**
- Uses modern `PROV_RSA_AES` with `MS_ENH_RSA_AES_PROV`
- Supports AES and strong algorithms
- `CRYPT_VERIFYCONTEXT` avoids persistent key storage

**Use Cases:**
- Required first step for nearly all CryptoAPI operations
- Enables access to high-quality RNG and encryption primitives

---
## 2. [CryptGenRandom & Random Session Key Generation](https://github.com/CyberForgeEx/MALFORGE/blob/main/Ransomware/Key%20Generation%20%26%20Derivation/Key_generate_deriviate.c#L41)
**Description:** Fills a buffer with cryptographically secure random bytes.  
**Key Features:**
- Used to generate IVs, salts, and random session keys
- Relies on OS entropy sources (hardware + system)
- Also shown via `CryptGenKey(CALG_AES_128, CRYPT_EXPORTABLE, ...)`

**Use Cases:**
- Initialization vectors (IVs)
- Salt in password hashing
- Generating ephemeral encryption keys

---
## 3. [CryptDeriveKey - Password-Based AES-256 Key Derivation](https://github.com/CyberForgeEx/MALFORGE/blob/main/Ransomware/Key%20Generation%20%26%20Derivation/Key_generate_deriviate.c#L66)
**Description:** Derives a symmetric key from hashed password data.  
**Key Features:**
- Uses SHA-256 hash of password as key material
- Produces full AES-256 key (256-bit)
- Legacy but still widely supported method (pre-CNG equivalent of PBKDF2)

**Use Cases:**
- Local file encryption tools
- Legacy application data protection
- Simple password-protected archives

---
## 4. [CryptSetKeyParam - Configuring AES Encryption](https://github.com/CyberForgeEx/MALFORGE/blob/main/Ransomware/Key%20Generation%20%26%20Derivation/Key_generate_deriviate.c#L114)
**Description:** Sets operational parameters on a key object.  
**Demonstrated Settings:**
- `KP_MODE` → `CRYPT_MODE_CBC` (Cipher Block Chaining)
- `KP_IV` → Random 16-byte initialization vector
- `KP_PADDING` → `PKCS5_PADDING` (automatically pads data)

**Use Cases:**
- Ensuring secure and standard-compliant encryption
- Full control over cipher behavior

---
## 5. [CryptEncrypt - Performing AES Encryption](https://github.com/CyberForgeEx/MALFORGE/blob/main/Ransomware/Key%20Generation%20%26%20Derivation/Key_generate_deriviate.c#L154)
**Description:** Encrypts data in-place using the configured key.  
**Key Features:**
- Final block handling with `TRUE` parameter
- Automatic padding via PKCS5/PKCS7
- Buffer must be large enough for padding expansion

**Use Cases:**
- Encrypting configuration files, credentials, or documents
- In-memory protection of sensitive strings

---
## Program Workflow
The demonstration follows a complete secure cryptographic workflow:
1. Acquire cryptographic context (CSP)
2. Generate random data with `CryptGenRandom`
3. Hash password using SHA-256
4. Derive AES-256 key from password hash
5. Retrieve and display hash properties
6. Configure key: CBC mode + random IV + PKCS5 padding
7. Encrypt sample plaintext data
8. Generate a separate random AES-128 session key (bonus)
9. Safely destroy all cryptographic objects

---
## Compilation Instructions
### Visual Studio / cl.exe
```bash
cl.exe crypto_key_derivation.c /link advapi32.lib
```

### MinGW / GCC
```bash
gcc crypto_key_derivation.c -o crypto_key_derivation.exe -ladvapi32
```

### Run (No admin rights required)
```cmd
crypto_key_derivation.exe
```

---
## Required Libraries
- `advapi32.lib` – Core CryptoAPI functions

> Note: Modern applications should migrate to **CNG (BCrypt/NCrypt)**, but CryptoAPI remains fully supported on all current Windows versions.

---
## System Requirements
- **OS:** Windows XP or later (including Windows 11)
- **Architecture:** x86 or x64
- **Provider:** `MS_ENH_RSA_AES_PROV` available since Windows XP SP3 / Vista+

---
## Error Handling
The program includes comprehensive error checking with `GetLastError()` reporting for:
- Failed context acquisition
- RNG or hash failures
- Key derivation issues
- Parameter setting errors
- Encryption failures
- Resource leaks prevented via `goto cleanup` pattern.

---
## API References
- [CryptAcquireContext](https://learn.microsoft.com/en-us/windows/win32/api/wincrypt/nf-wincrypt-cryptacquirecontexta)
- [CryptGenRandom](https://learn.microsoft.com/en-us/windows/win32/api/wincrypt/nf-wincrypt-cryptgenrandom)
- [CryptDeriveKey](https://learn.microsoft.com/en-us/windows/win32/api/wincrypt/nf-wincrypt-cryptderivekey)
- [CryptSetKeyParam](https://learn.microsoft.com/en-us/windows/win32/api/wincrypt/nf-wincrypt-cryptsetkeyparam)
- [CryptEncrypt](https://learn.microsoft.com/en-us/windows/win32/api/wincrypt/nf-wincrypt-cryptencrypt)
- [CryptGenKey](https://learn.microsoft.com/en-us/windows/win32/api/wincrypt/nf-wincrypt-cryptgenkey)

---
<p align="right">
  Last Updated: November 2025
</p>
