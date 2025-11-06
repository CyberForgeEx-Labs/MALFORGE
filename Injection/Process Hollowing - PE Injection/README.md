<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Injection/Process%20Hollowing%20-%20PE%20Injection/Process_Manipulation_Demonstration.c">Process Management and Thread Control APIs</a>
</h1>

## Overview
Process management APIs provide essential Windows capabilities for creating processes, controlling thread execution, manipulating process memory, and accessing thread context information. These APIs form the foundation for debuggers, process monitoring tools, security research, and legitimate software development. This demonstration covers seven primary APIs used in process and thread management, focusing on their educational and legitimate applications.

---

## Core APIs

| API | Purpose | Sophistication |
|-----|---------|----------------|
| `CreateProcessA` | Creates new process with control options | Intermediate |
| `GetThreadContext` | Retrieves thread CPU register state | Advanced |
| `SetThreadContext` | Modifies thread CPU register state | Advanced |
| `SuspendThread` | Suspends thread execution | Intermediate |
| `ResumeThread` | Resumes suspended thread | Intermediate |
| `VirtualAllocEx` | Allocates memory in remote process | Intermediate |
| `WriteProcessMemory` | Writes data to process memory | Intermediate |
| `NtUnmapViewOfSection` | Native API to unmap memory sections | Advanced |
| `NtResumeThread` | Native API for thread resumption | Advanced |
| `NtSuspendProcess` | Native API to suspend entire process | Advanced |

---

## 1. CreateProcessA
### Description
Creates a new process and its primary thread. The process can be created in a suspended state using the CREATE_SUSPENDED flag, allowing initialization and setup before execution begins. Provides extensive control over process creation including security attributes, environment variables, startup information, and creation flags. Returns handles to both the process and its primary thread in a PROCESS_INFORMATION structure.

### Key Features
- Process creation with full control
- Suspended state option
- Startup info customization
- Security descriptor specification
- Environment block control
- Working directory setting
- Handle inheritance options
- Creation flags support

### Use Cases
- Debugger process launching
- Sandboxed execution
- Process monitoring setup
- Security testing
- Child process management
- Application launchers
- Development tools
- Automated testing

### Example Output
```
================================================================
CreateProcessA - Creating Suspended Process
================================================================

[*] Creating notepad.exe in SUSPENDED state...
[+] Process created successfully!
    Process ID: 12345
    Thread ID: 67890
    Process Handle: 0x0000000000000194
    Thread Handle: 0x0000000000000198
    State: SUSPENDED (not running yet)

[*] Process is suspended. It will not execute until ResumeThread is called.
[*] Terminating suspended process for cleanup...
[+] Cleanup completed.
```

---

## 2. GetThreadContext
### Description
Retrieves the context (CPU register state) of a specified thread. The context includes all general-purpose registers, instruction pointer (EIP/RIP), stack pointer (ESP/RSP), base pointer (EBP/RBP), and flags register. The thread must be suspended or owned by the calling process. Essential for debuggers to examine execution state, implement breakpoints, and perform single-stepping. Architecture-specific - different structures for x86 and x64.

### Key Features
- Complete register state retrieval
- Instruction pointer access
- Stack pointer examination
- Flags register reading
- Debug register access
- Floating-point context
- Extended registers (x64)
- Context flags for selective retrieval

### Use Cases
- Debugger implementation
- Exception handling
- Thread state inspection
- Call stack unwinding
- Performance profiling
- Security analysis
- Crash dump analysis
- Educational purposes

### Example Output
```
================================================================
GetThreadContext - Reading Thread CPU Context
================================================================

[*] Creating suspended process to demonstrate GetThreadContext...
[+] Process created (PID: 12345)
[*] Calling GetThreadContext to read CPU registers...
[+] Thread context retrieved successfully!

CPU Register Values:
    RIP (Instruction Pointer): 0x00007ff6a2e81000
    RSP (Stack Pointer):       0x000000c7f5dff6e0
    RBP (Base Pointer):        0x0000000000000000
    RAX:                       0x0000000000000000
    RBX:                       0x0000000000000000
    RCX:                       0x0000000000000000

[*] These registers show where execution will start when thread resumes.
```

