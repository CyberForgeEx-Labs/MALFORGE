<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Injection/Classic%20DLL%20Injection/Classic_DLL_Injection.c">Classic DLL Injection Techniques</a>
</h1>

## Overview
Classic DLL injection APIs provide essential Windows capabilities for loading libraries into remote processes, creating threads in target processes, and manipulating process memory. These APIs form the foundation of traditional DLL injection techniques used in debugging, process monitoring, game modding, security research, and malware analysis. This demonstration covers three primary injection methods and the supporting APIs required for successful implementation.

---

## Core APIs

| API | Purpose | Sophistication |
|-----|---------|----------------|
| `CreateRemoteThread` | Creates thread in remote process | Intermediate |
| `CreateRemoteThreadEx` | Extended remote thread creation | Intermediate |
| `LoadLibraryA` | Loads DLL into process | Basic |
| `LoadLibraryExA` | Extended DLL loading with flags | Basic |
| `GetModuleHandleA` | Gets handle to loaded module | Basic |
| `GetProcAddress` | Gets function address from module | Basic |
| `WriteProcessMemory` | Writes data to process memory | Intermediate |
| `VirtualAllocEx` | Allocates memory in remote process | Intermediate |
| `OpenProcess` | Opens handle to target process | Intermediate |
| `OpenThread` | Opens handle to thread | Basic |
| `WaitForSingleObject` | Waits for object signaling | Basic |
| `NtCreateThreadEx` | Native API for remote thread creation | Advanced |

---

## 1. CreateRemoteThread
### Description
Creates a thread that runs in the virtual address space of another process. This is the classic method for DLL injection, where the thread executes LoadLibraryA in the target process to load a specified DLL. Requires PROCESS_CREATE_THREAD, PROCESS_VM_OPERATION, PROCESS_VM_WRITE, and PROCESS_VM_READ access rights. The function returns immediately, and the new thread begins execution.

### Key Features
- Remote thread creation
- Synchronous execution control
- Return value retrieval
- Thread attribute specification
- Security descriptor support
- Immediate thread start

### Use Cases
- Classic DLL injection
- Remote code execution
- Process instrumentation
- Debugging tools
- Game modding
- Security research


---

## 2. CreateRemoteThreadEx
### Description
Extended version of CreateRemoteThread that provides additional control over thread creation through attribute lists. Allows specification of advanced thread attributes such as security descriptors, ideal processor, and thread flags. Offers more flexibility than the standard CreateRemoteThread for modern injection scenarios. Available on Windows Vista and later.

### Key Features
- Extended attribute support
- Thread attribute lists
- Enhanced security control
- Processor affinity hints
- Stack size specification
- Thread creation flags

### Use Cases
- Modern DLL injection
- Enhanced security contexts
- Processor-specific threading
- Advanced debugging
- Security tool development
- Process monitoring

---

## 3. NtCreateThreadEx
### Description
Native API function from ntdll.dll for creating threads in remote processes. This undocumented API provides lower-level control over thread creation and is often used to bypass detection mechanisms that monitor CreateRemoteThread. Requires direct invocation via GetProcAddress and uses NTSTATUS return codes. More stealthy than kernel32 alternatives but less portable.

### Key Features
- Native API access
- Lower-level control
- Stealth capabilities
- NTSTATUS error codes
- Extended parameters
- Bypass potential for monitoring

### Use Cases
- Stealthy DLL injection
- Anti-detection techniques
- Advanced malware research
- Security tool evasion testing
- Rootkit development research
- Low-level debugging

---

## 4. LoadLibraryA
### Description
Loads the specified module (DLL) into the address space of the calling process. The DLL's entry point (DllMain) is called with DLL_PROCESS_ATTACH. Used as the target function for remote thread execution in DLL injection. The ANSI version accepts single-byte character strings. Returns a module handle that can be used with GetProcAddress.

### Key Features
- Dynamic library loading
- DllMain invocation
- Module handle return
- Path search capability
- Dependency resolution
- ANSI string support

### Use Cases
- DLL injection target function
- Plugin loading
- Dynamic feature loading
- Extension systems
- Runtime library access
- Modular application design

---

