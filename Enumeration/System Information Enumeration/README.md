<h1 align="center">
   <a href="hhttps://github.com/CyberForgeEx/MALFORGE/blob/main/Enumeration/System%20Information%20Enumeration/SystemInfoEnumeration.c">System Information Enumeration Techniques</a>
</h1>

## Overview
System information enumeration is a fundamental Windows API capability for discovering system configuration, hardware capabilities, and operating system details. This documentation covers advanced APIs for querying processor information, system architecture, OS version, language settings, and drive configurations.

---

## Core APIs

| API | Purpose | Sophistication |
|-----|---------|----------------|
| `GetLogicalProcessorInformation` | Gets processor topology and cache info | Intermediate |
| `GetLogicalProcessorInformationEx` | Extended processor info with efficiency | Advanced |
| `GetNativeSystemInfo` | Gets native system architecture | Basic |
| `GetSystemDefaultLangID` | Gets system default language ID | Basic |
| `GetVersionExA` | Gets Windows version (deprecated) | Basic |
| `RtlGetVersion` | Gets accurate OS version (NT native) | Intermediate |
| `IsWoW64Process` | Checks if process runs under WOW64 | Basic |
| `GetDriveTypeA` | Gets drive type for drives | Basic |

---

## 1. [GetLogicalProcessorInformation](https://github.com/CyberForgeEx/MALFORGE/blob/main/Enumeration/System%20Information%20Enumeration/SystemInfoEnumeration.c#L35)
### Description
Retrieves information about logical processors and their relationships, including processor cores, NUMA nodes, caches, and processor packages. This provides detailed CPU topology information.

### Key Features
- Processor core enumeration
- Cache hierarchy detection
- NUMA node identification
- Processor package information
- Hyper-threading detection
- Logical processor mapping

### Use Cases
- CPU topology analysis
- Performance optimization
- Thread affinity configuration
- Cache-aware algorithms
- System capability detection
- Hardware inventory

### Example Output
```
Processor Core #1
  Processor Mask: 0x3
  Flags: 0x00

Processor Core #2
  Processor Mask: 0xC
  Flags: 0x00

Processor Summary:
  Physical Cores: 4
  Logical Processors: 8
  Processor Packages: 1
  NUMA Nodes: 1
  L1 Caches: 8
  L2 Caches: 4
  L3 Caches: 1
  Hyper-Threading: Enabled
```

---

## 2. [GetLogicalProcessorInformationEx](https://github.com/CyberForgeEx/MALFORGE/blob/main/Enumeration/System%20Information%20Enumeration/SystemInfoEnumeration.c#L117)
### Description
Extended version providing detailed processor information including efficiency classes (for hybrid architectures), processor groups, and enhanced cache details. Supports modern CPUs with heterogeneous cores.

### Key Features
- Efficiency class detection (P-cores vs E-cores)
- Processor group support
- Enhanced cache information
- Hybrid architecture support
- Extended relationship data
- Future-proof design

### Use Cases
- Modern CPU optimization
- Hybrid architecture detection
- Advanced thread scheduling
- Performance core selection
- Energy-efficient computing
- Multi-group processor systems

### Example Output
```
Relationship Type         Efficiency       Details
-------------------------------------------------------------------------
Processor Core            High             Core #1, Flags: 0x00
Processor Core            Standard         Core #2, Flags: 0x00
Cache                     N/A              L1 Unified, 32 KB
Cache                     N/A              L2 Unified, 256 KB
Cache                     N/A              L3 Unified, 8192 KB
NUMA Node                 N/A              Node #0
Processor Package         N/A              Package #1

Extended Summary:
  Total Processor Cores: 8
  Total Processor Packages: 1
```

---

## 3. [GetNativeSystemInfo](https://github.com/CyberForgeEx/MALFORGE/blob/main/Enumeration/System%20Information%20Enumeration/SystemInfoEnumeration.c#L207)
### Description
Retrieves information about the native system architecture, including processor type, page size, address space limits, and the actual hardware architecture regardless of WOW64 emulation.

### Key Features
- Native architecture detection
- Memory page size information
- Address space boundaries
- Processor count and type
- Allocation granularity
- Architecture-independent queries

### Use Cases
- Cross-platform compatibility checks
- Memory management planning
- Address space calculations
- WOW64 detection
- System requirements validation
- Low-level programming

