<h1 align="center">
    Timing-Based Anti-Debug Techniques
</h1>

## Overview
Timing-based anti-debug techniques exploit the fundamental difference in code execution speed between normal execution and single-stepped debugging. These methods measure the time required for code blocks to execute and detect when a debugger has artificially inflated execution times through breakpoints, stepping, or instrumentation. Unlike direct API checks, timing analysis represents an elegant and difficult-to-bypass detection mechanism.

## Common APIs
| API | Sophistication |
|-----|----------------|
| `GetTickCount` | Basic | 
| `GetTickCount64` | Basic | 
| `QueryPerformanceCounter` | Advanced | 

---

## 1. [GetTickCount / GetTickCount64](https://github.com/CyberForgeEx/MalAPI-Arsenal/blob/main/Anti-Debugging/Timing-Based%20Detection/GetTickCount_GetTickCount64.c)

### Description
The most widely-deployed timing-based anti-debug technique that measures millisecond-precision elapsed time during code execution. By calculating the duration required for fast operations, this method detects the artificial delays introduced by debugger instrumentation, breakpoints, and single-stepping.

### Why It's Popular
- Measures real execution overhead added by debuggers
- Simple and direct detection methodology
- Available since Windows 95 with universal compatibility
- Extremely difficult to accurately spoof without full execution simulation

### How It Works

**GetTickCount Operation**: Calls `GetTickCount()` to retrieve the current system uptime in milliseconds, executes a dummy code block, then calls `GetTickCount()` again to measure elapsed time.

**Execution Time Analysis**: Under normal execution, fast operations complete in 0-1 milliseconds. When a debugger is present with breakpoints set on instructions within the timed block, each breakpoint triggers a handler context switch, adding 5-100+ milliseconds depending on the number of breakpoints and stepping operations.

**Threshold Detection**: Compares elapsed time against expected baseline thresholds. Any deviation beyond normal variance indicates debugger presence:
- Normal execution: 0-1 ms
- Debugged execution: 5-100+ ms
- Clear detection threshold: >2 ms

**GetTickCount64 Enhancement**: Provides identical functionality with 64-bit precision, eliminating rollover issues that occur with the 32-bit GetTickCount after approximately 49.7 days of system uptime.

### Key Advantages
- Lightweight performance impact
- Impossible to fully emulate without running actual code
- Single API call per timing measurement
- Works across all Windows versions
- Detects multiple debugger types and configurations

---

## 2. [QueryPerformanceCounter](https://github.com/CyberForgeEx/MalAPI-Arsenal/blob/main/Anti-Debugging/Timing-Based%20Detection/QueryPerformanceCounter.c)

### Description
High-precision microsecond-level timing analysis that detects single-stepping and fine-grained debugger instrumentation. This advanced technique provides sub-millisecond timing resolution, catching debuggers that attempt to minimize delay through sophisticated stepping algorithms or breakpoint optimization.

### Why It's Popular
- Provides microsecond-precision detection impossible to spoof accurately
- Detects optimized debuggers with minimal breakpoint overhead
- Captures hardware-level timing that reflects actual execution cost
- Significantly harder to bypass than millisecond-level timing

### How It Works

**Performance Counter Setup**: Retrieves the system's performance counter frequency using `QueryPerformanceFrequency()` to establish a baseline for timing calculations. This frequency varies by hardware but typically ranges from millions to tens of millions of cycles per second.

**High-Precision Timing**: Captures the performance counter value before and after code execution using `QueryPerformanceCounter()`. The difference in counter values represents the actual CPU cycles consumed during the code block.

**Microsecond Conversion**: Converts raw cycle counts to microsecond-equivalent values using the frequency: `elapsed_microseconds = cycles / (frequency / 1,000,000)`.

**Debugger Detection**: Compares measured timing against expected baselines. Sub-millisecond code execution typically requires <1000 microseconds; debugged execution with breakpoints easily produces 10,000+ microseconds of delay per breakpoint.

**Consistency Validation**: Cross-references QueryPerformanceCounter results with GetTickCount measurements to detect timing inconsistencies or anomalies that indicate debugger interference.

### Key Advantages
- Microsecond precision catches optimized debuggers
- Hardware-based timing resistant to software spoofing
- Detects sub-breakpoint overhead and stepping anomalies
- Impossible to fully emulate without actual execution
- Provides timing data that correlates directly with CPU cycles
- Highly resistant to timing-based bypasses

---

## Comparison

| Feature | GetTickCount/64 | QueryPerformanceCounter |
|---------|-----------------|------------------------|
| **Purpose** | Millisecond-precision debugger detection | Microsecond-precision debugger detection |
| **Complexity** | Basic | Advanced |
| **Bypass Difficulty** | Very Hard | Extremely Hard |
| **Performance Impact** | Negligible | Negligible |
| **Ideal For** | Standard debuggers, breakpoints | Optimized/stealthy debuggers |

---

## Best Practices for Implementation

1. **Dual-Timing Approach**: Use both GetTickCount and QueryPerformanceCounter to catch both standard and optimized debuggers
2. **Dummy Code Selection**: Choose target code that executes quickly but performs real operations (not easily optimized away)
3. **Volatile Variables**: Use `volatile` keyword to prevent compiler optimizations that eliminate timed code
4. **Consistency Checks**: Cross-reference different timing APIs to detect spoofed or manipulated results
5. **Reasonable Thresholds**: Allow for system variance and optimization—use 2-5x normal baseline as threshold
6. **Multiple Measurements**: Run timing checks multiple times; debuggers consistently add delays
7. **Obfuscate Timing Logic**: Embed timing checks within larger code blocks to avoid static analysis detection
8. **Variable Baselines**: Adapt thresholds based on system performance characteristics

---

## Educational Purpose

This documentation is provided for **educational and research purposes only**. Understanding timing-based anti-debugging techniques is essential for:
- Malware analysis and reverse engineering
- Debugger development and improvement
- Software security research
- Anti-tamper and anti-analysis mechanism design
- Cybersecurity education and understanding evasion techniques

---

## References
- MALAPI: [GetTickCount](https://malapi.io/winapi/GetTickCount)
- MALAPI: [GetTickCount64](https://malapi.io/winapi/GetTickCount64)
- MALAPI: [QueryPerformanceCounter](https://malapi.io/winapi/QueryPerformanceCounter)

---

<p align="right">
  Last Updated: October 2025
</p>
