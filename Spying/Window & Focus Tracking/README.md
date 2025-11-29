<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Window%20%26%20Focus%20Tracking/Windows_Focus_Tracking.c">Windows Window & Focus Tracking - Active Window Monitoring Demonstration</a>
</h1>

## Overview
This program demonstrates **legitimate use of Windows User32 API** for window focus tracking, active window monitoring, and thread input management. These techniques are widely used in real-world Windows applications for:

- Activity monitoring and time tracking software
- Window management utilities and task switchers
- Accessibility tools and screen readers
- Application usage analytics
- Productivity monitoring tools
- Focus-aware automation systems
- Multi-window coordination in complex applications

This is the foundation of window focus management in many Windows productivity and monitoring tools.

---
## Core APIs Demonstrated
| API                       | Purpose                                      | Use Case                                      |
|---------------------------|----------------------------------------------|-----------------------------------------------|
| `GetForegroundWindow`     | Get handle to currently active window        | Focus tracking, activity monitoring           |
| `GetWindowThreadProcessId`| Retrieve process/thread ID from window       | Process identification, thread management     |
| `AttachThreadInput`       | Share input queue between threads            | Synchronous input processing, focus control   |
| `GetWindowText`           | Retrieve window title text                   | Window identification and display             |
| `OpenProcess`             | Open handle to process for querying          | Process information retrieval                 |
| `GetModuleBaseName`       | Get executable name of process               | Application identification                    |
| `GetCurrentThreadId`      | Get calling thread's identifier              | Thread input attachment operations            |
| `FindWindow`              | Locate window by title or class              | Specific window targeting                     |
| `IsWindowVisible`         | Check if window is visible on screen         | Filter hidden/background windows              |
| `GetKeyState`             | Query keyboard key state                     | Shared input state verification               |

---
## 1. [GetForegroundWindow - Active Window Monitoring](https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Window%20%26%20Focus%20Tracking/Windows_Focus_Tracking.c#L30)
**Description:** Continuously monitors and detects active window changes in real-time.  
**Key Features:**
- 100ms polling interval for responsive detection
- Retrieves window handle, title, and process name
- Change-based logging (only reports when window switches)
- 10-second demonstration period with timestamp tracking

**Use Cases:**
- Time tracking applications
- Activity monitoring software
- User behavior analytics
- Application usage statistics
- Context-aware automation tools

---
## 2. [AttachThreadInput - Thread Input Queue Sharing](https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Window%20%26%20Focus%20Tracking/Windows_Focus_Tracking.c#L62)
**Description:** Demonstrates thread input attachment for synchronized input processing.  
**Key Features:**
- Attaches current thread to foreground window's input queue
- Enables shared keyboard state access (e.g., CAPS LOCK status)
- Demonstrates proper attachment and detachment lifecycle
- Thread ID comparison and validation

**Use Cases:**
- Custom window focus management
- Input synchronization between windows
- Advanced window manipulation utilities
- Cross-thread input coordination
- Accessibility tool development

---
## 3. [Focus Change Detection](https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Window%20%26%20Focus%20Tracking/Windows_Focus_Tracking.c#L115)
**Description:** High-frequency focus change detection with detailed event logging.  
**Key Features:**
- 50ms polling for rapid focus switch detection
- Sequential numbering of focus change events
- Timestamp and thread ID logging
- Statistical summary (total changes counted)

**Use Cases:**
- User interaction analysis
- Window switching pattern detection
- Productivity metrics collection
- Focus stability monitoring
- Multitasking behavior analysis

---
## 4. [Monitor Specific Window - Target Tracking](https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Window%20%26%20Focus%20Tracking/Windows_Focus_Tracking.c#L149)
**Description:** Monitors focus state of a user-specified window with usage statistics.  
**Key Features:**
- Interactive window selection by partial title match
- Visible window filtering using `IsWindowVisible`
- Focus gain/loss event detection
- Statistical analysis (total focus time, percentage)
- Real-time focus state tracking

**Use Cases:**
- Application-specific time tracking
- Distraction monitoring
- Focus-aware productivity tools
- Per-application usage analytics
- Parental control software

**Statistical Output:**
- Total time window had focus
- Percentage of monitoring period
- Timestamp for each focus transition

---
## Program Workflow
The demonstration follows a sequential interactive workflow:
1. **Initialize monitoring session** - Prepare tracking structures
2. **Poll GetForegroundWindow** - Continuously check active window
3. **Detect changes** - Compare current vs. previous window handle
4. **Retrieve window information** - Get title, process name, thread ID
5. **Log events** - Output timestamped focus change data
6. **Calculate statistics** - Summarize focus patterns and duration
7. **User interaction** - Interactive demo progression with Enter key

---
## Compilation Instructions
### Visual Studio / cl.exe
```bash
cl.exe window_focus_tracking.c /link user32.lib psapi.lib
```

### MinGW / GCC
```bash
gcc window_focus_tracking.c -o window_focus_tracking.exe -luser32 -lpsapi
```

### Run (No admin rights required)
```cmd
window_focus_tracking.exe
```

---
## Required Libraries
- `user32.lib` – Window management and input functions
- `psapi.lib` – Process Status API for process information

---
## System Requirements
- **OS:** Windows XP or later (Windows 7 through Windows 11 fully supported)
- **Architecture:** x86 or x64
- **Permissions:** Standard user rights (no elevation required)
- **GUI Session:** Must run in interactive desktop session with visible windows.

---
## Error Handling
The program includes error handling for:
- Null window handles (no foreground window)
- Process access failures (restricted processes)
- Thread attachment failures with `GetLastError()` reporting
- Window enumeration errors
- Invalid user input validation

---
## Technical Notes

### Polling Intervals
- **GetForegroundWindow demo:** 100ms (balanced responsiveness)
- **Focus detection:** 50ms (high sensitivity)
- **Specific window tracking:** 50ms (accurate statistics)

### Thread Input Attachment
- Only works between threads in same desktop session
- Cannot attach to system/elevated processes without privileges
- Requires proper detachment to avoid input queue corruption.

### Process Information
- Uses `PROCESS_QUERY_INFORMATION | PROCESS_VM_READ` access rights
- Falls back to "[Unknown]" for inaccessible processes
- Works with standard user permissions for most applications.

---
## API References
- [GetForegroundWindow](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getforegroundwindow)
- [AttachThreadInput](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-attachthreadinput)
- [GetWindowThreadProcessId](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getwindowthreadprocessid)
- [GetWindowText](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getwindowtext)
- [OpenProcess](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-openprocess)
- [GetModuleBaseName](https://learn.microsoft.com/en-us/windows/win32/api/psapi/nf-psapi-getmodulebasenamea)

---
<p align="right">
  Last Updated: November 2025
</p>
