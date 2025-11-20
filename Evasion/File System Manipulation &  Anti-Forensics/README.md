<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/File%20System%20Manipulation%20%26%20%20Anti-Forensics/File_System_manipulation.c">Windows File System APIs & Timestomping Demonstration</a>
</h1>

## Overview
This program demonstrates legitimate but powerful Windows File System APIs commonly abused in post-exploitation, anti-forensics, and evasion techniques with a strong focus on **timestomping**, file attribute manipulation, and memory mapped file operations. All functionality uses only official Win32 APIs.

Designed exclusively for **educational purposes, authorized penetration testing, red teaming, digital forensics training, and malware analysis research**.

---
## Core APIs Demonstrated
| API                    | Purpose                                      | Sophistication |
|------------------------|-----------------------------------------------|----------------|
| `CreateFile`           | Open/create file with full access control     | Basic          |
| `WriteFile`            | Write data to file                            | Basic          |
| `GetFileTime` / `SetFileTime` | Read and modify file timestamps (MACE) | Advanced       |
| `GetFileAttributesA` / `SetFileAttributesA` | Manipulate file attributes (Hidden, Read-only, System) | Intermediate   |
| `CreateFileMappingA`   | Create memory-mapped file object              | Advanced       |
| `MapViewOfFile`        | Map file contents into process memory         | Advanced       |
| `DeleteFileA`          | Delete files (bypassing some restrictions)    | Intermediate   |

---
## 1. [CreateFile + WriteFile](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/File%20System%20Manipulation%20%26%20%20Anti-Forensics/File_System_manipulation.c#L24)  
### Description
Creates a new file with `CREATE_ALWAYS` and writes sample data. Demonstrates basic file creation used as a foundation for further manipulation.
### Key Features
- Overwrites existing files safely
- Uses `GENERIC_WRITE` access
- Proper handle validation
### Use Cases
- Dropper payloads
- Log/file tampering
- Staging files in attacks

---
## 2. [GetFileTime + SetFileTime – Timestomping](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/File%20System%20Manipulation%20%26%20%20Anti-Forensics/File_System_manipulation.c#L60)  
### Description
Retrieves current Creation/Access/Modification (MACE) timestamps and overwrites all four values using `SetFileTime`. Classic **timestomping** technique to hide malicious file activity.
### Key Features
- Full control over Creation, Access, and Write times
- Sets timestamps to arbitrary past/future dates
- Bypasses timeline analysis in many forensic tools
### Use Cases
- Anti-forensic evasion
- Blending malicious files with legitimate ones
- Covering tracks after file drops

---
## 3. [SetFileAttributesA – Attribute Manipulation](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/File%20System%20Manipulation%20%26%20%20Anti-Forensics/File_System_manipulation.c#L74)  
### Description
Changes file attributes such as `HIDDEN`, `READONLY`, `SYSTEM`. Commonly used to conceal malicious payloads from users and basic scanners.
### Key Features
- Applies multiple attributes at once
- Reads current state before/after modification
- Real-world stealth technique
### Use Cases
- Hiding droppers or backdoors
- Evading user detection
- Mimicking system files

---
## 4. [CreateFileMappingA + MapViewOfFile](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/File%20System%20Manipulation%20%26%20%20Anti-Forensics/File_System_manipulation.c#L96)  
### Description
Creates a file mapping object and maps a file directly into memory. Demonstrates how malware can execute or read data without traditional file access patterns (useful for reflection or steganography).
### Key Features
- Full read/write memory mapping
- No intermediate disk reads after mapping
- Enables reflective loading techniques
### Use Cases
- Fileless malware techniques
- Reflective DLL injection helpers
- Covert data exfiltration channels

---
## 5. [DeleteFileA – Cleanup](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/File%20System%20Manipulation%20%26%20%20Anti-Forensics/File_System_manipulation.c#L131)  
### Description
Removes evidence files after demonstration. Shows how attackers clean up droppers, logs, or tools post-exploitation.
### Key Features
- Removes read-only attribute before deletion
- Graceful error handling
### Use Cases
- Self-deleting malware
- Anti-forensics cleanup phase

---
## Timestomping & Evasion Flow
```
Malicious File Dropped
        ↓
   SetFileTime() → Fake old timestamp
        ↓
SetFileAttributes() → HIDDEN + READONLY
        ↓
   Optional: MapViewOfFile → Fileless exec
        ↓
      DeleteFileA → Remove evidence
```

---
## Required Privileges (Minimal)
No administrative privileges required for basic operation.  
`SeBackupPrivilege` or `SeRestorePrivilege` may enhance certain operations but are **not required** here.

---
## Compilation Instructions
### Visual Studio / cl.exe
```bash
cl.exe FileSystemDemo.c /link advapi32.lib kernel32.lib
```

### MinGW / GCC
```bash
gcc FileSystemDemo.c -o FileSystemDemo.exe
```

### Run (No admin required for most features)
```cmd
FileSystemDemo.exe
```

---
## Required Libraries
- `kernel32.lib` – Core file and memory mapping APIs

---
## API References
- [SetFileTime](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-setfiletime)
- [SetFileAttributesA](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-setfileattributesa)
- [CreateFileMappingA](https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-createfilemappinga)
- [MapViewOfFile](https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-mapviewoffile)
- [GetFileTime](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfiletime)

---
<p align="right">
  Last Updated: November 2025
</p>
