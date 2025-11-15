<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Injection/Synchronization%20%26%20Waiting/Synchronization_waiting_operations.c">Synchronization & Waiting Techniques</a>
</h1>

## Overview
This educational demonstration for Windows synchronization and wait APIs for legitimate software development, system programming, and thread coordination. These implementations focus on **safe understanding and proper usage** of wait operations for multi-threaded applications, process coordination, IPC timing, and resource management. These APIs are fundamental to Windows programming, enabling developers to build responsive applications, coordinate concurrent operations, and manage shared resources effectively.

---

## Core APIs

| API | Purpose | Sophistication |
|-----|---------|----------------|
| `WaitForSingleObject` | Waits for single kernel object to signal | Basic |
| `WaitForSingleObjectEx` | Extended wait with alertable capability | Intermediate |
| `WaitForMultipleObjects` | Waits for multiple kernel objects | Intermediate |
| `WaitForMultipleObjectsEx` | Extended multiple wait with alertable | Advanced |
| `NtWaitForSingleObject` | Native NT API single object wait | Advanced |
| `NtWaitForMultipleObjects` | Native NT API multiple object wait | Advanced |

---

## 1. [WaitForSingleObject](https://github.com/CyberForgeEx/MALFORGE/blob/main/Injection/Synchronization%20%26%20Waiting/Synchronization_waiting_operations.c#L61)
### Description
The most commonly used synchronization function in Windows programming. Waits until the specified kernel object is in the signaled state or the timeout interval elapses. This function blocks the calling thread until one of these conditions occurs. It supports various kernel objects including threads, processes, events, mutexes, semaphores, and more. WaitForSingleObject is the foundation for thread synchronization, process coordination, and event-driven programming patterns in Windows applications.

### Key Features
- Simple, straightforward API
- Supports all waitable kernel objects
- Configurable timeout (including INFINITE)
- Thread-blocking behavior
- Return codes indicate wait result
- No APC delivery during wait

### Use Cases
- Thread synchronization
- Waiting for process completion
- Event-driven programming
- Resource locking with mutexes
- Timer-based operations
- IPC coordination


### Return Values
| Value | Description |
|-------|-------------|
| `WAIT_OBJECT_0` (0x00000000) | Object was signaled |
| `WAIT_TIMEOUT` (0x00000102) | Timeout elapsed |
| `WAIT_ABANDONED` (0x00000080) | Mutex was abandoned |
| `WAIT_FAILED` (0xFFFFFFFF) | Function failed (check GetLastError) |

---

## 2. [WaitForSingleObjectEx](https://github.com/CyberForgeEx/MALFORGE/blob/main/Injection/Synchronization%20%26%20Waiting/Synchronization_waiting_operations.c#L108)
### Description
Extended version of WaitForSingleObject with alertable wait capability. When the `bAlertable` parameter is TRUE, the function enters an alertable wait state, allowing Asynchronous Procedure Calls (APCs) to be delivered to the waiting thread. This is crucial for I/O completion routines, user-mode APCs, and scenarios where threads must remain responsive to asynchronous events while waiting. The alertable state enables sophisticated patterns like overlapped I/O and user-scheduled work items.

### Key Features
- Alertable wait state support
- APC delivery capability
- I/O completion routine execution
- User-mode APC handling
- All WaitForSingleObject features
- Interruptible by APCs

### Use Cases
- Overlapped I/O operations
- File read/write with completion routines
- Timer APCs (SetWaitableTimer)
- User-mode APC scheduling (QueueUserAPC)
- Responsive waiting patterns
- Asynchronous event handling.

### Return Values
| Value | Description |
|-------|-------------|
| `WAIT_OBJECT_0` | Object was signaled |
| `WAIT_TIMEOUT` | Timeout elapsed |
| `WAIT_ABANDONED` | Mutex was abandoned |
| `WAIT_IO_COMPLETION` (0x000000C0) | APC was executed (alertable only) |
| `WAIT_FAILED` | Function failed |

### APC Delivery Mechanism
```
Thread State Flow:
┌─────────────┐
│   Running   │
└──────┬──────┘
       │ WaitForSingleObjectEx(hEvent, INFINITE, TRUE)
       ▼
┌─────────────────┐
│ Alertable Wait  │ ◄─── APCs can interrupt here
└──────┬──────────┘
       │
       ├──► APC Queued? ──Yes──► Execute APC ──► Return WAIT_IO_COMPLETION
       │
       └──► Object Signaled? ──Yes──► Return WAIT_OBJECT_0
```

