<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/UI-Based%20Evasion/UI_Based_Evasion_APIs.c">UI-Based Evasion – CreateWindowEx & RegisterHotKey API Demonstration.</a>
</h1>

## Overview
This program demonstrates **legitimate Windows GUI and input APIs** that are heavily abused by modern malware, fileless payloads, and red team implants to:

- Detect headless/sandboxed environments (no GUI support)
- Perform anti-analysis via global hotkey monitoring
- Evade automated analysis systems that run without a desktop
- Implement stealthy user-interaction triggers

Many sandboxes (Cuckoo, ANY.RUN, Joe Sandbox, Hybrid Analysis, etc.) run samples in **non-interactive sessions** or **headless mode** → `CreateWindowEx`, message pumps, and `RegisterHotKey` either fail silently or behave abnormally.

Purely **educational & research-oriented** essential knowledge for malware analysts, EDR developers, and blue team operators.

---
## Core APIs Demonstrated
| API                    | Purpose                                      | Sophistication | Sandbox/Headless Bypass Potential |
|------------------------|----------------------------------------------|----------------|------------------------------------|
| `CreateWindowExA`      | Create visible or hidden window              | Intermediate   | High (fails in Session 0 / headless) |
| `RegisterHotKey`       | Register system-wide global hotkeys          | Advanced       | Very High (fails without user session) |
| `UnregisterHotKey`     | Clean up hotkeys on exit                     | Basic          | N/A                                |
| Message Loop (`GetMessage/DispatchMessage`) | Keep process alive & process input | Intermediate   | High (rarely emulated correctly)   |

---
### Execution Screenshot (Real System)
![Program Execution Screenshot](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/UI-Based%20Evasion/demo.png)  
*Window appears with instructions. Press Ctrl+Alt+H → Help dialog appears. Ctrl+Alt+E → Clean exit.*

---
## 1. [CreateWindowExA + Message Pump](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/UI-Based%20Evasion/UI_Based_Evasion_APIs.c#L12)
### Description
Creates a fully functional overlapped window with `WS_OVERLAPPEDWINDOW`. Requires a real user desktop session.
### Key Features
- Fails or behaves incorrectly in Session 0, RDP shadow sessions, or sandbox headless mode
- Requires active message queue and desktop heap
- Used by malware to detect interactive user presence
### Use Cases (Attack)
- Abort execution if no real desktop (common in APT droppers)
- Delay payload until analyst interacts
- GUI-based C2 triggers

---
## 2. [RegisterHotKey + WM_HOTKEY](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/UI-Based%20Evasion/UI_Based_Evasion_APIs.c#L57)
### Description
Registers **system-wide** hotkeys that work even when the window is minimized or not focused.
### Key Features
- Requires interactive logon session with a real keyboard hook
- **Fails completely** in most sandbox environments
- Extremely strong anti-analysis indicator
### Use Cases (Attack)
- Wake-on-hotkey payloads (press secret combo → decrypt & run)
- Detect human analyst (only real users press hotkeys)
- Used by Smokeloader, Qakbot, AsyncRAT, etc.

---
## 3. [Clean UnregisterHotKey on Exit](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/UI-Based%20Evasion/UI_Based_Evasion_APIs.c#L94)
Proper cleanup prevents resource leaks and suspicious lingering hotkeys — professional malware behavior.

---
## Why This Defeats Most Sandboxes
| Environment                  | CreateWindowEx | RegisterHotKey | Detection Result              |
|------------------------------|----------------|----------------|-------------------------------|
| Real User Desktop            | Works          | Works          | Payload executes              |
| Cuckoo / ANY.RUN (default)   | Fails/Silent   | Fails          | Sample exits early            |
| Headless / Session 0         | Fails          | Fails          | No execution                  |
| Wine / Fake Desktop          | Partial        | Usually fails  | Often detected                |

**Result**: Sample appears benign or crashes early → low priority or not detonated.

---
## Detection & Countermeasures
| Technique                | EDR Detection | Sandbox Countermeasure                        |
|--------------------------|---------------|------------------------------------------------|
| `CreateWindowExA`        | Medium        | Run samples in full interactive desktop        |
| `RegisterHotKey`        | High          | Emulate real user session + keyboard input     |
| Long-running message loop| Medium        | Monitor GUI + input activity correlation      |

**Blue Team Tip**: Flag any process that creates a window **and** registers global hotkeys followed by network activity or injection — classic red team/living-off-the-land pattern.

---
## Compilation Instructions
### Visual Studio / cl.exe
```bash
cl.exe UI_Based_Evasion_APIs.c /link user32.lib gdi32.lib
```

### MinGW / GCC
```bash
gcc UI_Based_Evasion_APIs.c -o UI_Hotkey_Demo.exe -mwindows
```
> Use `-mwindows` to suppress console window (stealthier)

### Run (Interactive session required!)
```cmd
UI_Hotkey_Demo.exe
```

---
## Required Libraries
- `user32.lib` – Window creation, hotkeys, message loop.
- `gdi32.lib` – Painting text via `TextOutA`

---
## API References
- [CreateWindowExA](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexa)
- [RegisterHotKey](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerhotkey)
- [UnregisterHotKey](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-unregisterhotkey)
- [Message Loop](https://learn.microsoft.com/en-us/windows/win32/winmsg/using-messages-and-message-queues)

---
<p align="right">
  Last Updated: November 2025
</p>
