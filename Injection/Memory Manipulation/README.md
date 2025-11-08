<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Injection/Memory%20Manipulation/Memory_Manipulation.c">Memory Management and Protection APIs Demonstration.</a>
</h1>

## Overview
Memory manipulation APIs provide essential Windows capabilities for allocating, protecting, reading, and writing memory in both local and remote processes. These APIs form the foundation for debuggers, memory profilers, JIT compilers, security research, and legitimate software development. This demonstration covers 15 primary APIs used in memory management, focusing on their educational and legitimate applications.

---

## Core APIs

| API | Purpose | Sophistication |
|-----|---------|----------------|
| `VirtualAlloc` | Allocates memory in current process | Intermediate |
| `VirtualAllocEx` | Allocates memory in remote process | Intermediate |
| `VirtualProtect` | Changes memory protection flags | Intermediate |
| `VirtualProtectEx` | Changes remote memory protection | Intermediate |
| `VirtualAllocExNuma` | NUMA-aware memory allocation | Advanced |
| `VirtualAlloc2` | Enhanced allocation API (Win10+) | Advanced |
| `NtAllocateVirtualMemory` | Native memory allocation | Advanced |
| `NtProtectVirtualMemory` | Native protection modification | Advanced |
| `NtReadVirtualMemory` | Native memory read | Advanced |
| `NtReadVirtualMemoryEx` | Extended native memory read | Advanced |
| `NtWriteVirtualMemory` | Native memory write | Advanced |
| `ReadProcessMemory` | Reads remote process memory | Intermediate |

---

## 1. VirtualAlloc
### Description
Allocates memory within the virtual address space of the calling process. Provides control over allocation type (MEM_COMMIT, MEM_RESERVE), size, and memory protection. Returns the base address of the allocated region. Memory is automatically freed when the process terminates. Allocations are page-aligned (typically 4KB on x86/x64). Essential for dynamic memory management, buffer allocation, and JIT compilation.

### Key Features
- Local process allocation
- Page-aligned addressing
- Protection specification
- Allocation type control (commit/reserve)
- NULL for automatic placement
- Size specification
- MEM_COMMIT for immediate use
- MEM_RESERVE for address space reservation

### Use Cases
- Dynamic buffer allocation
- JIT compilation buffers
- Custom memory managers
- Heap implementations
- Large object allocation
- Memory pools
- Performance optimization
- Data structure storage

### Example Output
```
================================================================
VirtualAlloc - Local Process Memory Allocation
================================================================

[*] Allocating 4096 bytes in current process...
[+] Memory allocated successfully!
    Base Address: 0x0000020a1b2f0000
    Size: 4096 bytes
    Protection: PAGE_READWRITE 

[*] Written test data: "VirtualAlloc test data"
[*] Verification read: "VirtualAlloc test data"
[+] Memory freed successfully.
```

---

## 2. VirtualAllocEx
### Description
Allocates memory within the virtual address space of a specified process. Identical functionality to VirtualAlloc but operates on remote processes. Requires PROCESS_VM_OPERATION access right to the target process. Returns the base address in the remote process's address space. Essential for inter-process communication, DLL injection preparation, and process instrumentation. Memory persists until explicitly freed or process termination.

### Key Features
- Remote process allocation
- Cross-process memory management
- Same protection options as VirtualAlloc
- Requires process handle
- PROCESS_VM_OPERATION access needed
- Page-aligned results
- Supports MEM_COMMIT and MEM_RESERVE
- Returns remote address

### Use Cases
- Inter-process communication
- DLL injection preparation
- Process instrumentation
- Remote buffer allocation
- Parameter passing to remote threads
- Shellcode staging areas
- Development tools
- Security research (authorized)

### Example Output
```
================================================================
VirtualAllocEx - Remote Process Memory Allocation
================================================================

[*] Creating target process (notepad.exe)...
[+] Process created (PID: 12345)
[*] Allocating 8192 bytes in remote process...
[+] Remote memory allocated successfully!
    Base Address: 0x0000020a1c3e0000
    Size: 8192 bytes
    Process: notepad.exe (PID: 12345)
    Protection: PAGE_READWRITE 

[*] Remote memory freed.
[+] Target process terminated and cleaned up.
```

---

## 3. VirtualProtect
### Description
Changes the protection on a region of committed pages in the virtual address space of the calling process. Allows modification of memory access permissions (read, write, execute). Returns the previous protection value. Must operate on committed memory pages. Common use case is enabling execute permissions on JIT-compiled code or protecting sensitive data. Can affect multiple pages if the region spans page boundaries.

