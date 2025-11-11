<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Injection/Heap%20Manipulation/Heap_Manipulation.c">Heap Manipulation Techniques</a>
</h1>

## Overview
Heap manipulation techniques utilize Windows heap management APIs to dynamically allocate, manage, and free memory during runtime. Unlike stack-based allocation, heap memory provides flexible, long-lived storage that persists across function calls and can be shared between threads. These APIs are fundamental for memory management, custom allocators, exploit development, and understanding process memory layout. Heap operations are essential for advanced injection techniques, shellcode deployment, and security research.

---

## Core APIs

| API | Purpose | Sophistication |
|-----|---------|----------------|
| `HeapCreate` | Creates private heap | Intermediate |
| `HeapAlloc` | Allocates memory from heap | Basic |
| `HeapReAlloc` | Reallocates heap memory | Intermediate |
| `GetProcessHeap` | Gets default process heap | Basic |
| `GetProcessHeaps` | Enumerates all heaps | Intermediate |
| `RtlCreateHeap` | Native heap creation | Advanced |
| `GlobalAlloc` | Allocates global memory | Basic (Legacy) |

---

## 1. [HeapCreate]
### Description
Creates a private heap object separate from the process's default heap. Private heaps provide isolated memory management with custom size limits and allocation strategies. The heap can be created with a fixed maximum size or as a growable heap (when max size is 0). Useful for scenarios requiring dedicated memory pools, improved locality of reference, or isolation from the default heap to prevent fragmentation. The heap exists until explicitly destroyed with HeapDestroy or the process terminates.

### Key Features
- Private, isolated memory pool
- Configurable initial and maximum sizes
- Serialization control (thread-safe by default)
- Generate exceptions on failure option
- Custom allocation granularity
- Automatic cleanup on process termination

### Use Cases
- Custom memory allocators
- Memory pool implementations
- Heap spray techniques
- Shellcode staging areas
- Isolated data structures
- Performance-critical allocations
- Heap-based injection techniques

### Flags
```c
// Common HeapCreate flags
0                           // Default (serialized)
HEAP_NO_SERIALIZE          0x00000001  // No thread synchronization
HEAP_GENERATE_EXCEPTIONS   0x00000004  // Raise exceptions on failure
HEAP_CREATE_ENABLE_EXECUTE 0x00040000  // Enable code execution
```

---

## 2. [HeapAlloc]
### Description
Allocates a block of memory from a specified heap. The fastest and most efficient method for dynamic memory allocation within a heap context. Memory can be allocated with zero-initialization (HEAP_ZERO_MEMORY) or left uninitialized for performance. Supports serialized (thread-safe) and non-serialized access. Returns NULL on failure unless HEAP_GENERATE_EXCEPTIONS flag was used during heap creation. The allocated memory must be freed with HeapFree to prevent memory leaks.

### Key Features
- Fast allocation from heap
- Optional zero-initialization
- Size flexibility (up to heap limits)
- Serialization control per allocation
- Exception generation support
- 8-byte alignment guarantee (x86/x64)

### Use Cases
- General dynamic memory allocation
- Buffer allocation for I/O operations
- Data structure storage
- Shellcode buffer preparation
- String manipulation buffers
- Temporary computation storage
- Payload staging memory

### Flags
```c
// Common HeapAlloc flags
0                      // Default allocation
HEAP_ZERO_MEMORY      0x00000008  // Zero-initialize memory
HEAP_NO_SERIALIZE     0x00000001  // No synchronization
HEAP_GENERATE_EXCEPTIONS 0x00000004 // Generate exceptions
```

---

## 3. [HeapReAlloc]
### Description
Changes the size of an allocated heap block, either growing or shrinking it. If growing, the heap manager attempts to extend the existing block in place; if impossible, it allocates a new larger block, copies the existing data, and frees the old block. When shrinking, excess memory is released back to the heap. The returned pointer may differ from the original, so callers must use the returned value. Can optionally zero-initialize expanded memory regions.

### Key Features
- In-place resizing when possible
- Automatic data preservation
- Grow or shrink operations
- Optional zero-initialization of new space
- Moveable reallocation support
- Efficient memory management

