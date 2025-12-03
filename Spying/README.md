<h1 align="center">
   Spying & Surveillance Techniques
</h1>

A comprehensive collection of C programs demonstrating core surveillance and input monitoring operations used in spyware and legitimate monitoring software, implemented via native Windows APIs.

## Overview

This repository provides a granular, code-level breakdown of the surveillance primitives and techniques that form the foundation of monitoring software. Each module contains a self-contained demonstration of a specific surveillance task, from clipboard monitoring and keystroke capture to screen recording and window focus tracking. The examples are designed for security analysis, reverse engineering, and malware research.

## Features

- **Clipboard Surveillance** - Monitoring and capturing clipboard content changes in real-time.
- **Input Device Registration (Raw Input)** - Low-level mouse and keyboard input capture using Raw Input API.
- **Keylogging & Keystroke Capture** - Recording keyboard input with various hooking techniques.
- **Message Interception Intelligence** - Intercepting and analyzing Windows messages for surveillance.
- **Screen Capture & Visual Surveillance** - Capturing screenshots and recording screen activity.
- **Window & Focus Tracking** - Monitoring active windows and application focus changes.
- **Window Hooking System-Wide Monitoring** - System-wide event monitoring using Windows hooks.

## Directory Structure

```
Spying/
├── Clipboard Surveillance/              # Clipboard content monitoring and capture techniques.
├── Input Device Registration Raw Input/ # Raw Input API for low-level device input capture.
├── Keylogging & Keystroke Capture/      # Keyboard input logging and keystroke recording.
├── Message Interception Intelligence/   # Windows message queue interception and analysis.
├── Screen Capture Visual Surveillance/  # Screenshot capture and screen recording utilities.
├── Window & Focus Tracking/             # Active window and focus change monitoring.
├── Window Hooking System-Wide Monitoring/ # System-wide hooks for comprehensive event capture.
```

## Core Modules

### Clipboard Surveillance
Demonstrates monitoring clipboard changes using `AddClipboardFormatListener` and `GetClipboardData`. Captures text, images, and files copied to the clipboard, commonly used in data exfiltration scenarios and credential theft.

### Input Device Registration (Raw Input)
Shows how to use `RegisterRawInputDevices` and `GetRawInputData` to capture low-level mouse and keyboard input system-wide. Includes hotkey registration with `RegisterHotKey` for global keyboard shortcuts and statistics tracking.

### Keylogging & Keystroke Capture
Implements various keystroke capture techniques including keyboard hooks, message filtering, and input interception. Demonstrates virtual key code mapping, scan code extraction, and modifier key state tracking for comprehensive keyboard monitoring.

### Message Interception Intelligence
Leverages Windows message queue interception using `WH_GETMESSAGE` hooks and message filtering. Captures keyboard messages (WM_KEYDOWN, WM_CHAR), mouse events, and application-specific messages for behavioral analysis.

### Screen Capture & Visual Surveillance
Provides examples of screen capture using GDI and Desktop Duplication API. Demonstrates bitmap manipulation, screenshot saving, and real-time screen recording capabilities commonly used in remote access tools and surveillance software.

### Window & Focus Tracking
Covers active window monitoring with `GetForegroundWindow`, thread input attachment using `AttachThreadInput`, and focus change detection. Includes statistical analysis of application usage patterns and window switching behavior.

### Window Hooking System-Wide Monitoring
Demonstrates comprehensive system-wide event monitoring using `SetWindowsHookEx` for keyboard hooks (WH_KEYBOARD_LL), message hooks (WH_GETMESSAGE), and accessibility event hooks (`SetWinEventHook`). Captures all user interactions across the entire system.

## Surveillance Considerations

- **Legacy vs. Modern APIs**: Examples include both legacy **User32 hooks** and modern **Raw Input API**. Different techniques offer varying levels of access, stealth, and system integration.
- **Detection Avoidance**: These examples use well-known API patterns that are monitored by EDR solutions. Real-world spyware employs obfuscation, driver-level hooks, and anti-analysis techniques.
- **Privilege Requirements**: Some techniques require administrative privileges or specific access rights (e.g., `SE_DEBUG_NAME` for process information).
- **Data Exfiltration**: Surveillance data must be stored or transmitted. Consider encryption, compression, and covert channels in real implementations.
- **Performance Impact**: System-wide hooks and high-frequency monitoring can impact system performance. Optimize callback routines and implement intelligent filtering.

## Technical Capabilities

### Input Monitoring
- **Keyboard**: Virtual key codes, scan codes, modifier states, key press/release events
- **Mouse**: Button clicks, wheel scrolling, movement tracking (relative and absolute)
- **Clipboard**: Text, images, files, custom formats
- **Raw Input**: Unprocessed device data, multiple device discrimination.

### Window & Application Tracking
- **Active Window**: Title, class name, process ID, thread ID
- **Focus Changes**: Timing, duration, switching patterns
- **Window Events**: Creation, destruction, show/hide, minimize/maximize
- **Thread Input**: Shared input queue state, focus management

### System-Wide Monitoring
- **Hook Types**: WH_KEYBOARD_LL, WH_GETMESSAGE, SetWinEventHook
- **Event Scope**: System-wide or thread-specific
- **Accessibility Events**: Window lifecycle, focus changes, UI state
- **Message Interception**: Posted messages, queued messages, sent messages

## Limitations

- **Windows-Centric**: All code is based on the Windows API and is not portable to other operating systems.
- **Detection**: The API call patterns demonstrated are well-known signatures monitored by modern security solutions.
- **Educational Purpose**: These are simplified demonstrations and lack the robustness, stealth techniques, and error handling of production-grade spyware.
- **Ethical Constraints**: Surveillance techniques must only be used for legitimate purposes (debugging, accessibility, parental controls, security research) with proper authorization.
- **Completeness**: This collection demonstrates individual surveillance components but does not represent a fully functional spyware package.

## Security & Privacy Implications

### Capabilities
- **Credential Theft**: Capture passwords and sensitive input
- **Data Exfiltration**: Monitor clipboard for confidential information
- **User Profiling**: Track application usage and behavior patterns
- **Screenshot Capture**: Visual surveillance of user activities
- **System-Wide Monitoring**: Comprehensive input and window event tracking.

### Countermeasures
- **Endpoint Detection**: EDR solutions monitor hook installation and suspicious API patterns
- **Behavioral Analysis**: Anomaly detection for unusual input monitoring
- **Privilege Restrictions**: Run applications with minimal required permissions.
- **User Awareness**: Security training to recognize surveillance indicators

## Responsible Use

These techniques are demonstrated for:
- **Security Research**: Understanding threat vectors and attack techniques.
- **Malware Analysis**: Reverse engineering spyware capabilities.

**Never use these techniques for unauthorized surveillance, data theft, or privacy violations.**

## References

- [Windows Hooks Overview](https://learn.microsoft.com/en-us/windows/win32/winmsg/about-hooks)
- [Raw Input API](https://learn.microsoft.com/en-us/windows/win32/inputdev/about-raw-input)
- [Clipboard Monitoring](https://learn.microsoft.com/en-us/windows/win32/dataxchg/clipboard-notifications)
- [Window Management](https://learn.microsoft.com/en-us/windows/win32/winmsg/windowing)
- [Accessibility Events](https://learn.microsoft.com/en-us/windows/win32/winauto/event-constants)
- [Screen Capture Techniques](https://learn.microsoft.com/en-us/windows/win32/gdi/capturing-an-image)
- [Windows Security Best Practices](https://learn.microsoft.com/en-us/windows/security/)
