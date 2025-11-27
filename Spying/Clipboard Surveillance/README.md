<h1 align="center"> <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Clipboard%20Surveillance/Get_Clipboard_Data.c">Windows Clipboard Surveillance & Real-Time Monitoring Demonstration.</a> </h1>

### Overview
This program demonstrates legitimate usage of the Windows Clipboard API and message-pump techniques to monitor, inspect, and log clipboard activity in real time. It showcases both one-time clipboard reading and continuous surveillance using the modern `AddClipboardFormatListener` API (introduced in Windows Vista+).

These techniques are commonly used in legitimate software for:

* Accessibility tools and screen readers  
* Password managers and form fillers  
* Enterprise data loss prevention (DLP) solutions  
* Clipboard history managers.
* Security research and forensic analysis tools.

This is educational code illustrating proper clipboard interaction patterns on Windows.

### Core APIs Demonstrated

| API                          | Purpose                                      | Use Case                                      |
|------------------------------|----------------------------------------------|-----------------------------------------------|
| `AddClipboardFormatListener` | Register a window to receive `WM_CLIPBOARDUPDATE` | Real-time clipboard change detection          |
| `RemoveClipboardFormatListener` | Unregister listener on cleanup             | Prevent resource leaks                        |
| `OpenClipboard` / `CloseClipboard` | Access clipboard data safely              | Required for all clipboard operations        |
| `GetClipboardData(CF_TEXT)`  | Retrieve ANSI text from clipboard            | Legacy text extraction                        |
| `GetClipboardData(CF_UNICODETEXT)` | Retrieve Unicode text (preferred)      | Modern text extraction                        |
| `EnumClipboardFormats`      | List all available data formats              | Detect images, files, rich text, custom data  |
| `GetClipboardFormatName`     | Resolve custom format IDs to names           | Identify proprietary clipboard data           |
| `CreateWindowEx` + message loop | Invisible window for receiving notifications | Required for `WM_CLIPBOARDUPDATE`             |

### Key Features Demonstrated

1. **Real-Time Clipboard Change Detection**  
   Uses `AddClipboardFormatListener` + hidden message-only window (`HWND_MESSAGE`) to receive instant notifications whenever clipboard content changes — no polling required.

2. **Comprehensive Clipboard Format Enumeration**  
   Lists every available clipboard format with human-readable names (including standard ones like `CF_BITMAP`, `CF_HDROP`, etc.).

3. **Smart Text Extraction**  
   Prioritizes `CF_UNICODETEXT`, falls back to `CF_TEXT`, and correctly handles null termination and buffer safety.

4. **Non-Text Content Detection**  
   Identifies images (bitmaps), dropped files (`CF_HDROP`), and other binary data.

5. **Timestamped Logging with Change Counter**  
   Each clipboard event includes precise timestamp and sequence number.

6. **One-Time vs Continuous Modes**  
   Offers both instant snapshot reading and persistent monitoring.

### Program Workflow

The program provides an interactive menu:

1. **Option 1** – Read current clipboard content immediately  
2. **Option 2** – Start real-time monitoring (blocks until terminated)  
3. **Option 3** – Exit  

When monitoring is active:
- Creates an invisible message-only window.
- Registers for clipboard updates.
- On each change: logs timestamp, formats, and attempts to extract text.
- Supports text, images, files, and unknown data types.

### Compilation Instructions

**Visual Studio / cl.exe**
```bash
cl.exe clipboard_monitor.c user32.lib advapi32.lib
```

**MinGW / GCC**
```bash
gcc clipboard_monitor.c -o clipboard_monitor.exe -luser32 -lgdi32
```

**Run (No admin rights required)**
```cmd
clipboard_monitor.exe
```

### Required Libraries

- `user32.lib` – Clipboard and windowing functions  
- `gdi32.lib` – Optional (for bitmap detection)

### System Requirements

- OS: Windows Vista or later (including Windows 11)  
  → `AddClipboardFormatListener` introduced in Vista
- Architecture: x86 or x64  
- Works in both console and GUI subsystems

### Error Handling

Full error reporting using `GetLastError()` for:
- Failed window creation/registration
- Clipboard access conflicts
- Listener registration failures
- Safe fallback when formats are unavailable

### Notes

- This uses the modern and recommended way to monitor the clipboard (post-XP era)
- Polling-based methods (`GetClipboardSequenceNumber`) are obsolete and inefficient.
- The hidden message-only window technique is standard practice in production tools.
- For older OS support (XP/2003), you would need to use a clipboard viewer chain via `SetClipboardViewer` (deprecated).

### API References

- [AddClipboardFormatListener](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-addclipboardformatlistener)
- [WM_CLIPBOARDUPDATE](https://learn.microsoft.com/en-us/windows/win32/dataxchg/wm-clipboardupdate)
- [EnumClipboardFormats](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-enumclipboardformats)
- [GetClipboardData](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getclipboarddata)

<p align="right"> Last Updated: November 2025 </p>
