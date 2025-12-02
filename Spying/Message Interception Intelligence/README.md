<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Message%20Interception%20Intelligence/Message_Interception.c">Windows Message Queue APIs - Comprehensive Interception & Intelligence Demonstration</a>
</h1>

## Overview
This program demonstrates **legitimate use of Windows Message Queue APIs** for inter-process communication, event handling, and message traffic analysis. These techniques are widely used in real-world Windows applications for:

- GUI application event handling and message loop processing
- Inter-thread communication and synchronization
- Asynchronous vs synchronous message delivery patterns
- Message queue monitoring and traffic analysis
- Custom message protocol implementation
- Thread-safe communication between UI and worker threads
- Educational demonstrations of Windows messaging architecture

This is the foundation of event-driven programming in Windows applications including GUI frameworks, system utilities, and communication protocols.

---
## Core APIs Demonstrated
| API                      | Purpose                                      | Use Case                                      |
|--------------------------|----------------------------------------------|-----------------------------------------------|
| `GetMessageA`            | Blocking message retrieval from queue        | Main event loop, message processing           |
| `PeekMessageA`           | Non-blocking message queue checking          | Game loops, background processing             |
| `PostMessageA`           | Asynchronous message posting (queued)        | Non-blocking notifications, UI updates        |
| `SendMessageA`           | Synchronous message sending (immediate)      | Direct window communication, data queries     |
| `PostThreadMessageA`     | Direct thread-to-thread messaging            | Worker thread communication                   |
| `SendMessageCallbackA`   | Async send with callback notification        | Long operations with completion handlers      |
| `SendMessageTimeoutA`    | Timed synchronous send with timeout          | Prevent hangs from unresponsive windows       |
| `SendNotifyMessageA`     | Non-blocking send for same thread            | Fast notifications without waiting            |
| `DispatchMessageA`       | Route message to window procedure            | Message delivery to WindowProc                |
| `TranslateMessage`       | Convert keystrokes to character messages     | Keyboard input processing                     |

---
## 1. [GetMessageA - Blocking Message Retrieval](https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Message%20Interception%20Intelligence/Message_Interception.c#L108)
**Description:** Demonstrates blocking message loop - the foundation of Windows GUI applications.  
**Key Features:**
- Blocks execution until message arrives in queue
- Returns > 0 for normal messages, 0 for WM_QUIT, -1 for errors
- Removes messages from queue automatically
- Processes messages sequentially in order received

**Message Flow:**
1. `PostMessageA()` queues messages to window
2. `GetMessageA()` blocks until message available
3. Message intercepted and logged
4. `TranslateMessage()` processes keyboard input
5. `DispatchMessageA()` sends to WindowProc

**Use Cases:**
- Traditional GUI application main loops
- Message-driven applications
- Event handlers that can wait for input
- Synchronous message processing

---
## 2. [PeekMessageA - Non-Blocking Message Checking](https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Message%20Interception%20Intelligence/Message_Interception.c#L151)
**Description:** Non-blocking message queue inspection allowing continuous processing.  
**Key Features:**
- Returns immediately with TRUE/FALSE (message available)
- `PM_REMOVE` flag removes message from queue
- `PM_NOREMOVE` flag peeks without removing
- Enables work between message checks

**Flags:**
- `PM_REMOVE` → Remove message from queue
- `PM_NOREMOVE` → Check without removing
- `PM_NOYIELD` → Prevent thread yield during peek

**Use Cases:**
- Game loops requiring continuous rendering
- Real-time processing applications
- Background tasks with event monitoring
- Performance-critical message handling

---
## 3. [PostMessageA vs SendMessageA - Async vs Sync Comparison](https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Message%20Interception%20Intelligence/Message_Interception.c#L200)
**Description:** Direct comparison of asynchronous and synchronous message delivery patterns.  

**PostMessageA (Asynchronous):**
- Returns immediately after queuing
- Does not wait for processing
- No return value from WindowProc
- Thread-safe, non-blocking
- Messages processed in queue order

**SendMessageA (Synchronous):**
- Blocks until WindowProc returns
- Receives return value from handler
- Direct execution, bypasses queue
- Can cause deadlocks if misused
- Immediate processing guaranteed

**Use Cases:**
- **PostMessageA**: UI updates, notifications, non-critical events
- **SendMessageA**: Data queries, synchronous commands, window state changes

---
## 4. [PostThreadMessageA - Thread-to-Thread Messaging](https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Message%20Interception%20Intelligence/Message_Interception.c#L237)
**Description:** Direct messaging between threads without window handles.  
**Key Features:**
- Messages sent directly to thread message queue
- No window handle required
- Thread must have message queue initialized
- Use `PeekMessageA` with `PM_NOREMOVE` to create queue

