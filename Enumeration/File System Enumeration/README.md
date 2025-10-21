<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE_API-ARSENAL/blob/main/Enumeration/File%20System%20Enumeration/File_SysEnum.c">File System Enumeration Techniques</a>
</h1>

## Overview
File system enumeration provides fundamental Windows API capabilities for discovering, analyzing, and monitoring files, directories, and drive configurations. This documentation covers essential APIs for navigating the file system, locating system directories, verifying file existence, and enumerating available storage devices.

---

## Core APIs

| API | Purpose | Sophistication |
|-----|---------|----------------|
| `FindFirstFileA` | Begin directory enumeration | Basic |
| `FindNextFileA` | Continue directory enumeration | Basic |
| `GetWindowsDirectoryA` | Get Windows installation path | Basic |
| `GetSystemDirectoryA` | Get System32 directory path | Basic |
| `PathFileExistsA` | Verify file/directory existence | Basic |
| `GetLogicalDrives` | Enumerate available drives | Basic |
| `GetDriveTypeA` | Determine drive type | Basic |

---

## FindFirstFileA
### Description
Initiates a search for files or subdirectories in a specified directory, returning a handle for iterating through matching entries with wildcard pattern support.

### Key Features
- Wildcard pattern matching
- Directory and file discovery
- File attribute access
- Creation/modification timestamps
- File size information

### Use Cases
- Directory content listing
- File search operations
- Recursive directory traversal
- File inventory creation
- Backup utility development

### Example Output
```
Listing files in C:\Windows (first 10 items):

Name                           Type
------------------------------------------
.                              [DIR]
..                             [DIR]
addins                         [DIR]
AppCompat                      [DIR]
AppPatch                       [DIR]
bfsvc.exe                      [FILE]
bootstat.dat                   [FILE]
```

---

## FindNextFileA
### Description
Continues a file search initiated by FindFirstFileA, retrieving subsequent files or directories that match the search pattern.

### Key Features
- Sequential file enumeration
- Consistent data structure
- Efficient iteration
- Same WIN32_FIND_DATA format
- Automatic search continuation

### WIN32_FIND_DATAA Structure
```
- dwFileAttributes: File attributes (directory, hidden, etc.)
- ftCreationTime: File creation timestamp
- ftLastAccessTime: Last access timestamp
- ftLastWriteTime: Last modification timestamp
- nFileSizeHigh: High-order file size DWORD
- nFileSizeLow: Low-order file size DWORD
- cFileName: File name (up to MAX_PATH)
- cAlternateFileName: 8.3 format filename
```

### Use Cases
- Complete directory scanning
- Batch file processing
- Recursive searches
- File filtering operations
- Directory tree building

---

## GetWindowsDirectoryA
### Description
Retrieves the path of the Windows installation directory, typically the location where the operating system is installed.

### Key Features
- System directory location
- Standard path retrieval
- Buffer size validation
- ASCII string format
- Consistent across versions

### Use Cases
- System file location
- Configuration file access
- Windows resource discovery
- Installation path verification
- System diagnostics

### Example Output
```
Windows Directory: C:\Windows
Path Length: 10 characters
```

---

## GetSystemDirectoryA
### Description
Retrieves the path of the System32 directory containing core Windows system files, drivers, and critical system components.

### Key Features
- System32 path retrieval
- Critical file location
- Architecture-aware path
- Buffer overflow protection
- Standard system location

### Use Cases
- System binary location
- Driver file access
- DLL discovery
- System utility execution
- Administrative tool access

### Example Output
```
System Directory: C:\Windows\System32
Path Length: 20 characters
```

---

## PathFileExistsA
### Description
Determines whether a specified file or directory exists on the file system, providing simple boolean existence verification.

### Key Features
- Fast existence check
- File and directory support
- Path validation
- Network path support
- Minimal overhead

### Use Cases
- Pre-operation validation
- Configuration file checks
- Dependency verification
- Error prevention
- Path validation

