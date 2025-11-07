<h1 align="center">
   Memory Copy Operations Demonstration
</h1>

## Overview
Memory copy operations provide essential Windows capabilities for moving and copying data within process memory and across process boundaries. These APIs are fundamental to memory management, inter-process communication, and system programming. This demonstration covers three primary APIs used in memory operations, focusing on their technical functionality and practical applications.

---

## Core APIs

| API | Purpose | Sophistication |
|-----|---------|----------------|
| `RtlMoveMemory` | Moves memory block (handles overlaps) | Basic |
| `RtlCopyMemory` | Copies memory block (non-overlapping) | Basic |
| `WriteProcessMemory` | Writes data to remote process memory | Intermediate |
| `ReadProcessMemory` | Read the data that has written to the memory | Intermediate |

---

## 1. RtlMoveMemory
### Description
Moves a block of memory from one location to another. This function properly handles overlapping source and destination buffers, making it safe for all memory move operations. Implemented as a wrapper around the memmove C runtime function. The operation is performed at the byte level and works with any data type. Also known by its macro name `MoveMemory`.

### Key Features
- Handles overlapping memory regions
- Safe for all move scenarios
- Byte-level precision
- No size limitations
- Works with any data type
- Efficient implementation
- No return value (void function)
- Direct memory manipulation

### Use Cases
- Buffer reorganization
- Data structure manipulation
- In-place data shifting
- Memory compaction
- String manipulation
- Array operations
- General memory management
- Performance-critical code

### Example Output
```
================================================================
RtlMoveMemory - Moving Memory Block
================================================================

[*] Before RtlMoveMemory:
    Source: "Original data in source buffer"
    Dest:   ""

[+] RtlMoveMemory completed!
    Bytes moved: 31

[*] After RtlMoveMemory:
    Source: "Original data in source buffer"
    Dest:   "Original data in source buffer"

[*] Overlapping move demonstration:
    Before: "ABCDEFGHIJKLMNOP"
    After:  "ABCDABCDEFGHMNOP"
    (Moved first 8 bytes to offset 4)
```

---

## 2. RtlCopyMemory
### Description
Copies a block of memory from source to destination. Optimized for non-overlapping regions but may produce undefined results if regions overlap. Implemented as a wrapper around the memcpy C runtime function. Typically faster than RtlMoveMemory for non-overlapping regions due to optimization assumptions. Also known by its macro name `CopyMemory`.

### Key Features
- Fast memory copying
- Non-overlapping optimization
- Byte-level precision
- No size restrictions
- Type-agnostic operation
- Efficient for large blocks
- No return value (void function)
- Direct memory access

### Use Cases
- Buffer copying
- Data duplication
- Structure copying
- Array copying
- Fast data transfer
- Memory initialization
- Data marshalling
- Performance optimization

### Example Output
```
================================================================
RtlCopyMemory - Copying Memory Block
================================================================

[*] Before RtlCopyMemory:
    Source (hex): 00 08 10 18 20 28 30 38 40 48 50 58 60 68 70 78 ...
    Dest (hex):   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ...

[+] RtlCopyMemory completed!
    Bytes copied: 32

[*] After RtlCopyMemory:
    Dest (hex):   00 08 10 18 20 28 30 38 40 48 50 58 60 68 70 78 ...
```

---

## 3. WriteProcessMemory
### Description
Writes data to a specified memory area in another process. Requires PROCESS_VM_WRITE and PROCESS_VM_OPERATION access rights. The entire write operation must occur within a single memory region with the same protection. Returns the number of bytes actually written. Used for inter-process communication and remote process manipulation. Automatically handles page protection changes if possible.

### Key Features
- Cross-process writing
- Byte-level precision
- Size validation
- Bytes written return
- Partial write detection
- Protection handling
- Error reporting
- Large buffer support

### Use Cases
- Inter-process communication
- Shared data transfer
- Remote process configuration
- Process instrumentation
- Debugging tools
- Testing frameworks
- Memory patching
- Development utilities

### Example Output
```
================================================================
WriteProcessMemory - Writing Data to Remote Process
================================================================

[*] Creating target process (suspended)...
[+] Process created (PID: 12345)
[+] Remote memory allocated at: 0x0000020a1b2f0000
[*] Writing 32 bytes to remote process...
    Data: "Hello from WriteProcessMemory!"
[+] Write successful!
    Bytes written: 32
    Target address: 0x0000020a1b2f0000

[*] Verification read:
    Bytes read: 32
    Data read back: "Hello from WriteProcessMemory!"
[+] Data integrity verified!

[*] Cleaning up...
[+] Cleanup completed.
```
---

### When to Use Each API

**RtlMoveMemory:**
- Source and destination may overlap
- Safety is priority over speed
- In-place data manipulation
- Uncertain about overlap

**RtlCopyMemory:**
- Guaranteed non-overlapping regions
- Maximum performance needed
- Simple buffer duplication
- Large data transfers

**WriteProcessMemory:**
- Cross-process data transfer
- Remote process manipulation
- Inter-process communication
- Requires target process handle

---

## Access Rights Required

### For WriteProcessMemory
```c
DWORD dwDesiredAccess = 
    PROCESS_VM_OPERATION |  // Required for VirtualAllocEx
    PROCESS_VM_WRITE |      // Required for WriteProcessMemory
    PROCESS_VM_READ;        // Optional, for verification
```

### Memory Protection Flags
```c
// Common protection values
PAGE_READWRITE          // Read and write access
PAGE_EXECUTE_READWRITE  // Execute, read, and write
PAGE_READONLY           // Read-only access
PAGE_EXECUTE_READ       // Execute and read access
```

---

## Error Handling

### WriteProcessMemory Errors
| Error | Cause | Solution |
|-------|-------|----------|
| ERROR_ACCESS_DENIED | Insufficient privileges | Check process access rights |
| ERROR_INVALID_HANDLE | Invalid process handle | Verify handle is valid |
| ERROR_PARTIAL_COPY | Partial write occurred | Check target address validity |
| ERROR_NOACCESS | Invalid address | Ensure memory is allocated |

---

## Required Libraries
- `kernel32.lib` - Core Windows APIs (WriteProcessMemory, VirtualAllocEx, etc.)
- `ntdll.lib` - Native API implementations (RtlMoveMemory, RtlCopyMemory)

---

## API References
- MSDN: [RtlMoveMemory](https://docs.microsoft.com/en-us/windows/win32/devnotes/rtlmovememory)
- MSDN: [RtlCopyMemory](https://docs.microsoft.com/en-us/windows/win32/devnotes/rtlcopymemory)
- MSDN: [WriteProcessMemory](https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-writeprocessmemory)
- MSDN: [VirtualAllocEx](https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualallocex)
- MSDN: [ReadProcessMemory](https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-readprocessmemory)

---

<p align="right">
  Last Updated: November 2025
</p>
