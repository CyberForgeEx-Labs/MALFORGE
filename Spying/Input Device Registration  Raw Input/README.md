<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Input%20Device%20Registration%20%20Raw%20Input/Input_Device_Registration.c">Windows Raw Input & HotKey Monitor - Device Input Capture Demonstration</a>
</h1>

## Overview
This program demonstrates **Use of Windows Raw Input API and HotKey registration** for low-level device input monitoring and global keyboard shortcuts. These techniques are widely used in real-world Windows applications for:

- Gaming peripherals and input device drivers.
- Macro recording and automation utilities
- Remote desktop and KVM software.
- Accessibility tools and assistive technology
- Input remapping and custom keyboard layouts
- System monitoring and usage analytics
- Multi-device input coordination
- Hardware testing and diagnostic tools
- Productivity enhancement applications

This is the foundation of low-level input handling in many Windows gaming, productivity, and system utility applications.

---
## Core APIs Demonstrated
| API                       | Purpose                                      | Use Case                                      |
|---------------------------|----------------------------------------------|-----------------------------------------------|
| `RegisterRawInputDevices` | Register for raw input from HID devices      | System-wide mouse/keyboard input capture      |
| `GetRawInputData`         | Retrieve raw input data from WM_INPUT        | Extract detailed device input information     |
| `RegisterHotKey`          | Register system-wide keyboard shortcut       | Global hotkey functionality                   |
| `UnregisterHotKey`        | Remove registered hotkey                     | Cleanup on application exit                   |
| `CreateWindowEx`          | Create message-only or hidden window         | Message handling for raw input                |
| `RegisterClassEx`         | Register custom window class                 | Define window procedure for message handling  |

---
## 1. [RegisterRawInputDevices - Device Registration](https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Input%20Device%20Registration%20%20Raw%20Input/Input_Device_Registration.c#L14)
**Description:** Registers application to receive raw input from mouse and keyboard devices system-wide.  
**Key Features:**
- Simultaneous registration for multiple device types
- `RIDEV_INPUTSINK` flag enables background input capture
- Works even when application is not in foreground
- HID Usage Page and Usage ID specification
- Target window handle binding

**Device Registration:**
```c
// Mouse: Usage Page 0x01 (Generic Desktop), Usage 0x02 (Mouse)
// Keyboard: Usage Page 0x01 (Generic Desktop), Usage 0x06 (Keyboard)
```

**Use Cases:**
- Multi-monitor mouse tracking applications
- Keyboard remapping utilities
- Input recording and playback software
- Hardware testing and diagnostics.
- Virtual machine guest tools

---
## 2. [ProcessRawMouse - Mouse Input Processing](https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Input%20Device%20Registration%20%20Raw%20Input/Input_Device_Registration.c#L39)
**Description:** Detailed processing of raw mouse input data with button state and movement tracking.  
**Key Features:**
- Button state detection (left, right, middle clicks)
- Raw relative mouse movement (delta X/Y)
- Mouse wheel scroll detection with delta value
- Click counter for statistical analysis
- Real-time event logging

**Mouse Events Captured:**
- `RI_MOUSE_LEFT_BUTTON_DOWN/UP` - Left button state
- `RI_MOUSE_RIGHT_BUTTON_DOWN/UP` - Right button state
- `RI_MOUSE_MIDDLE_BUTTON_DOWN/UP` - Middle button state
- `RI_MOUSE_WHEEL` - Scroll wheel movement
- `lLastX/lLastY` - Relative movement coordinates.

**Use Cases:**
- Custom mouse gesture recognition
- Gaming mouse input (FPS camera control)
- Drawing and graphics applications
- Mouse behavior analytics
- Accessibility mouse aids
- Multi-button mouse support.

**Technical Details:**
- **Movement Type:** Relative (delta) coordinates, not absolute screen position
- **Button Flags:** Bitmask values for simultaneous button detection
- **Wheel Data:** Signed 16-bit value (positive = forward, negative = backward)

---
## 3. [ProcessRawKeyboard - Keyboard Input Processing](https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Input%20Device%20Registration%20%20Raw%20Input/Input_Device_Registration.c#L67)
**Description:** Low-level keyboard input processing with virtual key and scan code extraction.  
**Key Features:**
- Virtual key code (VKey) identification
- Hardware scan code capture
- Key press/release state detection (RI_KEY_BREAK flag)
- Press counter for usage statistics
- Special key detection (ESC example)

