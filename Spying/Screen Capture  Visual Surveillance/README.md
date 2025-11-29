<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Screen%20Capture%20%20Visual%20Surveillance/Visual_Surveillance.c">Windows GDI Screen Capture - Device Context & BitBlt Demonstration</a>
</h1>

## Overview
This program demonstrates **legitimate use of Windows Graphics Device Interface (GDI) APIs** for screen capture, window rendering, and bitmap manipulation. These techniques are widely used in real-world Windows applications for:

- Full-screen and window-specific screenshot capture
- Remote desktop and monitoring software functionality
- Screen recording and streaming applications
- Image scaling and raster operations
- DWM (Desktop Window Manager) compatible window capture
- Educational demonstrations of Windows graphics programming

This is the foundation of screen capture functionality in many Windows software products including remote desktop tools, screen recorders, and monitoring applications.

---
## Core APIs Demonstrated
| API                  | Purpose                                      | Use Case                                      |
|----------------------|----------------------------------------------|-----------------------------------------------|
| `GetDC`              | Acquire device context for screen/window     | Access screen or window drawing surface       |
| `CreateCompatibleDC` | Create memory device context                 | Off-screen rendering and bitmap manipulation  |
| `CreateCompatibleBitmap` | Create bitmap compatible with device     | Allocate memory for captured images           |
| `BitBlt`             | Block transfer (copy) bitmap data            | Fast pixel-perfect screen capture             |
| `StretchBlt`         | Scaled block transfer with resizing          | Thumbnail generation, scaled screenshots      |
| `PrintWindow`        | Render window to device context (DWM-safe)   | Capture minimized/obscured windows            |
| `GetWindowRect`      | Retrieve window coordinates                  | Determine capture region dimensions           |
| `SelectObject`       | Select GDI object into device context        | Prepare bitmap for drawing operations         |
| `GetSystemMetrics`   | Query system display metrics                 | Get screen resolution and dimensions          |

---
## 1. [GetDC & Full Screen Capture](https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Screen%20Capture%20%20Visual%20Surveillance/Visual_Surveillance.c#L80)
**Description:** Captures the entire screen using device context and BitBlt operation.  
**Key Features:**
- Uses `GetDC(NULL)` to acquire screen device context
- Creates compatible memory DC and bitmap
- Performs pixel-perfect copy with `SRCCOPY` raster operation
- Saves output as 24-bit BMP file

**Use Cases:**
- Screenshot utilities
- Screen recording applications
- Multi-monitor capture support
- Desktop monitoring tools

---
## 2. [PrintWindow - DWM-Compatible Window Capture](https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Screen%20Capture%20%20Visual%20Surveillance/Visual_Surveillance.c#L136)
**Description:** Captures active window content using modern PrintWindow API.  
**Key Features:**
- Compatible with Desktop Window Manager (Windows Vista+)
- Can capture minimized or partially obscured windows
- Uses `PW_RENDERFULLCONTENT` flag for complete rendering
- Includes fallback method using screen coordinates

**Use Cases:**
- Application screenshot tools
- Window-specific monitoring
- UI automation and testing
- Documentation generation

---
## 3. [StretchBlt - Scaled Screen Capture](https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Screen%20Capture%20%20Visual%20Surveillance/Visual_Surveillance.c#L213)
**Description:** Captures and scales screen content to reduced dimensions.  
**Key Features:**
- High-quality scaling with `HALFTONE` stretch mode
- Reduces file size while maintaining visual quality
- Demonstrates real-time image processing
- Configurable scaling ratios

**Use Cases:**
- Thumbnail generation
- Preview images for applications
- Bandwidth optimization in remote desktop
- Image processing pipelines