### Example Output
```
Native System Information:

Processor Architecture:
  Architecture: x64 (AMD64)
  Architecture ID: 9
  Number of Processors: 8
  Processor Type: 8664
  Processor Level: 6
  Processor Revision: 15361

Memory Information:
  Page Size: 4096 bytes (4 KB)
  Allocation Granularity: 65536 bytes (64 KB)
  Minimum Application Address: 0x0000000000010000
  Maximum Application Address: 0x00007FFFFFFEFFFF

Active Processor Mask: 0xFF
```

---

## 4. [GetSystemDefaultLangID](https://github.com/CyberForgeEx/MALFORGE/blob/main/Enumeration/System%20Information%20Enumeration/SystemInfoEnumeration.c#L255)
### Description
Retrieves the default language identifier for the system, providing information about the primary language and sublanguage configuration.

### Key Features
- System default language
- Primary language ID
- Sublanguage ID
- Locale information access
- Language name retrieval
- Country/region detection

### Use Cases
- Internationalization (i18n)
- Localization (l10n)
- Language-specific features
- Regional settings detection
- Multi-language support
- User preference detection

### Example Output
```
Language Information:

System Default Language ID: 0x0409
  Primary Language ID: 0x0009
  Sub-language ID: 0x0001

User Default Language ID: 0x0409
  Primary Language ID: 0x0009
  Sub-language ID: 0x0001

System Language Name: English
System Country Name: United States
```

---

## 5. [GetVersionExA & RtlGetVersion](https://github.com/CyberForgeEx/MALFORGE/blob/main/Enumeration/System%20Information%20Enumeration/SystemInfoEnumeration.c#L286)
### Description
Two methods for retrieving Windows version information. GetVersionExA is deprecated and may return incorrect values due to application compatibility shims. RtlGetVersion provides accurate version information.

### Key Features
- OS version detection
- Build number retrieval
- Service pack information
- Platform identification
- Product type detection
- Accurate version reporting (RtlGetVersion)

### Use Cases
- Compatibility checking
- Feature detection
- Version-specific code paths
- System requirements validation
- Update verification
- OS identification

### Important Note
**GetVersionExA is deprecated** and subject to compatibility shims that may return Windows 8 version even on Windows 10/11. **Always use RtlGetVersion** for accurate version information.

### Example Output
```
GetVersionExA Results:
  Major Version: 6
  Minor Version: 2
  Build Number: 9200
  Platform ID: 2
  Service Pack: 
  Suite Mask: 0x0100
  Product Type: 1

RtlGetVersion Results (Accurate):
  Major Version: 10
  Minor Version: 0
  Build Number: 22631

Windows Version: Windows 11 (Build 22631)
```

---

## 6. [IsWoW64Process](https://github.com/CyberForgeEx/MALFORGE/blob/main/Enumeration/System%20Information%20Enumeration/SystemInfoEnumeration.c#L346)
### Description
Determines whether a process is running under WOW64 (Windows 32-bit on Windows 64-bit) emulation, indicating a 32-bit process on 64-bit Windows.

### Key Features
- WOW64 detection
- Architecture compatibility check
- Process type identification
- Simple boolean result
- Low overhead check
- Current or remote process support

### Use Cases
- Architecture-aware code
- File system redirection handling
- Registry redirection management
- 32-bit/64-bit compatibility
- DLL loading decisions
- System capability detection

### Example Output
```
WOW64 Status Check:

Current Process Running Under WOW64: Yes

Explanation:
  - This is a 32-bit process running on 64-bit Windows
  - WOW64 (Windows 32-bit on Windows 64-bit) subsystem is active
  - File system and registry are redirected

Process Architecture Details:
  Native System Architecture: x64 (64-bit)
  Process Type: 32-bit
```

---

## 7. [GetDriveTypeA](https://github.com/CyberForgeEx/MALFORGE/blob/main/Enumeration/System%20Information%20Enumeration/SystemInfoEnumeration.c#L399)
### Description
Retrieves the type of drive for a specified root directory, distinguishing between fixed drives, removable media, network drives, and optical drives.

### Key Features
- Drive type identification
- All drive enumeration
- Volume information access
- Storage capacity queries
- Network drive detection
- Removable media identification

### Use Cases
- Storage device enumeration
- Drive-specific operations
- Backup utilities
- File management tools
- Storage capacity monitoring
- Device type filtering

