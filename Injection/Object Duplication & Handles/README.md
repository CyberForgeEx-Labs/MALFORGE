<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Injection/Object%20Duplication%20%26%20Handles/Object_Duplication_handles.c">Object Duplication & Handle Management Techniques</a>
</h1>

## Overview
This educational tool demonstrates Windows handle duplication APIs for legitimate security research, system administration, and software development purposes. This implementation focuses on **safe understanding and inspection** of handle management mechanisms for inter-process communication, and resource sharing. These APIs are essential for understanding Windows security architecture, building IPC systems, process management tools, and defensive security research.

---

## Core APIs

| API | Purpose | Sophistication |
|-----|---------|----------------|
| `NtDuplicateObject` | Duplicates object handle between processes | Advanced |

---

## 1. NtDuplicateObject
### Description
Native NT API function that creates a duplicate handle to an existing kernel object, potentially across process boundaries. This low-level function provides the most direct control over handle duplication in Windows. It can duplicate handles within the same process or transfer them between different processes, enabling inter-process communication and resource sharing. The function operates at the kernel level through ntdll.dll and supports various options including closing the source handle atomically (DUPLICATE_CLOSE_SOURCE) and preserving access rights (DUPLICATE_SAME_ACCESS). Understanding NtDuplicateObject is crucial for advanced system programming, process injection research, and kernel-level debugging.

### Key Features
- Cross-process handle duplication
- Native NT-level operation
- Atomic source handle closure
- Access rights preservation
- Handle attribute control
- Direct kernel object access

### Use Cases
- Inter-process communication (IPC)
- Handle sharing between processes
- Process injection techniques (defensive research)
- Resource delegation
- Kernel object manipulation
- Security research and analysis

### Parameters
```c
NTSTATUS NtDuplicateObject(
    HANDLE SourceProcessHandle,      // Source process containing the handle
    HANDLE SourceHandle,             // Handle to duplicate
    HANDLE TargetProcessHandle,      // Target process for duplicated handle
    PHANDLE TargetHandle,            // Receives duplicated handle value
    ACCESS_MASK DesiredAccess,       // Desired access rights (0 = same as source)
    ULONG HandleAttributes,          // Handle attributes (e.g., INHERIT)
    ULONG Options                    // DUPLICATE_CLOSE_SOURCE, DUPLICATE_SAME_ACCESS
);
```

### Options Flags
| Flag | Value | Description |
|------|-------|-------------|
| `DUPLICATE_CLOSE_SOURCE` | 0x00000001 | Closes source handle after duplication |
| `DUPLICATE_SAME_ACCESS` | 0x00000002 | Duplicates with same access rights as source |

---

## Handle Duplication Flow Diagram

### NtDuplicateObject Process
```
Source Process                    Target Process
┌──────────────┐                 ┌──────────────┐
│  Handle A    │                 │              │
│  (0x1234)    │                 │              │
└──────┬───────┘                 └──────────────┘
       │                                  
       │ NtDuplicateObject()             
       │ SourceHandle: 0x1234            
       │ TargetProcess: PID 5678         
       ▼                                  
Kernel Object Manager                    
┌─────────────────────────┐             
│ Locate Kernel Object    │             
│ referenced by 0x1234    │             
├─────────────────────────┤             
│ Validate Access Rights  │             
├─────────────────────────┤             
│ Create Handle Entry in  │             
│ Target Process Handle   │             
│ Table                   │             
└──────────┬──────────────┘             
           │                             
           ▼                             
Target Process                           
┌──────────────┐                        
│  Handle B    │ ← New handle           
│  (0x5678)    │   (different value)    
└──────────────┘                        
           │                             
           └─────► Same Kernel Object ◄─┘
                  Both handles reference
                  the same object
```

---

## Handle Reference Counting

### Reference Count Behavior
```
Action                          Source Ref    Target Ref    Notes
─────────────────────────────────────────────────────────────────────
Initial State                   1             0             One handle exists
NtDuplicateObject (normal)      1             1             Both processes have handle
                                                            Object ref count = 2
NtDuplicateObject               0             1             Source closed, target created
(DUPLICATE_CLOSE_SOURCE)                                   Object ref count = 1
CloseHandle(Source)             0             1             Only target remains
CloseHandle(Target)             0             0             Object freed by kernel
```