---
## 4. [BitBlt Raster Operations](https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Screen%20Capture%20%20Visual%20Surveillance/Visual_Surveillance.c#L324)
**Description:** Demonstrates various raster operation codes (ROPs) for pixel manipulation.  
**Demonstrated Operations:**
- `SRCCOPY` → Direct pixel copy
- `NOTSRCCOPY` → Inverted copy (negative image)
- `SRCAND` → Logical AND combination
- `SRCINVERT` → XOR operation
- `BLACKNESS` → Fill with black
- `WHITENESS` → Fill with white

**Use Cases:**
- Image effects and filters
- Cursor rendering
- Transparency operations
- Visual effects in applications

---
## 5. [Advanced Window Capture - Screen Region Method](https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Screen%20Capture%20%20Visual%20Surveillance/Visual_Surveillance.c#L275)
**Description:** Captures window by coordinates from screen DC.  
**Key Features:**
- Uses `GetWindowRect` for precise positioning
- Captures from absolute screen coordinates
- Alternative method for problematic windows
- Handles multi-monitor setups

**Use Cases:**
- Window snipping tools
- Region-specific capture
- Multi-window composition
- Selective area screenshots

---
## Program Workflow
The demonstration provides an interactive menu with complete capture workflows:
1. Acquire device context (screen or window)
2. Create compatible memory DC and bitmap
3. Perform capture operation (BitBlt/StretchBlt/PrintWindow)
4. Convert bitmap to BMP file format
5. Write file with timestamped filename
6. Release all GDI resources (DCs, bitmaps, handles)

---
## Compilation Instructions
### Visual Studio / cl.exe
```bash
cl.exe screen_capture.c /link gdi32.lib user32.lib dwmapi.lib
```

### MinGW / GCC
```bash
gcc screen_capture.c -o screen_capture.exe -lgdi32 -luser32 -ldwmapi
```

### Run (No admin rights required)
```cmd
screen_capture.exe
```

---
## Required Libraries
- `gdi32.lib` – Graphics Device Interface functions
- `user32.lib` – Window management and system metrics
- `dwmapi.lib` – Desktop Window Manager functions (Windows Vista+)

> Note: All APIs are fully supported on modern Windows systems (Windows 7 through Windows 11).

---
## System Requirements
- **OS:** Windows Vista or later (Windows 7/8/10/11 fully supported)
- **Architecture:** x86 or x64
- **DWM:** Desktop Window Manager must be enabled for PrintWindow functionality
- **Permissions:** Standard user rights (no elevation required)

---
## Output Files
All screenshots are saved in the current directory with descriptive timestamped filenames:
- `Fullscreen_YYYYMMDD_HHMMSS.bmp` - Full screen capture
- `Window_YYYYMMDD_HHMMSS.bmp` - Active window capture
- `Scaled_YYYYMMDD_HHMMSS.bmp` - 50% scaled screen capture
- `Advanced_YYYYMMDD_HHMMSS.bmp` - Region-based capture
- `Bitblt_[OPERATION]_YYYYMMDD_HHMMSS.bmp` - Raster operation results

---
## Error Handling
The program includes comprehensive error checking with `GetLastError()` reporting for:
- Device context acquisition failures
- Bitmap creation errors
- Capture operation failures
- File I/O errors
- Resource cleanup validation
- Proper GDI object lifecycle management

---
## Interactive Menu
```
[1] Full Screen Capture (GetDC + BitBlt)
[2] Active Window Capture (PrintWindow - DWM Fixed)
[3] Scaled Capture (GetDC + StretchBlt)
[4] Advanced Capture (Screen Region)
[5] BitBlt Raster Operations Demo
[6] Run All Demonstrations
[7] Exit
```

---
## API References
- [GetDC](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getdc)
- [BitBlt](https://learn.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-bitblt)
- [StretchBlt](https://learn.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-stretchblt)
- [PrintWindow](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-printwindow)
- [CreateCompatibleDC](https://learn.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-createcompatibledc)
- [CreateCompatibleBitmap](https://learn.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-createcompatiblebitmap)

---
<p align="right">
  Last Updated: November 2025
</p>
