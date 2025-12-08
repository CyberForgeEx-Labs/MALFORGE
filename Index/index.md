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
| **IRQL (Interrupt Request Level)** | Priority level that decides which code can interrupt what — higher IRQL blocks everything below it   |
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
| **Section Object**              | Kernel object that represents a memory-mapped file — lets multiple processes share the same memory  |


more....

