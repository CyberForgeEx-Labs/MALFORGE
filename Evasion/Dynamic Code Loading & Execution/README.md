<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/Dynamic%20Code%20Loading%20%26%20Execution/Dynamic_Code_Loading.c">Dynamic Code Loading APIs Demonstration.</a>
</h1>

## Overview
This program demonstrates the legitimate Windows mechanism for embedding and extracting arbitrary binary data (payloads, configurations, scripts, shellcode, certificates, etc.) directly inside a PE executable using resources. This technique is heavily used in real-world malware, droppers, crypters, and red team tools to achieve fileless-like behavior without additional files on disk.

Pure Win32 API usage – no external dependencies.  
For **educational purposes, authorized penetration testing, red teaming, malware analysis, and reverse engineering training only**.

---
## Core APIs Demonstrated
| API                  | Purpose                                          | Sophistication |
|----------------------|---------------------------------------------------|----------------|
| `GetModuleHandle`    | Obtain handle to current executable              | Basic          |
| `FindResource`       | Locate embedded resource by ID and type          | Intermediate   |
| `LoadResource`       | Load resource into process memory                | Intermediate   |
| `SizeofResource`     | Query exact size of embedded data                | Intermediate   |
| `LockResource`       | Obtain direct pointer to resource data           | Advanced       |

---
## 1. [FindResource + LoadResource + LockResource](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/Dynamic%20Code%20Loading%20%26%20Execution/Dynamic_Code_Loading.c#L4)  
### Description
Locates a resource (e.g., `RT_RCDATA`, `RT_BITMAP`, custom type) embedded in the current executable, loads it into memory, and locks it to obtain a raw pointer to the data – without ever writing to disk.
### Key Features
- Zero disk footprint after compilation
- Direct in-memory access to embedded payloads
- Resources automatically freed on process exit
- Supports any data type (binary, text, shellcode, DLLs, configs)
### Use Cases
- Fileless malware & droppers
- Embedding encrypted payloads
- Storing configuration or C2 data
- Reflective DLL loading (with additional code)
- Anti-forensics (no secondary stage file)

---
## [2. Resource Embedding Example (.rc file)](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/Dynamic%20Code%20Loading%20%26%20Execution/resources.rc)
To embed data, create a resource script (`resources.rc`):
```rc
101 RCDATA "payload.bin"
// or
101 RCDATA "config.txt"
// or
101 RCDATA DISCARDABLE "encrypted_shellcode.bin"
```
Compile with:
```bash
rc.exe resources.rc
cl.exe ResourceDemo.c resources.res
```

---
## Full Workflow
```
Source Code
    ↓
Embed payload → .rc file → compiled into .exe
                     ↓
             Executable contains raw binary data
                     ↓
          FindResource → LoadResource → LockResource
                     ↓
                Direct pointer to data in memory
                     ↓
           Use payload without touching disk
```

---
## Advantages (Why Attackers Love This)
- Bypasses most AV file-based scanning
- No secondary stage file to detect
- Survives basic forensic timeline analysis
- Works even on restricted write environments
- Can be combined with encryption/XOR for evasion

---
## Compilation Instructions
### With embedded resource (recommended)
```bash
# 1. Create resources.rc containing your payload
echo 101 RCDATA "payload.bin" > resources.rc

# 2. Compile
rc.exe resources.rc
cl.exe Resource_Loading_Demo.c resources.res
```

### Without resource (demo mode – shows error handling)
```bash
cl.exe Resource_Loading_Demo.c
```

### (will print "Resource not found" – expected)
```

### MinGW / GCC
```bash
windres resources.rc -O coff -o resources.res
gcc Resource_Loading_Demo.c resources.res -o ResourceDemo.exe
```

---
## Required Libraries
- Only `kernel32.lib` (linked automatically)
- No external dependencies

---
## API References
- [FindResource](https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-findresourcea)
- [LoadResource](https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-loadresource)
- [LockResource](https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-lockresource)
- [SizeofResource](https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-sizeofresource)

---
<p align="right">
  Last Updated: November 2025
</p>
