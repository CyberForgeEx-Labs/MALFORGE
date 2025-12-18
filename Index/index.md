<div align="center">
<h1> MALFORGE – The Magic Windows Treasure Index </h1> 
</div>

### Common words explained

| Technical Term                  | One-Liner Explanation                                                                 |
|---------------------------------|----------------------------------------------------------------------------------------------------------|
| **Kernel**                      | The core part of Windows that directly controls the hardware and all critical resources                 |
| **User Mode vs Kernel Mode**    | User mode = apps run safely with limited powers; Kernel mode = full control over the entire system     |
| **Processes & Threads**         | Process = a running program with its own memory; Thread = the actual worker that executes code         |
| **Virtual Memory**              | Each process gets its own private “pretend” memory space so programs don’t interfere with each other   |
| **Page Fault**                  | When a program accesses memory that isn’t in RAM yet, Windows quickly loads it from disk               |
| **Handles**                     | Secure references (like tickets) that let a process safely access files, registry keys, etc.           |
| **Registry**                    | Windows’ central database where all configuration settings and options are stored                     |
| **IRQL (Interrupt Request Level)** | Priority level that decides which code can interrupt what  higher IRQL blocks everything below it   |
| **Driver**                      | Small program that lets Windows communicate with hardware (GPU, USB, network card, etc.)              |
| **Object Manager**              | The kernel component that creates, tracks, and secures all system objects (files, threads, etc.)      |
| **EPROCESS & ETHREAD**          | Kernel structures that hold all info about a process (EPROCESS) and each of its threads (ETHREAD)    |
| **PEB / TEB**                   | PEB = process-wide info block; TEB = per-thread info block (both visible only to the process itself) |
| **Windows Subsystem (CSRSS)**   | System process that manages console windows, thread creation, and shutdown (still needed in Win10/11)|
| **Win32k.sys**                  | Kernel-mode driver that handles GUI drawing, windows, mouse/keyboard input                           |
| **NTDLL**                       | Core user-mode DLL that translates Win32 API calls into native system calls (Nt/Zw functions)        |
| **Sysinternals Tools**         | Free Microsoft tools (ProcExp, ProcMon, etc.) that let you see exactly what Windows is doing          |
| **WinDbg / x64dbg**             | Powerful debuggers to pause, inspect, and step through code at the kernel or user level              |
| **Pool Memory**                 | Kernel’s own fast heap (paged or non-paged) used for drivers and system structures                    |
| **Executive vs Kernel**         | Executive = high-level kernel services (I/O, memory, objects); Kernel = low-level CPU/scheduler code |
| **APC (Asynchronous Procedure Call)** | Way to inject a function call into a thread and run it the next time the thread is alertable     |
| **DPC (Deferred Procedure Call)** | Short kernel routine that runs at lower priority after a hardware interrupt finishes               |
| **Interrupt**                   | Hardware signal that instantly forces the CPU to stop and run a specific handler                       |
| **Trap / Exception**            | Unexpected event (divide-by-zero, access violation, etc.) that transfers control to a handler        |
| **Section Object**              | Kernel object that represents a memory-mapped file lets multiple processes share the same memory  |

---

### Anti-Debugging

| Technical Term                     | One-Liner Explanation                                                                                          |
| ---------------------------------- | -------------------------------------------------------------------------------------------------------------- |
| **OutputDebugStringA**             | Sends a message meant for a debugger if nothing receives it, the program can detect no debugger is attached. |
| **SleepEx**                        | Makes the program “sleep,” and debuggers slow this down, so timing differences help detect debugging.          |
| **IsDebuggerPresent**              | Directly checks if the current process is being debugged.                                                      |
| **CheckRemoteDebuggerPresent**     | Checks if another debugger is attached to the process.                                                         |
| **NtQueryInformationProcess**      | Low-level Windows call used to ask the OS if a debugger is present.                                            |
| **GetLogicalProcessorInformation** | Reads CPU info debuggers/VMs often report unusual values that give away analysis environments.               |
| **CreateToolhelp32Snapshot**       | Can list processes and look for known debuggers running.                                                       |
| **FindWindowA**                    | Searches for debugger windows (like OllyDbg or x64dbg) by their window names.                                  |
| **ExitWindowsEx**                  | Malicious programs abuse this to shut down or log off the system if a debugger is detected.                    |
| **CountClipboardFormats**          | Checks clipboard format count debuggers can affect this value, revealing their presence.                     |
| **GetTickCount64**                 | Measures time passed; if stepping slows execution, it signals debugging.                                       |
| **QueryPerformanceCounter**        | High-precision timer used to catch execution slowdowns caused by breakpoints.                                  |


