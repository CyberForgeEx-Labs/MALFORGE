<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/Time-Based%20Evasion%20-%20Sandbox%20Detection/Time_Based_Sandbox_Detection.c">Time-Based Evasion & Sandbox Detection Techniques</a>
</h1>

## Overview
This program demonstrates **seven legitimate Windows APIs** commonly abused by malware and red team tools to detect automated analysis environments (sandboxes, EDRs, AVs) through **timing delays and environmental fingerprinting**. These techniques exploit the fact that most sandboxes accelerate or skip long delays to speed up analysis. Understanding these methods is critical for **defensive security engineers, malware analysts, and blue team operators**.

Purely **educational and research-oriented** — shows how attackers detect virtualized or instrumented environments.

---
## Core APIs Demonstrated
| API                        | Purpose                                      | Sophistication | Sandbox Bypass Potential |
|----------------------------|----------------------------------------------|----------------|--------------------------|
| `NtDelayExecution`         | High-resolution native sleep                 | Advanced       | High                     |
| `SetWaitableTimer`         | Kernel timer with callback or wait           | Advanced       | High                     |
| `CreateTimerQueueTimer`    | Thread pool-based timer                      | Intermediate   | Medium-High              |
| `SetTimer`                 | Classic window timer (message pump)          | Intermediate   | Medium                   |
| `select()`                 | Socket timeout delay (Winsock)               | Intermediate   | Medium                   |
| `IcmpSendEcho`             | Network round-trip timing (ping)             | Intermediate   | High (if network blocked)|
| `timeGetTime`              | Busy-loop timing via multimedia timer        | Basic          | Low-Medium               |

---
## 1. [NtDelayExecution](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/Time-Based%20Evasion%20-%20Sandbox%20Detection/Time_Based_Sandbox_Detection.c#L31)
### Description
Native API from `ntdll.dll` that provides precise sleep functionality with 100ns resolution. Preferred by malware because it bypasses most user-mode API hooks and is rarely intercepted by EDRs in stealth mode.
### Key Features
- Direct syscall (harder to hook)
- Nanosecond precision
- Used by sophisticated malware (e.g., APTs)
### Use Cases (Attack)
- Evade sandbox accelerated sleep
- Delay payload execution
- Anti-analysis timing checks


---
## 2. [SetWaitableTimer / CreateWaitableTimer](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/Time-Based%20Evasion%20-%20Sandbox%20Detection/Time_Based_Sandbox_Detection.c#L45)
### Description
Creates a kernel-level waitable timer object that can trigger after a specified delay. When used with `WaitForSingleObject`, it causes the thread to enter a wait state — many sandboxes skip or accelerate this.
### Key Features
- Enters true wait state (not busy loop)
- Kernel-backed timing
- Cannot be easily accelerated without detection
### Use Cases (Attack)
- Long-term persistence delay
- Staged payload execution
- Anti-sandbox timing bomb


---
## 3. [CreateTimerQueueTimer](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/Time-Based%20Evasion%20-%20Sandbox%20Detection/Time_Based_Sandbox_Detection.c#L59)
### Description
Creates a timer using the Thread Pool API. Executes callback in a worker thread after delay. Less monitored than `Sleep()` and harder to hook globally.
### Key Features
- Asynchronous execution
- Uses Windows thread pool
- Low visibility in user-mode hooks
### Use Cases (Attack)
- Delayed beaconing
- Stealthy persistence triggers
- Anti-VM timing checks


---
## 4. [SetTimer + Message Pump](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/Time-Based%20Evasion%20-%20Sandbox%20Detection/Time_Based_Sandbox_Detection.c#L73)
### Description
Legacy window timer that sends `WM_TIMER` messages. Requires a message loop. Often ignored by basic hooks and used to detect GUI-less environments (common in sandboxes).
### Key Features
- Requires active message pump
- Fails silently in non-GUI contexts
- Detects headless/sandboxed execution
### Use Cases (Attack)
- Detect Cuckoo/Anubis-style sandboxes
- Environment fingerprinting


---
## 5. [select() Winsock Timeout](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/Time-Based%20Evasion%20-%20Sandbox%20Detection/Time_Based_Sandbox_Detection.c#L90)
### Description
Uses BSD-style `select()` with a timeout to implement delay. Even with no sockets, it causes a real kernel wait. Common in network-aware malware.
### Key Features
- No Sleep() call
- Looks like network activity
- Bypasses basic API monitoring
### Use Cases (Attack)
- Fake network delay
- Evade static API analysis


---
## 6. [IcmpSendEcho (Ping)](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/Time-Based%20Evasion%20-%20Sandbox%20Detection/Time_Based_Sandbox_Detection.c#L104)
### Description
Sends an ICMP echo request and waits for reply (or timeout). Sandboxes often block outbound ICMP or have no route to 8.8.8.8 — causing instant timeout or failure.
### Key Features
- Real network fingerprinting
- Detects isolated sandboxes
- Measures real RTT
### Use Cases (Attack)
- Detect FakeNet / isolated networks
- Confirm internet connectivity
- Delay + environment check combo


---
## 7. [timeGetTime Busy Loop](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/Time-Based%20Evasion%20-%20Sandbox%20Detection/Time_Based_Sandbox_Detection.c#L120)
### Description
Uses multimedia timer to implement a CPU-burning busy wait. Some sandboxes detect and flag high CPU usage over time.
### Key Features
- No API call to Sleep
- High CPU usage
- Easily detectable by defenders
### Use Cases (Attack)
- Burn sandbox time quota
- Force early termination


---
## Timing Evasion Flow
```
Malware Starts → Checks Environment → Delays 30–300+ seconds
      ↓
If delay executes fully → Likely real system
If skipped/accelerated → Sandbox detected → Exit or sleep forever
```

---
## Detection & Countermeasures
| Technique                  | EDR Detection | Sandbox Countermeasure         |
|----------------------------|-------------|--------------------------------|
| `NtDelayExecution`         | Medium      | Patch/hook ntdll                  |
| `SetWaitableTimer`         | High        | Accelerate kernel timers          |
| `CreateTimerQueueTimer`    | Medium      | Monitor thread pool callbacks     |
| `IcmpSendEcho`             | High        | Block or fake ICMP replies        |
| `timeGetTime` loop         | High        | Detect sustained CPU usage        |

**Blue Team Tip**: Look for long delays (>15s) followed by malicious behavior.

---
## Compilation Instructions
### Visual Studio / cl.exe
```bash
cl.exe TimeEvasionDemo.c /link winmm.lib ws2_32.lib iphlpapi.lib advapi32.lib
```

### MinGW / GCC
```bash
gcc TimeEvasionDemo.c -o TimeEvasionDemo.exe -lwinmm -lws2_32 -liphlpapi
```

### Run (No admin required)
```cmd
TimeEvasionDemo.exe
```

---
## Required Libraries
- `winmm.lib`     – `timeGetTime`
- `ws2_32.lib`    – Winsock (`select`)
- `iphlpapi.lib`  – ICMP API
- `ntdll.dll`     – `NtDelayExecution` (loaded dynamically)

---
## API References
- [NtDelayExecution](https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-ntdelayexecution)
- [CreateWaitableTimer](https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-createwaitabletimera)
- [CreateTimerQueueTimer](https://learn.microsoft.com/en-us/windows/win32/api/threadpoollegacy/nf-threadpoollegacy-createtimerqueuetimer)
- [IcmpSendEcho](https://learn.microsoft.com/en-us/windows/win32/api/icmpapi/nf-icmpapi-icmpsendecho)

---
<p align="right">
  Last Updated: November 2025
</p>
