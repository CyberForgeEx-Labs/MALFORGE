<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MalAPI-Arsenal/blob/main/Enumeration/Module-Driver%20Enumeration/Module_DriverEnum.c">Module and Driver Enumeration Techniques</a>
</h1>

## Overview
Module and driver enumeration provides fundamental Windows API capabilities for discovering, analyzing, and monitoring dynamically loaded libraries, executables, and device drivers. This documentation covers advanced APIs for enumerating loaded modules across processes, analyzing driver dependencies, and extracting detailed module information.

---

## Core APIs

| API | Purpose | Sophistication |
|-----|---------|----------------|
| `EnumDeviceDrivers` | Enumerate all device driver modules | Intermediate |
| `EnumProcessModulesEx` | Extended enumeration (32/64-bit support) | Intermediate |
| `Module32First` | Retrieve first module from snapshot | Basic |
| `Module32Next` | Retrieve next module from snapshot | Basic |
| `GetModuleBaseNameA` | Get module base name (ASCII) | Basic |
| `GetModuleInformation` | Get detailed module information | Intermediate |
| `GetDeviceDriverBaseNameA` | Get driver base name (ASCII) | Basic |

---

## EnumDeviceDrivers
### Description
Enumerates all device driver modules loaded in the system kernel, providing access to driver base addresses and driver object information.

### Key Features
- System-wide driver enumeration
- Access to all loaded device drivers
- Driver base address retrieval
- Minimal permission requirements
- Fast enumeration capability

### Use Cases
- Driver inventory assessment
- Security driver detection
- System configuration analysis
- Kernel-level component tracking
- Malware driver identification

### Example Output
```
Total Device Drivers: 156

Driver Address      | Driver Name
----------------------------------------
0x00007ff80000f000 | ntoskrnl.exe
0x00007ff800200000 | hal.dll
0x00007ff800350000 | mcupdate_genuineintel.dll
0x00007ff8004b0000 | pshed.dll
```

---

## EnumProcessModulesEx
### Description
Extended module enumeration with support for both 32-bit and 64-bit modules in a process, particularly useful for WOW64 process analysis.

### Key Features
- Architecture-specific enumeration
- WOW64 process support
- 32-bit and 64-bit module differentiation
- Comprehensive module discovery
- Advanced filtering options

### Filter Options
| Option | Value | Description |
|--------|-------|-------------|
| LIST_MODULES_32BIT | 0x01 | Enumerate 32-bit modules only |
| LIST_MODULES_64BIT | 0x02 | Enumerate 64-bit modules only |
| LIST_MODULES_ALL | 0x03 | Enumerate all modules |

### Use Cases
- WOW64 process analysis
- Architecture-specific analysis
- Mixed-mode application profiling
- Comprehensive module inventory
- Advanced threat analysis

### Example Output
```
Total Modules (All Architectures): 142

Module Base     | Module Name
----------------------------------------
0x00400000      | explorer.exe (64-bit)
0x6a000000      | compat.dll (32-bit)
0x77d40000      | kernel32.dll (64-bit)
```

---

## Module32First
### Description
Retrieves the first module entry from a process snapshot created by CreateToolhelp32Snapshot, providing structured module information.

### Key Features
- Snapshot-based enumeration
- Structured MODULEENTRY32 data
- Module size information
- Base address details
- Memory range information

### MODULEENTRY32 Structure
```
- dwSize: Size of structure
- th32ModuleID: Module identifier
- th32ProcessID: Process ID
- GlblcntUsage: Usage count
- ProccntUsage: Process usage count
- modBaseAddr: Module base address
- modBaseSize: Module size in bytes
- hModule: Module handle
- szModule: Module name
- szExePath: Module file path
```

### Use Cases
- Detailed module metadata retrieval
- Memory range analysis
- Module size tracking
- Load order analysis
- Comprehensive module profiling

### Example Output
```
Module Base: 0x00400000 | Module Size: 2097152 | explorer.exe
Module Base: 0x77d40000 | Module Size: 1048576 | kernel32.dll
Module Base: 0x77ef0000 | Module Size: 655360 | ntdll.dll
```

---

## Module32Next
### Description
Retrieves subsequent module entries following a Module32First call, iterating through all modules in a process snapshot.