---

### Enumeration

---

#### File System Enumeration

| Technical Term           | One-Liner Explanation                                                   |
| ------------------------ | ----------------------------------------------------------------------- |
| **FindFirstFileA**       | Starts searching a directory for files/folders matching a pattern.      |
| **FindNextFileA**        | Continues the directory search started with FindFirstFileA.             |
| **GetWindowsDirectoryA** | Returns the path to the Windows installation folder.                    |
| **GetSystemDirectoryA**  | Returns the path to the main system folder (System32).                  |
| **PathFileExistsA**      | Checks if a file or folder actually exists.                             |
| **GetLogicalDrives**     | Lists all drive letters currently available (C:, D:, etc.).             |
| **GetDriveTypeA**        | Tells what type of drive a path belongs to (HDD, USB, CD-ROM, network). |

#### Local Time Enumeration

| Technical Term              | One-Liner Explanation                                            |
| --------------------------- | ---------------------------------------------------------------- |
| **GetSystemTime**           | Gets the current UTC date and time.                              |
| **GetSystemTimeAsFileTime** | Gets the current time in Windows FILETIME 64-bit format.         |
| **GetThreadLocale**         | Returns the language/region settings used by the current thread. |

#### Memory / File Query

| Technical Term         | One-Liner Explanation                                        |
| ---------------------- | ------------------------------------------------------------ |
| **ReadProcessMemory**  | Reads memory from another process.                           |
| **ReadFile**           | Reads data from a file on disk.                              |
| **VirtualQueryEx**     | Retrieves memory region info from another process.           |
| **GetFileAttributesA** | Gets file or directory properties (hidden, read-only, etc.). |
| **SearchPathA**        | Searches system paths to find the full path of a file.       |

#### Module / Driver Enumeration

| Technical Term                   | One-Liner Explanation                                |
| -------------------------------- | ---------------------------------------------------- |
| **EnumDeviceDrivers**            | Lists all loaded kernel drivers.                     |
| **EnumProcessModulesEx**         | Lists modules (DLLs) loaded by a process.            |
| **Module32First / Module32Next** | Walks through a process’s loaded modules one by one. |
| **GetModuleBaseNameA**           | Gets a module’s filename (e.g., “kernel32.dll”).     |
| **GetModuleInformation**         | Retrieves memory and size details about a module.    |
| **GetDeviceDriverBaseNameA**     | Gets the name of a loaded device driver.             |

#### Network Enumeration

| Technical Term          | One-Liner Explanation                                   |
| ----------------------- | ------------------------------------------------------- |
| **NetShareEnum**        | Lists shared folders on a target system.                |
| **NetShareGetInfo**     | Retrieves detailed info about a specific network share. |
| **NetShareCheck**       | Checks if a path is a valid network share.              |
| **WNetEnumResource**    | Enumerates network resources (shares, servers, etc.).   |
| **WNetAddConnection2A** | Connects to a network share using credentials.          |
| **GetAdaptersInfo**     | Lists local network adapters and their settings.        |
| **GetIpNetTable**       | Gets the system’s ARP table (IP ↔ MAC mapping).         |

#### Process Enumeration

| Technical Term           | One-Liner Explanation                                       |
| ------------------------ | ----------------------------------------------------------- |
| **EnumProcesses**        | Lists all running process IDs.                              |
| **OpenProcess**          | Opens a handle to another process for inspection.           |
| **GetModuleFileNameEx**  | Gets the full path of a module in a target process.         |
| **GetProcessMemoryInfo** | Retrieves memory usage stats for a process.                 |
| **GetProcessTimes**      | Provides CPU and runtime timing info for a process.         |
| **GetPriorityClass**     | Retrieves the scheduling priority of a process.             |
| **EnumProcessModules**   | Lists modules loaded in a process.                          |
| **GetExitCodeProcess**   | Checks whether a process is still running or its exit code. |

#### Registry Enumeration

| Technical Term              | One-Liner Explanation                                         |
| --------------------------- | ------------------------------------------------------------- |
| **RegEnumKeyExA**           | Lists subkeys within a registry key.                          |
| **RegEnumValueA**           | Lists values stored in a registry key.                        |
| **RegEnumKeyA**             | Another API for enumerating subkeys in a registry key.        |
| **RegQueryInfoKeyA**        | Retrieves metadata about a registry key (counts, timestamps). |
| **RegQueryMultipleValuesA** | Reads multiple registry values in one call.                   |
| **RegQueryValueExA**        | Gets the data of a specific registry entry.                   |
| **GetCurrentHwProfileA**    | Returns hardware profile info like machine GUID/name.         |