### Use Cases
- Dynamic array resizing
- Buffer expansion for variable-size data
- Memory-efficient data structures
- Growing payload buffers
- String concatenation optimization
- Streaming data collection
- Adaptive memory management

### Flags
```c
// Common HeapReAlloc flags
0                      // Default reallocation
HEAP_ZERO_MEMORY      0x00000008  // Zero new space
HEAP_NO_SERIALIZE     0x00000001  // No synchronization
HEAP_REALLOC_IN_PLACE_ONLY 0x00000010 // Fail if can't grow in place
```

---

## 4. [GetProcessHeap]
### Description
Returns a handle to the calling process's default heap. Every process has exactly one default heap created by the system during process initialization. This heap is used by malloc/new in C/C++ runtime libraries. Thread-safe and always available. The default heap cannot be destroyed and persists for the process lifetime. Commonly used for quick, convenient memory allocations without creating custom heaps.

### Key Features
- Always available default heap
- Thread-safe access
- Pre-initialized at process start
- Cannot be destroyed
- Used by C runtime (CRT)
- No handle management needed

### Use Cases
- Simple dynamic memory allocation
- CRT memory operations
- Quick buffer allocation
- Standard heap access
- General-purpose storage
- Legacy code compatibility
- Injection target analysis

---

## 5. [GetProcessHeaps]
### Description
Retrieves handles to all heaps in the current process, including the default heap and any private heaps created via HeapCreate or RtlCreateHeap. Returns the number of heaps when called with NULL buffer. Useful for enumerating heap structures, analyzing memory layout, detecting heap spray attempts, or validating heap handles. The returned array may change if heaps are created or destroyed between calls.

### Key Features
- Enumerates all process heaps
- Returns heap count
- Includes default and private heaps
- Snapshot of current state
- Array allocation by caller
- Dynamic heap tracking

### Use Cases
- Heap enumeration for analysis
- Memory forensics investigations
- Heap spray detection
- Security auditing tools
- Debugging memory issues
- Process introspection
- Anti-analysis techniques

---

## 6. [RtlCreateHeap]
### Description
Native API function from ntdll.dll for creating heap objects at the kernel level. More powerful than HeapCreate with additional control over heap characteristics, including NUMA node awareness, custom locking mechanisms, and advanced allocation strategies. Provides lower-level access and is often used in rootkits, advanced exploits, and system-level code. Returns a heap handle that can be used with standard heap APIs like HeapAlloc.

### Key Features
- Native kernel-level API
- Advanced heap configuration
- Custom locking mechanisms
- NUMA node support
- Low-level control
- Enhanced allocation attributes
- Bypass user-mode monitoring

### Use Cases
- Stealthy heap creation
- Rootkit development
- Advanced exploit techniques
- System-level programming
- Custom heap implementations
- Performance-critical applications
- Anti-detection mechanisms


---

## 7. [GlobalAlloc]
### Description
Legacy global memory allocation function maintained for backward compatibility with 16-bit Windows. Allocates memory from the process heap with moveable or fixed attributes. Moveable memory can be relocated by the memory manager and requires locking (GlobalLock) before use. Modern applications should use HeapAlloc instead. Still used in clipboard operations, OLE/COM interfaces, and legacy code. Returns a handle (HGLOBAL) rather than a direct pointer for moveable allocations.

### Key Features
- Fixed or moveable allocations
- Handle-based memory management
- Lock/unlock mechanism
- Optional zero-initialization
- Legacy compatibility
- Clipboard data format
- Resizable with GlobalReAlloc

### Use Cases
- Clipboard data handling
- OLE/COM data transfer
- Legacy application support
- DDE (Dynamic Data Exchange)
- Backward compatibility
- Shared memory (historical)
- Windows messaging (WM_COPY)

### Flags
```c
// Common GlobalAlloc flags
GMEM_FIXED         0x0000  // Fixed memory (pointer returned)
GMEM_MOVEABLE      0x0002  // Moveable memory (handle returned)
GMEM_ZEROINIT      0x0040  // Zero-initialize
GHND               (GMEM_MOVEABLE | GMEM_ZEROINIT)
GPTR               (GMEM_FIXED | GMEM_ZEROINIT)
```