---

## 3. SetThreadContext
### Description
Sets the context (CPU register state) of a specified thread. Allows modification of all registers including instruction pointer to redirect execution flow. The thread must be suspended before calling this function. Commonly used by debuggers to implement features like "set next statement", modify variable values, or skip code sections. Powerful API that requires careful use to maintain program stability and correctness.

### Key Features
- Register state modification
- Execution flow control
- Instruction pointer redirection
- Stack pointer adjustment
- Flags modification
- Debug register setting
- Exception context restoration
- Single-step control

### Use Cases
- Debugger "set next statement" feature
- Exception handler implementation
- Thread hijacking (debuggers)
- Register value modification
- Execution redirection
- Call stack manipulation
- Hook implementation
- Testing and QA tools

### Example Output
```
================================================================
SetThreadContext - Modifying Thread CPU Context
================================================================

[*] Creating suspended process...
[+] Process created (PID: 12345)
[*] Original RIP: 0x00007ff6a2e81000
[*] Demonstrating SetThreadContext (no actual modification)...
[+] SetThreadContext succeeded!
[*] In real scenarios, this API can redirect thread execution.
[*] Used legitimately in debuggers and exception handlers.
```

---

## 4. SuspendThread
### Description
Suspends the execution of a specified thread. Each call increments the thread's suspend count; the thread will not execute until the suspend count reaches zero through corresponding ResumeThread calls. Returns the previous suspend count. Used for thread synchronization, debugging, and process control. Suspended threads continue to exist but do not consume CPU time. Maximum suspend count is MAXIMUM_SUSPEND_COUNT (127).

### Key Features
- Thread execution control
- Suspend count tracking
- Multiple suspension levels
- Immediate effect
- Safe thread freezing
- Cross-process support
- Return previous count
- Debugger support

### Use Cases
- Debugger breakpoint handling
- Thread synchronization
- Safe state inspection
- Memory manipulation preparation
- Process snapshot creation
- Performance analysis
- Resource usage control
- System monitoring

---

## 5. ResumeThread
### Description
Decrements a thread's suspend count. When the suspend count reaches zero, the thread resumes execution. Returns the previous suspend count, or (DWORD)-1 on error. Each call to ResumeThread must correspond to a previous SuspendThread call. Essential counterpart to SuspendThread for process control. Does not guarantee immediate execution - thread enters ready state and waits for CPU scheduling.

### Key Features
- Thread resume control
- Suspend count decrement
- Paired with SuspendThread
- Previous count return
- Scheduling interaction
- Error indication
- Cross-process capability
- Debugger integration

### Use Cases
- Continuing from breakpoints
- Releasing suspended threads
- Process control restoration
- Debugger continue operation
- Thread management
- Synchronization release
- Process resumption
- State restoration

### Example Output
```
================================================================
SuspendThread / ResumeThread - Thread State Control
================================================================

[*] Creating process in normal (running) state...
[+] Process created and running (PID: 12345)
[*] Process is currently executing normally...

[*] Calling SuspendThread...
[+] Thread suspended! Suspend count: 0
    The process is now frozen and not executing.

[*] Calling ResumeThread...
[+] Thread resumed! Previous suspend count: 1
    The process is now executing again.

[*] Terminating process for cleanup...
[+] Cleanup completed.
```

---

## 6. VirtualAllocEx
### Description
Allocates memory within the virtual address space of a specified process. Provides control over allocation type (MEM_COMMIT, MEM_RESERVE), size, and memory protection (PAGE_READWRITE, PAGE_EXECUTE_READWRITE). Returns the base address of the allocated region. Requires PROCESS_VM_OPERATION access right. Essential for inter-process data transfer and remote memory management. Memory is automatically freed when the process terminates.