#### Resource Enumeration

| Technical Term              | One-Liner Explanation                                      |
| --------------------------- | ---------------------------------------------------------- |
| **EnumResourceTypesA**      | Lists resource types stored inside an executable.          |
| **EnumResourceTypesExA**    | Advanced version that lists resource types with filtering. |
| **FindFirstUrlCacheEntryA** | Starts enumerating browser cache entries.                  |
| **FindNextUrlCacheEntryA**  | Continues browser cache enumeration.                       |

#### System Info Enumeration

| Technical Term             | One-Liner Explanation                                                        |
| -------------------------- | ---------------------------------------------------------------------------- |
| **GetNativeSystemInfo**    | Provides CPU architecture and general system info.                           |
| **GetSystemDefaultLangID** | Returns the system’s default language setting.                               |
| **GetVersionExA**          | Retrieves Windows version info (deprecated but used).                        |
| **RtlGetVersion**          | Internal API to get accurate Windows version info.                           |
| **IsWoW64Process**         | Checks if a process is running under 32-bit compatibility on 64-bit Windows. |
| **GetDriveTypeA**          | Identifies the type of a drive (HDD/USB/CD/network).                         |

#### Thread Enumeration

| Technical Term                   | One-Liner Explanation                                    |
| -------------------------------- | -------------------------------------------------------- |
| **Thread32First / Thread32Next** | Walks through all threads in the system or a process.    |
| **GetProcessIdOfThread**         | Gets the process ID of a given thread.                   |
| **GetCurrentThread**             | Returns a pseudo-handle for the calling thread.          |
| **GetCurrentThreadId**           | Gets the thread ID of the current thread.                |
| **GetThreadId**                  | Retrieves the ID of any thread via its handle.           |
| **GetThreadInformation**         | Gets detailed info about a thread (state, timing, etc.). |

#### User / Account Enumeration

| Technical Term            | One-Liner Explanation                                 |
| ------------------------- | ----------------------------------------------------- |
| **GetUserNameA**          | Retrieves the currently logged-in username.           |
| **GetComputerNameA**      | Gets the system’s computer name.                      |
| **LookupAccountNameA**    | Converts a username to its SID (security identifier). |
| **LookupPrivilegeValueA** | Gets the numeric value of a Windows privilege.        |

#### Window / Desktop Enumeration

| Technical Term               | One-Liner Explanation                                 |
| ---------------------------- | ----------------------------------------------------- |
| **EnumWindows**              | Enumerates all top-level windows on the screen.       |
| **EnumDesktopWindows**       | Lists windows belonging to a specific desktop.        |
| **GetWindowText**            | Retrieves the title text of a window.                 |
| **GetClassName**             | Gets the class name of a window (type of UI element). |
| **GetWindowThreadProcessId** | Retrieves which thread and process own a window.      |
| **IsWindowVisible**          | Checks if a window is visible on the screen.          |
| **IsWindowEnabled**          | Checks if a window accepts user input.                |
| **GetWindowRect**            | Gets the position and size of a window.               |

---

### Injection

#### APC Injection

| Technical Term          | One-Liner Explanation                                                          |
| ----------------------- | ------------------------------------------------------------------------------ |
| **QueueUserAPC**        | Queues a function to run in a target thread when it enters an alertable state. |
| **NtQueueApcThread**    | Native API that inserts an APC directly into a specific thread.                |
| **NtQueueApcThreadEx2** | Extended APC queuing call that offers more control over APC delivery.          |
| **OpenThread**          | Opens a handle to an existing thread so it can be inspected or manipulated.    |


#### Classic DLL Injection

| Technical Term           | One-Liner Explanation                                           |
| ------------------------ | --------------------------------------------------------------- |
| **CreateRemoteThreadEx** | Creates a thread inside another process to execute code.        |
| **LoadLibraryExA**       | Loads a DLL into a process and resolves its dependencies.       |
| **GetModuleHandleA**     | Retrieves a handle to a loaded module without loading it again. |
| **GetProcAddress**       | Gets the address of an exported function from a loaded DLL.     |
| **WriteProcessMemory**   | Writes data into the memory of another process.                 |
| **VirtualAllocEx**       | Allocates memory inside a remote process.                       |
| **OpenProcess**          | Opens a handle to another process with specified access rights. |
| **OpenThread**           | Opens a handle to a thread for further operations.              |
| **NtCreateThreadEx**     | Low-level API to create a thread in another process.            |
| **LdrLoadDll**           | Internal loader routine that loads a DLL into a process.        |