### Example Output
```
Checking if files/directories exist:

Path                                          Exists?
-------------------------------------------------------
C:\Windows\System32\notepad.exe               [YES]
C:\Windows\System32\kernel32.dll              [YES]
C:\ThisFileDoesNotExist.txt                   [NO]
C:\Windows                                    [YES]
C:\Program Files                              [YES]
```

---

## GetLogicalDrives
### Description
Retrieves a bitmask representing all available logical drives on the system, where each bit corresponds to a drive letter from A to Z.

### Key Features
- Complete drive enumeration
- Bitmask representation
- Fast discovery
- All drive types included
- Single function call

### Bitmask Structure
- Bit 0 (0x00000001): Drive A:
- Bit 1 (0x00000002): Drive B:
- Bit 2 (0x00000004): Drive C:
- ...
- Bit 25 (0x02000000): Drive Z:

### Use Cases
- Drive availability check
- Storage device discovery
- System inventory
- Backup target selection
- Volume enumeration

### Example Output
```
Drive Bitmask: 0x0000001C

Drive      Type
-------------------------
C:\        Hard Disk
D:\        CD-ROM
E:\        Removable
```

---

## GetDriveTypeA
### Description
Determines the type of a specified drive, distinguishing between fixed disks, removable media, network drives, and other storage types.

### Key Features
- Drive type identification
- Multiple drive categories
- Quick type determination
- Path-based query
- Standard type constants

### Drive Type Constants
| Constant | Value | Description |
|----------|-------|-------------|
| DRIVE_UNKNOWN | 0 | Drive type cannot be determined |
| DRIVE_NO_ROOT_DIR | 1 | Root path is invalid |
| DRIVE_REMOVABLE | 2 | Removable media (USB, floppy) |
| DRIVE_FIXED | 3 | Fixed hard disk |
| DRIVE_REMOTE | 4 | Network drive |
| DRIVE_CDROM | 5 | CD-ROM or DVD drive |
| DRIVE_RAMDISK | 6 | RAM disk |

### Use Cases
- Storage categorization
- Backup strategy selection
- Media type detection
- Drive filtering
- System configuration analysis

---

## Comparison

| Feature | FindFirstFile | PathFileExists | GetLogicalDrives | GetWindowsDirectory |
|---------|---------------|----------------|------------------|---------------------|
| **Scope** | Directory-specific | Single path | System-wide | System path |
| **Speed** | Moderate | Very Fast | Very Fast | Instant |
| **Memory Overhead** | Low | Minimal | Minimal | Minimal |
| **Detail Level** | Full metadata | Boolean only | Drive list | Path string |
| **Iteration** | Yes | No | No | No |
| **Handle Required** | Yes | No | No | No |
| **Error Handling** | Handle-based | Boolean return | Bitmask/0 | Length/0 |

---

## Required Libraries
- `kernel32.lib` (Windows API) - Core file system operations
- `shlwapi.lib` (Shell Light-weight API) - Path helper functions
- `windows.h` - Main Windows API header
- `shlwapi.h` - Shell API header for PathFileExists

---

## Educational Purpose
This documentation is provided for **educational and research purposes only**. Understanding file system enumeration techniques is essential for:
- System administration and monitoring
- File management utility development
- Backup and recovery tools
- System diagnostics and troubleshooting
- Security audit tools
- Software development
- IT operations and automation
- Cybersecurity education

---

## API References
- MSDN: [FindFirstFileA](https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-findfirstfilea)
- MSDN: [FindNextFileA](https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-findnextfilea)
- MSDN: [FindClose](https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-findclose)
- MSDN: [GetWindowsDirectoryA](https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getwindowsdirectorya)
- MSDN: [GetSystemDirectoryA](https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getsystemdirectorya)
- MSDN: [PathFileExistsA](https://docs.microsoft.com/en-us/windows/win32/api/shlwapi/nf-shlwapi-pathfileexistsa)
- MSDN: [GetLogicalDrives](https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getlogicaldrives)
- MSDN: [GetDriveTypeA](https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getdrivetypea)

---

<p align="right">
  Last Updated: October 2025
</p>