### Key Features
- Remote memory allocation
- Page-aligned addressing
- Protection specification
- Allocation type control
- Size specification
- Address hinting support
- NULL for automatic placement
- Error handling

### Use Cases
- Inter-process communication
- Shared data structures
- Parameter passing to remote threads
- DLL injection preparation
- Buffer allocation for WriteProcessMemory
- Process instrumentation
- Memory management tools
- Development utilities

### Example Output
```
================================================================
VirtualAllocEx - Allocating Memory in Remote Process
================================================================

[*] Creating target process...
[+] Process created (PID: 12345)
[*] Allocating 4096 bytes in remote process...
[+] Memory allocated successfully!
    Base Address: 0x0000020a1b2f0000
    Size: 4096 bytes
    Protection: PAGE_READWRITE
    Type: MEM_COMMIT | MEM_RESERVE

[*] This memory exists in the target process's address space.
[*] Can be written to using WriteProcessMemory.
[*] Memory freed.
```

---

## 7. WriteProcessMemory
### Description
Writes data to a specified memory area in another process. Requires PROCESS_VM_WRITE and PROCESS_VM_OPERATION access rights. The entire write operation must occur within a single memory region with the same protection. Returns the number of bytes actually written. Used for inter-process communication, process patching, and parameter passing. Automatically handles page protection changes if possible.

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
- Inter-process data transfer
- Parameter passing
- Configuration injection
- Debugging tools
- Process instrumentation
- Data patching (with permission)
- Testing frameworks
- Development utilities

### Example Output
```
================================================================
WriteProcessMemory - Writing Data to Remote Process
================================================================

[*] Creating target process...
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
```

---

## 8. NtUnmapViewOfSection
### Description
Native API function from ntdll.dll that unmaps a view of a section from the virtual address space of a process. This is a lower-level API than the kernel32 equivalents. Used to unmap memory-mapped files, shared memory sections, or loaded executables. Returns NTSTATUS codes rather than Win32 error codes. Undocumented but widely used in system programming and security research.

### Key Features
- Native API access
- Section unmapping
- Process memory manipulation
- NTSTATUS return codes
- Lower-level control
- Executable image unmapping
- Shared memory handling
- Advanced memory management

### Use Cases
- Memory-mapped file management
- Shared memory cleanup
- Low-level memory operations
- Security research (educational)
- System programming
- Advanced debugging
- Memory forensics
- Educational demonstrations

---

## 9. NtResumeThread
### Description
Native API version of ResumeThread from ntdll.dll. Decrements the thread's suspend count and returns the previous count. Uses NTSTATUS return codes instead of Win32 error codes. Provides identical functionality to kernel32's ResumeThread but at a lower abstraction level. Sometimes used to avoid user-mode API hooks or for direct system interaction. Returns STATUS_SUCCESS (0) on success.

### Key Features
- Native API access
- Suspend count decrement
- NTSTATUS return codes
- Lower-level than kernel32
- Previous count return
- Direct system call
- Hook bypass potential
- System-level control

### Use Cases
- Low-level thread control
- System programming
- Direct NT API usage
- Educational purposes
- Advanced debugging
- Research tools
- Kernel interaction
- API comparison studies

---

## 10. NtSuspendProcess
### Description
Native API function that suspends all threads in a specified process. Unlike SuspendThread which targets a single thread, this suspends the entire process atomically. No direct kernel32 equivalent exists. Returns NTSTATUS codes. Commonly used in process debugging, snapshots, and system tools. More efficient than suspending each thread individually. Requires PROCESS_SUSPEND_RESUME access right.

### Key Features
- Whole-process suspension
- Atomic operation
- All threads affected
- NTSTATUS return codes
- Native API access
- No kernel32 equivalent
- Efficient bulk operation
- Process-wide freezing

