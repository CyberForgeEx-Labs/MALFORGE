<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Injection/APC%20Injection/APC_Injection_Demonstration.c">APC Injection API Demonstration</a>
</h1>

## Overview
Asynchronous Procedure Call (APC) injection is a sophisticated Windows thread hijacking technique that leverages the operating system's native APC mechanism. APCs are functions that execute asynchronously in the context of a specific thread when that thread enters an alertable wait state. This demonstration covers six APIs used in APC injection, focusing on their educational applications in debuggers, thread monitoring, and legitimate software development.

---

## Core APIs

| API | Purpose | Sophistication |
|-----|---------|----------------|
| `QueueUserAPC` | Queues user-mode APC to thread | Intermediate |
| `NtQueueApcThread` | Native API for APC injection | Advanced |
| `NtQueueApcThreadEx` | Extended APC injection with reserve handle | Advanced |
| `NtQueueApcThreadEx2` | Newest extended version with flags | Advanced |
| `OpenThread` | Opens thread handle for APC queueing | Intermediate |

---

## 1. QueueUserAPC
### Description
Adds a user-mode asynchronous procedure call (APC) to the APC queue of a specified thread. The APC function executes when the target thread enters an alertable wait state using functions like SleepEx, WaitForSingleObjectEx, or MsgWaitForMultipleObjectsEx with the bAlertable parameter set to TRUE. This is the primary documented method for user-mode APC injection. The function returns immediately after queueing; actual execution is deferred until the thread becomes alertable.

### Key Features
- User-mode APC queueing
- Deferred execution model
- Alertable wait requirement
- Thread context execution
- Single parameter passing
- Non-blocking operation
- Simple documented API
- Reliable execution guarantee

### Use Cases
- Thread notification mechanisms
- Asynchronous callback implementation
- Debugger thread control
- Inter-thread communication
- Timer callbacks (SetWaitableTimer)
- I/O completion (ReadFileEx, WriteFileEx)
- Thread interruption for cleanup
- Educational thread manipulation

### Example Output
```
================================================================
QueueUserAPC - Queue APC to Thread
================================================================

[*] Created alertable thread
[Thread] Alertable thread started (TID: 12345)
[Thread] Entering alertable wait state...
[*] Queuing APC using QueueUserAPC...
[+] QueueUserAPC succeeded!
    APC queued to thread
    Will execute when thread enters alertable wait

[APC] APC executed with parameter: 12345
[Thread] Returned from alertable wait
[*] Thread completed and cleaned up
```

---