**Keyboard Data Structure:**
- **VKey:** Virtual key code (standard Windows VK_* constants)
- **MakeCode:** Hardware scan code from keyboard controller
- **Flags:** `RI_KEY_BREAK` (0 = press, 1 = release), `RI_KEY_E0/E1` (extended keys)
- **Message:** WM_KEYDOWN, WM_KEYUP, WM_SYSKEYDOWN, WM_SYSKEYUP

**Use Cases:**
- Keyboard macro recording
- Custom keyboard layouts (Dvorak, Colemak)
- Gaming key binding systems
- Input method editors (IMEs)
- Keyboard diagnostics and testing
- Key remapping utilities

**Technical Details:**
- **Scan Code:** Hardware-specific code from keyboard (layout-independent)
- **Virtual Key:** Windows abstraction (layout-dependent)
- **RI_KEY_BREAK:** Bit flag indicating key release (if set)

---
## 4. [RegisterHotKey - Global Keyboard Shortcuts](https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Input%20Device%20Registration%20%20Raw%20Input/Input_Device_Registration.c#L91)
**Description:** System-wide hotkey registration for global keyboard shortcuts.  
**Key Features:**
- Multiple hotkey registration support
- Modifier key combinations (Ctrl, Alt, Shift, Win)
- System-wide scope (works in any application)
- WM_HOTKEY message notification
- Automatic conflict detection

**Registered Hotkeys:**
1. **Ctrl+Alt+K** - Display statistics (click/key counts)
2. **Ctrl+Shift+M** - Reset counters

**Modifier Keys:**
- `MOD_CONTROL` - Ctrl key required
- `MOD_ALT` - Alt key required
- `MOD_SHIFT` - Shift key required
- `MOD_WIN` - Windows key required (Vista+)
- `MOD_NOREPEAT` - Prevent auto-repeat (Windows 7+)

**Use Cases:**
- Application launchers and shortcuts
- Global clipboard managers
- Screen capture utilities
- Music player controls
- Window management tools
- Productivity automation

**Technical Details:**
- **Hotkey ID:** User-defined integer identifier (1-0xBFFF)
- **Conflict Resolution:** Registration fails if hotkey already registered
- **Message:** `WM_HOTKEY` with hotkey ID in wParam

---
## 5. [WM_INPUT Message Processing](https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Input%20Device%20Registration%20%20Raw%20Input/Input_Device_Registration.c#L106)
**Description:** Windows message handler for raw input data extraction and processing.  
**Key Features:**
- Two-phase data retrieval (size query, then data)
- Dynamic memory allocation for variable-size input
- Device type discrimination (mouse vs keyboard)
- RAWINPUTHEADER inspection for device identification
- Error handling with GetLastError()

**Processing Flow:**
1. Call `GetRawInputData` with NULL buffer to get size
2. Allocate buffer of required size
3. Call `GetRawInputData` again with buffer to retrieve data
4. Cast to `RAWINPUT*` structure
5. Check `header.dwType` to determine device type
6. Process device-specific data (`mouse` or `keyboard` union member)
7. Free allocated buffer

**Device Types:**
- `RIM_TYPEMOUSE` (0) - Mouse input data
- `RIM_TYPEKEYBOARD` (1) - Keyboard input data
- `RIM_TYPEHID` (2) - Generic HID device

**Use Cases:**
- Multi-device input handling
- Input event logging and replay
- Input remapping engines
- Hardware testing frameworks

---
## 6. [Hidden Window Architecture](https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Input%20Device%20Registration%20%20Raw%20Input/Input_Device_Registration.c#L182)
**Description:** Message-only window for raw input handling without visible GUI.  
**Key Features:**
- Creates hidden window for message processing
- No visual components required
- Minimal resource footprint
- Background operation capability
- Standard window message handling

**Window Creation Purpose:**
- Raw input requires window handle as target (`hwndTarget`)
- Hotkeys require window for `WM_HOTKEY` message delivery
- Enables console application to receive GUI messages
- Alternative: Use `HWND_MESSAGE` for message-only window

**Use Cases:**
- Background monitoring applications
- System tray utilities
- Service-like applications with GUI message handling
- Lightweight input capture tools

