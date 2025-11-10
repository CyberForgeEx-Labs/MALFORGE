<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Injection/Section%20-%20Mapping%20Injection/Section_Mapping.c">Section/Mapping Injection Techniques</a>
</h1>

## Overview
Section/Mapping injection techniques utilize Windows memory-mapped file objects (sections) to inject code into remote processes. Unlike traditional DLL injection which uses CreateRemoteThread and LoadLibrary, section mapping creates shared memory regions that can be mapped into multiple process address spaces simultaneously. This approach offers advantages in stealth, flexibility, and bypassing certain security controls. These APIs are essential for advanced injection techniques, inter-process communication, shared memory operations, and security research.

---

## Core APIs

| API | Purpose | Sophistication |
|-----|---------|----------------|
| `CreateFileMappingA` | Creates file mapping object | Basic |
| `MapViewOfFile` | Maps view of file into memory | Basic |
| `MapViewOfFile2` | Extended mapping API | Intermediate |
| `MapViewOfFile3` | Newer mapping API | Intermediate |
| `MapViewOfFileEx` | Maps at specific address | Intermediate |
| `OpenFileMappingA` | Opens existing file mapping | Basic |
| `NtCreateSection` | Native section creation | Advanced |
| `NtMapViewOfSection` | Native section mapping | Advanced |

---

## 1. [CreateFileMappingA]
### Description
Creates a file mapping object for a specified file or page-file-backed memory. File mapping objects enable multiple processes to share the same memory region through mapped views. Can create named or unnamed mappings. When INVALID_HANDLE_VALUE is passed as the file handle, the mapping is backed by the system page file (anonymous mapping). The mapping object persists until all handles are closed.

### Key Features
- Named or unnamed objects
- File-backed or page-file-backed
- Configurable size and protection
- Security descriptor support
- Object inheritance control
- Automatic cleanup on handle close

### Use Cases
- Section-based DLL injection
- Inter-process communication
- Shared memory buffers
- Memory-mapped file I/O
- Large dataset sharing
- Process synchronization data

---

## 2. [MapViewOfFile]
### Description
Maps a view of a file mapping into the address space of the calling process. The view is a portion (or all) of the file mapping object. Multiple views can be created from a single file mapping object. The system chooses the base address unless MapViewOfFileEx is used. The mapped memory can be accessed like regular memory through pointer operations.

### Key Features
- Automatic address selection
- Configurable access rights
- Offset specification support
- Partial or full mapping
- Copy-on-write support
- Automatic commit on access

### Use Cases
- Accessing shared memory sections
- Memory-mapped file operations
- Loading DLL images
- Data sharing between processes
- Large file processing
- Circular buffer implementation

---

## 3. [MapViewOfFile2]
### Description
Extended version of MapViewOfFile available on Windows Vista and later. Adds a process handle parameter allowing mapping into a different process's address space. Provides more control over the mapping operation with additional parameters for file offset and size. Useful for cross-process section mapping scenarios without requiring native API calls.

### Key Features
- Cross-process mapping capability
- 64-bit offset support
- Process handle parameter
- Enhanced size control
- Backward compatible parameters
- Simplified remote mapping

### Use Cases
- Remote process memory mapping
- Cross-process data sharing
- Advanced injection techniques
- Process monitoring tools
- Memory forensics
- Debugging utilities

---

## 4. [MapViewOfFile3]
### Description
Newest mapping API introduced in Windows 10 with the most extensive parameter set. Provides extended parameters for fine-grained control over the mapping operation including allocation type, protection flags, and custom attributes. Offers maximum flexibility for advanced scenarios requiring specific memory characteristics or NUMA node preferences.

### Key Features
- Extended parameter structure
- NUMA node awareness
- Enhanced allocation control
- Custom attribute support
- Maximum flexibility
- Future-proof design

