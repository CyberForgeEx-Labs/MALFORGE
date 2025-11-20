<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/Process%20Creation%20%26%20Injection/Process_Creation_Injection.c">Windows Process Creation & DLL Injection APIs Demonstration</a>
</h1>

## Overview
This program demonstrates advanced Windows process creation and DLL injection techniques using the undocumented `CreateProcessInternalW` API. It explores low-level process manipulation, memory allocation in remote processes, and dynamic library loading core concepts in Windows internals and security research. Designed exclusively for **educational and authorized security research**, this tool helps understand process injection mechanics used in red teaming, malware analysis, penetration testing, and defensive security research.

---
## Core APIs Demonstrated
| API                            | Purpose                                      | Sophistication |
|--------------------------------|----------------------------------------------|----------------|
| `CreateProcessInternalW`       | Undocumented process creation API            | Advanced       |
| `VirtualAllocEx`               | Allocates memory in remote process           | Advanced       |
| `WriteProcessMemory`           | Writes data to remote process memory         | Advanced       |
| `CreateRemoteThread`           | Executes code in remote process              | Advanced       |
| `LoadLibrary`                  | Loads kernel32.dll to resolve APIs           | Intermediate   |
| `GetProcAddress`               | Resolves function addresses dynamically      | Intermediate   |
| `ResumeThread`                 | Resumes suspended thread execution           | Intermediate   |
| `WaitForSingleObject`          | Synchronizes thread completion               | Intermediate   |

---
## 1. [CreateProcessInternalW (Undocumented API)](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/Process%20Creation%20%26%20Injection/Process_Creation_Injection.c#L48)
### Description
Uses the undocumented `CreateProcessInternalW` function from kernel32.dll to create a new process in a suspended state. This API provides lower-level control than the standard `CreateProcess` family and is used internally by Windows. The process is created suspended to allow pre-execution modification (DLL injection).
### Key Features
- Direct access to internal process creation mechanism
- Creates process in suspended state (`CREATE_SUSPENDED`)
- Bypasses some higher-level process creation hooks
- Demonstrates undocumented API usage patterns
### Use Cases
- Low-level process manipulation research
- Understanding Windows process creation internals
- Advanced debugging and instrumentation
- Security research and evasion technique analysis
- Malware behavior analysis (defensive)

---
## 2. [VirtualAllocEx + WriteProcessMemory](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/Process%20Creation%20%26%20Injection/Process_Creation_Injection.c#L77)
### Description
Allocates memory in the target process's address space using `VirtualAllocEx` and writes the DLL path into that allocated region using `WriteProcessMemory`. This establishes the payload location for subsequent injection.
### Key Features
- Remote memory allocation with read/write permissions
- Cross-process memory writing
- Proper memory protection flags (`PAGE_READWRITE`)
- Unicode string handling for DLL paths
### Use Cases
- DLL injection frameworks
- Process memory manipulation tools
- Code cave injection research
- Shellcode injection techniques
- Security tool development

---
## 3. [CreateRemoteThread + LoadLibraryW](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/Process%20Creation%20%26%20Injection/Process_Creation_Injection.c#L23)
### Description
Creates a new thread in the remote process that executes `LoadLibraryW` with the injected DLL path as the parameter. This is the classic DLL injection technique, forcing the target process to load arbitrary code.
### Key Features
- Remote thread creation in target process
- Leverages legitimate Windows loader (`LoadLibraryW`)
- Synchronous injection with thread completion waiting
- Clean thread handle management
### Use Cases
- DLL injection for hooking/instrumentation
- Game modding and trainer development
- Security research and red teaming
- Process monitoring and debugging tools
- Understanding injection detection mechanisms

---
## 4. [Dynamic API Resolution](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/Process%20Creation%20%26%20Injection/Process_Creation_Injection.c#L29)
### Description
Dynamically resolves the undocumented `CreateProcessInternalW` function address using `GetProcAddress`. Demonstrates runtime API discovery, a common technique in both legitimate software and malware.
### Key Features
- Runtime function pointer resolution
- Safe fallback with error checking
- Typedef-based function pointer casting
- Module handle management
### Use Cases
- Bypassing static API analysis
- Version-agnostic tool development
- Understanding malware API obfuscation
- Dynamic instrumentation frameworks

---
## 5. [Process Suspension & Resumption](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/Process%20Creation%20%26%20Injection/Process_Creation_Injection.c#L107)
### Description
Creates the target process in a suspended state, performs injection operations, then resumes the main thread using `ResumeThread`. This ensures the DLL is loaded before the process executes its normal code path.
### Key Features
- Controlled process execution lifecycle
- Pre-execution injection capability
- Thread state manipulation
- Synchronization primitives usage
### Use Cases
- Early-stage process instrumentation
- Anti-debugging technique analysis
- Process hollowing research
- Secure process initialization

---

## Injection Flow
```
Load kernel32.dll → Resolve CreateProcessInternalW
         ↓
Create Process (SUSPENDED) → notepad.exe
         ↓
VirtualAllocEx (allocate memory in target)
         ↓
WriteProcessMemory (write DLL path to target)
         ↓
GetProcAddress (resolve LoadLibraryW address)
         ↓
CreateRemoteThread (execute LoadLibraryW in target)
         ↓
WaitForSingleObject (wait for injection to complete)
         ↓
ResumeThread (resume main thread)
         ↓
Target process runs with injected DLL loaded
```

---
## Required Privileges
| Privilege                     | Purpose                              |
|------------------------------|--------------------------------------|
| `SeDebugPrivilege`           | Required for some process operations |
| Administrator Rights         | Process creation and memory access   |

---
## Compilation Instructions
### Visual Studio / cl.exe
```bash
cl.exe ProcessInjection.c /link kernel32.lib
```

### MinGW / GCC
```bash
gcc ProcessInjection.c -o ProcessInjection.exe -lkernel32
```

### Run as Administrator (Required)
```cmd
ProcessInjection.exe
```

---
## Configuration
Before running, modify the DLL path in the code:
```c
wchar_t dllPath[] = L"C:\\path\\to\\your\\dll.dll";  // Replace with actual DLL path
```

The target process can also be changed:
```c
wchar_t appName[] = L"C:\\Windows\\System32\\notepad.exe";  // Replace with target
```

---
## Required Libraries
- `kernel32.lib` – Process creation, memory, and threading APIs.

---

## Security Considerations
⚠️ **WARNING**: This code demonstrates techniques commonly used in malware and can be flagged by antivirus software. Use only in controlled, authorized environments.

---
## API References
- [CreateProcessW](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessw) (Documented equivalent)
- [VirtualAllocEx](https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualallocex)
- [WriteProcessMemory](https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-writeprocessmemory)
- [CreateRemoteThread](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createremotethread)
- [LoadLibraryW](https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-loadlibraryw)
- [GetProcAddress](https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getprocaddress)

---
## Limitations & Notes
- `CreateProcessInternalW` is undocumented and may change between Windows versions.
- Some security solutions actively monitor and block these techniques.
- The injected DLL must be compiled for the same architecture as the target process (x86/x64).
- This technique leaves forensic artifacts (remote threads, memory allocations).

---
<p align="right">
  Last Updated: November 2025
</p>