---
## Program Workflow
The demonstration follows this execution flow:

1. **Initialization Phase**
   - Register custom window class (`RawInputDemoClass`)
   - Create hidden window for message handling
   - Store window handle in global variable

2. **Device Registration**
   - Call `RegisterRawInputDevices` for mouse
   - Call `RegisterRawInputDevices` for keyboard
   - Set `RIDEV_INPUTSINK` flag for background capture
   - Bind to window handle

3. **Hotkey Registration**
   - Register `Ctrl+Alt+K` for statistics display
   - Register `Ctrl+Shift+M` for counter reset
   - Handle registration failures gracefully

4. **Message Loop**
   - Enter infinite `GetMessage` loop
   - Translate virtual key messages
   - Dispatch to `WindowProc` callback
   - Process `WM_INPUT` and `WM_HOTKEY` messages

5. **Event Processing**
   - **WM_INPUT:** Extract raw input data, process mouse/keyboard
   - **WM_HOTKEY:** Handle hotkey commands (statistics, reset)
   - **WM_CREATE:** Register hotkeys during window creation
   - **WM_DESTROY:** Cleanup hotkeys and exit

6. **Cleanup Phase**
   - Unregister hotkeys
   - Post quit message
   - Exit message loop

---
## Compilation Instructions
### Visual Studio / cl.exe
```bash
cl.exe Input_Device_Registration.c /link user32.lib
```

### MinGW / GCC
```bash
gcc Input_Device_Registration.c -o RawInput_HotKey_Demo.exe -luser32
```

### Run (No admin rights required)
```cmd
RawInput_HotKey_Demo.exe
```

---
## Required Libraries
- `user32.lib` – Window management, raw input, and hotkey functions

---
## System Requirements
- **OS:** Windows XP or later (Windows 7 through Windows 11 fully supported)
- **Architecture:** x86 or x64
- **Permissions:** Standard user rights (no elevation required)
- **Session:** Interactive desktop session required for input capture
- **Hardware:** Standard HID-compatible mouse and keyboard.

---
## Usage Instructions

### Real-Time Monitoring
1. Run the executable in a console window
2. Move mouse and click buttons - see raw input events logged
3. Press any keyboard keys - see virtual key codes and scan codes
4. All input is captured system-wide (even from other applications)

### Hotkey Commands
- **Ctrl+Alt+K** - Display current statistics
  - Total mouse clicks counted
  - Total key presses counted
  
- **Ctrl+Shift+M** - Reset all counters
  - Clears mouse click count
  - Clears key press count

### Exit Methods
- Press **Alt+F4** in console window
- Close console window manually
- Hotkeys and raw input will be unregistered automatically

---
## Output Examples

### Mouse Input Events
```
[+] Left mouse button DOWN (Total clicks: 1)
[+] Mouse moved: X=5, Y=-3
[+] Mouse moved: X=12, Y=8
[+] Right mouse button DOWN
[+] Mouse wheel: 120
[+] Middle mouse button DOWN
```

### Keyboard Input Events
```
[+] Key pressed: VKey=0x41, ScanCode=0x1E (Total: 1)
[+] Key pressed: VKey=0x42, ScanCode=0x30 (Total: 2)
[+] Key pressed: VKey=0x1B, ScanCode=0x01 (Total: 3)
[+] ESC key detected! Press Ctrl+Alt+K to see statistics.
```

### Statistics Display (Ctrl+Alt+K)
```
[+] STATISTICS (Ctrl+Alt+K pressed)
  -> Total mouse clicks: 47
  -> Total key presses: 152
=====================================
```

### Counter Reset (Ctrl+Shift+M)
```
[+] Ctrl+Shift+M pressed - Resetting counters.
```

---
## API References
- [RegisterRawInputDevices](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerrawinputdevices)
- [GetRawInputData](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getrawinputdata)
- [RAWINPUT Structure](https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-rawinput)
- [RegisterHotKey](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerhotkey)
- [UnregisterHotKey](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-unregisterhotkey)
- [Raw Input Overview](https://learn.microsoft.com/en-us/windows/win32/inputdev/about-raw-input)
- [Keyboard Input Overview](https://learn.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input)
- [Mouse Input Overview](https://learn.microsoft.com/en-us/windows/win32/inputdev/about-mouse-input)

---
<p align="right">
  Last Updated: December 2025
</p>