### Key Features
- Protection flag modification
- Previous protection retrieval
- Operates on committed pages
- Affects entire page range
- Returns old protection value
- Supports all PAGE_* constants
- Immediate effect
- Thread-safe operation

### Use Cases
- JIT compilation (adding execute permissions)
- Memory protection hardening
- Data encryption (remove write access)
- Copy-on-write implementation
- Debugging tools
- Security hardening
- Performance optimization
- Dynamic code generation

### Example Output
```
================================================================
VirtualProtect - Modifying Memory Protection
================================================================

[*] Allocating memory with PAGE_READWRITE...
[+] Memory allocated at: 0x0000020a1b2f0000
    Protection: PAGE_READWRITE 

[*] Written test data successfully.
[*] Changing protection to PAGE_EXECUTE_READWRITE...
[+] Protection changed successfully!
    Old Protection: 0x00000004
    Protection: PAGE_EXECUTE_READWRITE 

[*] Memory now has execute permissions.
[*] Use case: JIT compilation, dynamic code generation

[+] Memory freed.
```

---

## 4. VirtualProtectEx
### Description
Changes the protection on a region of committed pages in the virtual address space of a specified process. Remote process equivalent of VirtualProtect. Requires PROCESS_VM_OPERATION access right. Returns previous protection value through an output parameter. Used by debuggers, process monitors, and security tools. Must specify the exact base address and size of the region to modify.

### Key Features
- Remote protection modification
- Cross-process capability
- Previous protection retrieval
- Requires process handle
- PROCESS_VM_OPERATION access needed
- All PAGE_* protection flags supported
- Operates on page boundaries
- Returns old protection

### Use Cases
- Debugger implementation
- Process monitoring tools
- Security analysis
- Anti-debugging techniques (detection)
- Memory forensics
- Process instrumentation
- Hook implementation
- Research and analysis

### Example Output
```
================================================================
VirtualProtectEx - Remote Memory Protection Change
================================================================

[*] Creating target process...
[+] Process created (PID: 12345)
[+] Remote memory allocated at: 0x0000020a1c3e0000
    Protection: PAGE_READWRITE 

[*] Changing remote memory protection to PAGE_EXECUTE_READ...
[+] Remote protection changed successfully!
    Old Protection: 0x00000004
    Protection: PAGE_EXECUTE_READ 

[+] Cleanup completed.
```

---

## 5. VirtualAllocExNuma
### Description
Allocates memory in a specified process and binds it to a specific NUMA (Non-Uniform Memory Access) node. Provides performance benefits on NUMA systems by allocating memory physically close to the processor that will use it. Identical to VirtualAllocEx except for the additional NUMA node parameter. Available on Windows Vista and later. Returns NULL and sets ERROR_NOT_SUPPORTED if NUMA is not available.

### Key Features
- NUMA node specification
- Performance optimization
- Remote process support
- Same as VirtualAllocEx with node parameter
- Processor affinity optimization
- Memory locality control
- Available on multi-socket systems
- Automatic fallback on non-NUMA systems

### Use Cases
- High-performance computing
- Server applications
- Multi-socket systems
- Performance-critical code
- Database engines
- Scientific computing
- Real-time systems
- Memory-intensive applications

### Example Output
```
================================================================
VirtualAllocExNuma - NUMA-Aware Memory Allocation
================================================================

[*] Creating target process...
[+] Process created (PID: 12345)
[*] Attempting NUMA-aware allocation on node 0...
[+] NUMA memory allocated successfully!
    Base Address: 0x0000020a1c3e0000
    NUMA Node: 0
    Size: 4096 bytes
    Protection: PAGE_READWRITE 

[*] Use case: Performance optimization on NUMA systems

[+] Cleanup completed.
```

---

## 6. VirtualAlloc2
### Description
Enhanced version of VirtualAlloc introduced in Windows 10 version 1803. Provides additional control through extended parameters structure. Supports custom allocators, placeholder addresses, and advanced memory management scenarios. Can allocate memory for the current process or a specified process. Offers more flexibility than legacy VirtualAlloc/VirtualAllocEx APIs. Backward compatible with older allocation methods.

### Key Features
- Extended parameter support
- Process handle parameter
- Placeholder management
- Custom alignment support
- Address requirements specification
- Enhanced error reporting
- Unified API for local/remote
- Modern memory management