## 5. LoadLibraryExA
### Description
Extended version of LoadLibraryA that accepts additional flags controlling the DLL loading behavior. Flags include DONT_RESOLVE_DLL_REFERENCES (load without calling DllMain), LOAD_LIBRARY_AS_DATAFILE (load as data file), and LOAD_WITH_ALTERED_SEARCH_PATH (alternate search path). Provides fine-grained control over library loading for specialized scenarios.

### Key Features
- Flag-based loading control
- DllMain bypass option
- Data-only loading
- Alternate search paths
- Image resource access
- Enhanced loading options

### Use Cases
- Resource extraction
- DLL analysis without execution
- Custom loading behavior
- Security analysis
- Library inspection
- Controlled initialization

---

## 6. GetModuleHandleA
### Description
Retrieves a handle to a module that is already loaded in the calling process. Does not increment the module's reference count and does not load the module if it's not already present. The ANSI version accepts single-byte character strings. Essential for obtaining base addresses of system DLLs like kernel32.dll and ntdll.dll for subsequent function resolution.

### Key Features
- No reference counting
- Fast module lookup
- No loading overhead
- Null for current process executable
- ANSI string support
- System DLL access

### Use Cases
- Function address resolution
- Module base address retrieval
- System DLL access
- Injection preparation
- Address calculation
- Runtime module inspection

---

## 7. GetProcAddress
### Description
Retrieves the address of an exported function or variable from a specified DLL. Returns a pointer that can be cast to the appropriate function type. Used in DLL injection to obtain the address of LoadLibraryA for remote thread execution. Function names are case-sensitive. Can also retrieve exports by ordinal number.

### Key Features
- Export resolution
- Function pointer retrieval
- Ordinal support
- Case-sensitive lookup
- Cross-module access
- Type-casting capability

### Use Cases
- Function pointer resolution
- Dynamic API calling
- Injection address calculation
- Plugin interface discovery
- API hooking preparation
- Runtime binding

---

## 8. WriteProcessMemory
### Description
Writes data to an area of memory in a specified process. Requires PROCESS_VM_WRITE and PROCESS_VM_OPERATION access rights. The entire write operation must occur within one memory region. Used in DLL injection to write the DLL path string into the target process's memory space before creating a remote thread.

### Key Features
- Cross-process writing
- Byte-level precision
- Size validation
- Partial write detection
- Access rights enforcement
- Error reporting

### Use Cases
- DLL path injection
- Remote code writing
- Process patching
- Memory modification
- Hook installation
- Data injection

---

## 9. VirtualAllocEx
### Description
Allocates memory within the virtual address space of a specified process. Requires PROCESS_VM_OPERATION access right. Supports various allocation types (MEM_COMMIT, MEM_RESERVE) and protection flags (PAGE_READWRITE, PAGE_EXECUTE_READWRITE). Returns the base address of allocated region. Essential for allocating space for DLL paths and shellcode in target processes.

### Key Features
- Remote memory allocation
- Page-level granularity
- Protection specification
- Allocation type control
- Address hinting
- Reserve and commit options

### Use Cases
- DLL injection buffer allocation
- Shellcode buffer creation
- Parameter passing
- Remote data structures
- Code cave creation
- Process memory expansion

---

## 10. OpenProcess
### Description
Opens an existing process object and returns a handle with specified access rights. Access rights include PROCESS_VM_READ, PROCESS_VM_WRITE, PROCESS_VM_OPERATION, PROCESS_CREATE_THREAD, and PROCESS_QUERY_INFORMATION. The handle must be closed with CloseHandle when no longer needed. Requires appropriate privileges based on target process security.

### Key Features
- Process handle acquisition
- Access rights specification
- Handle inheritance control
- Security context based
- Multiple access flags
- Process ID targeting

### Use Cases
- Injection target opening
- Process manipulation
- Memory access
- Thread creation preparation
- Process monitoring
- Security analysis

---

## 11. OpenThread
### Description
Opens an existing thread object and returns a handle with specified access rights. Common access rights include THREAD_SUSPEND_RESUME, THREAD_GET_CONTEXT, THREAD_SET_CONTEXT, and THREAD_QUERY_INFORMATION. Used for thread manipulation and monitoring. The handle must be closed with CloseHandle. Requires appropriate privileges based on thread security.

