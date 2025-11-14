<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Injection/DLL%20Loading%20%26%20Module%20Management/DLL_Loading_module_management.c">DLL Loading & Module Management Techniques</a>
</h1>

## Overview
This educational tool demonstrates Windows DLL loading and module management APIs for legitimate security research, system administration, and software development purposes. This implementation focuses on **safe inspection and understanding** of module loading mechanisms without performing malicious injection or unauthorized code execution. These APIs are essential for understanding Windows process architecture, building diagnostic tools, plugin systems, and defensive security research.

---

## Core APIs

| API | Purpose | Sophistication |
|-----|---------|----------------|
| `LoadLibraryA` | Loads DLL into process | Basic |
| `LoadLibraryExA` | Extended DLL loading with flags | Intermediate |
| `LdrLoadDll` | Native NT-level DLL loader | Advanced |
| `GetModuleHandleA` | Retrieves handle to loaded module | Basic |

---

## 1. [LoadLibraryA](https://github.com/CyberForgeEx/MALFORGE/blob/main/Injection/DLL%20Loading%20%26%20Module%20Management/DLL_Loading_module_management.c#L33)
### Description
Loads the specified dynamic-link library (DLL) module into the address space of the calling process. This is the most common method for loading DLLs at runtime. The function searches for the DLL using a defined search order, maps it into memory, resolves import dependencies, and executes DllMain with DLL_PROCESS_ATTACH. Returns a module handle that can be used with GetProcAddress to retrieve function addresses. Essential for plugin architectures, optional features, and runtime extension loading.

### Key Features
- Automatic dependency resolution
- Standard DLL search order
- Reference counting mechanism
- DllMain execution
- Thread-safe loading
- Import address table resolution

### Use Cases
- Loading plugins and extensions
- Runtime feature loading
- Dynamic function binding
- Optional dependency loading
- Testing and development tools
- System library access

---

## 2. [LoadLibraryExA](https://github.com/CyberForgeEx/MALFORGE/blob/main/Injection/DLL%20Loading%20%26%20Module%20Management/DLL_Loading_module_management.c#L71)
### Description
Extended version of LoadLibrary that provides additional control through flags parameter. Allows loading DLLs with modified behavior such as skipping import resolution (DONT_RESOLVE_DLL_REFERENCES), loading as data file for resource inspection (LOAD_LIBRARY_AS_DATAFILE), or altering the search path. Critical for scenarios requiring precise control over loading behavior, resource-only access, or security-conscious loading. Flags can be combined for complex loading requirements.

### Key Features
- Flag-based behavior control
- Resource-only loading option
- Import resolution control
- Modified search paths
- Code authorization bypass
- Image resource loading

### Use Cases
- Resource inspection tools
- Security-conscious loading
- Performance optimization
- Debugging and analysis
- Custom search path scenarios
- Delayed import resolution

---

## 3. [LdrLoadDll](https://github.com/CyberForgeEx/MALFORGE/blob/main/Injection/DLL%20Loading%20%26%20Module%20Management/DLL_Loading_module_management.c#L117)
### Description
Native NT API function for loading DLLs at the lowest level before reaching Win32 LoadLibrary functions. Part of the Native API (NTAPI) exposed by ntdll.dll. Uses UNICODE_STRING structures and NTSTATUS return codes instead of Win32 conventions. Provides the most direct control over DLL loading and is used internally by all higher-level loading functions. Understanding LdrLoadDll is crucial for advanced system programming, security research, and reverse engineering Windows internals.

### Key Features
- Native NT-level loading
- UNICODE_STRING parameters
- NTSTATUS return codes
- Lowest-level loader access
- Direct kernel interface
- No Win32 wrapper overhead

### Use Cases
- Advanced system programming
- Security research and analysis
- Reverse engineering studies
- Kernel-level understanding
- Custom loader development
- Bypass detection research

---

## 4. [GetModuleHandleA](https://github.com/CyberForgeEx/MALFORGE/blob/main/Injection/DLL%20Loading%20%26%20Module%20Management/DLL_Loading_module_management.c#L175)
### Description
Retrieves a module handle for a DLL that is already loaded in the calling process's address space. Unlike LoadLibrary, it does NOT load the module or increment its reference count. Returns NULL if the module is not currently loaded. Special behavior: passing NULL retrieves a handle to the current executable. Essential for checking module presence, obtaining base addresses, and working with already-loaded modules without affecting reference counts or triggering additional loads.

### Key Features
- Query-only operation
- No reference count change
- No loading performed
- Current process scope
- NULL = executable handle
- Fast handle retrieval

### Use Cases
- Module presence verification
- Base address retrieval
- Function address resolution
- Avoiding redundant loads
- Module enumeration support
- Diagnostic information

---

## DLL Search Order

### Standard Search Sequence
When LoadLibrary is called without a full path, Windows searches in this order:

```
1. The directory of the application executable
2. The system directory (C:\Windows\System32)
3. The 16-bit system directory (C:\Windows\System)
4. The Windows directory (C:\Windows)
5. The current directory
6. Directories listed in PATH environment variable
```

