<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Window%20Hooking%20%20System-Wide%20Monitoring/Window_Hooking.c">Windows System-Wide Input & Window Event Monitor Demonstration</a>
</h1>

## Overview
This is an educational C program that demonstrates legitimate, well-documented Windows hooking and event monitoring techniques using only the official User32 and Accessibility APIs **no DLL injection required**.

It showcases three powerful, fully legal monitoring methods commonly used in professional software such as:
- Parental control & digital wellbeing tools
- Accessibility software and screen readers
- Advanced input automation and macro tools
- Application usage analytics and time trackers
- Security research and endpoint monitoring solutions
- UI testing and automation frameworks

All techniques shown are used daily in legitimate commercial software (e.g., RescueTime, ManicTime, Actual Window Manager, DisplayFusion, accessibility tools, etc.).

---
## Core APIs & Techniques Demonstrated

| Technique                     | API Used                        | Scope              | Injection Required? | Use Case Example                     |
|-----------------------------|----------------------------------|---------------------|----------------------|--------------------------------------|
| Low-Level Keyboard Hook     | `SetWindowsHookEx(WH_KEYBOARD_LL)` | System-wide        | No                | Global hotkeys, macro recorders      |
| Message Retrieval Hook      | `SetWindowsHookEx(WH_GETMESSAGE)`  | Current thread only | No (thread-local) | Input message inspection             |
| WinEvent Hooks              | `SetWinEventHook()`              | System-wide        | No                | Accessibility tools, window tracking |

---
### [WH_KEYBOARD_LL – True System-Wide Keyboard Monitoring (No DLL Injection)](https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Window%20Hooking%20%20System-Wide%20Monitoring/Window_Hooking.c#L47)

**Clean method** for global keyboard capture on Windows.

**Key Features:**
- Captures every key press system-wide
- Works across all applications and privilege levels
- No DLL injection needed (unlike WH_KEYBOARD)
- Filters out self-injected events
- Detects physical scan codes and virtual key codes
- Logs active window title, class, and PID per keystroke
- Shows real-time modifier state (Shift/Ctrl/Alt)
- Secret exit combo: **Ctrl+Alt+S** (blocks propagation)

---
### [WH_GETMESSAGE Hook – Message Queue Inspection](https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Window%20Hooking%20%20System-Wide%20Monitoring/Window_Hooking.c#L89)

Demonstrates intercepting posted Windows messages in the current thread.

**Logs:**
- WM_KEYDOWN / WM_KEYUP / WM_CHAR
- Mouse events
- Window handle, class name, title, and owning PID
- Character input from WM_CHAR (proper text entry)

> Note: For system-wide WH_GETMESSAGE, a DLL would be needed this demo shows the thread-local version.

---
### [WinEvent Hooks – Accessibility & Shell Event Monitoring](https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Window%20Hooking%20%20System-Wide%20Monitoring/Window_Hooking.c#L130)

Uses the Accessibility subsystem (used by screen readers like NVDA, JAWS, Narrator).

**Monitors system-wide events such as:**
- Foreground window changes
- Window create/destroy/show/hide
- Minimize/maximize
- Focus changes
- Name/title changes

Extremely reliable and used heavily in enterprise monitoring and accessibility tools.

---
## Features Summary

| Feature                            | Implemented |
|------------------------------------|-------------|
| System-wide keystroke logging      | Yes         |
| Active window title & PID tracking | Yes         |
| Physical key vs injected filtering | Yes         |
| Modifier key state detection       | Yes         |
| Timestamped console + file logging | Yes         |
| Clean shutdown via Ctrl+Alt+S      | Yes         |
| Alternative: Press Enter in console| Yes         |
| No external dependencies           | Yes         |
| Works on Windows 7–11              | Yes         |
| Runs without admin rights          | Yes         |

---
## Compilation Instructions

### Visual Studio / cl.exe
```cmd
cl.exe Window_Hooking.c /link user32.lib psapi.lib advapi32.lib
```

### MinGW / GCC
```bash
gcc Window_Hooking.c -o hook_monitor.exe -luser32 -lpsapi -ladvapi32
```

---
## How to Run

1. Compile the program
2. Run `hook_monitor.exe` (no admin rights needed)
3. Start typing in any application — all keys will be logged
4. Watch real-time output in console
5. Check `hook_monitor.log` for full detailed history

**To stop monitoring:**
- Press **Ctrl + Alt + S** anywhere (secret combo, blocked from apps)
- OR press **Enter** in the console window

---
## [Sample Log Output (`hook_monitor.log`)](https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Window%20Hooking%20%20System-Wide%20Monitoring/hook_monitor.log)
---
## Technical Notes

- Uses `SE_DEBUG_PRIVILEGE` only to improve process name resolution (optional)
- Ignores injected events via `LLKHF_INJECTED` flag
- Safe cleanup of all hooks and handles
- Low CPU usage with `Sleep(10)` in message loop
- Works in locked screen/session (with limitations).

---
## API References

- [SetWindowsHookEx - WH_KEYBOARD_LL](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowshookexa)
- [LowLevelKeyboardProc](https://learn.microsoft.com/en-us/windows/win32/winmsg/lowlevelkeyboardproc)
- [SetWinEventHook](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwineventhook)
- [WinEvents](https://learn.microsoft.com/en-us/windows/win32/winauto/event-constants)

---
<p align="right">
  <strong>Last Updated: December 2025</strong><br>
</p>
