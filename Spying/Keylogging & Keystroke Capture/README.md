<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Keylogging%20%26%20Keystroke%20Capture/Keystroke_Capture.c">Windows Keyboard Input API - Comprehensive Demonstration</a>
</h1>

## Overview
This program demonstrates **legitimate use of Windows Keyboard Input APIs** for key state monitoring, keyboard layout management, and input processing. These techniques are widely used in real-world Windows applications for:

- Keyboard shortcut detection and hotkey management
- Text input processing and character mapping
- Game input handling and control schemes
- Accessibility tools and assistive technologies
- Keyboard layout detection and multi-language support
- Educational demonstrations of Windows input programming

This is the foundation of keyboard input functionality in many Windows software products including IDEs, games, productivity tools, and system utilities.

---
## Core APIs Demonstrated
| API                  | Purpose                                      | Use Case                                      |
|----------------------|----------------------------------------------|-----------------------------------------------|
| `GetAsyncKeyState`   | Query current physical key state             | Real-time key press detection, game input     |
| `GetKeyboardState`   | Retrieve state of all 256 virtual keys      | Comprehensive keyboard state snapshot         |
| `GetKeyState`        | Get toggle state of specific key             | CAPS LOCK, NUM LOCK, SCROLL LOCK detection    |
| `GetKeyNameTextA`    | Convert scan code to readable key name       | Display user-friendly key descriptions        |
| `MapVirtualKeyA`     | Map between virtual keys and scan codes      | Keyboard layout independent key handling      |
| `MapVirtualKeyExA`   | Map keys with specific keyboard layout      | Multi-language keyboard support               |
| `GetKeyboardLayout`  | Query active keyboard layout                 | Detect current input language/layout          |

---
## 1. [GetAsyncKeyState - Real-Time Key Detection](https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Keylogging%20%26%20Keystroke%20Capture/Keystroke_Capture.c#L6)
**Description:** Monitors physical keyboard state in real-time using asynchronous key state queries.  
**Key Features:**
- Detects key presses at hardware level (outside message queue)
- High-order bit (0x8000) indicates key is currently down
- Suitable for applications without message loops
- Includes debouncing to prevent repeated detections

**Use Cases:**
- Game input systems
- Global hotkey detection
- Background monitoring applications
- Real-time control interfaces

---
## 2. [GetKeyboardState - Complete Keyboard Snapshot](https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Keylogging%20%26%20Keystroke%20Capture/Keystroke_Capture.c#L39)
**Description:** Captures the state of all 256 virtual keys in a single operation.  
**Key Features:**
- Returns complete keyboard state array
- Tracks modifier keys (Shift, Ctrl, Alt) combinations
- Monitors alphanumeric and special key states
- Implements change detection to avoid duplicate output

**Use Cases:**
- Keyboard state logging
- Multi-key combination detection
- Input recording and playback
- Keyboard layout testing

---
## 3. [GetKeyState - Toggle Key State Detection](https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Keylogging%20%26%20Keystroke%20Capture/Keystroke_Capture.c#L113)
**Description:** Queries toggle state of lock keys using low-order bit detection.  
**Key Features:**
- Low-order bit (0x0001) indicates toggle state
- Monitors CAPS LOCK and NUM LOCK status
- Persistent state across application lifetime
- Real-time toggle detection

**Use Cases:**
- Text editor status indicators
- Keyboard state UI displays
- Input validation for data entry
- Accessibility feature detection

---
## 4. [GetKeyNameTextA - Scan Code to Key Name Conversion](https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Keylogging%20%26%20Keystroke%20Capture/Keystroke_Capture.c#L141)
**Description:** Translates hardware scan codes into human-readable key names.  
**Key Features:**
- Converts scan codes to localized key descriptions
- Supports extended scan codes for special keys
- Returns system-specific key naming
- Demonstrates common key scan code mappings

**Demonstrated Keys:**
- Letter keys (A, B)
- Function keys (F1, F10)
- Special keys (Enter, Space, Escape)
- Arrow keys (Up, Left)

**Use Cases:**
- Key binding configuration interfaces
- Input remapping tools
- Keyboard layout documentation
- User-facing key name display

---
## 5. [MapVirtualKeyA - Virtual Key ↔ Scan Code Mapping](https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Keylogging%20%26%20Keystroke%20Capture/Keystroke_Capture.c#L174)
**Description:** Bidirectional mapping between virtual key codes and hardware scan codes.  
**Key Features:**
- `MAPVK_VK_TO_VSC` (0): Virtual key → Scan code
- `MAPVK_VSC_TO_VK` (1): Scan code → Virtual key
- `MAPVK_VK_TO_CHAR` (2): Virtual key → Character value
- Demonstrates common key mappings (Enter, Space, letters)