**Thread Message Queue:**
```c
// Force queue creation in target thread
MSG msg;
PeekMessageA(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
```

**Use Cases:**
- Worker thread communication
- Thread synchronization via messages
- Custom inter-thread protocols
- Background task coordination

---
## 5. [SendMessageCallbackA - Asynchronous Send with Callback](https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Message%20Interception%20Intelligence/Message_Interception.c#L261)
**Description:** Asynchronous message sending with completion notification via callback.  
**Key Features:**
- Non-blocking send operation
- Callback executed when message processed
- Can pass custom data to callback (`ULONG_PTR` parameter)
- Callback receives WindowProc return value

**Callback Signature:**
```c
void CALLBACK MessageCallback(
    HWND hwnd,           // Target window
    UINT msg,            // Message type
    ULONG_PTR data,      // Custom data
    LRESULT result       // WindowProc return value
);
```

**Use Cases:**
- Long-running operations with completion handlers
- Async request-response patterns
- Non-blocking RPC-style communication
- Event-driven architectures

---
## 6. [SendMessageTimeoutA - Timed Synchronous Send](https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Message%20Interception%20Intelligence/Message_Interception.c#L332)
**Description:** Synchronous message with timeout protection to prevent indefinite blocking.  
**Key Features:**
- Timeout specified in milliseconds
- Returns 0 if timeout expires
- `GetLastError()` returns `ERROR_TIMEOUT` on timeout
- Prevents hangs from unresponsive windows

**Flags:**
- `SMTO_NORMAL` → Return if timeout expires
- `SMTO_ABORTIFHUNG` → Return if window hung
- `SMTO_BLOCK` → Prevent timeout during processing
- `SMTO_NOTIMEOUTIFNOTHUNG` → Wait if not hung

**Use Cases:**
- Cross-process window communication
- Defensive programming against hangs
- System utilities querying window states
- Network communication with timeout guarantees

---
## 7. [SendNotifyMessageA - Fast Non-Blocking Send](https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Message%20Interception%20Intelligence/Message_Interception.c#L374)
**Description:** Non-blocking send for same-thread or cross-thread communication.  
**Key Features:**
- Same thread: behaves like `SendMessageA`
- Different thread: behaves like `PostMessageA`
- No return value from WindowProc
- Faster than `SendMessageA` for cross-thread

**Behavior:**
- **Same thread** → Direct call to WindowProc
- **Different thread** → Message queued

**Use Cases:**
- Fire-and-forget notifications
- One-way communication patterns
- Performance-critical messaging
- Event broadcasting

---
## 8. [Message Intelligence & Traffic Analysis](https://github.com/CyberForgeEx/MALFORGE/blob/main/Spying/Message%20Interception%20Intelligence/Message_Interception.c#L400)
**Description:** Comprehensive message traffic monitoring and analysis demonstration.  
**Key Features:**
- Intercepts all messages in queue
- Categorizes system vs custom messages
- Measures processing time and throughput
- Analyzes message patterns and frequencies

**Metrics Collected:**
- Total message count
- Message type distribution
- Processing time per message
- Average throughput (messages/second)
- Custom vs system message ratio

**Analysis Output:**
```
[*] Traffic Analysis Status
[+] Total messages intercepted: 13
[+] Custom commands: 10
[+] System messages: 3
[+] Processing time: 15 ms
[+] Average time per message: 1.15 ms
```

**Use Cases:**
- Performance profiling and optimization
- Message queue debugging
- Event-driven system monitoring
- Application behavior analysis

---
## Program Architecture

### Window Procedure (Message Handler)
```c
LRESULT CALLBACK DemoWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
```
- Receives all messages via `DispatchMessageA()`
- Handles custom messages (`WM_CUSTOM_COMMAND`, `WM_CUSTOM_DATA`)
- Returns values to `SendMessageA()` callers
- Implements application-specific logic

### Custom Messages
```c
#define WM_CUSTOM_COMMAND (WM_USER + 1)    // Custom command message
#define WM_CUSTOM_DATA    (WM_USER + 2)    // Custom data transfer
#define WM_STOP_DEMO      (WM_USER + 100)  // Stop demonstration
```

### Message Loop Pattern
```c
MSG msg;
while (GetMessageA(&msg, NULL, 0, 0) > 0) {
    TranslateMessage(&msg);  // Keyboard processing
    DispatchMessageA(&msg);  // Route to WindowProc
}
```

---
## Compilation Instructions
### Visual Studio / cl.exe
```bash
cl.exe Message_Interception.c /link user32.lib
```

### MinGW / GCC
```bash
gcc Message_Interception.c -o message_demo.exe -luser32 -mwindows
```