### Key Features
- Thread handle acquisition
- Access rights control
- Security-based access
- Thread ID targeting
- Handle inheritance option
- Cross-process capability

### Use Cases
- Thread monitoring
- Context manipulation
- Suspend/resume control
- Thread inspection
- Security analysis
- Debugging operations

---

## 12. WaitForSingleObject
### Description
Waits until the specified object is in the signaled state or the timeout interval elapses. Used in DLL injection to wait for the remote thread to complete execution of LoadLibraryA. Returns WAIT_OBJECT_0 if the object is signaled, WAIT_TIMEOUT if the interval elapses, or WAIT_FAILED on error. Supports infinite timeout with INFINITE constant.

### Key Features
- Object state waiting
- Timeout support
- Return value indication
- Multiple wait states
- Infinite wait option
- Error detection

### Use Cases
- Thread completion waiting
- Synchronization
- Injection verification
- Process synchronization
- Event waiting
- Mutex acquisition

---

## DLL Injection Process Flow

### Classic Injection Steps
1. **Target Identification**: Find target process by name or PID
2. **Process Opening**: Call OpenProcess with required access rights
3. **Memory Allocation**: Use VirtualAllocEx to allocate buffer for DLL path
4. **Path Writing**: Write DLL path string with WriteProcessMemory
5. **Function Resolution**: Get LoadLibraryA address with GetModuleHandleA and GetProcAddress
6. **Thread Creation**: Create remote thread with CreateRemoteThread/Ex or NtCreateThreadEx
7. **Execution Wait**: Use WaitForSingleObject to wait for completion
8. **Cleanup**: Free allocated memory and close handles

### Memory Layout
```
Target Process Memory Space:
┌─────────────────────────────┐
│ Process Code & Data         │
├─────────────────────────────┤
│ System DLLs                 │
│ - kernel32.dll              │
│ - ntdll.dll                 │
│ - user32.dll                │
├─────────────────────────────┤
│ Allocated Buffer            │ ← VirtualAllocEx
│ "C:\Path\To\Inject.dll\0"  │ ← WriteProcessMemory
├─────────────────────────────┤
│ Remote Thread Stack         │ ← CreateRemoteThread
│ Entry: LoadLibraryA         │
│ Param: Buffer Address       │
└─────────────────────────────┘
```

---

## Access Rights Required

### Process Access Rights
```c
// For standard DLL injection
PROCESS_CREATE_THREAD    0x0002  // Create threads in process
PROCESS_VM_OPERATION     0x0008  // Modify virtual memory
PROCESS_VM_WRITE         0x0020  // Write to virtual memory
PROCESS_VM_READ          0x0010  // Read virtual memory
PROCESS_QUERY_INFORMATION 0x0400 // Query process information

// Combined for injection
DWORD dwAccess = PROCESS_CREATE_THREAD | 
                 PROCESS_VM_OPERATION | 
                 PROCESS_VM_WRITE | 
                 PROCESS_VM_READ | 
                 PROCESS_QUERY_INFORMATION;
```

### Thread Access Rights
```c
THREAD_QUERY_INFORMATION 0x0040  // Query thread information
THREAD_SUSPEND_RESUME    0x0002  // Suspend/resume thread
SYNCHRONIZE              0x00100000 // Wait on thread object
THREAD_ALL_ACCESS        0x1FFFFF // All possible access rights
```

---

## Security Considerations

### Process Protection
- **Protected Processes**: Cannot inject into protected processes (e.g., System, CSRSS)
- **Session Isolation**: Session 0 isolation prevents injection into services
- **Integrity Levels**: Must have equal or higher integrity level than target
- **Administrator Rights**: Many injections require elevated privileges
- **Anti-Malware**: Modern AV/EDR solutions detect and block injection attempts

### Detection Mechanisms
- **API Monitoring**: Security software monitors CreateRemoteThread
- **Memory Scanning**: Injected DLLs can be detected by memory scanners
- **Behavioral Analysis**: Unusual process relationships trigger alerts
- **Thread Analysis**: Unexpected threads from kernel32 are suspicious
- **Module Validation**: Unsigned or suspicious DLLs are flagged

---

## Common Injection Failures