---

## 3. [WaitForMultipleObjects](https://github.com/CyberForgeEx/MALFORGE/blob/main/Injection/Synchronization%20%26%20Waiting/Synchronization_waiting_operations.c#L151)
### Description
Waits for multiple kernel objects simultaneously, returning when either all objects are signaled (wait all mode) or when any single object is signaled (wait any mode). This function is essential for coordinating multiple concurrent operations, implementing complex synchronization patterns, and managing multiple resources. It can wait for up to 64 objects in a single call, making it powerful for scenarios requiring coordination across multiple threads, processes, or events.

### Key Features
- Wait for up to 64 objects simultaneously
- Two wait modes: wait-all and wait-any
- Single function call efficiency
- Index-based result reporting
- Atomic multi-object coordination
- Supports mixed object types

### Use Cases
- Multi-threaded application coordination
- Parallel task completion detection
- Resource pool management
- Multiple process synchronization
- Event multiplexing
- Barrier implementation


### Return Values
| Value | Description |
|-------|-------------|
| `WAIT_OBJECT_0` to `WAIT_OBJECT_0 + nCount - 1` | Index of signaled object (wait-any mode) |
| `WAIT_OBJECT_0` | All objects signaled (wait-all mode) |
| `WAIT_TIMEOUT` | Timeout elapsed |
| `WAIT_ABANDONED_0` to `WAIT_ABANDONED_0 + nCount - 1` | Abandoned mutex index |
| `WAIT_FAILED` | Function failed |

### Wait Modes Comparison
```
Wait-Any Mode (bWaitAll = FALSE):
─────────────────────────────────────────
Event A: ─────●──────────────  (signaled at T1)
Event B: ──────────●─────────  (signaled at T2)
Event C: ───────────────●────  (signaled at T3)
                ▲
         Returns here (T1) with index 0

Wait-All Mode (bWaitAll = TRUE):
─────────────────────────────────────────
Event A: ─────●──────────────  (signaled at T1)
Event B: ──────────●─────────  (signaled at T2)
Event C: ───────────────●────  (signaled at T3)
                        ▲
                 Returns here (T3) when ALL signaled
```

---

## 4. [WaitForMultipleObjectsEx](https://github.com/CyberForgeEx/MALFORGE/blob/main/Injection/Synchronization%20%26%20Waiting/Synchronization_waiting_operations.c#L212)
### Description
Combines the capabilities of WaitForMultipleObjects with alertable wait state support from WaitForSingleObjectEx. This powerful function enables waiting on multiple objects while remaining responsive to APCs, making it ideal for complex asynchronous I/O scenarios, concurrent operation coordination, and responsive multi-threaded applications. It supports all features of both parent functions.

### Key Features
- Multiple object waiting (up to 64)
- Alertable wait capability
- APC delivery support
- Wait-all and wait-any modes
- I/O completion integration
- Maximum flexibility

### Use Cases
- Multiple overlapped I/O operations
- Concurrent async file operations
- Complex thread coordination with APCs
- Multi-source event handling
- Parallel I/O with completion routines
- Advanced synchronization patterns


### Return Values
Same as WaitForMultipleObjects, plus:
| Value | Description |
|-------|-------------|
| `WAIT_IO_COMPLETION` | APC was executed (alertable only) |

---

## 5. [NtWaitForSingleObject](https://github.com/CyberForgeEx/MALFORGE/blob/main/Injection/Synchronization%20%26%20Waiting/Synchronization_waiting_operations.c#L259)
### Description
Native NT API function providing low-level wait operations directly at the kernel interface level. Unlike the Win32 WaitForSingleObject, this function uses NTSTATUS return codes and accepts timeout in 100-nanosecond intervals. It provides more precise control and is used internally by Win32 wait functions. Understanding this API is valuable for system-level programming, kernel research, and implementing custom synchronization primitives.

### Key Features
- Native NT API (ntdll.dll)
- Precise timeout control (100ns units)
- NTSTATUS return codes
- Alertable wait support
- Absolute and relative timeouts
- Kernel-level interface

