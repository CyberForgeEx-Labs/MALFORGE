 <h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Enumeration/Memory-File%20Attribute%20Query/Memory-File_Attribute.c">Memory and File Attribute Query Techniques</a>
</h1>

## Overview
Memory and file attribute query APIs provide essential Windows capabilities for reading process memory, accessing files, querying virtual memory regions, retrieving file timestamps, examining file attributes, and searching for files in system paths. These APIs are fundamental for system monitoring, debugging, file management, and security analysis applications.

---

## Core APIs

| API | Purpose | Sophistication |
|-----|---------|----------------|
| `ReadProcessMemory` | Reads memory from process | Intermediate |
| `ReadFile` | Reads data from file | Basic |
| `VirtualQueryEx` | Queries virtual memory information | Advanced |
| `GetFileAttributesA` | Gets file attributes | Basic |
| `SearchPathA` | Searches for file in path | Intermediate |

---

## 1. ReadProcessMemory
### Description
Reads data from a specified memory region in a target process. This API enables inter-process memory access, allowing one process to read the memory contents of another. Requires appropriate access rights (PROCESS_VM_READ) to the target process. Commonly used for debugging, process monitoring, and memory analysis.

### Key Features
- Cross-process memory reading
- Arbitrary address access
- Byte-level precision
- Size validation
- Error reporting
- Security context dependent

### Use Cases
- Debugger implementation
- Process monitoring tools
- Memory analysis utilities
- Anti-cheat systems
- Security research
- Process instrumentation

### Example Output
```
ReadProcessMemory - Reading Memory from Current Process
================================================================
[*] Test data address: 0x00007FF6A2B01000
[*] Test data content: "Hello, World"

[+] ReadProcessMemory succeeded!
    Bytes Read: 23
    Content: "Hello, World!"

    Hex Dump:
    48 65 6C 6C 6F 2C 20 
    65 6D 6F 72 79 21 00 
```

---

## 2. ReadFile
### Description
Reads data from a file, device, or pipe. This is the primary API for file I/O operations in Windows. Supports synchronous and asynchronous operations, and can read from various types of handles including disk files, pipes, communication devices, and console buffers. Returns the actual number of bytes read.

### Key Features
- Synchronous/asynchronous I/O
- Buffer-based reading
- Byte count reporting
- Device-independent
- Overlapped I/O support
- Error handling

### Use Cases
- File content reading
- Log file processing
- Configuration file parsing
- Binary file analysis
- Stream processing
- Data extraction

### Example Output
```
ReadFile - Reading Data from File
================================================================
[*] Target file: C:\Temp\test_readfile.txt

[+] File opened successfully!
    File Size: 64 bytes

[+] ReadFile succeeded!
    Bytes Read: 64

    File Content:
    -------------
    This is a test file for ReadFile demonstration.
    Line 2
    Line 3
    -------------
```

---

## 3. VirtualQueryEx
### Description
Retrieves information about a range of pages in the virtual address space of a specified process. Returns detailed memory attributes including base address, size, protection flags, state (committed/reserved/free), and type (private/mapped/image). Essential for memory mapping, allocation tracking, and security analysis.

### Key Features
- Memory region information
- Protection flags query
- State enumeration (commit/reserve/free)
- Type identification (private/mapped/image)
- Size reporting
- Cross-process capability

### Use Cases
- Memory leak detection
- Virtual memory analysis
- Memory protection auditing
- Process memory mapping
- Security scanning
- Allocation tracking

### Example Output
```
VirtualQueryEx - Querying Virtual Memory Information
================================================================
[*] Allocated memory at: 0x000001A2C4F50000
[*] Allocation size: 4096 bytes

[+] VirtualQueryEx succeeded!
    Bytes Returned: 48

    MEMORY_BASIC_INFORMATION:
    -------------------------
    BaseAddress:       0x000001A2C4F50000
    AllocationBase:    0x000001A2C4F50000
    AllocationProtect: 0x00000004 (PAGE_READWRITE)
    RegionSize:        4096 bytes
    State:             0x00001000 (MEM_COMMIT)
    Protect:           0x00000004 (PAGE_READWRITE)
    Type:              0x00020000 (MEM_PRIVATE)
```

---

## 5. GetFileAttributesA
### Description
Retrieves file system attributes for a specified file or directory. Returns a bitmask containing flags such as read-only, hidden, system, directory, archive, compressed, and encrypted. This is a lightweight, efficient way to check file properties without opening the file. The ANSI version (A suffix) accepts single-byte character strings.

### Key Features
- Bitmask attribute flags
- No file handle required
- Fast attribute checking
- Multiple attributes in one call
- Path-based access
- ANSI string support

### Use Cases
- File filtering
- Attribute validation
- Directory traversal
- Backup attribute checking
- Hidden file detection
- System file identification

### Example Output
```
GetFileAttributesA - Getting File Attributes
================================================================
[*] Target file: C:\Windows\System32\notepad.exe

[+] GetFileAttributesA succeeded!
    Attributes Value: 0x00000020

    File Attributes:
    ----------------
    [X] ARCHIVE        - File should be archived
    [ ] COMPRESSED
    [ ] DIRECTORY
    [ ] ENCRYPTED
    [ ] HIDDEN
    [ ] NORMAL
    [ ] READONLY
    [ ] SYSTEM
    [ ] TEMPORARY
```

---

## 6. SearchPathA
### Description
Searches for a specified file in a specified path or in standard Windows search locations. By default, searches the application directory, current directory, system directory, Windows directory, and directories in the PATH environment variable. Returns the full path of the first matching file found. The ANSI version (A suffix) uses single-byte character strings.