## DLL Loading Flow Diagram

### LoadLibrary Process
```
User Application
      |
      | LoadLibraryA("mydll.dll")
      v
Kernel32.dll (Win32 Layer)
      |
      | Convert to Unicode
      | Apply search order
      v
LdrLoadDll (NTDLL.dll - Native API)
      |
      | Parse PE headers
      | Allocate memory
      | Map sections
      v
Load Dependencies (Recursive)
      |
      | Resolve imports
      | Fix relocations
      v
Execute DllMain(DLL_PROCESS_ATTACH)
      |
      | Initialize DLL
      v
Return Module Handle
```

### Module Memory Layout
```
Module Base (HMODULE)
┌────────────────────────┐
│ DOS Header             │ <- MZ signature
├────────────────────────┤
│ PE Header              │ <- PE signature
├────────────────────────┤
│ Section Headers        │
├────────────────────────┤
│ .text (Code)           │ <- Executable code
├────────────────────────┤
│ .rdata (Read-only)     │ <- Constants, imports
├────────────────────────┤
│ .data (Initialized)    │ <- Global variables
├────────────────────────┤
│ .rsrc (Resources)      │ <- Icons, strings, etc.
├────────────────────────┤
│ .reloc (Relocations)   │ <- Address fixups
└────────────────────────┘
```

---


### Reference Count Visualization
```
Time    Action                    Ref Count    Status
----------------------------------------------------------
T1      LoadLibrary              1            Loaded
T2      LoadLibrary (same)       2            Loaded
T3      GetModuleHandle          2            Loaded (no change)
T4      FreeLibrary              1            Loaded
T5      FreeLibrary              0            Unloaded
T6      GetModuleHandle          -            Returns NULL
```

---

## Common Errors and Solutions

### Error Scenarios
| Error | Cause | Solution |
|-------|-------|----------|
| Module not found (126) | DLL not in search path | Use full path or check search order |
| Access denied (5) | Insufficient permissions | Run with appropriate privileges |
| Invalid module (193) | Wrong architecture (32/64-bit) | Use matching architecture DLL |
| Dependencies missing (127) | Required DLLs not found | Install dependencies, check PATH |
| Already loaded | Module exists in process | Use GetModuleHandle instead |
| Invalid handle | Module unloaded or never loaded | Check handle before use |
| Memory allocation failed | Insufficient memory | Free memory, check system resources |
| DllMain failed | Initialization error | Check DLL logs, debug DllMain |

---

## Required Libraries
- `kernel32.lib` - LoadLibrary, GetModuleHandle functions
- `ntdll.lib` - Native API functions (LdrLoadDll)
- `psapi.lib` - Process and module enumeration (EnumProcessModules)

---

## Compilation Instructions

### Microsoft Visual C++
```console
cl.exe DLL_Module_Inspector.c
```

### Microsoft Visual C++ (with Psapi for enumeration)
```console
cl.exe DLL_Module_Inspector.c psapi.lib
```

### MinGW GCC (with Psapi)
```console
gcc DLL_Module_Inspector.c -o DLL_Module_Inspector.exe -lpsapi
```

### Build with Debugging Symbols
```console
cl.exe /Zi /DEBUG DLL_Module_Inspector.c
```

---

## API References
- MSDN: [LoadLibraryA](https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-loadlibrarya)
- MSDN: [LoadLibraryExA](https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-loadlibraryexa)
- MSDN: [GetModuleHandleA](https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulehandlea)
- MSDN: [FreeLibrary](https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-freelibrary)
- MSDN: [GetProcAddress](https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getprocaddress)
- MSDN: [EnumProcessModules](https://docs.microsoft.com/en-us/windows/win32/api/psapi/nf-psapi-enumprocessmodules)
- MSDN: [Dynamic-Link Library Best Practices](https://docs.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-best-practices)
- MSDN: [Dynamic-Link Library Security](https://docs.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-security)
- Undocumented NT: [LdrLoadDll](http://undocumented.ntinternals.net/)

---

## Additional Resources

### Learning Materials
- [Windows Internals (Book)](https://docs.microsoft.com/en-us/sysinternals/resources/windows-internals) - Comprehensive Windows architecture
- [PE Format Specification](https://docs.microsoft.com/en-us/windows/win32/debug/pe-format) - Portable Executable structure
- [DLL Best Practices](https://docs.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-best-practices) - Microsoft guidelines
- [Native API Reference](http://undocumented.ntinternals.net/) - Undocumented functions

### Security Research
- [DLL Hijacking Attacks](https://attack.mitre.org/techniques/T1574/001/) - MITRE ATT&CK framework
- [DLL Search Order](https://docs.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-search-order) - Official search sequence
- [Code Signing](https://docs.microsoft.com/en-us/windows/win32/seccrypto/cryptography-tools) - Verification techniques

---

<p align="right">
  Last Updated: November 2025
</p>