---

## Heap Architecture

### Process Heap Layout
```
Process Address Space:
┌─────────────────────────────────┐
│ Default Process Heap            │
│ (Created at process init)       │
│ - Used by malloc/new            │
│ - Thread-safe                   │
│ Handle: GetProcessHeap()        │
└─────────────────────────────────┘
┌─────────────────────────────────┐
│ Private Heap #1                 │
│ Created: HeapCreate()           │
│ - Custom allocation pool        │
│ - Isolated from default heap   │
└─────────────────────────────────┘
┌─────────────────────────────────┐
│ Private Heap #2                 │
│ Created: RtlCreateHeap()        │
│ - Low-level native heap         │
│ - Advanced configuration        │
└─────────────────────────────────┘
┌─────────────────────────────────┐
│ CRT Heap (debug builds)         │
│ - Used by debug malloc          │
│ - Additional metadata           │
└─────────────────────────────────┘
```

### Heap Internal Structure
```
Heap Object:
┌─────────────────────────────────┐
│ HEAP Header                     │
│ - Signature                     │
│ - Flags                         │
│ - Segment list                  │
│ - Free list pointers            │
│ - Lock (if serialized)          │
└─────────────────────────────────┘
        ↓
┌─────────────────────────────────┐
│ Heap Segment                    │
│ ┌─────────────────────────────┐ │
│ │ Allocated Block #1          │ │
│ │ - Block header              │ │
│ │ - User data                 │ │
│ │ - Block footer (debug)      │ │
│ └─────────────────────────────┘ │
│ ┌─────────────────────────────┐ │
│ │ Free Block                  │ │
│ │ - Free list pointers        │ │
│ └─────────────────────────────┘ │
│ ┌─────────────────────────────┐ │
│ │ Allocated Block #2          │ │
│ └─────────────────────────────┘ │
└─────────────────────────────────┘
```

---

## Heap-Based Injection Techniques

### Technique 1: Heap Spray
```
Concept: Fill process heap with controlled data
to increase probability of hitting payload

Steps:
1. Allocate many heap blocks
2. Fill with NOP sled + shellcode
3. Trigger vulnerability (e.g., use-after-free)
4. Execution lands in sprayed region

Application:
- Exploit development
- Reliability improvement
- ASLR bypass assistance
```

### Technique 2: Heap Overflow
```
Concept: Overflow heap block to corrupt adjacent
block headers or data

Steps:
1. Allocate target heap block
2. Overflow buffer to overwrite metadata
3. Trigger heap operation (alloc/free)
4. Gain control via corrupted pointers

Application:
- Memory corruption exploits
- Function pointer overwrites
- Heap metadata manipulation
```

### Technique 3: Heap Feng Shui
```
Concept: Carefully arrange heap layout to
place objects in predictable locations

Steps:
1. Study target heap layout
2. Allocate/free blocks in specific pattern
3. Shape heap to desired layout
4. Exploit with known offsets

Application:
- Exploit reliability
- UAF (Use-After-Free) exploitation
- Controlled memory layout
```

### Technique 4: Custom Heap Injection
```
Concept: Create private heap for payload
storage, isolated from default heap

Steps:
1. HeapCreate() in target or self
2. HeapAlloc() for shellcode
3. Mark executable if needed
4. Transfer execution via hijacking

Application:
- Stealthy code injection
- Isolated payload storage
- Evasion of heap monitoring
```

---

## Security Considerations

### Heap Exploitation Mitigations
- **ASLR (Address Space Layout Randomization)**: Randomizes heap base addresses
- **DEP (Data Execution Prevention)**: Prevents execution from heap
- **Heap Randomization**: Windows 8+ randomizes heap allocations
- **Guard Pages**: Detect out-of-bounds access
- **Heap Metadata Encoding**: XOR encoding of heap structures
- **Terminate on Corruption**: Immediate termination on detected corruption