### Use Cases
- Modern application development
- Advanced memory management
- Placeholder address ranges
- Custom memory allocators
- Performance optimization
- Large-page allocation
- Address-sensitive allocations
- Windows 10+ exclusive features

### Example Output
```
================================================================
VirtualAlloc2 - Modern Memory Allocation API
================================================================

[*] Attempting to load VirtualAlloc2 (Windows 10+ only)...
[+] VirtualAlloc2 function found!
[*] Allocating memory with VirtualAlloc2...
[+] Memory allocated successfully!
    Base Address: 0x0000020a1b2f0000
    Size: 4096 bytes
    Protection: PAGE_READWRITE 

[*] VirtualAlloc2 provides enhanced control over memory allocation
[*] Supports extended parameters for advanced scenarios

[+] Memory freed.
```

---


## 7. NtAllocateVirtualMemory
### Description
Native API function from ntdll.dll that allocates memory in a process's virtual address space. Lower-level than kernel32's VirtualAlloc/VirtualAllocEx. Returns NTSTATUS codes rather than Win32 error codes. Provides direct access to the NT kernel's memory manager. Used in system programming, rootkit detection, and scenarios requiring direct NT API access. Undocumented but widely used.

### Key Features
- Native NT API access
- NTSTATUS return codes
- Lower abstraction level
- Direct kernel access
- Process handle parameter
- ZeroBits parameter for address constraints
- RegionSize output parameter
- System-level control

### Use Cases
- System programming
- Low-level memory management
- Direct NT API usage
- Security research
- Rootkit detection
- API hooking avoidance
- Kernel interaction
- Advanced debugging

### Example Output
```
================================================================
Native APIs - ntdll.dll Memory Functions
================================================================

[+] ntdll.dll base address: 0x00007ffd14b00000

Native API Addresses:
    NtAllocateVirtualMemory: 0x00007ffd14b1a2c0
    NtProtectVirtualMemory:  0x00007ffd14b2e450
    NtReadVirtualMemory:     0x00007ffd14b2f120
    NtWriteVirtualMemory:    0x00007ffd14b31890

[*] Calling NtAllocateVirtualMemory...
[+] NtAllocateVirtualMemory succeeded!
    NTSTATUS: 0x00000000 (STATUS_SUCCESS)
    Base Address: 0x0000020a1b2f0000
    Region Size: 4096 bytes
```

---

## 8. NtProtectVirtualMemory
### Description
Native API function for changing memory protection. Lower-level equivalent of VirtualProtect/VirtualProtectEx. Returns NTSTATUS codes. Provides direct access to the memory manager's protection mechanisms. Used in system programming and scenarios requiring NT API access. Operates on page boundaries and can modify multiple pages atomically.

### Key Features
- Native protection change
- NTSTATUS return codes
- Lower-level API
- Process handle support
- BaseAddress input/output
- RegionSize input/output
- OldProtect output
- Direct kernel access

### Use Cases
- System-level programming
- Direct NT API access
- Hook detection/avoidance
- Security research
- Low-level protection management
- Kernel-level interaction
- Advanced debugging tools

---

## 9. NtReadVirtualMemory
### Description
Native API function for reading memory from a process. Lower-level equivalent of ReadProcessMemory. Returns NTSTATUS codes. Provides direct access to the memory manager's read operations. Used in system programming, memory forensics, and direct NT API scenarios. Can read from any readable memory region in the target process.

### Key Features
- Native memory read
- NTSTATUS return codes
- Cross-process reading
- Byte-level precision
- Size specification
- Bytes read output
- Lower abstraction level
- Direct kernel access

### Use Cases
- Memory forensics
- System programming
- Direct NT API usage
- Process analysis
- Security research
- Hook bypass techniques
- Low-level debugging
- Advanced monitoring

---

## 10. NtWriteVirtualMemory
### Description
Native API function for writing memory to a process. Lower-level equivalent of WriteProcessMemory. Returns NTSTATUS codes. Provides direct access to the memory manager's write operations. Used in system programming, process instrumentation, and direct NT API scenarios. Requires appropriate memory protection on the target region.

### Key Features
- Native memory write
- NTSTATUS return codes
- Cross-process writing
- Byte-level precision
- Size specification
- Bytes written output
- Lower abstraction level
- Direct kernel access