### Use Cases
- High-performance computing
- NUMA-aware applications
- Advanced memory management
- Custom protection schemes
- Research and development
- Next-generation injection techniques

---

## 5. [MapViewOfFileEx]
### Description
Maps a view of a file mapping at a specific base address in the process's address space. The requested address must be aligned on a system allocation granularity boundary (typically 64KB). If the requested address is unavailable, the function fails unless NULL is specified. Useful for scenarios requiring deterministic memory layouts or specific address ranges.

### Key Features
- Address specification capability
- Alignment requirement enforcement
- Precise memory placement
- Address conflict detection
- Base address control
- Deterministic mapping

### Use Cases
- Position-dependent code loading
- Address space layout control
- DLL relocation scenarios
- Exploit development research
- Memory forensics analysis
- Custom memory allocators

---

## 6. [OpenFileMappingA]
### Description
Opens an existing named file mapping object created by another process or the same process. Does not create a new mapping if one doesn't exist. Requires the exact name of the mapping object including namespace prefix (Global\\ or Local\\). Returns a handle that can be used with MapViewOfFile to access the shared memory. Essential for inter-process communication using shared memory.

### Key Features
- Named object access
- Cross-process opening
- Handle inheritance option
- Namespace awareness
- Access rights specification
- Session boundary respect

### Use Cases
- IPC shared memory access
- Producer-consumer patterns
- Multi-process applications
- Service communication
- Named pipe alternatives
- Distributed data caching

---

## 7. [NtCreateSection]
### Description
Native API function from ntdll.dll for creating section objects at the kernel level. More powerful and flexible than CreateFileMappingA with additional allocation attributes like SEC_IMAGE for PE file sections. Returns NTSTATUS codes instead of Win32 error codes. Provides lower-level control and is often used in advanced injection techniques to bypass user-mode monitoring.

### Key Features
- Native kernel-level API
- SEC_IMAGE support for PE files
- Extended allocation attributes
- NTSTATUS return codes
- Object attributes structure
- Enhanced security descriptors

### Use Cases
- Stealthy section-based injection
- PE image section creation
- Rootkit research
- Anti-detection techniques
- Kernel-level operations
- Advanced malware analysis

---

## 8. [NtMapViewOfSection]
### Description
Native API function for mapping section views into process address spaces. Counterpart to NtCreateSection operating at the kernel level. Allows mapping sections into remote processes without additional kernel32 APIs. Supports inherit disposition parameters for child process inheritance. More powerful than MapViewOfFile with additional control over commit size and zero bits.

### Key Features
- Remote process mapping
- Inherit disposition control
- Zero bits specification
- Commit size parameter
- Native API stealth
- Enhanced protection flags

### Use Cases
- Stealthy remote mapping
- PE injection techniques
- Process hollowing
- Memory manipulation
- Security research
- Advanced exploitation

---

## Section Mapping Injection Process Flow

### Section-Based Injection Steps
1. **DLL Loading**: Read target DLL file into memory buffer
2. **Section Creation**: Call NtCreateSection with appropriate size and protection
3. **Local Mapping**: Map section into local process with NtMapViewOfSection
4. **DLL Copy**: Copy DLL contents from buffer into mapped section
5. **Target Opening**: Open target process with PROCESS_ALL_ACCESS
6. **Remote Mapping**: Map same section into target process address space
7. **Entry Point**: Optionally create thread at DLL entry point (DllMain)
8. **Cleanup**: Unmap views and close handles

### Memory Layout
```
Section Object (Kernel):
┌─────────────────────────────┐
│ Section Object              │
│ - Size: DLL size            │
│ - Protection: RWX           │
│ - Attributes: SEC_COMMIT    │
└─────────────────────────────┘
         ↓           ↓
    ┌────────┐  ┌────────┐
    │ Local  │  │ Remote │
    │ Process│  │ Process│
    └────────┘  └────────┘
         ↓           ↓
┌─────────────┐ ┌─────────────┐
│ Mapped View │ │ Mapped View │
│ Base: 0x... │ │ Base: 0x... │
│ Size: DLL   │ │ Size: DLL   │
│ [DLL Image] │ │ [DLL Image] │
└─────────────┘ └─────────────┘
```

