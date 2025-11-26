<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Ransomware/Hashing%20Operations/Hashing_Operations.c">Windows CryptoAPI Hashing Demonstration</a>
</h1>


<p align="center">
  <strong>A clean, educational example of using the Windows Cryptography API (CAPI) to compute cryptographic hashes (MD5, SHA-1, SHA-256)</strong>
</p>

## Overview

This C program demonstrates proper usage of the legacy **Windows Cryptography API (CAPI)** to perform cryptographic hashing operations using common algorithms:

- **SHA-256** (primary focus with detailed step-by-step explanation)
- **SHA-1**
- **MD5**

It serves as a clear, well-commented reference for developers learning how to:
- Acquire a cryptographic provider context
- Create and manage hash objects
- Hash string/data
- Retrieve hash values and metadata
- Perform proper cleanup
- Handle errors gracefully

> **Note:** This code uses the **legacy CryptoAPI** (`advapi32.dll`). For new projects, Microsoft recommends using **CNG (Cryptography API: Next Generation)**. This example is preserved for educational and compatibility purposes.

---

## Core APIs Demonstrated

| API                    | Purpose                                      |
|------------------------|-----------------------------------------------|
| `CryptAcquireContext`  | Acquire a handle to a cryptographic provider  |
| `CryptCreateHash`      | Create a hash object                          |
| `CryptHashData`        | Add data to the hash                          |
| `CryptGetHashParam`    | Retrieve hash value, size, or algorithm ID    |
| `CryptDestroyHash`     | Destroy the hash object                       |
| `CryptReleaseContext`  | Release the cryptographic context             |

---

## Features

- Step-by-step verbose output showing each CryptoAPI call
- Full error reporting with `GetLastError()` codes
- Demonstrates **SHA-256** in detail with algorithm verification
- Bonus: Computes **MD5**, **SHA-1**, and **SHA-256** for the same input
- Safe resource cleanup using `goto cleanup` pattern
- Hex conversion utility for binary hash output

---

## Compilation Instructions

### Using Visual Studio / cl.exe
```bash
cl.exe hashing_demo.c /link advapi32.lib
```

### Using MinGW / GCC
```bash
gcc hashing_demo.c -o hashing_demo.exe -ladvapi32
```

### Run
```cmd
hashing_demo.exe
```

No administrator privileges required.

---

## Required Library

- `advapi32.lib` – Provides CryptoAPI functions

---

## System Requirements

- **OS:** Windows XP or later (including Windows 11)
- **Architecture:** x86 or x64
- **Compiler:** Any Windows-compatible C compiler (MSVC, MinGW, etc.)

---

## API References

- [CryptAcquireContext](https://learn.microsoft.com/en-us/windows/win32/api/wincrypt/nf-wincrypt-cryptacquirecontexta)
- [CryptCreateHash](https://learn.microsoft.com/en-us/windows/win32/api/wincrypt/nf-wincrypt-cryptcreatehash)
- [CryptHashData](https://learn.microsoft.com/en-us/windows/win32/api/wincrypt/nf-wincrypt-crypthashdata)
- [CryptGetHashParam](https://learn.microsoft.com/en-us/windows/win32/api/wincrypt/nf-wincrypt-cryptgethashparam)
- [CryptDestroyHash](https://learn.microsoft.com/en-us/windows/win32/api/wincrypt/nf-wincrypt-cryptdestroyhash)

---

<p align="right">
  <em>Last Updated: November 2025</em>
</p>