## 2. NtQueueApcThread
### Description
Native API function from ntdll.dll that queues an APC to a specified thread. This is the lower-level version of QueueUserAPC, providing direct access to the native NT kernel interface. Accepts three optional parameters (compared to QueueUserAPC's single parameter), allowing more complex data passing. Returns NTSTATUS codes instead of Win32 error codes. Undocumented but widely used in system programming and security research for finer control over APC behavior.

### Key Features
- Native NT API access
- Three parameter support
- NTSTATUS return codes
- Lower abstraction level
- Direct kernel interaction
- Bypass user-mode hooks
- No additional validation
- Enhanced control

### Use Cases
- Low-level thread control
- System programming
- Security research tools
- Anti-detection techniques
- Advanced debugging utilities
- Kernel communication bridges
- Custom APC implementations
- Educational demonstrations

### Example Output
```
================================================================
NtQueueApcThread - Native API APC Queueing
================================================================

[*] Created alertable thread
[Thread] Alertable thread started (TID: 12345)
[Thread] Entering alertable wait state...
[*] Queuing APC using NtQueueApcThread (native API)...
[+] NtQueueApcThread succeeded!
    NTSTATUS: 0x00000000 (STATUS_SUCCESS)
    Native API successfully queued APC

[APC] APC executed with parameter: 67890
[Thread] Returned from alertable wait
[*] Thread completed and cleaned up
```

---

## 3. NtQueueApcThreadEx
### Description
Extended native API version of NtQueueApcThread that adds a ReserveHandle parameter for special APC queueing scenarios. Introduced in Windows Vista to support kernel-mode APCs from user mode under specific conditions. The ReserveHandle is typically NULL for standard user-mode APCs. Provides additional control over APC delivery and execution priority. Used in advanced scenarios requiring precise APC behavior control or kernel-mode APC scheduling from user mode.

### Key Features
- Reserve handle support
- Extended control options
- Vista+ availability
- Kernel-mode APC support
- Priority control
- Enhanced flexibility
- Special queueing modes
- Advanced scenarios

### Use Cases
- Advanced APC management
- Kernel-mode APC from user mode
- Priority-based execution
- Complex synchronization scenarios
- System-level programming
- Advanced research tools
- Custom execution contexts
- Extended debugging features

### Example Output
```
================================================================
NtQueueApcThreadEx - Extended Native APC Queueing
================================================================

[*] Created alertable thread
[Thread] Alertable thread started (TID: 12345)
[Thread] Entering alertable wait state...
[*] Queuing APC using NtQueueApcThreadEx...
[+] NtQueueApcThreadEx succeeded!
    NTSTATUS: 0x00000000 (STATUS_SUCCESS)
    Extended native API queued APC
    Provides additional control over APC queueing

[APC] APC executed with parameter: 11111
[Thread] Returned from alertable wait
[*] Thread completed and cleaned up
```

---

## 4. NtQueueApcThreadEx2
### Description
The newest extension of the native APC queueing API, introduced in Windows 10 version 1607 (Anniversary Update). Adds an ApcFlags parameter that provides fine-grained control over APC behavior, including special APC modes and execution characteristics. Supports both user-mode and special kernel-mode APCs through flag specification. Represents the most advanced and flexible user-mode accessible APC API. May not be available on older Windows versions.

### Key Features
- Flags-based control
- Windows 10 1607+ only
- Special APC modes
- Enhanced flexibility
- Execution characteristic control
- User and kernel mode support
- Most advanced user-mode API
- Future-proof interface

### Use Cases
- Modern APC techniques
- Flag-controlled behavior
- Advanced synchronization
- Special execution modes
- Latest Windows features
- Research on modern systems
- Future-compatible code
- Cutting-edge debugging

### Example Output
```
================================================================
NtQueueApcThreadEx2 - Newer Extended APC Queueing
================================================================

[*] Created alertable thread
[Thread] Alertable thread started (TID: 12345)
[Thread] Entering alertable wait state...
[*] Queuing APC using NtQueueApcThreadEx2...
[+] NtQueueApcThreadEx2 succeeded!
    NTSTATUS: 0x00000000 (STATUS_SUCCESS)
    Newest extended API with flags support
    Allows special APC queueing modes

[APC] APC executed with parameter: 22222
[Thread] Returned from alertable wait
[*] Thread completed and cleaned up
```

---

## 5. OpenThread
### Description
Opens an existing thread object and returns a handle with specified access rights. Essential prerequisite for cross-process APC injection, as it provides the thread handle needed by QueueUserAPC and native APC APIs. Requires appropriate access rights (THREAD_SET_CONTEXT) to queue APCs. Can target threads in other processes if the calling process has sufficient privileges. The returned handle must be closed with CloseHandle to prevent resource leaks.

### Key Features
- Thread handle acquisition
- Access rights specification
- Cross-process capability
- Handle management
- Security context enforcement
- Privilege requirements
- Resource cleanup
- Standard handle operations

### Use Cases
- Cross-process APC injection
- Remote thread manipulation
- Handle acquisition for APIs
- Process instrumentation
- Debugging other processes
- Thread monitoring tools
- Security research
- Inter-process control

### Example Output
```
================================================================
OpenThread - Opening Thread for APC Injection
================================================================

[*] Created thread (TID: 12345)
[Thread] Alertable thread started (TID: 12345)
[Thread] Entering alertable wait state...
[*] Opening thread handle using OpenThread...
[+] OpenThread succeeded!
    Thread handle: 0x00000000000001A4
    Thread ID: 12345
    Access rights: THREAD_SET_CONTEXT | THREAD_QUERY_INFORMATION

[*] Queuing APC to opened thread...
[+] APC queued successfully using OpenThread handle

[APC] APC executed with parameter: 99999
[Thread] Returned from alertable wait
[*] Thread completed and cleaned up
```

---


## APC Execution Flow

### Standard APC Injection Sequence
1. **Target Selection**: Identify target thread (own process or remote)
2. **Handle Acquisition**: Use OpenThread if targeting remote thread
3. **APC Queueing**: Call QueueUserAPC or native variant
4. **Wait for Alertable State**: Thread must enter alertable wait
5. **APC Execution**: System dequeues and executes APC routine
6. **Return to Normal**: Thread continues after APC completes
7. **Handle Cleanup**: Close thread handle if opened

### Alertable Wait States
```
Functions that Enable APC Execution:
┌─────────────────────────────────────┐
│ SleepEx(time, TRUE)                 │ ← Most common
├─────────────────────────────────────┤
│ WaitForSingleObjectEx(..., TRUE)    │
├─────────────────────────────────────┤
│ WaitForMultipleObjectsEx(..., TRUE) │
├─────────────────────────────────────┤
│ MsgWaitForMultipleObjectsEx(..., TRUE) │
├─────────────────────────────────────┤
│ SignalObjectAndWait(..., TRUE)      │
└─────────────────────────────────────┘

When thread enters alertable wait:
1. Kernel checks thread's APC queue
2. If APCs pending, dequeues oldest
3. Executes APC routine in thread context
4. Returns to wait or continues execution
```

---

## APC Queue Architecture

### Thread APC Queue Structure
```
Thread Object
├── User-Mode APC Queue
│   ├── APC 1 (Queued by QueueUserAPC)
│   ├── APC 2 (Queued by NtQueueApcThread)
│   └── APC 3 (Queued by NtQueueApcThreadEx)
│
└── Kernel-Mode APC Queue
    ├── Normal Kernel APC
    └── Special Kernel APC
    
Execution Order:
1. Special Kernel APCs (highest priority)
2. Normal Kernel APCs
3. User-Mode APCs (when alertable)
```

### APC Types
| Type | Privilege Level | Requires Alertable | Use Case |
|------|----------------|-------------------|----------|
| User-Mode APC | Ring 3 | Yes | Normal user-mode operations |
| Normal Kernel APC | Ring 0 | No | Kernel operations, I/O completion |
| Special Kernel APC | Ring 0 | No | Critical system operations |

---

## Access Rights Required

### Thread Access Rights
```c
// For APC queueing
THREAD_SET_CONTEXT          0x0010  // Queue APCs
THREAD_QUERY_INFORMATION    0x0040  // Query thread state
THREAD_SUSPEND_RESUME       0x0002  // Optional: control execution
SYNCHRONIZE                 0x00100000 // Wait on thread

// Combined for APC operations
DWORD dwAccess = THREAD_SET_CONTEXT | THREAD_QUERY_INFORMATION;

// Opening thread for APC injection
HANDLE hThread = OpenThread(dwAccess, FALSE, dwThreadId);
```

---

## Cross-Process APC Injection

### Injection Steps
```c
// 1. Get target thread ID (via toolhelp32, enumeration, etc.)
DWORD dwTargetTID = /* ... */;

// 2. Open thread handle
HANDLE hThread = OpenThread(
    THREAD_SET_CONTEXT | THREAD_SUSPEND_RESUME,
    FALSE,
    dwTargetTID
);

// 3. Allocate memory in target process for APC routine
LPVOID pRemoteBuffer = VirtualAllocEx(
    hTargetProcess,
    NULL,
    dwSize,
    MEM_COMMIT | MEM_RESERVE,
    PAGE_EXECUTE_READWRITE
);

// 4. Write APC routine code to remote memory
WriteProcessMemory(
    hTargetProcess,
    pRemoteBuffer,
    pApcCode,
    dwSize,
    NULL
);

// 5. Queue APC pointing to injected code
QueueUserAPC((PAPCFUNC)pRemoteBuffer, hThread, 0);

// 6. Thread executes APC when alertable
```

---


### Calling Convention
- **x86**: __stdcall for APC routines
- **x64**: Microsoft x64 calling convention (fastcall variant)
- Parameter passed in RCX (x64) or on stack (x86)

---

## Security Considerations

### Protection Mechanisms
- **Protected Processes**: Cannot queue APCs to protected processes (PPL/PPL-Lite)
- **Integrity Levels**: Must have equal or higher integrity than target
- **Session Isolation**: Session 0 isolation prevents cross-session APC
- **Kernel Patch Protection (PatchGuard)**: Monitors kernel APC usage
- **Anti-Malware**: Security software detects suspicious APC patterns
- **DEP/ASLR**: Code execution protections affect injected code

### Detection Vectors
- API hooking (QueueUserAPC, native variants)
- Thread context monitoring
- Alertable state tracking
- APC queue inspection (kernel callbacks)
- Behavioral analysis (repeated APC usage)
- Memory scanning for APC shellcode

### Legitimate Use Guidelines
- Only inject APCs into your own processes or with authorization
- Use for debugging, development, or authorized testing
- Implement proper error handling and cleanup
- Document APC usage purpose clearly
- Respect process security boundaries
- Follow organizational security policies
- Test in isolated/sandboxed environments.

---

## Error Handling

### Common Errors and Solutions
| Error | Cause | Solution |
|-------|-------|----------|
| Access Denied | Insufficient privileges | Run as administrator or adjust process privileges |
| Invalid Handle | Thread terminated or handle closed | Verify thread existence before queueing |
| APC Not Executing | Thread never alertable | Ensure target thread enters alertable wait |
| STATUS_INVALID_PARAMETER | Wrong parameters | Check native API parameter alignment |
| Process Protected | Target is protected process | Cannot inject - protected by OS |

---

## Compilation Instructions

### Microsoft Visual C++
```bash
cl.exe APC_Injection_Demonstration.c /link kernel32.lib ntdll.lib
```

### GCC/MinGW
```bash
gcc APC_Injection_Demonstration.c -o APC_Injection_Demo.exe -lkernel32 -lntdll
```

---

## Required Libraries
- `kernel32.lib` - Core Windows APIs (QueueUserAPC, OpenThread)
- `ntdll.lib` - Native API (NtQueueApcThread, NtQueueApcThreadEx, etc.)

---

## API References
- MSDN: [QueueUserAPC](https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-queueuserapc)
- MSDN: [OpenThread](https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-openthread)
- Undocumented NT: [NtQueueApcThread](http://undocumented.ntinternals.net/)

---

**Educational Purpose Only**: This demonstration is provided for understanding Windows internals, developing security tools, and authorized research. Misuse of APC injection for malicious purposes is illegal and unethical.

---

<p align="right">
  Last Updated: November 2025
</p>