### Section vs Traditional Injection
```
Traditional Injection:
Process A                    Process B
┌──────────┐                ┌──────────┐
│ Injector │                │ Target   │
└──────────┘                └──────────┘
     │                            │
     ├─ VirtualAllocEx ──────────→│
     ├─ WriteProcessMemory ───────→│
     ├─ CreateRemoteThread ───────→│
     │   LoadLibraryA("path")      │
     └────────────────────────────→│

Section Mapping Injection:
Process A                    Process B
┌──────────┐                ┌──────────┐
│ Injector │                │ Target   │
└──────────┘                └──────────┘
     │                            │
     ├─ NtCreateSection           │
     │       ↓                    │
     │  ┌─────────┐               │
     │  │ Section │               │
     │  │ Object  │               │
     │  └─────────┘               │
     │       ↓                    │
     ├─ NtMapViewOfSection (local)│
     ├─ Copy DLL to section       │
     ├─ NtMapViewOfSection ───────→│
     │   (remote)                  │
     └────────────────────────────→│
```

---

## Access Rights Required

### File Mapping Access Rights
```c
// For CreateFileMappingA / OpenFileMappingA
FILE_MAP_WRITE          0x0002    // Write access
FILE_MAP_READ           0x0004    // Read access
FILE_MAP_ALL_ACCESS     0x000F001F // All access rights
FILE_MAP_EXECUTE        0x0020    // Execute access
FILE_MAP_COPY           0x0001    // Copy-on-write

// Typical combination for injection
DWORD dwAccess = FILE_MAP_ALL_ACCESS;


// For injection
DWORD dwSectionAccess = SECTION_ALL_ACCESS;
```

### Process Access Rights
```c
// For OpenProcess (remote mapping)
PROCESS_VM_OPERATION    0x0008    // Required for mapping
PROCESS_VM_READ         0x0010    // Read process memory
PROCESS_VM_WRITE        0x0020    // Write process memory
PROCESS_CREATE_THREAD   0x0002    // Optional for thread creation
PROCESS_QUERY_INFORMATION 0x0400 // Query process info

// Combined for section injection
DWORD dwProcessAccess = PROCESS_VM_OPERATION | 
                        PROCESS_VM_READ | 
                        PROCESS_VM_WRITE |
                        PROCESS_QUERY_INFORMATION;
```

---


## Security Considerations

### Section Object Security
- **Named Sections**: Global sections accessible across sessions require admin rights
- **Session Isolation**: Local sections isolated to current session
- **Security Descriptors**: Can specify custom security attributes
- **Object Hijacking**: Named sections vulnerable to race conditions
- **Namespace**: Proper namespace (Global\\ vs Local\\) prevents unintended access

### Detection and Mitigation
- **Section Monitoring**: Security tools monitor NtCreateSection calls
- **Memory Scanning**: Injected sections detectable by memory analysis
- **Handle Tracking**: Suspicious section handles trigger alerts
- **Integrity Checks**: Modern EDR validates section contents
- **Kernel Callbacks**: PsSetCreateProcessNotifyRoutineEx can detect mapping


---

## Common Injection Failures

### Error Scenarios
| Error | Cause | Solution |
|-------|-------|----------|
| CreateFileMapping fails | Invalid size or protection | Verify parameters and rights |
| MapViewOfFile returns NULL | Insufficient address space | Check available memory |
| OpenFileMapping fails | Section doesn't exist | Verify section name and namespace |
| NtCreateSection fails | Invalid parameters | Check NTSTATUS code details |
| Remote mapping fails | Insufficient process rights | Ensure PROCESS_VM_OPERATION |
| Address collision | Requested address occupied | Use NULL for automatic selection |
| SEC_IMAGE failure | Invalid PE file | Validate PE headers and structure |

