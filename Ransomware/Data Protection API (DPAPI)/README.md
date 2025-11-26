<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Ransomware/Data%20Protection%20API%20(DPAPI)/Data_Protection.c">Windows Data Protection API (DPAPI) Demonstration</a>
</h1>

## Overview
This program demonstrates **comprehensive usage** of the Windows Data Protection API (DPAPI) to perform **user-transparent encryption and decryption** of sensitive data without manual key management.

It showcases best-practice patterns used in real Windows software for:
- Protecting passwords, credentials, and secrets
- Using entropy for additional security layers
- Understanding user-scope vs machine-scope protection
- Handling both string and binary data protection
- Leveraging audit flags for security monitoring

This is the exact methodology used by Windows applications including browsers, credential managers, and enterprise software for protecting local secrets at rest.

---
## Core APIs Demonstrated
| API                  | Purpose                                      | Use Case                                      |
|----------------------|----------------------------------------------|-----------------------------------------------|
| `CryptProtectData`   | Encrypt data using DPAPI                     | Protect sensitive data tied to user/machine   |
| `CryptUnprotectData` | Decrypt DPAPI-protected data                 | Recover original plaintext securely           |
| `LocalFree`          | Free DPAPI-allocated memory buffers          | Proper cleanup of encrypted/decrypted blobs   |

---
## 1. [Basic Data Protection](https://github.com/CyberForgeEx/MALFORGE/blob/main/Ransomware/Data%20Protection%20API%20(DPAPI)/Data_Protection.c#L19)
**Description:** Demonstrates fundamental DPAPI encryption and decryption workflow.  
**Key Features:**
- Simple password/secret protection
- Optional description metadata storage
- Automatic key management by Windows
- No explicit key handling required

**Use Cases:**
- Storing application passwords
- Protecting API keys and tokens
- Securing configuration secrets

---
## 2. [Protection with Entropy](https://github.com/CyberForgeEx/MALFORGE/blob/main/Ransomware/Data%20Protection%20API%20(DPAPI)/Data_Protection.c#L85)
**Description:** Adds an additional entropy parameter (secondary secret) for enhanced security.  
**Key Features:**
- Entropy acts as application-specific salt
- Decryption fails without matching entropy
- Prevents cross-application data access
- Demonstrates entropy requirement validation

**Use Cases:**
- Multi-tenant applications with isolated secrets
- Adding application-specific protection layers
- Preventing unauthorized decryption by other apps

---
## 3. [Protection Scope - User vs Machine](https://github.com/CyberForgeEx/MALFORGE/blob/main/Ransomware/Data%20Protection%20API%20(DPAPI)/Data_Protection.c#L169)
**Description:** Compares user-specific protection (default) with machine-wide protection.  
**Key Features:**
- Default mode: Only encrypting user can decrypt
- `CRYPTPROTECT_LOCAL_MACHINE`: Any valid user on machine can decrypt
- Demonstrates both scope configurations
- Shows encrypted blob size differences

**Use Cases:**
- User-scope: Browser passwords, personal credentials
- Machine-scope: Shared service accounts, system-wide secrets

---
## 4. [Binary Data Protection](https://github.com/CyberForgeEx/MALFORGE/blob/main/Ransomware/Data%20Protection%20API%20(DPAPI)/Data_Protection.c#L243)
**Description:** Protects raw binary data such as encryption keys or certificates.  
**Key Features:**
- Handles arbitrary binary blobs (not just strings)
- Preserves exact byte integrity
- Memory comparison verification
- Suitable for cryptographic material storage

**Use Cases:**
- Protecting encryption keys at rest
- Securing certificate private keys
- Storing binary tokens or session data

---
## 5. [Advanced Flags - Audit Support](https://github.com/CyberForgeEx/MALFORGE/blob/main/Ransomware/Data%20Protection%20API%20(DPAPI)/Data_Protection.c#L313)
**Description:** Demonstrates DPAPI audit flag for security event logging.  
**Key Features:**
- `CRYPTPROTECT_AUDIT` generates Windows Security events
- Enables compliance and forensic tracking
- Integrates with Windows Event Viewer
- No impact on encryption/decryption functionality

**Use Cases:**
- Enterprise compliance requirements
- Security incident investigation
- Access pattern monitoring

---
## Program Workflow
1. **Basic Protection**: Encrypt → Decrypt password with description metadata
2. **Entropy Demo**: Encrypt with entropy → Fail without entropy → Succeed with entropy
3. **Scope Demo**: Compare user-specific vs machine-wide protection
4. **Binary Demo**: Protect and verify 32-byte binary key integrity
5. **Audit Demo**: Enable audit logging for compliance tracking
6. **Cleanup**: Properly free all DPAPI-allocated buffers

Confirms full round-trip integrity across all scenarios.

---
## Compilation Instructions
### Visual Studio / cl.exe
```bash
cl.exe DPAPI_Demo.c /link crypt32.lib
```

### MinGW / GCC
```bash
gcc DPAPI_Demo.c -o dpapi_demo.exe -lcrypt32
```

### Run (No admin rights required for user-scope)
```cmd
dpapi_demo.exe
```

---
## Required Libraries
- `crypt32.lib` – DPAPI functions (`CryptProtectData`, `CryptUnprotectData`)

> Note: DPAPI is the **recommended** approach for protecting user secrets on Windows. It handles all key management internally using user credentials or machine keys.

---
## System Requirements
- **OS:** Windows 2000 or later (including Windows 11)
- **Architecture:** x86 or x64
- **Permissions:** Standard user for user-scope; admin may be needed for machine-scope in some scenarios

---
## DPAPI Security Model
| Protection Scope | Key Source | Who Can Decrypt |
|------------------|------------|-----------------|
| User (Default)   | User's DPAPI master key | Only the same user on same machine |
| Local Machine    | Machine DPAPI key | Any authenticated user on same machine |

**Key Storage:**
- User keys: `%APPDATA%\Microsoft\Protect\<SID>\`
- Machine keys: `%WINDIR%\System32\Microsoft\Protect\`

---
## Error Handling
Comprehensive error checking with `GetLastError()` for:
- Encryption failures (invalid parameters, access denied)
- Decryption failures (wrong user, missing entropy, corrupted data)
- Memory allocation issues
- Graceful cleanup even on error paths

---
## Helper Function
**`PrintHex`**: Utility function to display binary data in hexadecimal format with 16-byte line wrapping for readability.

---
## API References
- [CryptProtectData](https://learn.microsoft.com/en-us/windows/win32/api/dpapi/nf-dpapi-cryptprotectdata)
- [CryptUnprotectData](https://learn.microsoft.com/en-us/windows/win32/api/dpapi/nf-dpapi-cryptunprotectdata)
- [DATA_BLOB Structure](https://learn.microsoft.com/en-us/previous-versions/windows/desktop/legacy/aa381414(v=vs.85))

---
## Security Considerations
| Consideration | Recommendation |
|---------------|----------------|
| Entropy Usage | Always use application-specific entropy for sensitive data |
| Scope Selection | Use user-scope unless machine-wide access is explicitly required |
| Memory Handling | Zero sensitive buffers before freeing (not shown in demo) |
| Audit Logging | Enable `CRYPTPROTECT_AUDIT` for compliance-sensitive applications |

---
<p align="right">
  Last Updated: November 2025
</p>