### Key Features
- Iterative module enumeration
- Consistent with Module32First
- Same MODULEENTRY32 structure
- Complete snapshot traversal
- Sequential access pattern

### Use Cases
- Full module list iteration
- Batch module processing
- Sequential analysis
- Complete dependency mapping
- Comprehensive module collection

---

## GetModuleBaseNameA
### Description
Retrieves the base name of a module loaded by a process, extracting only the filename without the full path.

### Key Features
- Filename extraction only
- ASCII format output
- Process-specific context
- Minimal access requirements
- Quick name resolution

### Use Cases
- Module identification
- Display purposes
- Quick name lookup
- Simplified logging
- User-friendly output

### Example Output
```
First 5 Module Base Names:
Module Base Name
----------------------------------------
explorer.exe
kernel32.dll
ntdll.dll
user32.dll
gdi32.dll
```

---

## Comparison

| Feature | EnumDeviceDrivers | EnumProcessModules | Module32First | EnumProcessModulesEx |
|---------|-------------------|-------------------|---------------|----------------------|
| **Scope** | System-wide | Per-process | Per-process | Per-process |
| **Speed** | Very Fast | Fast | Moderate | Fast |
| **Memory Overhead** | Low | Low | Moderate | Low |
| **Detail Level** | Basic | Module info | Full structure | Full structure |
| **Architecture Support** | Native | Native | Both | Both (explicit) |
| **Snapshot Required** | No | No | Yes | No |
| **Privileges Needed** | Minimal | Limited | Limited | Limited |

---

## Best Practices for Implementation

1. **Error Handling**: Always check return values and handle access denied errors gracefully
2. **Access Rights**: Use minimal required permissions:
   - `PROCESS_QUERY_INFORMATION` for basic module enumeration
   - `PROCESS_QUERY_INFORMATION | PROCESS_VM_READ` for detailed module information
3. **Handle Cleanup**: Always close process handles with `CloseHandle()` and snapshot handles
4. **Buffer Sizing**: Allocate sufficient buffers for module arrays (256+ recommended)
5. **Combined Analysis**: Use multiple APIs for comprehensive module profiling
6. **Privilege Considerations**: Administrator privileges may be required for certain processes
7. **Performance**: Cache results when analyzing multiple modules
8. **Architecture Awareness**: Use `EnumProcessModulesEx` for WOW64 process analysis
9. **Null Termination**: Ensure proper null-termination of module name strings
10. **Snapshot Validity**: Validate snapshot handles before use

---

## Required Libraries
- `psapi.lib` (Process Status API) - For module enumeration and information queries
- `kernel32.lib` (Windows API) - For core process and snapshot operations
- `tlhelp32.h` - Header file for snapshot operations

---

## Educational Purpose
This documentation is provided for **educational and research purposes only**. Understanding module enumeration techniques is essential for:
- System administration and monitoring
- Malware analysis and reverse engineering
- Software security research
- Performance monitoring and optimization
- Cybersecurity education
- Legitimate software development
- Application profiling and optimization

---

## API References
- MSDN: [EnumDeviceDrivers](https://docs.microsoft.com/en-us/windows/win32/api/psapi/nf-psapi-enumdevicedrivers)
- MSDN: [EnumProcessModules](https://docs.microsoft.com/en-us/windows/win32/api/psapi/nf-psapi-enumprocessmodules)
- MSDN: [EnumProcessModulesEx](https://docs.microsoft.com/en-us/windows/win32/api/psapi/nf-psapi-enumprocessmodulesex)
- MSDN: [Module32First](https://docs.microsoft.com/en-us/windows/win32/api/tlhelp32/nf-tlhelp32-module32first)
- MSDN: [Module32Next](https://docs.microsoft.com/en-us/windows/win32/api/tlhelp32/nf-tlhelp32-module32next)
- MSDN: [GetModuleBaseNameA](https://docs.microsoft.com/en-us/windows/win32/api/psapi/nf-psapi-getmodulebasenamea)
- MSDN: [GetModuleInformation](https://docs.microsoft.com/en-us/windows/win32/api/psapi/nf-psapi-getmoduleinformation)
- MSDN: [GetDeviceDriverBaseNameA](https://docs.microsoft.com/en-us/windows/win32/api/psapi/nf-psapi-getdevicedriverbasenamea)

---

<p align="right">
  Last Updated: October 2025
</p>