#### Heap Manipulation

| Technical Term      | One-Liner Explanation                                 |
| ------------------- | ----------------------------------------------------- |
| **HeapCreate**      | Creates a private heap for dynamic memory allocation. |
| **HeapAlloc**       | Allocates a block of memory from a heap.              |
| **HeapReAlloc**     | Resizes an existing heap memory block.                |
| **GetProcessHeap**  | Returns a handle to the default process heap.         |
| **GetProcessHeaps** | Lists all heaps currently allocated by a process.     |
| **RtlCreateHeap**   | Native API used to create a heap structure directly.  |
| **GlobalAlloc**     | Legacy function that allocates global memory blocks.  |


#### Memory Copy Operations

| Technical Term         | One-Liner Explanation                                  |
| ---------------------- | ------------------------------------------------------ |
| **RtlMoveMemory**      | Copies memory safely between overlapping regions.      |
| **RtlCopyMemory**      | Copies a block of memory from one location to another. |
| **WriteProcessMemory** | Copies data into another process’s memory space.       |
| **ReadProcessMemory**  | Reads memory from another process’s address space.     |


#### Memory Manipulation

| Technical Term              | One-Liner Explanation                                         |
| --------------------------- | ------------------------------------------------------------- |
| **VirtualAllocEx**          | Allocates virtual memory in a remote process.                 |
| **VirtualProtectEx**        | Changes memory protection flags in another process.           |
| **VirtualAllocExNuma**      | Allocates memory on a specific NUMA node.                     |
| **NtAllocateVirtualMemory** | Native API to reserve or commit virtual memory.               |
| **NtProtectVirtualMemory**  | Native call to modify memory protection attributes.           |
| **NtReadVirtualMemoryEx**   | Reads memory from a process using extended native parameters. |
| **NtWriteVirtualMemory**    | Writes data into a process’s virtual memory via native API.   |

#### Object Duplication

| Technical Term        | One-Liner Explanation                              |
| --------------------- | -------------------------------------------------- |
| **NtDuplicateObject** | Duplicates a handle from one process into another. |

### Process Management

| Technical Term           | One-Liner Explanation                                        |
| ------------------------ | ------------------------------------------------------------ |
| **CreateProcessA**       | Creates a new process and its primary thread.                |
| **GetThreadContext**     | Retrieves the CPU register state of a suspended thread.      |
| **SetThreadContext**     | Modifies the CPU register state of a suspended thread.       |
| **SuspendThread**        | Pauses execution of a specified thread.                      |
| **ResumeThread**         | Resumes execution of a previously suspended thread.          |
| **NtUnmapViewOfSection** | Unmaps a section (memory view) from a process address space. |
| **NtResumeThread**       | Native API to resume a suspended thread.                     |
| **NtSuspendProcess**     | Suspends all threads within a target process.                |

### Section Mapping

| Technical Term         | One-Liner Explanation                                         |
| ---------------------- | ------------------------------------------------------------- |
| **CreateFileMappingA** | Creates a memory-backed or file-backed shared memory section. |
| **MapViewOfFileEx**    | Maps a section into a process at a specified address.         |
| **OpenFileMappingA**   | Opens an existing named file mapping object.                  |
| **NtCreateSection**    | Native API to create a section object for shared memory.      |
| **NtMapViewOfSection** | Maps a section into a process using low-level NT APIs.        |

### Specialized Techniques

| Technical Term                  | One-Liner Explanation                                              |
| ------------------------------- | ------------------------------------------------------------------ |
| **SetPropA**                    | Attaches custom data to a window object.                           |
| **SetProcessDEPPolicy**         | Configures Data Execution Prevention (DEP) behavior for a process. |
| **EnumSystemLocalesA**          | Enumerates system locale identifiers installed on the OS.          |
| **UuidFromStringA**             | Converts a UUID string into its binary representation.             |
| **DebugActiveProcessStop**      | Detaches a debugger from a target process.                         |
| **LocalAlloc**                  | Allocates memory from the local heap.                              |
| **Toolhelp32ReadProcessMemory** | Reads another process’s memory using Toolhelp snapshot APIs.       |


### Synchronization