### Use Cases
- Process instrumentation
- System programming
- Direct NT API usage
- Memory patching
- Security research
- Hook implementation
- Low-level debugging
- Advanced monitoring

### Example Output (Native APIs continued)
```
[*] Calling NtWriteVirtualMemory...
[+] NtWriteVirtualMemory succeeded!
    Bytes written: 21

[*] Calling NtReadVirtualMemory...
[+] NtReadVirtualMemory succeeded!
    Bytes read: 256
    Data: "Native API test data"

[*] Calling NtProtectVirtualMemory...
[+] NtProtectVirtualMemory succeeded!
    Old Protection: 0x00000004
    New Protection: PAGE_EXECUTE_READ

[+] Memory freed.

[*] Native APIs provide lower-level access than kernel32 equivalents
[*] Used in system programming and security research
```

---

## 11. ReadProcessMemory
### Description
Reads data from a specified memory area in another process. Requires PROCESS_VM_READ access right. Returns the number of bytes actually read. Used for process monitoring, debugging, and inter-process communication. Can read from any readable memory region. Handles page protection automatically for readable pages. Essential tool for debuggers and analysis tools.

### Key Features
- Cross-process reading
- Byte-level precision
- Size validation
- Bytes read return
- Partial read detection
- Protection handling
- Error reporting
- Large buffer support

### Use Cases
- Debugger implementation
- Process monitoring
- Memory analysis
- Game trainers (with permission)
- Cheat detection
- Performance profiling
- Security analysis
- Development tools

### Example Output
```
================================================================
ReadProcessMemory - Reading Remote Process Memory
================================================================

[*] Creating target process...
[+] Process created (PID: 12345)
[*] Writing test data to remote memory...
[+] Wrote 36 bytes to remote address: 0x0000020a1c3e0000
    Data: "Hello from ReadProcessMemory demo!"

[*] Reading data back from remote process...
[+] Read 256 bytes successfully!
    Data read: "Hello from ReadProcessMemory demo!"
[+] Data integrity verified!

[+] Cleanup completed.
```

---

## Memory Management Flow

### Typical Memory Allocation Sequence
1. **Allocation**: Call VirtualAlloc/VirtualAllocEx to reserve and commit memory
2. **Protection Setup**: Optionally use VirtualProtect/VirtualProtectEx for specific permissions
3. **Data Operations**: Write data with WriteProcessMemory or directly
4. **Reading**: Read data back with ReadProcessMemory for verification
5. **Protection Changes**: Modify permissions as needed (e.g., add execute for JIT)
6. **Usage**: Use the allocated memory for intended purpose
7. **Cleanup**: Free memory with VirtualFree/VirtualFreeEx
8. **Handle Closure**: Close process handles when done

### Memory State Transitions
```
Memory State Flow:
┌─────────────┐
│    FREE     │ ← Initial state (no allocation)
└──────┬──────┘
       │ VirtualAlloc(MEM_RESERVE)
       ↓
┌─────────────┐
│  RESERVED   │ ← Address space reserved, no physical memory
└──────┬──────┘
       │ VirtualAlloc(MEM_COMMIT)
       ↓
┌─────────────┐
│  COMMITTED  │ ← Physical memory allocated and accessible
└──────┬──────┘
       │ VirtualProtect
       ↓
┌─────────────┐
│  MODIFIED   │ ← Protection changed (e.g., added execute)
│ PROTECTION  │
└──────┬──────┘
       │ VirtualFree(MEM_DECOMMIT)
       ↓
┌─────────────┐
│  RESERVED   │ ← Physical memory released, address space kept
└──────┬──────┘
       │ VirtualFree(MEM_RELEASE)
       ↓
┌─────────────┐
│    FREE     │ ← Fully deallocated
└─────────────┘
```

---

## Memory Protection Flags

### Common Protection Constants
```c
// Read/Write Permissions
PAGE_NOACCESS           0x01  // No access
PAGE_READONLY           0x02  // Read only
PAGE_READWRITE          0x04  // Read and write
PAGE_WRITECOPY          0x08  // Read and copy-on-write

// Execute Permissions
PAGE_EXECUTE            0x10  // Execute only
PAGE_EXECUTE_READ       0x20  // Execute and read
PAGE_EXECUTE_READWRITE  0x40  // Execute, read, and write
PAGE_EXECUTE_WRITECOPY  0x80  // Execute, read, and copy-on-write

// Additional Modifiers
PAGE_GUARD              0x100 // Guard page (trap first access)
PAGE_NOCACHE            0x200 // Disable caching
PAGE_WRITECOMBINE       0x400 // Write-combining (performance)
PAGE_TARGETS_INVALID    0x40000000 // CFG: mark as invalid target
PAGE_TARGETS_NO_UPDATE  0x40000000 // CFG: don't update
```