**Use Cases:**
- Cross-platform key code translation
- Hardware-independent input handling
- Keyboard emulation and simulation
- Input system abstraction layers

---
## 6. [MapVirtualKeyExA - Layout-Specific Key Mapping](https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Keylogging%20%26%20Keystroke%20Capture/Keystroke_Capture.c#L196)
**Description:** Maps keys using specific keyboard layout handles for multi-language support.  
**Key Features:**
- Uses `GetKeyboardLayout()` to query active layout
- Maps keys according to current input language
- Supports international keyboard configurations
- Layout handle (HKL) based mapping

**Use Cases:**
- Multi-language applications
- International keyboard support
- Layout-aware input processing
- Localization testing

---
## Program Workflow
The demonstration provides an interactive menu system with complete keyboard monitoring:
1. Display interactive menu with API demonstrations
2. User selects demonstration type
3. Initialize keyboard state monitoring loop
4. Detect and display key events in real-time
5. Process key states with appropriate API
6. Format and output results to console
7. Handle exit conditions (ESC key)

---
## Compilation Instructions
### Visual Studio / cl.exe
```bash
cl.exe Keystroke_Capture.c /link user32.lib
```

### MinGW / GCC
```bash
gcc Keystroke_Capture.c -o keyboard_demo.exe -luser32
```

### Run (No admin rights required)
```cmd
keyboard_demo.exe
```

---
## Required Libraries
- `user32.lib` – User interface and input functions
- `windows.h` – Core Windows API definitions.

---
## System Requirements
- **OS:** Windows XP or later (Windows 7/8/10/11 fully supported)
- **Architecture:** x86 or x64
- **Permissions:** Standard user rights (no elevation required)
- **Console:** Standard Windows Console or Terminal application

---
## Interactive Menu
```
[1] GetAsyncKeyState - Real-time key detection
[2] GetKeyboardState - All keys state snapshot
[3] GetKeyState - Toggle key monitoring
[4] GetKeyNameTextA - Scan code to key name
[5] MapVirtualKeyA - Virtual key mapping
[6] MapVirtualKeyExA - Layout-specific mapping
[0] Exit
```

---
## Key Concepts
### Virtual Key Codes
- Platform-independent key identifiers (e.g., VK_RETURN, VK_SPACE)
- Defined in `winuser.h` header
- Range: 0x00 to 0xFF (256 virtual keys)
- Common codes: VK_ESCAPE (0x1B), VK_SPACE (0x20), 'A' (0x41)

### Scan Codes
- Hardware-specific key identifiers from keyboard controller
- Independent of keyboard layout
- Used for low-level input processing
- Format: Extended flag + OEM scan code

### Key State Bits
- **High-order bit (0x8000)**: Key is currently pressed
- **Low-order bit (0x0001)**: Toggle state (for CAPS, NUM, SCROLL)
- Retrieved via bitwise AND operations

---
## Error Handling
The program includes basic error handling for:
- Invalid menu selections
- Key state query failures
- Console input errors
- Resource cleanup on exit

---
## Output Examples
### GetAsyncKeyState
```
[+] Press ESC to exit, or any key to test...

 -> Key 'A' is currently pressed
 -> SPACE is currently pressed
 -> ENTER is currently pressed
 -> ESC pressed - exiting demo
```

### GetKeyState
```
CAPS LOCK: ON  | NUM LOCK: OFF
CAPS LOCK: OFF | NUM LOCK: ON
```

### GetKeyNameTextA
```
A key           -> 'A'
Enter           -> 'Enter'
Space           -> 'Space'
Up Arrow        -> 'Up'
```

---
## API References
- [GetAsyncKeyState](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getasynckeystate)
- [GetKeyboardState](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getkeyboardstate)
- [GetKeyState](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getkeystate)
- [GetKeyNameTextA](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getkeynametexta)
- [MapVirtualKeyA](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-mapvirtualkeya)
- [MapVirtualKeyExA](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-mapvirtualkeyexa)
- [GetKeyboardLayout](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getkeyboardlayout)

---
## Best Practices
1. **Always check high-order bit for key press detection**: `if (GetAsyncKeyState(key) & 0x8000)`
2. **Implement debouncing** for repeated key press prevention
3. **Use Sleep()** in polling loops to reduce CPU usage
4. **Release resources properly** on program exit
5. **Handle both physical and virtual key states** based on use case
6. **Consider keyboard layout** for international applications.

---
<p align="right">
  Last Updated: December 2025
</p>