### Use Cases
- Process snapshot creation
- Debugging entire processes
- System monitoring tools
- Process analysis
- Memory dump creation
- Security research
- Process control utilities
- Educational demonstrations

### Example Output
```
================================================================
Native APIs - ntdll.dll Function Demonstration
================================================================

[+] ntdll.dll base address: 0x00007ffd14b00000

Native API Addresses:
    NtUnmapViewOfSection: 0x00007ffd14b2a450
    NtResumeThread:       0x00007ffd14b2e780
    NtSuspendProcess:     0x00007ffd14b2f1a0

[*] These are undocumented/semi-documented native APIs from ntdll.dll
[*] They provide lower-level access than kernel32.dll functions

API Purposes:
    - NtUnmapViewOfSection: Unmaps a section from process address space
    - NtResumeThread:       Native version of ResumeThread
    - NtSuspendProcess:     Suspends entire process (all threads)

[*] Created suspended process (PID: 12345)
[*] Calling NtResumeThread (native API)...
[+] NtResumeThread succeeded!
    NTSTATUS: 0x00000000 (STATUS_SUCCESS)
    Previous suspend count: 1

[*] Calling NtSuspendProcess...
[+] NtSuspendProcess succeeded!
    NTSTATUS: 0x00000000 (STATUS_SUCCESS)
    Entire process is now suspended.
```

---

## Process Management Flow

### Typical Process Control Sequence
1. **Process Creation**: Call CreateProcessA with CREATE_SUSPENDED flag
2. **Handle Acquisition**: Receive process and thread handles in PROCESS_INFORMATION
3. **Context Retrieval**: Use GetThreadContext to examine initial state
4. **Memory Allocation**: Allocate buffers with VirtualAllocEx if needed
5. **Data Writing**: Write data to process memory with WriteProcessMemory
6. **Context Modification**: Optionally modify registers with SetThreadContext
7. **Execution Control**: Resume thread with ResumeThread or NtResumeThread
8. **Monitoring**: Suspend/resume as needed for inspection
9. **Cleanup**: Close handles and free allocated resources

### Memory Management
```
Process Memory Layout:
┌─────────────────────────────┐
│ Executable Image            │ ← Original program
├─────────────────────────────┤
│ System DLLs                 │ ← kernel32, ntdll, etc.
├─────────────────────────────┤
│ Stack                       │ ← Thread stacks
├─────────────────────────────┤
│ Heap                        │ ← Dynamic allocations
├─────────────────────────────┤
│ VirtualAllocEx Buffers      │ ← Remote allocations
│ - Data buffers              │
│ - Communication areas       │
└─────────────────────────────┘
```

---

## Access Rights Required

### Process Access Rights
```c
// For process management and debugging
PROCESS_CREATE_THREAD       0x0002  // Create threads
PROCESS_VM_OPERATION        0x0008  // Virtual memory operations
PROCESS_VM_WRITE            0x0020  // Write to memory
PROCESS_VM_READ             0x0010  // Read from memory
PROCESS_QUERY_INFORMATION   0x0400  // Query process info
PROCESS_SUSPEND_RESUME      0x0800  // Suspend/resume process
SYNCHRONIZE                 0x00100000 // Wait on process

// Combined for full control
DWORD dwAccess = PROCESS_ALL_ACCESS; // All rights
```

### Thread Access Rights
```c
THREAD_GET_CONTEXT          0x0008  // GetThreadContext
THREAD_SET_CONTEXT          0x0010  // SetThreadContext
THREAD_SUSPEND_RESUME       0x0002  // Suspend/resume
THREAD_QUERY_INFORMATION    0x0040  // Query information
SYNCHRONIZE                 0x00100000 // Wait on thread
THREAD_ALL_ACCESS           0x1FFFFF // All thread rights
```

---

## Architecture Considerations