### Run (No admin rights required)
```cmd
message_demo.exe
```

---
## Required Libraries
- `user32.lib` – Window management and message queue functions
- `windows.h` – Core Windows API definitions.

---
## System Requirements
- **OS:** Windows XP or later (Windows 7/8/10/11 fully supported)
- **Architecture:** x86 or x64
- **Permissions:** Standard user rights (no elevation required)
- **Console:** Standard Windows Console or Terminal application.

---
## Program Flow
The demonstration runs 8 sequential demonstrations with user control:
1. **GetMessageA** - Blocking message loop demonstration
2. **PeekMessageA** - Non-blocking queue checking
3. **Post vs Send** - Async vs sync comparison
4. **PostThreadMessageA** - Thread messaging
5. **SendMessageCallbackA** - Callback-based async send
6. **SendMessageTimeoutA** - Timeout-protected send
7. **SendNotifyMessageA** - Fast non-blocking send
8. **Message Intelligence** - Traffic analysis

Press Enter between demonstrations to proceed at your own pace.

---
## Output Examples

### GetMessageA Demo
```
[+] GetMessageA (Blocking Retrieval)
[+] Sending 3 custom messages...
[+] Starting message loop with GetMessageA...

[Msg #1] Intercepted: WM_CUSTOM_COMMAND (0x0401) - wParam=1, lParam=100
  [WindowProc] Received WM_CUSTOM_COMMAND: wParam=1, lParam=100
[Msg #2] Intercepted: WM_CUSTOM_COMMAND (0x0401) - wParam=2, lParam=200
  [WindowProc] Received WM_CUSTOM_COMMAND: wParam=2, lParam=200
```

### PostMessageA vs SendMessageA
```
[1] PostMessageA (Asynchronous):
   Posting message...
   PostMessageA returned immediately: Success
   (Message queued, not processed yet)

[2] SendMessageA (Synchronous):
   Sending message...
  [WindowProc] Received WM_CUSTOM_COMMAND: wParam=20, lParam=2000
   SendMessageA returned: 100 (message processed immediately)
[+] Return value came from WindowProc
```

### Message Intelligence Analysis
```
[*] Traffic Analysis Status
[+] Total messages intercepted: 13
[+] Custom commands: 10
[+] System messages: 3
[+] Processing time: 15 ms
[+] Average time per message: 1.15 ms
```

---
## Key Concepts

### Message Types
- **System Messages** (`WM_*`) - Predefined Windows messages (0x0000 - 0x03FF)
- **User Messages** (`WM_USER + n`) - Custom application messages (0x0400+)
- **Registered Messages** - System-wide unique messages via `RegisterWindowMessageA()`


### Blocking vs Non-Blocking
| API                | Blocking | Queue | Return Value |
|--------------------|----------|-------|--------------|
| GetMessageA        | Yes      | Yes   | None         |
| PeekMessageA       | No       | Yes   | None         |
| SendMessageA       | Yes      | No    | LRESULT      |
| PostMessageA       | No       | Yes   | None         |
| SendMessageTimeoutA| Yes*     | No    | LRESULT      |

*With timeout protection

---
## Message Queue Behavior

### Posted Messages (Queued)
- Added to thread's message queue
- Processed by `GetMessageA()` or `PeekMessageA()`
- FIFO order (First In, First Out)
- Asynchronous, non-blocking

### Sent Messages (Direct)
- Bypass message queue
- Call WindowProc directly
- Synchronous, blocking
- Return value available

---
## Error Handling
The program includes comprehensive error handling for:
- Window creation failures with `GetLastError()` codes
- Message posting failures (invalid handles)
- Thread creation errors
- Callback registration failures
- Timeout detection and reporting
- Queue state validation.

---
## Common Pitfalls
- **Forgetting to pump messages** in long-running operations (causes UI freeze)
- **SendMessageA deadlocks** when both threads send synchronously to each other
- **Missing queue initialization** before PostThreadMessageA (messages lost)
- **Not checking GetMessageA return** for -1 (error condition)
- **Ignoring WM_QUIT** in custom message loops (process won't exit cleanly)

---
## API References
- [GetMessageA](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getmessagea)
- [PeekMessageA](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-peekmessagea)
- [PostMessageA](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-postmessagea)
- [SendMessageA](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-sendmessagea)
- [PostThreadMessageA](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-postthreadmessagea)
- [SendMessageCallbackA](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-sendmessagecallbacka)
- [SendMessageTimeoutA](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-sendmessagetimeouta)
- [SendNotifyMessageA](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-sendnotifymessagea)
- [DispatchMessageA](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-dispatchmessagea)

---
<p align="right">
  Last Updated: December 2025
</p>
