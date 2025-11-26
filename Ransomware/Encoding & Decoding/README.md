<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Ransomware/Encoding%20%26%20Decoding/Encoding_Decoding.c">Windows Cryptographic Encoding/Decoding Demonstration</a>
</h1>

## Overview
This program demonstrates **comprehensive usage** of the Windows Cryptographic String Encoding APIs to perform **binary-to-string and string-to-binary conversions** using various encoding formats including Base64 and Hexadecimal.

It showcases best-practice patterns used in real Windows software for:
- Base64 encoding and decoding of binary data
- Hexadecimal encoding and decoding operations
- Multiple encoding format comparisons
- Proper two-pass buffer sizing methodology
- Data integrity verification after round-trip conversions

This is the exact methodology used by Windows applications for encoding binary data in configuration files, network protocols, certificate handling, and data serialization.

---
## Core APIs Demonstrated
| API                    | Purpose                                      | Use Case                                      |
|------------------------|----------------------------------------------|-----------------------------------------------|
| `CryptBinaryToString`  | Convert binary data to encoded string        | Base64/Hex encoding for storage or transmission |
| `CryptStringToBinary`  | Convert encoded string back to binary        | Decode Base64/Hex data to original bytes      |

---
## 1. [Base64 Encoding/Decoding](https://github.com/CyberForgeEx/MALFORGE/blob/main/Ransomware/Encoding%20%26%20Decoding/Encoding_Decoding.c#L19)
**Description:** Demonstrates complete Base64 encode and decode cycle with verification.  
**Key Features:**
- Two-pass approach: size query then actual conversion
- `CRYPT_STRING_NOCRLF` flag for clean output without line breaks
- Full round-trip verification with `memcmp`
- Reports skipped characters and detected format flags

**Use Cases:**
- Encoding binary data for JSON/XML storage
- Email attachment encoding (MIME)
- Embedding binary in text protocols
- Certificate and key encoding (PEM format)

---
## 2. [Hexadecimal Encoding/Decoding](https://github.com/CyberForgeEx/MALFORGE/blob/main/Ransomware/Encoding%20%26%20Decoding/Encoding_Decoding.c#L102)
**Description:** Demonstrates raw hexadecimal encoding and decoding operations.  
**Key Features:**
- `CRYPT_STRING_HEXRAW` for compact hex output
- No separators or formatting characters
- Ideal for hash values and binary identifiers
- Complete integrity verification

**Use Cases:**
- Displaying cryptographic hashes (MD5, SHA-256)
- Binary data in log files
- Network packet inspection tools
- Registry binary value representation

---
## 3. [Multiple Encoding Formats Comparison](https://github.com/CyberForgeEx/MALFORGE/blob/main/Ransomware/Encoding%20%26%20Decoding/Encoding_Decoding.c#L169)
**Description:** Side-by-side comparison of all available encoding formats.  
**Key Features:**
- Demonstrates five different encoding styles
- Shows formatted vs raw output differences
- Illustrates CRLF behavior options
- Enables format selection for specific needs

**Formats Demonstrated:**
| Format Flag | Description | Output Style |
|-------------|-------------|--------------|
| `CRYPT_STRING_BASE64` | Standard Base64 | With line breaks (76 char lines) |
| `CRYPT_STRING_BASE64 \| NOCRLF` | Base64 compact | Single line, no breaks |
| `CRYPT_STRING_HEX` | Formatted hex | Space-separated, formatted |
| `CRYPT_STRING_HEXRAW` | Raw hex | Continuous string, no spaces |
| `CRYPT_STRING_HEXASCII` | Hex + ASCII | Hex dump with ASCII sidebar |

---
## Program Workflow
1. **Base64 Demo**: Encode "Hello World!" → Base64 string → Decode → Verify match
2. **Hex Demo**: Encode binary pattern → Hex string → Decode → Verify match
3. **Format Comparison**: Display same data in 5 different encoding formats
4. **Verification**: Confirm data integrity after each round-trip conversion
5. **Cleanup**: Properly free all allocated buffers


This pattern prevents buffer overflows and is the **recommended approach** for all Windows APIs that return variable-length data.

---
## Compilation Instructions
### Visual Studio / cl.exe
```bash
cl.exe Encoding_Decoding.c /link crypt32.lib
```

### MinGW / GCC
```bash
gcc Encoding_Decoding.c -o crypto_encoding.exe -lcrypt32
```

### Run (No admin rights required)
```cmd
crypto_encoding.exe
```

---
## Required Libraries
- `crypt32.lib` – Cryptographic string encoding functions

> Note: These APIs are part of the stable Windows Cryptography API and are fully supported on all modern Windows versions.

---
## System Requirements
- **OS:** Windows XP or later (including Windows 11)
- **Architecture:** x86 or x64
- **Permissions:** Standard user (no elevation required)

---
## Encoding Format Reference
| Flag Constant | Value | Description |
|---------------|-------|-------------|
| `CRYPT_STRING_BASE64` | 0x01 | Base64 with CRLF line breaks |
| `CRYPT_STRING_HEX` | 0x04 | Formatted hexadecimal |
| `CRYPT_STRING_HEXRAW` | 0x0C | Raw hexadecimal (no formatting) |
| `CRYPT_STRING_HEXASCII` | 0x05 | Hex dump with ASCII representation |
| `CRYPT_STRING_NOCRLF` | 0x40000000 | Modifier: suppress line breaks |
| `CRYPT_STRING_NOCR` | 0x80000000 | Modifier: use LF only (no CR) |

---
## Error Handling
Comprehensive error checking with `GetLastError()` for:
- Invalid encoding format specifications
- Buffer size calculation failures
- Memory allocation failures
- Malformed input string detection
- Graceful cleanup on all error paths

---
## Helper Function
**`PrintHex`**: Utility function to display binary data in formatted hexadecimal with configurable label and 16-byte line wrapping for readability.

```c
void PrintHex(const char* label, BYTE* data, DWORD len);
```

---
## Common Use Cases
| Scenario | Recommended Format |
|----------|-------------------|
| JSON/XML data embedding | `CRYPT_STRING_BASE64 \| CRYPT_STRING_NOCRLF` |
| PEM certificate files | `CRYPT_STRING_BASE64` (with headers) |
| Log file binary data | `CRYPT_STRING_HEXRAW` |
| Debug/inspection output | `CRYPT_STRING_HEXASCII` |
| Network protocol encoding | `CRYPT_STRING_BASE64 \| CRYPT_STRING_NOCRLF` |
| Hash value display | `CRYPT_STRING_HEXRAW` |

---
## API References
- [CryptBinaryToString](https://learn.microsoft.com/en-us/windows/win32/api/wincrypt/nf-wincrypt-cryptbinarytostringa)
- [CryptStringToBinary](https://learn.microsoft.com/en-us/windows/win32/api/wincrypt/nf-wincrypt-cryptstringtobinarya)
- [Encoding Format Constants](https://learn.microsoft.com/en-us/windows/win32/api/wincrypt/nf-wincrypt-cryptbinarytostringa#parameters)

---
## Security Considerations
| Consideration | Recommendation |
|---------------|----------------|
| Encoding ≠ Encryption | Base64/Hex provides NO security - use for transport only |
| Input Validation | Always validate decoded data before use |
| Buffer Management | Use two-pass sizing to prevent overflows |
| Memory Cleanup | Zero sensitive buffers before freeing |

---
<p align="right">
  Last Updated: November 2025
</p>
