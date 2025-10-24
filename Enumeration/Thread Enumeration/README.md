<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MalAPI-Arsenal/blob/main/Enumeration/Thread%20Enumeration/ThreadEnumeration.c">Thread Enumeration Techniques</a>
</h1> 

## Overview
Thread enumeration and analysis is a critical Windows API capability for discovering, monitoring, and managing system threads. This documentation covers APIs for enumerating threads, obtaining detailed thread information, and analyzing thread characteristics across processes.

---

## Core APIs

| API | Purpose | Sophistication |
|-----|---------|----------------|
| `Thread32First` | Retrieve first thread from snapshot | Basic |
| `Thread32Next` | Retrieve next thread from snapshot | Basic |
| `GetProcessIdOfThread` | Get process ID owning a thread | Basic |
| `GetCurrentThread` | Get pseudo-handle to current thread | Basic |
| `GetCurrentThreadId` | Get current thread identifier | Basic |
| `GetThreadId` | Get thread identifier from handle | Basic |
| `GetThreadInformation` | Retrieve thread information | Intermediate |

---

## 1. [Thread32First + Thread32Next](https://github.com/CyberForgeEx/MALFORGE_API-ARSENAL/blob/main/Enumeration/Thread%20Enumeration/ThreadEnumeration.c#L9)
### Description
Retrieves thread information from a system snapshot, enabling enumeration of all threads or threads belonging to a specific process.

### Key Features
- Enumerates all system threads
- Process-specific thread filtering
- Base priority information
- Creation flags access
- Minimal overhead iteration

### Use Cases
- Complete thread discovery
- Process thread analysis
- System thread monitoring
- Thread priority examination
- Lightweight thread scanning

### Example Output
```
TID      | PID      | Base Priority | State
--------------------------------------------------
1024     | 5428     | 8             | Active
1840     | 5428     | 8             | Active
2156     | 5428     | 9             | Active
3420     | 4        | 13            | Active
```

---

## 2. [GetProcessIdOfThread](https://github.com/CyberForgeEx/MALFORGE_API-ARSENAL/blob/main/Enumeration/Thread%20Enumeration/ThreadEnumeration.c#L48)
### Description
Retrieves the process ID that owns a specific thread, establishing the relationship between threads and their parent process.

### Key Features
- Reverse thread-to-process mapping
- Minimal access requirements
- Fast lookup operation
- Works with thread handles
- Reliable process association

### Use Cases
- Thread ownership verification
- Process-thread relationship tracking
- Thread monitoring and filtering
- Privilege verification
- System thread classification

### Example Output
```
Thread ID: 1024 -> Process ID: 5428
Thread ID: 1840 -> Process ID: 5428
Thread ID: 3420 -> Process ID: 4
```

---

## 3. [GetCurrentThread](https://github.com/CyberForgeEx/MALFORGE_API-ARSENAL/blob/main/Enumeration/Thread%20Enumeration/ThreadEnumeration.c#L95)
### Description
Retrieves a pseudo-handle to the calling thread, useful for thread-specific operations without knowing the actual thread ID.

### Key Features
- Pseudo-handle reference
- Works in current thread context
- Valid only within calling thread
- Constant value: 0xFFFFFFFE

### Use Cases
- Current thread manipulation
- Thread-local operations
- Internal thread management
- Thread context operations

### Example Output
```
Current Thread Handle: 0xfffffffe
Current Thread ID: 1024
Current Process ID: 5428
```

---

## 4. [GetCurrentThreadId](https://github.com/CyberForgeEx/MALFORGE_API-ARSENAL/blob/main/Enumeration/Thread%20Enumeration/ThreadEnumeration.c#L108)
### Description
Retrieves the unique identifier of the calling thread within the system.

### Key Features
- Direct thread ID retrieval
- No handle required
- Globally unique within system

### Use Cases
- Thread identification
- Logging and diagnostics
- Thread context detection
- Debug information collection

### Example Output
```
Current Thread ID: 1024
```

---

## 5. [GetThreadId](https://github.com/CyberForgeEx/MALFORGE_API-ARSENAL/blob/main/Enumeration/Thread%20Enumeration/ThreadEnumeration.c#L136)
### Description
Obtains the unique thread identifier from an open thread handle, converting handle references to standard thread IDs.

### Key Features
- Handle-to-ID conversion
- Works with thread handles
- Fast lookup
- Reliable identification
- Simple one-to-one mapping

### Use Cases
- Thread handle resolution
- ID verification
- Thread tracking
- Handle validation
- Thread management

### Example Output
```
Thread Handle 0x12ab4cd0 -> Thread ID: 1024
Thread Handle 0x56ef7890 -> Thread ID: 1840
Thread Handle 0x9abc0123 -> Thread ID: 2156
```

---

## 6. [GetThreadInformation](https://github.com/CyberForgeEx/MALFORGE_API-ARSENAL/blob/main/Enumeration/Thread%20Enumeration/ThreadEnumeration.c#L153)
### Description
Retrieves various types of information about a thread including ideal processor, and other scheduling attributes.

### Key Features
- Scheduling attributes

### Use Cases
- Performance analysis
- CPU time measurement
- Thread scheduling analysis
- Performance profiling

### Example Output
```
Thread ID: 1024
  Priority: 8
  
Thread ID: 1840
  Priority: 8
```

---

## Educational Purpose
This documentation is provided for **educational and research purposes only**. Understanding thread enumeration techniques is essential for:
- System administration and monitoring
- Malware analysis and reverse engineering
- Software security research
- Performance analysis and optimization
- Cybersecurity education
- Debugging and profiling

---

## API References
- MSDN: [Thread32First](https://docs.microsoft.com/en-us/windows/win32/api/tlhelp32/nf-tlhelp32-thread32first)
- MSDN: [Thread32Next](https://docs.microsoft.com/en-us/windows/win32/api/tlhelp32/nf-tlhelp32-thread32next)
- MSDN: [GetProcessIdOfThread](https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getprocessidofthread)
- MSDN: [GetCurrentThread](https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getcurrentthread)
- MSDN: [GetCurrentThreadId](https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getcurrentthreadid)
- MSDN: [GetThreadId](https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getthreadid)
- MSDN: [GetThreadInformation](https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getthreadinformation)

---

<p align="right">
  Last Updated: October 2025
</p>
