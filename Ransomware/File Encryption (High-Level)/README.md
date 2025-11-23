<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Ransomware/File%20Encryption%20(High-Level)/File_Encryption.c">Windows File Encryption (EFS) Demonstration</a>
</h1>

## Overview

This program demonstrates **legitimate Windows Encrypting File System (EFS) APIs** commonly used for file protection, backup solutions, and security applications. The code showcases reading, encrypting, and decrypting files using Windows native encryption capabilities.

These techniques are used in real-world applications for:
- Data protection and confidentiality
- Secure file handling in enterprise environments
- EFS status verification and encryption management
- Error handling for encryption operations
---

## Core APIs Demonstrated

| API | Purpose | Use Case |
|-----|---------|----------|
| `FileEncryptionStatusA` | Query file encryption capabilities and status | Determine if file can be encrypted on current system |
| `EncryptFileA` | Encrypt a file using Windows EFS | Protect sensitive files with user's EFS certificate |
| `DecryptFileA` | Decrypt an EFS-encrypted file | Restore access to encrypted files |

---

## 1. [FileEncryptionStatusA](https://github.com/CyberForgeEx/MALFORGE/blob/main/Ransomware/File%20Encryption%20(High-Level)/File_Encryption.c#L42)

**Description:** Retrieves the encryption status and capabilities of a specified file.

**Key Features:**
- Checks if file is encryptable (FILE_ENCRYPTABLE flag)
- Detects read-only status (FILE_READ_ONLY flag)
- Indicates if directory encryption is disallowed (FILE_DIR_DISALLOWED flag)
- Works on NTFS filesystems

**Use Cases:**
- Verify encryption eligibility before attempting to encrypt.
- Check system/drive compatibility with EFS.
- Validate file permissions before encryption operations.

---

## 2. [EncryptFileA](https://github.com/CyberForgeEx/MALFORGE/blob/main/Ransomware/File%20Encryption%20(High-Level)/File_Encryption.c#L67)

**Description:** Encrypts a file using the Windows Encrypting File System (EFS).

**Key Features:**
- Encrypts file with user's EFS certificate.
- File remains accessible to the user without additional keys.
- Encrypted files can be decrypted by the file owner.
- Requires NTFS filesystem.

**Use Cases:**
- Protect sensitive data at rest
- Comply with data protection requirements
- Secure temporary files and configuration data.

**Common Errors:**
- `ERROR_NOT_SUPPORTED`: EFS not available or file not on NTFS
- `ERROR_ACCESS_DENIED`: Insufficient permissions or EFS rights
- `ERROR_FILE_NOT_FOUND`: Target file does not exist
- `ERROR_SHARING_VIOLATION`: File is currently in use

---

## 3. [DecryptFileA](https://github.com/CyberForgeEx/MALFORGE/blob/main/Ransomware/File%20Encryption%20(High-Level)/File_Encryption.c#L107)

**Description:** Decrypts an EFS-encrypted file.

**Key Features:**
- Removes encryption from file
- Requires ownership or EFS decryption rights
- Makes file accessible without EFS credentials
- Fast operation once decryption rights are verified

**Use Cases:**
- Restore access to encrypted files
- Migrate files to unencrypted storage
- Prepare files for backup or transfer

**Common Errors:**
- `ERROR_NOT_SUPPORTED`: File is not encrypted
- `ERROR_ACCESS_DENIED`: No decryption rights
- `ERROR_FILE_NOT_FOUND`: File does not exist
- `ERROR_SHARING_VIOLATION`: File is in use

---

## Program Workflow

The demonstration includes a complete workflow:

1. **Create Test File** - Generates a sample file with content
2. **Display Original Content** - Reads and displays file contents
3. **Check Encryption Status** - Queries file encryption capabilities
4. **Encrypt File** - Applies EFS encryption
5. **Verify Encryption** - Confirms encryption was successful
6. **Decrypt File** - Removes encryption
7. **Display Decrypted Content** - Verifies file accessibility
8. **Cleanup** - Removes test file

---

## Compilation Instructions

### Visual Studio / cl.exe
```bash
cl.exe file_encryption.c /link advapi32.lib crypt32.lib rpcrt4.lib
```

### MinGW / GCC
```bash
gcc file_encryption.c -o file_encryption.exe -ladvapi32 -lcrypt32 -lrpcrt4
```

### Run (No admin required for reading status; encryption may require elevated privileges)
```cmd
file_encryption.exe
```

---

## Required Libraries

- `advapi32.lib` - Windows encryption APIs.
- `crypt32.lib` - Cryptography functions.
- `rpcrt4.lib` - RPC runtime.

---

## System Requirements

- **OS:** Windows XP or later
- **Filesystem:** NTFS (EFS only works on NTFS volumes)
- **Permissions:** User must have EFS rights for encryption/decryption.
- **Architecture:** x86 or x64.

---

## Error Handling

The program provides detailed error interpretation for common failure scenarios:

- File not found or inaccessible
- Insufficient permissions
- EFS not supported on filesystem
- File currently in use by another process.
- Unknown system errors.

---

## API References

- [FileEncryptionStatusA](https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-fileencryptionstatusa)
- [EncryptFileA](https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-encryptfilea)
- [DecryptFileA](https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-decryptfilea)

---
<p align="right">
  Last Updated: November 2025
</p>
