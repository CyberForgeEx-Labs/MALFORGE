<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/Environment%20Manipulation/Environment_Manipulation.c">Windows Environment Manipulation & Locale Fingerprinting</a>
</h1>

## Overview
This program demonstrates **legitimate Windows APIs** frequently abused by malware, droppers, and red team tools to **read, modify, and fingerprint the process environment** as well as **enumerate and query system locale settings**.

These techniques are commonly used in real-world malware for:
- Environment-aware execution
- Sandbox/VM detection (via uncommon or missing environment variables)
- Anti-analysis (checking for debugger/analyst-set variables)
- Persistence and configuration staging
- Geographic or language-based payload selection

Purely **educational and research-oriented** — intended for defensive security engineers, malware analysts, and blue team operators.

---
## Core APIs Demonstrated
| API                        | Purpose                                      | Sophistication | Common Malware Use Case               |
|--------------------------------|----------------------------------------------|----------------|---------------------------------------|
| `GetEnvironmentVariableA`      | Read process/block environment variables     | Basic          | Detect sandbox artifacts (e.g., "SANDBOX=") |
| `SetEnvironmentVariableA`      | Create/modify/delete environment variables   | Intermediate   | Drop config, modify PATH, stage persistence |
| `EnumSystemLocalesA`           | Enumerate all supported system locales       | Intermediate   | Detect bare-metal vs sandbox locale count |
| `GetLocaleInfoA`               | Query detailed locale/country/language info  | Intermediate   | Geo-fencing, language-specific payloads    |
| `GetSystemDefaultLCID`         | Retrieve current system default locale ID    | Basic          | Fingerprint OS region settings            |

---
## 1. [GetEnvironmentVariableA](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/Environment%20Manipulation/Environment_Manipulation.c#L44)
### Description
Retrieves the value of a specified environment variable from the current process environment block.
### Key Features
- Fast, lightweight lookup
- Commonly checked variables: `USERNAME`, `COMPUTERNAME`, `TEMP`, `PATH`
- Malware often looks for analyst-set flags like `DEBUG=1`, `ANALYZER=1`
### Use Cases (Attack)
- Detect sandbox-specific variables
- Avoid execution in monitored environments
- Gather system context

---
## 2. [SetEnvironmentVariableA](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/Environment%20Manipulation/Environment_Manipulation.c#L66)
### Description
Creates, modifies, or deletes an environment variable in the current process. Passing `NULL` as value deletes the variable.
### Key Features
- Can temporarily extend `PATH` (common in installers & droppers)
- Used to stage configuration before writing to disk
- Changes are inherited by child processes
### Use Cases (Attack)
- Modify `PATH` to hijack DLL loading
- Set persistence flags
- Communicate between stages without files

---
## 3. [EnumSystemLocalesA + Callback](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/Environment%20Manipulation/Environment_Manipulation.c#L5)
### Description
Enumerates all locales supported by the system via a callback function. Many sandboxes have a reduced locale list (often <50 vs 200+ on real systems).
### Key Features
- Callback-based enumeration
- Reveals full multilingual capabilities
- Strong sandbox indicator when count is low
### Use Cases (Attack)
- Detect Cuckoo, Joe Sandbox, ANY.RUN, etc.
- Abort if locale count is suspiciously low

---
## 4. [GetLocaleInfoA + GetSystemDefaultLCID](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/Environment%20Manipulation/Environment_Manipulation.c#L115)
### Description
Queries detailed information about the current or specified locale (language, country, date format, currency, etc.).
### Key Features
- Geographic and cultural fingerprinting
- Used in targeted attacks (e.g., only run in specific countries)
### Use Cases (Attack)
- Geo-targeted ransomware
- Language-specific phishing payloads
- Avoid execution in analysis regions (e.g., US, Germany)

---
## Detection & Countermeasures
| Technique                     | EDR Detection | Sandbox/Defender Countermeasure                     |
|-------------------------------|---------------|-----------------------------------------------------|
| `GetEnvironmentVariableA`     | Low           | Populate fake/realistic variables                   |
| `SetEnvironmentVariableA`     | Medium        | Hook and alert on suspicious names (e.g., containing "BEACON") |
| `EnumSystemLocalesA`          | Medium-High   | Patch to return large realistic list                |
| Excessive locale queries      | Medium        | Behavioral rule: flag enumeration + delay/malicious action |

**Blue Team Tip**: Monitor for processes that read many environment variables then enumerate locales common pre-execution check pattern in modern malware.

---
## Compilation Instructions
### Visual Studio / cl.exe
```bash
cl.exe Environment_Manipulation.c /link advapi32.lib
```

### MinGW / GCC
```bash
gcc Environment_Manipulation.c -o Environment_Locale_Demo.exe
```

### Run (No admin required)
```cmd
Environment_Locale_Demo.exe
```

---
## Required Libraries
- Uses: `kernel32.dll`, `user32.dll` (implicitly linked)

---
## API References
- [GetEnvironmentVariableA](https://learn.microsoft.com/en-us/windows/win32/api/processenv/nf-processenv-getenvironmentvariablea)
- [SetEnvironmentVariableA](https://learn.microsoft.com/en-us/windows/win32/api/processenv/nf-processenv-setenvironmentvariablea)
- [EnumSystemLocalesA](https://learn.microsoft.com/en-us/windows/win32/api/winnls/nf-winnls-enumsystemlocalesa)
- [GetLocaleInfoA](https://learn.microsoft.com/en-us/windows/win32/api/winnls/nf-winnls-getlocaleinfoa)
- [GetSystemDefaultLCID](https://learn.microsoft.com/en-us/windows/win32/api/winnls/nf-winnls-getsystemdefaultlcid)

---
<p align="right">
  Last Updated: November 2025
</p>