### Key Features
- Multi-directory search
- PATH environment support
- Full path resolution
- File part extraction
- Custom path support
- Extension handling

### Use Cases
- Executable location
- DLL discovery
- Configuration file finding
- Resource file location
- Path resolution
- Dependency checking

### Example Output
```
SearchPathA - Searching for Files in System Paths
================================================================
[*] Searching for common system files...

  Searching for: notepad.exe
  [+] Found!
      Full Path:  C:\Windows\System32\notepad.exe
      File Part:  notepad.exe
      Path Len:   35 characters

  Searching for: cmd.exe
  [+] Found!
      Full Path:  C:\Windows\System32\cmd.exe
      File Part:  cmd.exe
      Path Len:   31 characters

  Searching for: kernel32.dll
  [+] Found!
      Full Path:  C:\Windows\System32\kernel32.dll
      File Part:  kernel32.dll
      Path Len:   36 characters
```

---

## MEMORY_BASIC_INFORMATION Structure

### Format
```c
typedef struct _MEMORY_BASIC_INFORMATION {
    PVOID  BaseAddress;          // Base address of region
    PVOID  AllocationBase;       // Base address of allocation
    DWORD  AllocationProtect;    // Initial protection
    SIZE_T RegionSize;           // Size of region in bytes
    DWORD  State;                // State of pages (commit/reserve/free)
    DWORD  Protect;              // Current protection
    DWORD  Type;                 // Type of pages (private/mapped/image)
} MEMORY_BASIC_INFORMATION;
```

### State Values
- `MEM_COMMIT` (0x1000) - Committed pages with physical storage
- `MEM_RESERVE` (0x2000) - Reserved pages without physical storage
- `MEM_FREE` (0x10000) - Free pages available for allocation

### Protection Flags
- `PAGE_NOACCESS` (0x01) - No access allowed
- `PAGE_READONLY` (0x02) - Read-only access
- `PAGE_READWRITE` (0x04) - Read and write access
- `PAGE_EXECUTE` (0x10) - Execute access
- `PAGE_EXECUTE_READ` (0x20) - Execute and read access
- `PAGE_EXECUTE_READWRITE` (0x40) - Execute, read, and write access

### Type Values
- `MEM_PRIVATE` (0x20000) - Private memory pages
- `MEM_MAPPED` (0x40000) - Memory-mapped file pages
- `MEM_IMAGE` (0x1000000) - Memory pages containing executable image

---

## File Attribute Flags

### Common Attributes
```c
FILE_ATTRIBUTE_READONLY      0x00000001  // Read-only file
FILE_ATTRIBUTE_HIDDEN        0x00000002  // Hidden file
FILE_ATTRIBUTE_SYSTEM        0x00000004  // System file
FILE_ATTRIBUTE_DIRECTORY     0x00000010  // Directory
FILE_ATTRIBUTE_ARCHIVE       0x00000020  // Archive bit set
FILE_ATTRIBUTE_DEVICE        0x00000040  // Reserved
FILE_ATTRIBUTE_NORMAL        0x00000080  // No other attributes
FILE_ATTRIBUTE_TEMPORARY     0x00000100  // Temporary file
FILE_ATTRIBUTE_SPARSE_FILE   0x00000200  // Sparse file
FILE_ATTRIBUTE_REPARSE_POINT 0x00000400  // Reparse point
FILE_ATTRIBUTE_COMPRESSED    0x00000800  // Compressed file
FILE_ATTRIBUTE_OFFLINE       0x00001000  // Offline storage
FILE_ATTRIBUTE_ENCRYPTED     0x00004000  // Encrypted file
```

---

## Security Considerations

### Process Access Rights
- **PROCESS_VM_READ** - Required for ReadProcessMemory
- **PROCESS_QUERY_INFORMATION** - Required for VirtualQueryEx
- Administrator privileges may be needed for system processes
- Protected processes cannot be accessed

### File Access Security
- File permissions affect ReadFile and GetFileAttributesA
- NTFS ACLs control access
- Some system files require elevation
- Symbolic links and junctions may affect SearchPathA results

### Best Practices
- Always check return values
- Handle INVALID_HANDLE_VALUE properly
- Use appropriate access flags
- Close handles after use
- Validate buffer sizes
- Implement proper error handling

---

## Related APIs

### Memory Operations
- `WriteProcessMemory` - Write to process memory
- `VirtualAllocEx` - Allocate memory in process
- `VirtualFreeEx` - Free memory in process
- `VirtualProtectEx` - Change memory protection

### Path Operations
- `GetModuleFileNameA` - Get module path
- `GetSystemDirectoryA` - Get system directory
- `GetWindowsDirectoryA` - Get Windows directory
- `GetTempPathA` - Get temporary directory

---


## Required Libraries
- `kernel32.lib` - All APIs in this document
- `psapi.lib` - Process API definitions (headers only)

---

## API References
- MSDN: [ReadProcessMemory](https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-readprocessmemory)
- MSDN: [ReadFile](https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-readfile)
- MSDN: [VirtualQueryEx](https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualqueryex)
- MSDN: [GetFileTime](https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfiletime)
- MSDN: [GetFileAttributesA](https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfileattributesa)
- MSDN: [SearchPathA](https://docs.microsoft.com/en-us/windows/win32/api/processenv/nf-processenv-searchpatha)

---

<p align="right">
  Last Updated: November 2025
</p>
