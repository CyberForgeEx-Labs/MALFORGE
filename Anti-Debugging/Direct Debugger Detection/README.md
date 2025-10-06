# Direct Debugger Detection Techniques

## Overview

Direct debugger detection techniques explicitly check if a debugger is attached to the process. These are the most common anti-debugging methods used in malware, game protection, and software security.

## Common APIs

| API | Sophistication | 
|-----|---------------|
| `IsDebuggerPresent` | Basic |
| `CheckRemoteDebuggerPresent` | Basic |
| `NtQueryInformationProcess` | Advanced |
| `OutputDebugStringA` | Basic |

---

## 1. IsDebuggerPresent

### Description
The simplest and fastest debugger detection method. Checks the `BeingDebugged` flag in the Process Environment Block (PEB).

### Why It's Popular
- Single API call with no parameters
- Fastest execution time
- First line of defense in most anti-debug routines


---

## 2. NtQueryInformationProcess

### Description
A more sophisticated approach that can check multiple debug indicators through different information classes.

### Why It's Advanced
- Can check `ProcessDebugPort` (kernel debugger port)
- Can check `ProcessDebugFlags` (NoDebugInherit flag)
- Can check `ProcessDebugObjectHandle` (debug object)
- Harder to bypass than `IsDebuggerPresent`

### Information Classes

| Class | Value | Description |
|-------|-------|-------------|
| `ProcessDebugPort` | 7 | Returns -1 if debugger present |
| `ProcessDebugFlags` | 31 | Returns 0 if debugger present (NoDebugInherit) |
| `ProcessDebugObjectHandle` | 30 | Returns handle to debug object |

---

## Comparison

| Feature | IsDebuggerPresent | NtQueryInformationProcess |
|---------|-------------------|---------------------------|
| **Complexity** | Very Low | Medium-High |
| **Speed** | Fastest | Slower (requires ntdll call) |
| **Bypass Difficulty** | Easy | Medium |
| **Detection Rate** | Good | Better |
| **Usage** | First check | Secondary/Advanced check |

---

## Best Practices for Implementation

1. **Layer Multiple Checks**: Don't rely on a single method
2. **Obfuscate Calls**: Use dynamic API resolution
3. **Time-Based Checks**: Combine with timing attacks
4. **Continuous Monitoring**: Check periodically, not just at startup
5. **Encrypted Responses**: Don't obviously terminate on detection

---

## Compilation

### Windows (MinGW)
```bash
gcc -o debugger_check.exe debugger_check.c -O2 
```

---

## Educational Purpose

This documentation is provided for **educational and research purposes only**. Understanding anti-debugging techniques is essential for:
- Malware analysis and reverse engineering
- Software security research
- Developing legitimate software protection
- Cybersecurity education

---

## References

- Microsoft Documentation: [IsDebuggerPresent](https://docs.microsoft.com/en-us/windows/win32/api/debugapi/nf-debugapi-isdebuggerpresent)
- MSDN: [NtQueryInformationProcess](https://docs.microsoft.com/en-us/windows/win32/api/winternl/nf-winternl-ntqueryinformationprocess)
- MALAPI: [NtQueryInformationProcess](https://malapi.io/winapi/NtQueryInformationProcess)
- MALAPI: [IsDebuggerPresent](https://malapi.io/winapi/IsDebuggerPresent)

---

**Last Updated:** October 2025