### Example Output
```
Scanning system drives...

Drive  Type                 Description
----------------------------------------------------------------------
C:\    Fixed                Fixed hard disk
       Size: 476.94 GB, Free: 125.38 GB (73.7% used)
D:\    Fixed                Fixed hard disk
       Size: 931.51 GB, Free: 542.18 GB (41.8% used)
E:\    CD-ROM               CD-ROM or DVD drive
F:\    Removable            Removable media (USB, floppy, etc.)
       Size: 29.82 GB, Free: 15.24 GB (48.9% used)
Z:\    Network              Network drive

Drive Summary:
  Total Drives Found: 5
```

---

## Comparison

| Feature | GetLogicalProc | GetLogicalProcEx | GetNativeSystemInfo | GetVersionEx | RtlGetVersion |
|---------|----------------|------------------|---------------------|--------------|---------------|
| **Speed** | Fast | Fast | Very Fast | Very Fast | Very Fast |
| **Detail Level** | High | Very High | Moderate | Basic | Basic |
| **Modern CPU** | Limited | Full Support | Yes | N/A | N/A |
| **Accuracy** | High | High | High | Low (shimmed) | High |
| **Deprecated** | No | No | No | Yes | No |

---

## Required Libraries
- `kernel32.lib` (Windows API) - For most system information functions
- `ntdll.lib` (NT Native API) - For RtlGetVersion

---

## Common Applications

### System Monitoring Tools
- CPU-Z, HWiNFO, Speccy
- Performance Monitor
- Resource monitors
- Hardware inventory tools

### Development Tools
- Compilers and build systems
- Profilers and analyzers
- Debuggers
- IDE system requirement checks

### System Utilities
- Disk management tools
- System information utilities
- Backup software
- Drive management applications

### Performance Software
- Process managers
- Thread affinity tools
- CPU optimization utilities
- Memory management tools

---

## Architecture-Specific Considerations

### x86 (32-bit)
- Limited to 4GB address space (3GB user-mode)
- WOW64 not applicable on 32-bit Windows
- Simpler processor topologies

### x64 (64-bit)
- Large address space.
- WOW64 supports 32-bit applications
- More complex processor topologies
- Better NUMA support

### ARM64
- Emerging architecture
- Hybrid core configurations common
- Efficiency class support critical
- Different cache hierarchies

---

## Version-Specific Features

### Windows 7
- Basic processor information
- Limited hybrid CPU support
- Standard NUMA support

### Windows 8/8.1
- Improved processor group support
- Better NUMA handling
- Extended processor information

### Windows 10
- Full hybrid architecture support
- Efficiency class detection
- Enhanced processor groups
- Better version detection needed

### Windows 11
- Build number >= 22000
- Enhanced hybrid CPU support
- Improved power management
- Better efficiency core handling

---

## Troubleshooting

### Common Issues

**GetVersionExA returns wrong version**
- Solution: Use RtlGetVersion instead, not subject to compatibility shims

**GetLogicalProcessorInformation buffer too small**
- Solution: Call twice - first to get size, then with allocated buffer

**IsWoW64Process returns FALSE on 32-bit Windows**
- Expected: WOW64 doesn't exist on 32-bit systems

**GetDriveTypeA slow for network drives**
- Solution: Check drive type before querying volume info

**Missing processor groups in results**
- Solution: Use GetLogicalProcessorInformationEx for full support

---

## API References
- MSDN: [GetLogicalProcessorInformation](https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getlogicalprocessorinformation)
- MSDN: [GetLogicalProcessorInformationEx](https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getlogicalprocessorinformationex)
- MSDN: [GetNativeSystemInfo](https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getnativesysteminfo)
- MSDN: [GetSystemDefaultLangID](https://docs.microsoft.com/en-us/windows/win32/api/winnls/nf-winnls-getsystemdefaultlangid)
- MSDN: [GetVersionExA](https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getversionexa) (Deprecated)
- MSDN: [RtlGetVersion](https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlgetversion)
- MSDN: [IsWoW64Process](https://docs.microsoft.com/en-us/windows/win32/api/wow64apiset/nf-wow64apiset-iswow64process)
- MSDN: [GetDriveTypeA](https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getdrivetypea)

---

### Required Compiler Flags
- C99 or later standard
- Link against `kernel32.lib` and `ntdll.lib`
- Windows SDK headers required
- Disable deprecation warnings for GetVersionExA (educational purposes)

---

<p align="right">
  Last Updated: October 2025
</p>