### 32-bit vs 64-bit Context
```c
// CONTEXT structure differs by architecture
#ifdef _WIN64
    // 64-bit registers
    DWORD64 Rip;    // Instruction pointer
    DWORD64 Rsp;    // Stack pointer
    DWORD64 Rbp;    // Base pointer
    DWORD64 Rax, Rbx, Rcx, Rdx;  // General purpose
#else
    // 32-bit registers
    DWORD Eip;      // Instruction pointer
    DWORD Esp;      // Stack pointer
    DWORD Ebp;      // Base pointer
    DWORD Eax, Ebx, Ecx, Edx;    // General purpose
#endif
```
---

## Security Considerations

### Protection Mechanisms
- **Protected Processes**: Cannot manipulate protected processes (PPL/PPL-Lite)
- **Session Isolation**: Session 0 isolation prevents service manipulation
- **Integrity Levels**: Must have equal or higher integrity than target
- **DEP/ASLR**: Memory protections affect remote operations
- **Anti-Malware**: Security software monitors these APIs closely

### Legitimate Use Guidelines
- Only manipulate processes you own or have permission for
- Use for debugging, development, or authorized testing only
- Implement proper error handling and cleanup
- Respect process security boundaries
- Document the purpose of process manipulation
- Follow organizational security policies
- Test in isolated environments first

### Ethical Boundaries
- **DO**: Use for debugging your own applications
- **DO**: Implement legitimate development tools
- **DO**: Conduct authorized security research
- **DO**: Educational demonstrations in safe environments
- **DON'T**: Manipulate processes without authorization
- **DON'T**: Bypass security mechanisms maliciously
- **DON'T**: Violate software licenses or terms of service
- **DON'T**: Use for cheating, fraud, or unauthorized access

---

## Error Handling

### Common Errors and Solutions
| Error | Cause | Solution |
|-------|-------|----------|
| Access Denied | Insufficient privileges | Run as administrator |
| Invalid Handle | Handle closed or invalid | Verify handle validity |
| Invalid Parameter | Wrong parameter value | Check API documentation |
| Context Mismatch | Thread not suspended | Suspend before GetThreadContext |
| Memory Protection | Write to protected memory | Use VirtualProtectEx first |

---

## Compilation Instructions

### Microsoft Visual C++
```bash
cl.exe Process_Management_Demo.c /link kernel32.lib ntdll.lib
```

### GCC/MinGW
```bash
gcc Process_Management_Demo.c -o Process_Management_Demo.exe -lkernel32 -lntdll
```

---

## Usage Examples

### Basic Demonstration
```bash
Process_Management_Demo.exe
```
Runs all API demonstrations with notepad.exe as target (processes are immediately terminated for cleanup).

### Administrative Execution
```bash
# Run Command Prompt as Administrator
Process_Management_Demo.exe
```
Required for full functionality due to process access restrictions.

---

## Required Libraries
- `kernel32.lib` - Core Windows APIs (CreateProcess, threads, memory)
- `ntdll.lib` - Native API (NtCreateThreadEx, NtUnmapViewOfSection, etc.)

---

## API References
- MSDN: [CreateProcessA](https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessa)
- MSDN: [GetThreadContext](https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getthreadcontext)
- MSDN: [SetThreadContext](https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-setthreadcontext)
- MSDN: [SuspendThread](https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-suspendthread)
- MSDN: [ResumeThread](https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-resumethread)
- MSDN: [VirtualAllocEx](https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualallocex)
- MSDN: [WriteProcessMemory](https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-writeprocessmemory)

---

## Disclaimer

This educational material demonstrates Windows API functionality for learning purposes. The techniques shown can be used legitimately in debuggers, development tools, and authorized security research. However, the same techniques can be misused maliciously.

By using this code, you acknowledge that:
- You will only use it for legitimate, authorized purposes
- You understand the legal implications of process manipulation
- You will not use it to harm systems, violate privacy, or break laws
- You accept full responsibility for your use of these techniques

---

<p align="right">
  Last Updated: November 2025
</p>
