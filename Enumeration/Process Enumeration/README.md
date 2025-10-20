<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MalAPI-Arsenal/blob/main/Enumeration/Process%20Enumeration/ProcessEnumeration.c">Process Enumeration Techniques</a>
</h1>

## Overview
Process enumeration and analysis is a fundamental Windows API capability for discovering, monitoring, and managing running processes. This documentation covers advanced APIs for enumerating system processes, obtaining detailed process information, and analyzing process characteristics.

---

## Core APIs

| API | Purpose | Sophistication |
|-----|---------|----------------|
| `EnumProcesses` | Get all process IDs | Basic |
| `OpenProcess` | Open handle to process | Basic |
| `GetModuleFileNameEx` | Get process executable path | Basic |
| `GetProcessMemoryInfo` | Get memory statistics | Intermediate |
| `GetProcessTimes` | Get CPU time information | Intermediate |
| `GetPriorityClass` | Get process priority | Basic |
| `EnumProcessModules` | Enumerate loaded modules | Intermediate |
| `GetExitCodeProcess` | Check process status | Basic |

---

## 1. EnumProcesses + GetModuleFileNameEx
### Description
Retrieves all process IDs and combines them with process names to provide a complete process listing without detailed snapshots.

### Key Features
- Fast enumeration of all running processes
- Retrieves process executable paths
- Minimal overhead
- No parent-child relationship info
- Works with individual process handles

### Use Cases
- Quick process discovery
- Process name resolution
- Lightweight process monitoring
- Finding specific applications

### Example Output
```
PID      | Process Name
----------------------------------------
4        | System
316      | smss.exe
508      | csrss.exe
5428     | explorer.exe
```

---

## 2. GetProcessMemoryInfo
### Description
Retrieves memory usage statistics for a specific process including working set, peak memory, and page file usage.

### Key Features
- Detailed memory statistics
- Working set information
- Page file usage tracking
- Peak memory measurements
- Performance analysis capability

### Use Cases
- Memory usage monitoring
- Performance analysis
- Resource tracking
- Memory leak detection
- System optimization

### Example Output
```
Process ID: 5428
Working Set Size: 245 KB
Peak Working Set: 512 KB
Page File Usage: 128 KB
Peak Page File: 256 KB
```

---

## 3. GetProcessTimes
### Description
Obtains CPU time usage and timing information for a process including kernel time, user time, and creation time.

### Key Features
- Kernel mode CPU time
- User mode CPU time
- Creation timestamp
- Exit timestamp
- Precision timing data

### Use Cases
- CPU time analysis
- Process performance metrics
- Uptime calculation
- Resource consumption tracking
- Performance profiling

### Example Output
```
Process ID: 5428
Kernel Time: 15000 ms
User Time: 45000 ms
Process Creation Time: 8f2c1a40:c3f5e2b0
```

---

## 4. GetPriorityClass
### Description
Retrieves the priority class of a process determining CPU scheduling priority.

### Priority Levels
| Level | Value | Usage |
|-------|-------|-------|
| Real-time | REALTIME_PRIORITY_CLASS | Critical system tasks |
| High | HIGH_PRIORITY_CLASS | Important applications |
| Above Normal | ABOVE_NORMAL_PRIORITY_CLASS | User applications |
| Normal | NORMAL_PRIORITY_CLASS | Standard priority |
| Below Normal | BELOW_NORMAL_PRIORITY_CLASS | Background tasks |
| Idle | IDLE_PRIORITY_CLASS | Idle tasks |

### Use Cases
- Process priority analysis
- System resource management
- Monitoring critical processes
- Performance optimization
- Priority classification

### Example Output
```
Process ID: 5428
Priority Class: High (0x00000080)
```

---

## 5. EnumProcessModules + GetModuleBaseName
### Description
Enumerates all DLL and EXE modules loaded by a process, revealing dependencies and library usage.

### Key Features
- Lists all loaded modules
- DLL dependency tracking
- Module base name retrieval
- Module address information
- Library analysis capability

### Use Cases
- Dependency analysis
- Malware detection
- System monitoring
- Application profiling
- Module tracking

### Example Output
```
Process ID: 5428 loaded 125 modules:
Module Name
----------------------------------------
explorer.exe
kernel32.dll
ntdll.dll
user32.dll
gdi32.dll
... and 120 more modules
```

---

## 6. GetExitCodeProcess
### Description
Determines whether a process is still running and retrieves its exit code if terminated.

### Key Features
- Process status verification
- Exit code retrieval
- Real-time status checking
- Minimal access requirements
- Quick validity checking

### Use Cases
- Process status monitoring
- Process termination detection
- Exit code analysis
- Background task verification
- Process cleanup handling

### Example Output
```
Process 5428: Running
Process 1234: Terminated
Exit Code: 0
```

---

## Comparison

| Feature | EnumProcesses | Memory Info | Times Info | Priority |
|---------|---------------|------------|-----------|----------|
| **Speed** | Very Fast | Slow | Fast | Fast |
| **Memory Overhead** | Low | Moderate | Low | Low |
| **Detail Level** | Names/IDs | Memory Stats | CPU Time | Priority |
| **Resource Intensive** | No | Yes | No | No |
| **Access Required** | Limited | Query Info | Query Info | Query Info |

---

## Best Practices for Implementation

1. **Error Handling**: Always check return values and handle access denied errors
2. **Access Rights**: Use minimal required permissions:
   - `PROCESS_QUERY_LIMITED_INFORMATION` for basic info
   - `PROCESS_QUERY_INFORMATION | PROCESS_VM_READ` for detailed info
3. **Handle Cleanup**: Always close process handles with `CloseHandle()`
4. **Buffer Sizing**: Allocate sufficient buffers for module enumeration
5. **Combined Analysis**: Use multiple APIs for comprehensive profiling
6. **Privilege Considerations**: Some APIs require elevated privileges
7. **Performance**: Cache results when monitoring multiple processes

---

## Required Libraries
- `psapi.lib` (Process Status API) - For process information queries
- `kernel32.lib` (Windows API) - For core process operations

---

## Educational Purpose
This documentation is provided for **educational and research purposes only**. Understanding process enumeration techniques is essential for:
- System administration and monitoring
- Malware analysis and reverse engineering
- Software security research
- Performance monitoring and optimization
- Cybersecurity education
- Legitimate software development

---

## Common Applications

### System Monitoring Tools
- Task Manager
- Process Monitor
- Performance Monitor
- System resource monitors

### Security Tools
- Antivirus software
- Behavioral analysis tools
- Intrusion detection systems
- Endpoint protection

### Development Tools
- Debuggers
- Profilers
- Performance analyzers
- Application monitors

---

## API References
- MSDN: [EnumProcesses](https://docs.microsoft.com/en-us/windows/win32/api/psapi/nf-psapi-enumprocesses)
- MSDN: [GetModuleFileNameEx](https://docs.microsoft.com/en-us/windows/win32/api/psapi/nf-psapi-getmodulefilenameex)
- MSDN: [GetProcessMemoryInfo](https://docs.microsoft.com/en-us/windows/win32/api/psapi/nf-psapi-getprocessmemoryinfo)
- MSDN: [GetProcessTimes](https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getprocesstimes)
- MSDN: [GetPriorityClass](https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getpriorityclass)
- MSDN: [EnumProcessModules](https://docs.microsoft.com/en-us/windows/win32/api/psapi/nf-psapi-enumprocessmodules)
- MSDN: [GetExitCodeProcess](https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getexitcodeprocess)

---

<p align="right">
  Last Updated: October 2025
</p>