### Protection Use Cases
| Protection | Common Uses |
|-----------|-------------|
| `PAGE_READWRITE` | General data buffers, heap memory |
| `PAGE_EXECUTE_READWRITE` | JIT compilation, dynamic code |
| `PAGE_EXECUTE_READ` | Loaded executable code, after JIT |
| `PAGE_READONLY` | Constants, read-only data |
| `PAGE_GUARD` | Stack overflow detection, debugging |
| `PAGE_NOACCESS` | Memory access traps, security |

---

## Access Rights Required

### Process Access Rights
```c
// For memory operations
PROCESS_VM_OPERATION    0x0008  // VirtualAllocEx, VirtualProtectEx
PROCESS_VM_WRITE        0x0020  // WriteProcessMemory
PROCESS_VM_READ         0x0010  // ReadProcessMemory
PROCESS_QUERY_INFORMATION 0x0400 // Query process info
SYNCHRONIZE             0x00100000 // Wait on process

// Combined for memory management
DWORD dwAccess = PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ;
```
---

## Security Considerations

### Protection Mechanisms
- **DEP (Data Execution Prevention)**: Prevents execution of non-executable memory
- **ASLR (Address Space Layout Randomization)**: Randomizes memory addresses
- **CFG (Control Flow Guard)**: Validates indirect call targets
- **Protected Processes**: PPL/PPL-Lite prevents memory access
- **Integrity Levels**: Must have equal or higher integrity
- **Session Isolation**: Session 0 isolation for services
- **Sandboxing**: UWP apps have restricted memory access

### Legitimate Use Guidelines
- Only allocate memory you need and free it promptly
- Use appropriate protection flags for intended use
- Never make stack memory executable unnecessarily
- Implement proper error handling and cleanup
- Respect process security boundaries
- Document memory allocation purposes
- Test in isolated environments
- Follow secure coding practices

---

## Error Handling

### Common Errors and Solutions
| Error | Cause | Solution |
|-------|-------|----------|
| Access Denied | Insufficient privileges | Run as administrator |
| Invalid Handle | Handle closed or invalid | Verify handle validity |
| Invalid Parameter | Wrong parameter value | Check API documentation |
| Not Enough Memory | System resources exhausted | Reduce allocation size |
| Invalid Address | Address not allocated | Verify address before use |
| Protection Violation | Writing to read-only memory | Change protection first |

---

## Compilation Instructions

### Microsoft Visual C++
```bash
cl.exe Memory_Manipulation_Demo.c /link kernel32.lib ntdll.lib
```

### GCC/MinGW
```bash
gcc Memory_Manipulation_Demo.c -o Memory_Manipulation_Demo.exe -lkernel32 -lntdll
```

---

## Usage Examples

### Administrative Execution
```bash
# Run Command Prompt as Administrator
Memory_Manipulation_Demo.exe
```
Required for full functionality with remote process operations.

---

## Performance Considerations

### Allocation Performance
- **VirtualAlloc**: Fastest for large allocations (>1 MB)
- **Heap allocations**: Better for small, frequent allocations
- **MEM_RESERVE then MEM_COMMIT**: Useful for large sparse allocations
- **VirtualAllocExNuma**: Best on multi-socket NUMA systems
- **Large pages**: Significant performance boost for large memory regions

---

## Required Libraries
- `kernel32.lib` - Core Windows APIs (VirtualAlloc, VirtualProtect, etc.)
- `ntdll.lib` - Native APIs (NtAllocateVirtualMemory, etc.)

---

## API References
- MSDN: [VirtualAlloc](https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualalloc)
- MSDN: [VirtualAllocEx](https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualallocex)
- MSDN: [VirtualProtect](https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualprotect)
- MSDN: [VirtualProtectEx](https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualprotectex)
- MSDN: [ReadProcessMemory](https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-readprocessmemory)
- MSDN: [VirtualAlloc2](https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualalloc2)

---

## Additional Resources

### Learning Resources
- MSDN Memory Management Documentation
- Windows API Programming Reference
- System Programming Blog Posts.

---

<p align="right">
  Last Updated: November 2025
</p>