### Error Scenarios
| Error | Cause | Solution |
|-------|-------|----------|
| OpenProcess fails | Insufficient privileges | Run as administrator |
| VirtualAllocEx fails | Invalid parameters or access | Check access rights and size |
| WriteProcessMemory fails | Protection violation | Verify memory protection flags |
| CreateRemoteThread fails | No thread creation rights | Add PROCESS_CREATE_THREAD |
| LoadLibraryA returns NULL | DLL not found or invalid | Verify full DLL path |
| WaitForSingleObject timeout | Thread execution blocked | Increase timeout or investigate |

---

## Architecture Considerations

### 32-bit vs 64-bit
- **Same Architecture Required**: 32-bit process cannot inject into 64-bit process
- **WOW64 Limitations**: Special considerations for WOW64 processes
- **Pointer Sizes**: Handle pointer size differences correctly
- **System DLL Paths**: Different locations for System32 vs SysWOW64
- **Function Addresses**: Addresses differ between architectures.

---

## Usage Examples

### Basic Demonstration (No Injection)
```bash
Classic_DLL_Injection.exe
```
This runs API demonstrations without performing actual injection.

### DLL Injection
```bash
Classic_DLL_Injection.exe notepad.exe C:\Temp\MyDLL.dll
```
This attempts to inject MyDLL.dll into notepad.exe process.

### Administrative Execution
```bash
# Run Command Prompt as Administrator, then:
Classic_DLL_Injection.exe explorer.exe C:\Tools\Hook.dll
```

---

## Related APIs

### Memory Management
- `VirtualFreeEx` - Free allocated memory in remote process
- `VirtualProtectEx` - Change memory protection flags
- `ReadProcessMemory` - Read data from remote process
- `VirtualQueryEx` - Query virtual memory information

### Thread Management
- `GetThreadContext` - Retrieve thread context
- `SetThreadContext` - Modify thread context
- `SuspendThread` - Suspend thread execution
- `ResumeThread` - Resume suspended thread
- `TerminateThread` - Terminate thread

### Process Management
- `CreateProcess` - Create new process
- `TerminateProcess` - Terminate process
- `GetExitCodeThread` - Retrieve thread exit code
- `GetExitCodeProcess` - Retrieve process exit code

---

## Required Libraries
- `kernel32.lib` - Core Windows APIs (CreateRemoteThread, LoadLibrary, etc.)
- `ntdll.lib` - Native API (NtCreateThreadEx)
- `advapi32.lib` - Additional process management APIs (optional)

---

## API References
- MSDN: [CreateRemoteThread](https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createremotethread)
- MSDN: [CreateRemoteThreadEx](https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createremotethreadex)
- MSDN: [LoadLibraryA](https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-loadlibrarya)
- MSDN: [LoadLibraryExA](https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-loadlibraryexa)
- MSDN: [GetModuleHandleA](https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulehandlea)
- MSDN: [GetProcAddress](https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getprocaddress)
- MSDN: [WriteProcessMemory](https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-writeprocessmemory)
- MSDN: [VirtualAllocEx](https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualallocex)
- MSDN: [OpenProcess](https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-openprocess)
- MSDN: [OpenThread](https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-openthread)
- MSDN: [WaitForSingleObject](https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-waitforsingleobject)

---

## Legal and Ethical Notice

**Important**: This code is provided for educational and research purposes only. DLL injection techniques can be used for both legitimate purposes (debugging, game modding, security research) and malicious purposes (malware, cheating, unauthorized access).

### Legitimate Use Cases
- Software debugging and development
- Game modification with permission
- Security research in controlled environments
- Anti-cheat system analysis
- Process monitoring tools
- Educational demonstrations

### Legal Considerations
- Only inject into processes you own or have permission to modify
- Understand and comply with computer fraud laws in your jurisdiction
- Respect software licenses and terms of service.
- Obtain proper authorization before testing on production systems
- Follow responsible disclosure practices for security research

### Ethical Guidelines
- Do not use for unauthorized access or data theft
- Do not bypass security mechanisms without authorization
- Do not use to cheat in online games
- Do not distribute malicious payloads
- Always prioritize system stability and user safety

**The author assume no liability for misuse of this code. Users are responsible for ensuring their use complies with applicable laws and regulations.**

---

<p align="right">
  Last Updated: November 2025
</p>