### Use Cases
- System-level programming
- Kernel driver communication
- Custom synchronization primitives
- Low-level debugging tools
- Native subsystem development
- Security research


### Return Values (NTSTATUS)
| Value | Description |
|-------|-------------|
| `STATUS_SUCCESS` (0x00000000) | Object was signaled |
| `STATUS_TIMEOUT` (0x00000102) | Timeout elapsed |
| `STATUS_USER_APC` (0x000000C0) | APC delivered (alertable) |
| `STATUS_ALERTED` (0x00000101) | Thread was alerted |
| `STATUS_ABANDONED` | Mutex was abandoned |

---

## 6. [NtWaitForMultipleObjects](https://github.com/CyberForgeEx/MALFORGE/blob/main/Injection/Synchronization%20%26%20Waiting/Synchronization_waiting_operations.c#L308)
### Description
Native NT API for waiting on multiple kernel objects, providing the lowest-level multi-object wait interface in Windows. This function offers precise control over wait behavior, timeout handling, and object type specification. It uses the WAIT_TYPE enumeration to specify wait-all or wait-any semantics and supports all features of the Win32 counterpart while providing native-level control.

### Key Features
- Native multi-object wait
- WAIT_TYPE enumeration
- Precise timeout control
- Alertable wait support
- Up to MAXIMUM_WAIT_OBJECTS (64)
- Native NTSTATUS codes

### Use Cases
- Native subsystem development
- System-level multi-threading
- Kernel mode synchronization research
- Custom wait implementations
- Low-level coordination primitives
- Security and debugging tools


### Return Values
| Value | Description |
|-------|-------------|
| `STATUS_SUCCESS` | Object(s) signaled |
| `STATUS_TIMEOUT` | Timeout elapsed |
| `STATUS_USER_APC` | APC delivered |
| `STATUS_WAIT_0` to `STATUS_WAIT_63` | Index of signaled object |

---

## Wait Operation Flow Diagrams

### Single Object Wait Flow
```
Thread Execution
│
├──► WaitForSingleObject(hEvent, 5000)
│    │
│    ├─► Check Object State
│    │   │
│    │   ├── Signaled? ──Yes──► Return WAIT_OBJECT_0
│    │   │
│    │   └── Not Signaled
│    │       │
│    │       ├─► Block Thread (5000ms timeout)
│    │       │   │
│    │       │   ├── Object Signaled ──► Return WAIT_OBJECT_0
│    │       │   │
│    │       │   └── Timeout Reached ──► Return WAIT_TIMEOUT
│    │
│    └─► Resume Execution
│
▼
Continue Thread
```

### Multiple Objects Wait Flow (Wait-Any)
```
WaitForMultipleObjects([Event1, Event2, Event3], FALSE, timeout)
│
├─► Check All Objects
│   │
│   ├── Event1: Not Signaled
│   ├── Event2: Signaled ◄──────┐
│   └── Event3: Not Signaled    │
│                               │
└─► Return WAIT_OBJECT_0 + 1 ───┘
    (Index of Event2)
```

### Multiple Objects Wait Flow (Wait-All)
```
WaitForMultipleObjects([Event1, Event2, Event3], TRUE, timeout)
│
├─► Monitor All Objects
│   │
│   ├─► Event1: ─────●─────  (Signaled at T1)
│   ├─► Event2: ────────●──  (Signaled at T2)
│   └─► Event3: ──────────●  (Signaled at T3) ◄── All signaled
│                             │
└─────────────────────────────┘
      Return WAIT_OBJECT_0
```

### Alertable Wait with APC
```
Thread State Flow
│
├──► WaitForSingleObjectEx(hEvent, INFINITE, TRUE)
│    │
│    ├─► Enter Alertable Wait State
│    │   │
│    │   ├── Object Signaled? ──Yes──► Return WAIT_OBJECT_0
│    │   │
│    │   ├── APC Queued?
│    │   │   │
│    │   │   └──Yes──► Exit Wait
│    │   │             │
│    │   │             ├─► Execute APC
│    │   │             │
│    │   │             └─► Return WAIT_IO_COMPLETION
│    │   │
│    │   └── Continue Waiting...
│    │
│    └─► Resume Execution
│
▼
```

---

