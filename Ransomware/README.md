<h1 align="center">
   Ransomware Cryptographic Techniques.
</h1>

A comprehensive collection of C programs demonstrating core cryptographic operations used in malware and legitimate software, implemented via native Windows Cryptography APIs.

## Overview

This repository provides a granular, code-level breakdown of the cryptographic primitives and techniques that form the idea of ransomware. Each module contains a self-contained demonstration of a specific cryptographic task, from key generation and hashing to high-level file encryption. The examples are designed for security analysis, reverse engineering, and malware research.

## Features

- **Cryptographic Context Management** - Interfacing with Windows cryptographic providers (CAPI).
- **Data Protection API (DPAPI)** - Securely storing secrets (e.g., encryption keys) bound to a user or machine.
- **Encoding & Decoding** - Base64 and Hexadecimal encoding for data transport and obfuscation.
- **File Encryption (High-Level)** - Utilizing built-in Windows file encryption capabilities like EFS.
- **Hashing Operations** - Implementing standard hashing algorithms for data integrity and key derivation.
- **Key Generation & Derivation** - Creating and deriving cryptographic keys from passwords or random data.

## Directory Structure

```
Ransomware/
├── Cryptographic Context Management/   # Legacy CryptoAPI provider setup and management.
├── Data Protection API (DPAPI)/        # User/machine-bound data protection using DPAPI.
├── Encoding & Decoding/                # Base64 and Hexadecimal encoding/decoding utilities.
├── File Encryption (High-Level)/       # Demonstrations of high-level file encryption APIs (EFS).
├── Hashing Operations/                 # Implementations of SHA, MD5, and other hashing functions.
├── Key Generation & Derivation/        # Generating and deriving symmetric/asymmetric keys.
└── README.md                           # This documentation.
```

## Core Modules

### Cryptographic Context Management
Demonstrates the foundational step for most legacy crypto operations: acquiring a handle to a Cryptographic Service Provider (CSP) using `CryptAcquireContext`. This is essential for accessing algorithms and key storage.

### Data Protection API (DPAPI)
Shows how to use `CryptProtectData` and `CryptUnprotectData` to encrypt data, such as a master encryption key, using keys transparently managed by Windows and tied to a user's logon credentials or the machine itself.

### Encoding & Decoding
Implements binary-to-string conversions using `CryptBinaryToString` and `CryptStringToBinary`. These are commonly used to encode binary data (like keys or encrypted blobs) in Base64 or Hex for storage in text-based formats.

### File Encryption (High-Level)
Leverages high-level, integrated Windows APIs like the Encrypting File System (EFS) with `EncryptFile` and `DecryptFile`. Ransomware might use these APIs in opportunistic scenarios.

### Hashing Operations
Provides examples of creating and using hash objects via `CryptCreateHash` and `CryptHashData`. Hashing is critical for verifying data integrity, creating digital signatures, and as a component of key derivation functions.

### Key Generation & Derivation
Covers the core of any crypto-system: creating keys. This includes generating random symmetric keys with `CryptGenKey` and deriving keys from passwords or other secrets using `CryptDeriveKey`.

## Cryptographic Considerations

- **Legacy vs. Modern APIs**: Most examples use the legacy **CryptoAPI (CAPI)**. Modern Windows applications should prefer the more flexible and robust **Cryptography API: Next Generation (CNG)**.
- **Key Management**: These examples are for demonstration. In real-world scenarios, secure key management (generation, storage, and destruction) is the most critical and difficult aspect. Never hardcode cryptographic keys.
- **Algorithm Choice**: The security of the system depends on choosing strong, modern algorithms (e.g., AES-256 in GCM mode) and appropriate key lengths.
- **Secure Memory**: Sensitive data like plaintext keys should be securely zeroed from memory after use with functions like `SecureZeroMemory`.

## Limitations

- **Windows-Centric**: All code is based on the Windows API and is not portable.
- **Detection**: The API call patterns demonstrated are well-known and likely monitored by modern EDR and anti-virus solutions.
- **Educational Purpose**: These are simplified demonstrations and lack the robustness, error handling, and obfuscation of production-grade malware.
- **Completeness**: This collection does not represent a fully functional ransomware but rather its constituent cryptographic building blocks.

## References

- [Microsoft CryptoAPI (CAPI)](https://learn.microsoft.com/en-us/windows/win32/seccrypto/cryptoapi-system-architecture)
- [Microsoft Data Protection API (DPAPI)](https://learn.microsoft.com/en-us/windows/win32/seccrypto/dpapi)
- [Encrypting File System (EFS)](https://learn.microsoft.com/en-us/windows/win32/fileio/file-encryption)
- [Cryptography API: Next Generation (CNG)](https://learn.microsoft.com/en-us/windows/win32/seccng/cng-features)
