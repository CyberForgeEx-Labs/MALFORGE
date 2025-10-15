<h1 align="center">
    Process/System Enumeration Anti-Debug Techniques
</h1>

## Overview
Process and system enumeration techniques detect debugging tools and analysis processes by scanning the running environment. Unlike passive detection methods, these techniques actively search for evidence of security analysis tools, debuggers, disassemblers, and monitoring applications through process lists and window enumeration.

## Common APIs
| API | Sophistication | 
|-----|---------------|
| `CreateToolhelp32Snapshot` | Intermediate |
| `FindWindowA` | Basic |

---

## 1. [CreateToolhelp32Snapshot](https://github.com/CyberForgeEx/MalAPI-Arsenal/blob/main/Anti-Debugging/Process-System%20Enumeration/CreateToolhelp32Snapshot.c)

### Description
The most powerful and widely adopted process enumeration technique. Takes a snapshot of all running processes and searches for known debugger, disassembler, and analysis tool executables by name.

### Why It's Popular
- Comprehensive coverage of analysis tools
- Direct process name matching with high accuracy
- Can detect hidden or renamed tools through process characteristics
- Industry standard for process-level anti-analysis

### How It Works
```c
HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
PROCESSENTRY32 pe32;
Process32First(hSnapshot, &pe32);
do {
    // Check process name against blacklist
} while(Process32Next(hSnapshot, &pe32));
```

The API creates a snapshot of all running processes and enables iteration through:
- **Process Names**: ollydbg.exe, x64dbg.exe, ida.exe, etc.
- **Process IDs**: For further analysis or termination
- **Parent Process IDs**: To detect debugger injection chains
- **Thread Counts**: Unusual patterns may indicate debugging

### Target Detection List

**Debuggers (Primary Targets):**
- OllyDbg: `ollydbg.exe`
- x64dbg/x32dbg: `x64dbg.exe`, `x32dbg.exe`
- WinDbg: `windbg.exe`
- Immunity Debugger: `immunitydebugger.exe`
- GDB: `gdb.exe`

**Disassemblers:**
- IDA Pro: `ida.exe`, `ida64.exe`, `idaq.exe`, `idaq64.exe`, `idaw.exe`, `idaw64.exe`
- Ghidra: `ghidra.exe`
- Radare2: `radare2.exe`, `r2.exe`
- Binary Ninja: `binaryninja.exe`

**Process/System Monitors:**
- Process Hacker: `processhacker.exe`
- Process Explorer: `procexp.exe`, `procexp64.exe`
- Process Monitor: `procmon.exe`, `procmon64.exe`
- API Monitor: `apimonitor.exe`

**Network Analysis:**
- Wireshark: `wireshark.exe`
- Fiddler: `fiddler.exe`
- TCPView: `tcpview.exe`

**Malware Analysis Tools:**
- PE Studio: `pestudio.exe`
- Detect It Easy: `die.exe`
- Scylla: `scylla.exe`
- ImportREC: `importrec.exe`

### Detection Logic
1. Create process snapshot using `TH32CS_SNAPPROCESS` flag
2. Iterate through all running processes
3. Compare each process name against blacklist (case-insensitive)
4. Flag environment as compromised if matches found
5. Optionally enumerate parent processes to detect injection

### Key Advantages
- **Comprehensive Detection**: Single snapshot captures all processes
- **Accuracy**: Direct name matching minimizes false positives
- **Low Overhead**: Snapshot is fast and efficient
- **Extensible**: Easy to add new tool names to blacklist
- **No Special Privileges**: Works with standard user permissions.

### Real-World Impact
- Used by **major malware families**: Emotet, TrickBot, Maze, Conti
- Forces analysts to rename tools or use process hiding techniques
- Combined with other checks for multi-layered protection

---

## 2. [FindWindowA](https://github.com/CyberForgeEx/MalAPI-Arsenal/blob/main/Anti-Debugging/Process-System%20Enumeration/FindWindowA.c)

### Description
Window-based detection technique that searches for debugger and analysis tool windows by their class names or window titles. Provides GUI-level detection complementing process enumeration.

### Why It's Popular
- Detects GUI debuggers that users actively interact with
- Simple implementation with immediate results
- Catches debuggers even if process name is changed

### How It Works
```c
// Search by window class name
HWND hwnd = FindWindowA("OLLYDBG", NULL);

// Search by window title
hwnd = FindWindowA(NULL, "x64dbg");

// Combine both for higher accuracy
hwnd = FindWindowA("WinDbgFrameClass", "WinDbg");
```

The API searches for windows matching:
- **Class Names**: Unique identifiers for application window types
- **Window Titles**: Text displayed in title bar
- **Both**: For precise identification

### Target Window Classes & Titles

**OllyDbg:**
- Class: `OLLYDBG`
- Title: `OllyDbg`

**x64dbg/x32dbg:**
- Class: `Qt5QWindowIcon` (Qt framework)
- Title: `x64dbg`, `x32dbg`

**WinDbg:**
- Class: `WinDbgFrameClass`
- Title: `WinDbg`

**IDA Pro:**
- Class: `QWidget`, `Qt5QWindowIcon`
- Title: Contains `IDA` or `IDA v`

**Immunity Debugger:**
- Class: `ID`
- Title: `Immunity Debugger`

**Ghidra:**
- Title: `Ghidra`

**Process Hacker:**
- Class: `ProcessHacker`
- Title: `Process Hacker`

**Process Explorer/Monitor:**
- Title: `Process Explorer`, `Process Monitor`

**Wireshark:**
- Title: `Wireshark`

### Detection Logic
1. Enumerate known debugger window classes and titles
2. Call `FindWindowA` for each combination
3. If window handle returned (not NULL), tool is detected
4. Optionally use `GetForegroundWindow` to check active window
5. Flag environment based on findings

### Key Advantages
- **GUI Detection**: Catches actively used debuggers
- **Rename Resilient**: Works even if process is renamed
- **Real-Time**: Detects windows as they appear
- **User Interaction Aware**: Can detect which tool analyst is using
- **Minimal Overhead**: Direct window lookup is fast
- **No Admin Rights**: Standard user permissions sufficient

---

## Comparison

| Feature | CreateToolhelp32Snapshot | FindWindowA |
|---------|--------------------------|-------------|
| **Purpose** | Process Detection | Window Detection |
| **Complexity** | Medium | Low |
| **Detection Scope** | All Processes | GUI Applications |
| **Evasion Difficulty** | Hard | Medium |
| **False Positives** | Very Low | Low |
| **Overhead** | Low | Minimal |
| **Admin Required** | No | No |
| **Detects Hidden Processes** | No | No |

---

## Educational Purpose
This documentation is provided for **educational and research purposes only**. Understanding process and system enumeration techniques is essential for:
- Malware analysis and reverse engineering
- Security research and threat intelligence
- Developing detection and response capabilities
- Building analysis-resistant software (legitimate use cases)
- Cybersecurity education and training
- Improving analysis environment hardening

**Disclaimer**: Implementing anti-debugging techniques in malicious software is illegal. This information is provided solely for defensive cybersecurity purposes and academic research.

---

## References
- MALAPI: [CreateToolhelp32Snapshot](https://malapi.io/winapi/CreateToolhelp32Snapshot)
- MALAPI: [FindWindowA](https://malapi.io/winapi/FindWindowA)

---

<p align="right">
  Last Updated: October 2025
</p>