<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Enumeration/Window-Desktop%20Enumeration/WindowDesktopEnum.c">Windows-Desktop Techniques</a>
</h1>

## Overview
Window and desktop enumeration is a fundamental Windows API capability for discovering, monitoring, and analyzing windows and desktop environments. This documentation covers advanced APIs for enumerating system windows, desktop-specific windows, and obtaining detailed window characteristics.

---

## Core APIs

| API | Purpose | Sophistication |
|-----|---------|----------------|
| `EnumWindows` | Enumerate all top-level windows | Basic |
| `EnumDesktopWindows` | Enumerate windows on specific desktop | Intermediate |
| `GetWindowText` | Get window title text | Basic |
| `GetClassName` | Get window class name | Basic |
| `GetWindowThreadProcessId` | Get window's process ID | Basic |
| `IsWindowVisible` | Check if window is visible | Basic |
| `IsWindowEnabled` | Check if window is enabled | Basic |
| `GetWindowRect` | Get window dimensions | Basic |

---

## 1. EnumWindows
### Description
Enumerates all top-level windows on the screen by passing the handle to each window to an application-defined callback function. This provides a system-wide view of all visible and hidden windows.

### Key Features
- System-wide window enumeration
- Callback-based iteration
- Enumerates all top-level windows
- Includes visible and hidden windows
- Fast and efficient enumeration
- No desktop restriction

### Use Cases
- Window manager development
- System monitoring tools
- Application discovery
- Window hierarchy analysis
- Task switching implementations
- Security monitoring

### Example Output
```
HWND       PID      Visible  Window Title                                       Class Name
--------------------------------------------------------------------------
0x000A0124 5428     Yes      Microsoft Visual Studio Code                       Chrome_WidgetWin_1
0x000B0356 1234     Yes      Windows Task Manager                               TaskManagerWindow
0x000C0892 8764     No       (No Title)                                         Shell_TrayWnd
0x000D0456 2156     Yes      Mozilla Firefox                                    MozillaWindowClass

Statistics:
  Total Windows: 247
  Visible Windows: 89
  Windows with Title: 45
  Hidden Windows: 158
```

---

## 2. EnumDesktopWindows
### Description
Enumerates all top-level windows associated with a specific desktop. This allows for desktop-scoped enumeration, useful in multi-desktop environments or terminal services scenarios.

### Key Features
- Desktop-specific enumeration
- Callback-based iteration
- Multi-desktop support
- Terminal services compatibility
- Desktop handle requirement

### Use Cases
- Virtual desktop management
- Terminal services monitoring
- Desktop-specific tools
- Multi-user environments
- Remote desktop applications
- Desktop security analysis

### Example Output
```
Desktop Name: Default
Desktop Handle: 0x00000078

HWND       PID      Visible  Enabled  Window Title                             Class Name
--------------------------------------------------------------------------
0x000A0124 5428     Yes      Yes      File Explorer                            CabinetWClass
0x000B0356 1234     Yes      Yes      Google Chrome                            Chrome_WidgetWin_1
0x000C0892 8764     No       Yes      (No Title)                               WorkerW
0x000D0456 2156     Yes      Yes      Notepad                                  Notepad

Desktop Window Statistics:
  Desktop Name: Default
  Total Windows: 156
  Visible: 67 (42.9%)
  Enabled: 152 (97.4%)
  With Title: 38 (24.4%)
```

---

## Comparison

| Feature | EnumWindows | EnumDesktopWindows |
|---------|-------------|-------------------|
| **Scope** | System-wide | Desktop-specific |
| **Speed** | Very Fast | Fast |
| **Memory Overhead** | Low | Low |
| **Desktop Handle** | Not Required | Required |
| **Multi-Desktop** | All Desktops | Single Desktop |
| **Use Case** | General enumeration | Desktop-specific tasks |
| **Terminal Services** | All Sessions | Current Session |

---

## Advanced Window Analysis

### Window Styles
The implementation analyzes various window styles:

| Style | Constant | Description |
|-------|----------|-------------|
| Overlapped | `WS_OVERLAPPEDWINDOW` | Standard application window |
| Popup | `WS_POPUP` | Popup or dialog window |
| Child | `WS_CHILD` | Child window within parent |
| Visible | `WS_VISIBLE` | Currently visible window |
| Disabled | `WS_DISABLED` | Disabled window (grayed) |

### Window Properties Retrieved
- **HWND**: Window handle (unique identifier)
- **Process ID**: Process owning the window
- **Title**: Window title text
- **Class Name**: Window class identifier
- **Visibility**: Whether window is visible
- **Enabled State**: Whether window accepts input
- **Dimensions**: Window rectangle coordinates
- **Style Flags**: Window style attributes
- **Parent Handle**: Parent window (if any)

---

## Implementation Details

### Callback Function Signature
```c
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    // Process window
    // Return TRUE to continue, FALSE to stop
}
```

---

## Required Libraries
- `user32.lib` (User Interface API) - For window enumeration and management
- `kernel32.lib` (Windows API) - For core Windows operations


---

## Common Applications

### System Monitoring Tools
- Task Manager window tracking
- Process Explorer integration
- System resource monitors
- Window hierarchy viewers


### Desktop Management
- Virtual desktop managers
- Window organizers
- Multi-monitor tools
- Workspace management

### Security Tools
- Window behavior analysis
- Application monitoring
- Malware detection (suspicious windows)
- Security auditing tools

---

## Performance Considerations

### EnumWindows Performance
- Enumerates all top-level windows
- Execution time: 1-10 milliseconds on modern systems
- Memory usage: Minimal (depends on callback implementation)
- System impact: Very low

### EnumDesktopWindows Performance
- Enumerates desktop-specific windows (typically 50-300 windows)
- Execution time: 1-8 milliseconds on modern systems
- Memory usage: Minimal (depends on callback implementation)
- System impact: Very low
- Slightly faster than EnumWindows due to reduced scope

---

## API References
- MSDN: [EnumWindows](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-enumwindows)
- MSDN: [EnumDesktopWindows](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-enumdesktopwindows)
- MSDN: [GetWindowText](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getwindowtexta)
- MSDN: [GetClassName](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getclassnamea)
- MSDN: [GetWindowThreadProcessId](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getwindowthreadprocessid)
- MSDN: [IsWindowVisible](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-iswindowvisible)
- MSDN: [IsWindowEnabled](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-iswindowenabled)
- MSDN: [GetWindowRect](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getwindowrect)
- MSDN: [GetThreadDesktop](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getthreaddesktop)

---

<p align="right">
  Last Updated: October 2025
</p>