## Timeout Calculations

### Win32 API Timeouts (Milliseconds)
| Constant | Value | Description |
|----------|-------|-------------|
| `INFINITE` | 0xFFFFFFFF | Never timeout |
| `0` | 0 | Return immediately (test only) |
| `5000` | 5000 | 5 seconds |
| `60000` | 60000 | 1 minute |


### Timeout Behavior Comparison
```
Timeout Value          Behavior
─────────────────────────────────────────────────────────
INFINITE / NULL        Never timeout, wait indefinitely
0                      Test object state, return immediately
Positive (Win32)       Wait up to N milliseconds
Negative (Native)      Wait relative N * 100ns from now
Positive (Native)      Wait until absolute system time
```

---

## Object Types and Wait Behavior

### Waitable Object Characteristics
| Object Type | Signaled State | Auto-Reset | Notes |
|-------------|----------------|------------|-------|
| Thread | Thread terminates | No | Remains signaled after termination |
| Process | Process terminates | No | Remains signaled after exit |
| Event (Manual) | SetEvent called | No | Requires ResetEvent |
| Event (Auto) | SetEvent called | Yes | Auto-resets after single wait |
| Mutex | Not owned by any thread | Yes | Acquiring mutex resets it |
| Semaphore | Count > 0 | Yes | Count decremented on wait |
| Waitable Timer | Timer expires | Manual/Auto | Depends on timer type |
| File Handle | I/O complete | Varies | Depends on operation |
| Job | All processes terminated | No | Signaled when empty |

### Object State Transitions
```
Mutex State:
Signaled (Available) ◄─── ReleaseMutex() ───┐
       │                                     │
       │ WaitForSingleObject()              │
       ▼                                     │
Non-Signaled (Owned) ─────────────────────┘

Event (Auto-Reset):
Non-Signaled ◄──── Wait() ◄──┐
       │                      │
       │ SetEvent()          │
       ▼                      │
Signaled ─────────────────────┘
(Single wait satisfied, auto-resets)

Event (Manual-Reset):
Non-Signaled ──────┐
       ▲            │ SetEvent()
       │            ▼
  ResetEvent()  Signaled
       │            │
       └────────────┘
(Multiple waits satisfied until ResetEvent)
```

---

## Error Handling and Best Practices

### Common Errors and Solutions
| Error Scenario | Cause | Solution |
|----------------|-------|----------|
| WAIT_FAILED | Invalid handle | Validate handle before waiting |
| WAIT_ABANDONED | Mutex owner died | Check for abandoned state, reset mutex |
| Deadlock | Circular wait dependency | Use timeout, implement proper lock ordering |
| Timeout issues | Too short timeout | Calculate appropriate timeout values |
| APC not delivered | Not alertable | Use *Ex functions with bAlertable=TRUE |
| Handle leak | Not closing handles | Always close handles in cleanup code |

---

## Performance Considerations

### Wait Operation Costs
| Operation | Typical Cost | Notes |
|-----------|-------------|-------|
| Object already signaled | ~100-200 ns | Fast path, no context switch |
| Wait with context switch | ~3-10 μs | Kernel transition + scheduler |
| Multiple objects (signaled) | ~200-500 ns | Per object check |
| APC delivery overhead | ~5-15 μs | Additional kernel processing |
| Wait timeout (elapsed) | ~1-3 ms | Timer resolution dependent |

---

## Required Libraries
- `kernel32.lib` - Win32 synchronization functions
- `ntdll.lib` - Native API functions

---

## Compilation Instructions

### Microsoft Visual C++ with Debug Symbols
```console
cl.exe /Zi /DEBUG Synchronization_Wait_Operations.c ntdll.lib
```

### MinGW GCC with Optimizations
```console
gcc -O2 Synchronization_Wait_Operations.c -o sync_wait.exe -lntdll
```

### Clang with Address Sanitizer
```console
clang -fsanitize=address Synchronization_Wait_Operations.c -o sync_wait.exe -lntdll
```

---