### NTSTATUS Error Codes
```c
// Common NtCreateSection / NtMapViewOfSection errors
STATUS_SUCCESS              0x00000000  // Success
STATUS_INVALID_PARAMETER    0xC000000D  // Invalid parameter
STATUS_ACCESS_DENIED        0xC0000022  // Access denied
STATUS_INSUFFICIENT_RESOURCES 0xC000009A // Out of memory
STATUS_SECTION_TOO_BIG      0xC0000040  // Section exceeds limits
STATUS_CONFLICTING_ADDRESSES 0xC0000018 // Address conflict
```

---

## Architecture Considerations

### 32-bit vs 64-bit
- **Architecture Matching**: Section contents must match target architecture
- **Address Space**: 32-bit limited to 2GB user space, 64-bit has much more
- **Pointer Sizes**: Handle structure size differences in section data
- **WOW64**: Special considerations when injecting between architectures
- **Native APIs**: Function prototypes differ between architectures

---

## Advantages Over Traditional Injection

### Section Mapping Benefits
1. **Stealth**: No CreateRemoteThread detection
2. **Efficiency**: Single copy shared across processes
3. **Flexibility**: Map/unmap without recreation
4. **No LoadLibrary**: Bypass user-mode DLL loading
5. **Direct Execution**: Jump directly to code
6. **Copy-on-Write**: Efficient memory usage

### Performance Comparison
```
Traditional Injection:
- VirtualAllocEx: Allocates new memory in target
- WriteProcessMemory: Copies entire DLL
- CreateRemoteThread: Creates new thread
- LoadLibrary: Loads and relocates DLL
Total Time: ~50-100ms

Section Mapping:
- NtCreateSection: Creates section once
- NtMapViewOfSection: Maps existing memory
- Direct execution: No loading overhead
Total Time: ~10-30ms
```
---

## Required Libraries
- `kernel32.lib` - User-mode file mapping APIs
- `ntdll.lib` - Native API functions (NtCreateSection, NtMapViewOfSection)
- `advapi32.lib` - Security descriptor functions (optional)

---

## API References
- MSDN: [CreateFileMappingA](https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-createfilemappinga)
- MSDN: [MapViewOfFile](https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-mapviewoffile)
- MSDN: [MapViewOfFile2](https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-mapviewoffile2)
- MSDN: [MapViewOfFile3](https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-mapviewoffile3)
- MSDN: [MapViewOfFileEx](https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-mapviewoffileex)
- MSDN: [OpenFileMappingA](https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-openfilemappinga)
- MSDN: [File Mapping](https://docs.microsoft.com/en-us/windows/win32/memory/file-mapping)
- Undocumented NT: [NtCreateSection](http://undocumented.ntinternals.net/)
- Undocumented NT: [NtMapViewOfSection](http://undocumented.ntinternals.net/)

---

## Legal and Ethical Notice

**Important**: This code is provided for educational and research purposes only. Section mapping techniques can be used for both legitimate purposes (IPC, shared memory, debugging) and malicious purposes (malware injection, privilege escalation, rootkits).

### Legitimate Use Cases
- Inter-process communication systems
- Shared memory databases
- Memory-mapped file I/O optimization
- Debugging and process analysis tools
- Security research in controlled environments
- High-performance computing applications

### Legal Considerations
- Only inject into processes you own or have explicit permission to modify
- Understand computer fraud and abuse laws in your jurisdiction
- Comply with software licenses and terms of service
- Obtain proper authorization before testing on systems
- Follow responsible disclosure for security vulnerabilities
- Document and report findings appropriately.


**The author assumes no liability for misuse of this code. Users are solely responsible for ensuring their use complies with all applicable laws and regulations.**

---

<p align="right">
  Last Updated: November 2025
</p>