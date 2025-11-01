<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Enumeration/Locale-Time%20Enumeration/LocalTimeEnumeration.c">Locale-Time Enumeration Techniques</a>
</h1>

## Overview
Locale and time enumeration provides critical Windows API capabilities for querying system time, locale information, and internationalization settings. This documentation covers essential APIs for retrieving current time in various formats, thread locale settings, and enumerating installed system locales used in Windows internationalization model.

---

## Core APIs

| API | Purpose | Sophistication |
|-----|---------|----------------|
| `GetSystemTime` | Gets current system time (UTC) | Basic |
| `GetSystemTimeAsFileTime` | Gets system time as FILETIME | Basic |
| `GetThreadLocale` | Gets current thread locale | Intermediate |

---

## 1. [GetSystemTime](https://github.com/CyberForgeEx/MALFORGE/blob/main/Enumeration/Locale-Time%20Enumeration/LocalTimeEnumeration.c#L12)
### Description
Retrieves the current system time in Coordinated Universal Time (UTC). The function fills a SYSTEMTIME structure with date and time components including year, month, day, hour, minute, second, and milliseconds. This provides a human-readable format for system time.

### Key Features
- UTC time retrieval
- SYSTEMTIME structure format
- Millisecond precision
- Component-based access
- No time zone conversion
- Direct system clock access

### Use Cases
- Timestamp generation
- Log file entries
- Time synchronization
- Scheduling applications
- Performance monitoring
- Audit trail creation

### Example Output
```
Current System Time (UTC):
----------------------------
Date: 2025-10-29
Time: 14:32:45.123
Day of Week: 3 (Wednesday)

STRUCTURE DETAILS:
  wYear:         2025
  wMonth:        10
  wDayOfWeek:    3
  wDay:          29
  wHour:         14
  wMinute:       32
  wSecond:       45
  wMilliseconds: 123

```

---

## 2. [GetSystemTimeAsFileTime](https://github.com/CyberForgeEx/MALFORGE/blob/main/Enumeration/Locale-Time%20Enumeration/LocalTimeEnumeration.c#L49)
### Description
Retrieves the current system time as a FILETIME structure. FILETIME represents time as a 64-bit value containing 100-nanosecond intervals since January 1, 1601 UTC. This is the most efficient Windows API for obtaining high-resolution timestamps and is widely used in file systems and system logging.

### Key Features
- 64-bit time value
- 100-nanosecond resolution
- High performance
- File system compatible
- UNIX timestamp conversion
- Arithmetic operations support

### Use Cases
- High-resolution timestamps
- File time operations
- Performance measurement
- Time interval calculations
- Database timestamps
- System logging

### Example Output
```
FILETIME Structure:
----------------------------
dwLowDateTime:  0x12345678 (305419896)
dwHighDateTime: 0x01DB4567 (31196519)

64-bit FILETIME Value:
----------------------------
Decimal: 133986234567890000
Hex:     0x01DB456712345678

Time Calculations:
----------------------------
100-nanosecond intervals since Jan 1, 1601: 133986234567890000
Seconds since Jan 1, 1601:                   13398623456
Days since Jan 1, 1601:                      155078972
UNIX Timestamp (seconds since Jan 1, 1970): 1730207056

Converted to SYSTEMTIME:
----------------------------
Date: 2025-10-29
Time: 14:32:45.789 UTC
```

---

## 3. [GetThreadLocale](https://github.com/CyberForgeEx/MALFORGE/blob/main/Enumeration/Locale-Time%20Enumeration/LocalTimeEnumeration.c#L102)
### Description
Retrieves the locale identifier (LCID) for the calling thread. The LCID identifies the language, country, and cultural conventions used for formatting dates, times, numbers, and currency. Each thread can have its own locale setting, which affects string operations, sorting, and formatting behavior.

### Key Features
- Thread-specific locale
- LCID format (32-bit)
- Language and country codes
- Formatting preferences
- Cultural conventions
- Sort order determination

### Use Cases
- Internationalization (i18n)
- Localization (l10n)
- Date/time formatting
- Number formatting
- Currency display
- String comparison and sorting

### Example Output
```
Thread Locale Information:
----------------------------
LCID (Locale ID): 0x00000409 (1033)

Date & Time Format:
----------------------------
Short Date Format:     M/d/yyyy
Long Date Format:      dddd, MMMM d, yyyy
Time Format:           h:mm:ss tt

```

---

## SYSTEMTIME Structure

### Format
```c
typedef struct _SYSTEMTIME {
    WORD wYear;           // Year (1601-30827)
    WORD wMonth;          // Month (1-12)
    WORD wDayOfWeek;      // Day of week (0=Sunday, 6=Saturday)
    WORD wDay;            // Day of month (1-31)
    WORD wHour;           // Hour (0-23)
    WORD wMinute;         // Minute (0-59)
    WORD wSecond;         // Second (0-59)
    WORD wMilliseconds;   // Milliseconds (0-999)
} SYSTEMTIME;
```

### Key Points
- All fields are unsigned 16-bit integers
- wDayOfWeek: 0=Sunday through 6=Saturday
- UTC time unless using GetLocalTime()
- Size: 16 bytes
- Used for human-readable time

---

## FILETIME Structure

### Format
```c
typedef struct _FILETIME {
    DWORD dwLowDateTime;   // Low-order 32 bits
    DWORD dwHighDateTime;  // High-order 32 bits
} FILETIME;
```

### Key Points
- 64-bit value split into two 32-bit DWORDs
- Represents 100-nanosecond intervals
- Epoch: January 1, 1601, 00:00:00 UTC
- Size: 8 bytes
- Can represent dates up to year 30828


---

## LCID (Locale Identifier) Structure

### Format
```
LCID (32-bit value)
├── Sort ID (4 bits)
└── Language ID (16 bits)
    ├── Sub-Language ID (10 bits)
    └── Primary Language ID (6 bits)
```

---

## Time Conversion Functions

### Related Time APIs
- `GetLocalTime` - Get local system time (with time zone)
- `SystemTimeToFileTime` - Convert SYSTEMTIME to FILETIME
- `FileTimeToSystemTime` - Convert FILETIME to SYSTEMTIME
- `FileTimeToLocalFileTime` - Convert UTC FILETIME to local
- `LocalFileTimeToFileTime` - Convert local FILETIME to UTC
- `GetTimeZoneInformation` - Get time zone settings
- `GetDynamicTimeZoneInformation` - Get extended time zone info

---

## Required Libraries
- `kernel32.lib` - All APIs in this document.

---

## API References
- MSDN: [GetSystemTime](https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getsystemtime)
- MSDN: [GetSystemTimeAsFileTime](https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getsystemtimeasfiletime)
- MSDN: [GetThreadLocale](https://docs.microsoft.com/en-us/windows/win32/api/winnls/nf-winnls-getthreadlocale)
- MSDN: [SYSTEMTIME Structure](https://docs.microsoft.com/en-us/windows/win32/api/minwinbase/ns-minwinbase-systemtime)
- MSDN: [FILETIME Structure](https://docs.microsoft.com/en-us/windows/win32/api/minwinbase/ns-minwinbase-filetime)

---

## Required Compiler Flags
- C99 or later standard
- Link against `kernel32.lib`
- No special optimization flags needed

---

<p align="right">
  Last Updated: October 2025
</p>