## API References
- MSDN: [WaitForSingleObject](https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-waitforsingleobject)
- MSDN: [WaitForSingleObjectEx](https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-waitforsingleobjectex)
- MSDN: [WaitForMultipleObjects](https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-waitformultipleobjects)
- MSDN: [WaitForMultipleObjectsEx](https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-waitformultipleobjectsex)
- MSDN: [Synchronization Objects](https://learn.microsoft.com/en-us/windows/win32/sync/synchronization-objects)
- MSDN: [Asynchronous Procedure Calls](https://learn.microsoft.com/en-us/windows/win32/sync/asynchronous-procedure-calls)

---

## Additional Resources

### Learning Materials
- [Windows Internals (Book)](https://docs.microsoft.com/en-us/sysinternals/resources/windows-internals) - Thread scheduling and synchronization
- [Windows System Programming](https://www.microsoftpressstore.com/store/windows-system-programming-9780134382258) - Comprehensive wait operations
- [Concurrent Programming on Windows](https://www.amazon.com/Concurrent-Programming-Windows-Joe-Duffy/dp/032143482X) - Advanced synchronization patterns
- [MSDN: Synchronization](https://learn.microsoft.com/en-us/windows/win32/sync/synchronization) - Official documentation

### Native API Resources
- [Native API Reference](http://undocumented.ntinternals.net/) - Undocumented NT functions
- [Windows NT Native API](https://www.amazon.com/Windows-Native-Reference-Gary-Nebbett/dp/1578701996) - Gary Nebbett's reference
- [ReactOS Source](https://github.com/reactos/reactos) - Open-source NT implementation

### Synchronization Patterns
- [Microsoft Parallel Patterns Library](https://learn.microsoft.com/en-us/cpp/parallel/concrt/parallel-patterns-library-ppl)
- [Synchronization Primitives Best Practices](https://learn.microsoft.com/en-us/windows/win32/sync/synchronization-and-overlapped-input-and-output)

---

## Demonstration Features

The included demonstration code showcases:

1. **Basic Single Object Wait**: Simple event waiting with timeout
2. **Alertable Wait**: Demonstrating APC delivery during wait
3. **Multiple Object Wait (All)**: Waiting for all objects to signal
4. **Multiple Object Wait (Any)**: Waiting for any object to signal
5. **Native API Usage**: Low-level NtWaitForSingleObject
6. **Native Multiple Wait**: NtWaitForMultipleObjects with WAIT_TYPE
7. **Process Synchronization**: Real-world process coordination example
8. **Error Handling**: Comprehensive error checking and reporting
9. **Timeout Management**: Various timeout scenarios
10. **Resource Cleanup**: Proper handle management

---

## Thread Safety Notes

### Wait Function Thread Safety
All wait functions are thread-safe and can be called from multiple threads simultaneously:

- **WaitForSingleObject**: Thread-safe, multiple threads can wait on same object
- **WaitForMultipleObjects**: Thread-safe, each thread gets independent results
- **Native APIs**: Fully thread-safe at kernel level

### Synchronization Object States
Understanding object state changes during waits:

```c
// Mutex: Only one thread succeeds
WaitForSingleObject(hMutex, INFINITE); // Thread A: succeeds
WaitForSingleObject(hMutex, INFINITE); // Thread B: blocks

// Event (Manual): Multiple threads succeed
SetEvent(hManualEvent);
WaitForSingleObject(hManualEvent, INFINITE); // Thread A: succeeds
WaitForSingleObject(hManualEvent, INFINITE); // Thread B: succeeds
ResetEvent(hManualEvent);

// Event (Auto): First thread wins
SetEvent(hAutoEvent);
WaitForSingleObject(hAutoEvent, INFINITE); // Thread A: succeeds, auto-resets
WaitForSingleObject(hAutoEvent, INFINITE); // Thread B: blocks

// Semaphore: Count-based
ReleaseSemaphore(hSemaphore, 3, NULL); // Count = 3
WaitForSingleObject(hSemaphore, INFINITE); // Thread A: count = 2
WaitForSingleObject(hSemaphore, INFINITE); // Thread B: count = 1
WaitForSingleObject(hSemaphore, INFINITE); // Thread C: count = 0
WaitForSingleObject(hSemaphore, INFINITE); // Thread D: blocks
```

---

## Platform Compatibility

### Architecture Support
- x86 (32-bit): Fully supported
- x64 (64-bit): Fully supported
- ARM64: Supported on Windows 10+

---

<p align="right">
  <b>Last Updated: November 2025</b>
</p>