| Technical Term               | One-Liner Explanation                                           |
| ---------------------------- | --------------------------------------------------------------- |
| **WaitForSingleObjectEx**    | Waits for a single object with optional alertable behavior.     |
| **WaitForMultipleObjectsEx** | Waits for one or more objects with alertable execution support. |
| **NtWaitForMultipleObjects** | Native API to wait on multiple kernel objects.                  |
| **NtWaitForSingleObject**    | Native API to wait for a single kernel object.                  |

### Token Security Manipulation

| Technical Term            | One-Liner Explanation                                              |
| ------------------------- | ------------------------------------------------------------------ |
| **OpenProcessToken**      | Opens the access token associated with a process.                  |
| **AdjustTokenPrivileges** | Enables or disables privileges within an access token.             |
| **DuplicateToken**        | Creates a duplicate access token from an existing one.             |
| **GetTokenInformation**   | Retrieves security details stored in an access token.              |
| **LookupPrivilegeValue**  | Resolves a privilege name to its locally unique identifier (LUID). |
| **LookupPrivilegeName**   | Converts a privilege LUID back into its human-readable name.       |

---

### Evasion

#### Data Protection

| Technical Term                 | One-Liner Explanation                                                            |
| ------------------------------ | -------------------------------------------------------------------------------- |
| **CryptProtectData**           | Encrypts data using Windows DPAPI tied to the current user or system.            |
| **CryptUnprotectData**         | Decrypts data previously protected with DPAPI.                                   |
| **UuidFromStringA**            | Converts a UUID string into binary data, often used for encoded payload storage. |
| **UuidCreate + UuidToStringA** | Generates UUIDs and converts them to strings to disguise or serialize data.      |

#### Dynamic Code Loading

| Technical Term      | One-Liner Explanation                                           |
| ------------------- | --------------------------------------------------------------- |
| **GetModuleHandle** | Retrieves a handle to a loaded module without loading it again. |
| **FindResource**    | Locates an embedded resource inside a module.                   |
| **LoadResource**    | Loads a resource into memory for access.                        |
| **SizeofResource**  | Returns the size of an embedded resource.                       |
| **LockResource**    | Provides a pointer to the loaded resource data in memory.       |

#### Environment Manipulation

| Technical Term              | One-Liner Explanation                                                   |
| --------------------------- | ----------------------------------------------------------------------- |
| **GetEnvironmentVariableA** | Reads environment variable values from the process.                     |
| **SetEnvironmentVariableA** | Modifies or creates environment variables at runtime.                   |
| **EnumSystemLocalesA**      | Enumerates available system locales, often used for environment checks. |
| **GetLocaleInfoA**          | Retrieves locale-specific configuration information.                    |
| **GetSystemDefaultLCID**    | Returns the system’s default locale identifier.                         |

#### Anti-Forensic File Manipulation

| Technical Term                              | One-Liner Explanation                                     |
| ------------------------------------------- | --------------------------------------------------------- |
| **CreateFile**                              | Creates or opens a file or device handle.                 |
| **WriteFile**                               | Writes raw data directly to a file.                       |
| **GetFileTime / SetFileTime**               | Reads or modifies file timestamps to hide activity.       |
| **GetFileAttributesA / SetFileAttributesA** | Reads or changes file attributes like hidden or system.   |
| **CreateFileMappingA**                      | Maps a file into memory for stealthy access.              |
| **MapViewOfFile**                           | Maps a file-mapped object into the process address space. |
| **DeleteFileA**                             | Removes a file from disk to erase artifacts.              |

#### Time-Based Evasion

| Technical Term            | One-Liner Explanation                                                    |
| ------------------------- | ------------------------------------------------------------------------ |
| **NtDelayExecution**      | Suspends execution for a specified time at the native API level.         |
| **SetWaitableTimer**      | Triggers execution after a delayed or scheduled time.                    |
| **CreateTimerQueueTimer** | Creates a timer that executes a callback after a delay.                  |
| **SetTimer**              | Generates timer-based messages for delayed execution.                    |
| **select()**              | Uses network timeout waits as an indirect sleep mechanism.               |
| **IcmpSendEcho**          | Sends ICMP requests and waits for replies to introduce execution delays. |
| **timeGetTime**           | Retrieves system uptime in milliseconds to detect timing anomalies.      |

#### UI-Based Evasion

| Technical Term                   | One-Liner Explanation                                                    |
| -------------------------------- | ------------------------------------------------------------------------ |
| **CreateWindowExA**              | Creates a window to blend execution into normal GUI behavior.            |
| **RegisterHotKey**               | Registers a hotkey to trigger actions only on user interaction.          |
| **GetMessage / DispatchMessage** | Uses a message loop to idle execution until user or system events occur. |

---