### Common Heap Vulnerabilities
| Vulnerability | Description | Impact |
|---------------|-------------|--------|
| Heap Overflow | Write beyond allocated size | Metadata corruption, code execution |
| Use-After-Free | Access freed memory | Info leak, code execution |
| Double Free | Free same memory twice | Heap corruption, exploitation |
| Heap Spray | Fill heap with attacker data | ASLR bypass, exploit reliability |
| Integer Overflow | Size calculation overflow | Incorrect allocation size |
| Uninitialized Data | Using uninitialized heap memory | Information disclosure |

---

## Common Heap Errors

### Error Scenarios
| Error | Cause | Solution |
|-------|-------|----------|
| HeapCreate returns NULL | Insufficient memory | Reduce initial/max size |
| HeapAlloc returns NULL | Heap exhausted or size invalid | Check heap size limits |
| HeapReAlloc fails | Cannot resize in-place | Allow moveable reallocation |
| Invalid heap handle | Heap destroyed or invalid | Validate handle before use |
| Access violation | Use-after-free or invalid pointer | Track allocation lifetimes |
| Heap corruption detected | Overflow or double-free | Enable page heap for debugging |
| GlobalLock returns NULL | Memory was discarded | Check lock count and flags |

---

## Heap Flags Reference

### HeapCreate Flags
```c
#define HEAP_NO_SERIALIZE           0x00000001
#define HEAP_GROWABLE               0x00000002  // Used internally
#define HEAP_GENERATE_EXCEPTIONS    0x00000004
#define HEAP_ZERO_MEMORY            0x00000008  // For HeapAlloc
#define HEAP_REALLOC_IN_PLACE_ONLY  0x00000010  // For HeapReAlloc
#define HEAP_TAIL_CHECKING_ENABLED  0x00000020  // Debug
#define HEAP_FREE_CHECKING_ENABLED  0x00000040  // Debug
#define HEAP_DISABLE_COALESCE_ON_FREE 0x00000080
#define HEAP_CREATE_ALIGN_16        0x00010000
#define HEAP_CREATE_ENABLE_TRACING  0x00020000
#define HEAP_CREATE_ENABLE_EXECUTE  0x00040000
```

### GlobalAlloc Flags
```c
#define GMEM_FIXED       0x0000
#define GMEM_MOVEABLE    0x0002
#define GMEM_NOCOMPACT   0x0010
#define GMEM_NODISCARD   0x0020
#define GMEM_ZEROINIT    0x0040
#define GMEM_MODIFY      0x0080
#define GMEM_DISCARDABLE 0x0100
#define GMEM_NOT_BANKED  0x1000
#define GMEM_SHARE       0x2000
#define GMEM_DDESHARE    0x2000
#define GMEM_NOTIFY      0x4000
#define GMEM_VALID_FLAGS 0x7F72
#define GHND             (GMEM_MOVEABLE | GMEM_ZEROINIT)
#define GPTR             (GMEM_FIXED | GMEM_ZEROINIT)
```

---

## Required Libraries
- `kernel32.lib` - User-mode heap APIs (HeapCreate, HeapAlloc, GlobalAlloc)
- `ntdll.lib` - Native API functions (RtlCreateHeap, RtlDestroyHeap)

---

## API References
- MSDN: [Heap Functions](https://docs.microsoft.com/en-us/windows/win32/memory/heap-functions)
- MSDN: [HeapCreate](https://docs.microsoft.com/en-us/windows/win32/api/heapapi/nf-heapapi-heapcreate)
- MSDN: [HeapAlloc](https://docs.microsoft.com/en-us/windows/win32/api/heapapi/nf-heapapi-heapalloc)
- MSDN: [HeapReAlloc](https://docs.microsoft.com/en-us/windows/win32/api/heapapi/nf-heapapi-heaprealloc)
- MSDN: [GetProcessHeap](https://docs.microsoft.com/en-us/windows/win32/api/heapapi/nf-heapapi-getprocessheap)
- MSDN: [GetProcessHeaps](https://docs.microsoft.com/en-us/windows/win32/api/heapapi/nf-heapapi-getprocessheaps)
- MSDN: [GlobalAlloc](https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-globalalloc)
- Undocumented NT: [RtlCreateHeap](http://undocumented.ntinternals.net/)
- Windows Internals: Heap Manager Architecture.

---

<p align="right">
  Last Updated: November 2025
</p>