### Kernel Object Lifetime
```
┌─────────────────────────────────────────────────┐
│ Kernel Object (e.g., Mutex, Event, File)       │
│                                                 │
│ Reference Count: 2                              │
│                                                 │
│ Referenced by:                                  │
│ ├─ Handle 0x1234 (Process A)                   │
│ └─ Handle 0x5678 (Process B)                   │
│                                                 │
│ Object persists until all handles are closed   │
└─────────────────────────────────────────────────┘

After CloseHandle in Process A:
┌─────────────────────────────────────────────────┐
│ Kernel Object                                   │
│ Reference Count: 1                              │
│ Referenced by:                                  │
│ └─ Handle 0x5678 (Process B)                   │
└─────────────────────────────────────────────────┘

After CloseHandle in Process B:
┌─────────────────────────────────────────────────┐
│ Object Destroyed                                │
│ Memory freed, resources released                │
└─────────────────────────────────────────────────┘
```
---

## Common Errors and Solutions

### Error Scenarios
| Error | Cause | Solution |
|-------|-------|----------|
| ACCESS_DENIED (5) | Insufficient privileges to duplicate | Elevate privileges or request PROCESS_DUP_HANDLE |
| INVALID_HANDLE (6) | Source handle is invalid or closed | Verify handle validity before duplication |
| INVALID_PARAMETER (87) | Invalid options or parameters | Check NTSTATUS code, verify parameter values |
| STATUS_ACCESS_VIOLATION | Invalid pointer to target handle | Ensure TargetHandle pointer is valid |
| STATUS_PROCESS_IS_TERMINATING | Target process is exiting | Verify target process state before duplication |
| STATUS_OBJECT_TYPE_MISMATCH | Handle type cannot be duplicated | Verify object type supports duplication |

---

## Handle Types and Duplication Support

### Duplicable Object Types
| Object Type | Supports Duplication | Notes |
|-------------|---------------------|-------|
| File | ✓ | File position maintained |
| Event | ✓ | State preserved |
| Mutex | ✓ | Ownership transferred if owned |
| Semaphore | ✓ | Count preserved |
| Thread | ✓ | Thread must still be alive |
| Process | ✓ | Process must still be alive |
| Section | ✓ | Memory mappings preserved |
| Registry Key | ✓ | Transaction state maintained |
| Job | ✓ | Job limits apply to target |

---

## Required Libraries
- `kernel32.lib` - Win32 handle management functions
- `ntdll.lib` - Native API functions (NtDuplicateObject)

---

## Compilation Instructions


### Microsoft Visual C++ with Debug Symbols
```console
cl.exe /Zi /DEBUG Object_Duplication_Handles.c advapi32.lib ntdll.lib
```


### MinGW GCC with Optimizations
```console
gcc -O2 Object_Duplication_Handles.c -o Object_Duplication_Handles.exe -ladvapi32 -lntdll
```

---

## API References
- MSDN: [NtDuplicateObject](https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-zwduplicateobject)
- MSDN: [ImpersonateLoggedOnUser](https://learn.microsoft.com/en-us/windows/win32/api/securitybaseapi/nf-securitybaseapi-impersonateloggedonuser)
- MSDN: [Impersonation Levels](https://learn.microsoft.com/en-us/windows/win32/secauthz/impersonation-levels)

---

## Additional Resources

### Learning Materials
- [Windows Internals (Book)](https://docs.microsoft.com/en-us/sysinternals/resources/windows-internals) - Comprehensive Windows architecture
- [Handle Management](https://learn.microsoft.com/en-us/windows/win32/sysinfo/handles-and-objects) - Microsoft documentation
- [Native API Reference](http://undocumented.ntinternals.net/) - Undocumented NT functions

### Security Research
- [Process Injection: Handle Injection](https://attack.mitre.org/techniques/T1055/012/) - MITRE documentation


## Demonstration Features

The included demonstration code showcases:
1. **Basic Handle Duplication**: Simple same-process handle duplication
2. **Cross-Process Duplication**: Handle sharing between parent and child processes
3. **Atomic Transfer**: Using DUPLICATE_CLOSE_SOURCE for ownership transfer
4. **Error Handling**: Comprehensive error checking and reporting
5. **Information Query**: Displaying handle and metadata

---

<p align="right">
  Last Updated: November 2025
</p>