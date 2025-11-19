<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/Token%20Manipulation%20%26%20Privilege%20Escalation/Token_Manipulation_Escalation.c">Windows Token Manipulation & Privilege Privilege Escalation APIs Demonstration</a>
</h1>

## Overview
This program demonstrates advanced Windows security token manipulation and privilege escalation techniques using legitimate Win32 APIs. It explores how access tokens control user identity and privileges, how impersonation works, and how privileges can be enabled — all core concepts in Windows security architecture. Designed exclusively for **educational and authorized security research**, this tool helps understand token mechanics used in red teaming, penetration testing, forensics, and privilege escalation analysis.

---
## Core APIs Demonstrated
| API                            | Purpose                                      | Sophistication |
|--------------------------------|----------------------------------------------|----------------|
| `OpenProcessToken`             | Opens access token of a process              | Intermediate   |
| `LookupPrivilegeValue`         | Resolves privilege name to LUID              | Intermediate   |
| `AdjustTokenPrivileges`        | Enables/disables privileges in a token       | Advanced       |
| `ImpersonateLoggedOnUser`      | Sets thread to impersonate another user      | Advanced       |
| `DuplicateTokenEx`             | Creates a copy of an existing token          | Advanced       |
| `GetTokenInformation`          | Queries token details (user, privileges)     | Intermediate   |
| `CreateToolhelp32Snapshot`     | Enumerates running processes                 | Intermediate   |
| `RevertToSelf`                 | Ends impersonation and restores original context | Intermediate   |

---
## 1. [OpenProcessToken / GetTokenInformation](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/Token%20Manipulation%20%26%20Privilege%20Escalation/Token_Manipulation_Escalation.c#L67)
### Description
Opens the access token associated with a process or thread and retrieves detailed information including the current user (SID), token type, impersonation level, and all assigned privileges with their enabled/disabled status.
### Key Features
- Retrieves current user (DOMAIN\Username)
- Lists all privileges in the token
- Shows enabled vs disabled state
- Supports both primary and impersonation tokens
### Use Cases
- Security auditing tools
- Privilege assessment utilities
- Forensic analysis
- Access control debugging
- Token forensics

---
## 2. [AdjustTokenPrivileges + EnablePrivilege](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/Token%20Manipulation%20%26%20Privilege%20Escalation/Token_Manipulation_Escalation.c#L27)
### Description
Enables or disables specific privileges (e.g., `SeDebugPrivilege`, `SeImpersonatePrivilege`) in the current process token. Requires ownership of the token and the privilege to be present (even if disabled by default).
### Key Features
- Dynamic privilege enabling
- Safe fallback on missing privileges
- Proper error handling with `ERROR_NOT_ALL_ASSIGNED`
- Supports high-value privileges like `SeDebug`, `SeTcb`, `SeImpersonate`
### Use Cases
- Debuggers (requires SeDebugPrivilege)
- Backup/restore tools (SeBackup/Restore)
- Security research tools
- Legitimate privilege escalation (e.g., UAC bypass analysis)


---
## 3. [DuplicateTokenEx + ImpersonateLoggedOnUser](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/Token%20Manipulation%20%26%20Privilege%20Escalation/Token_Manipulation_Escalation.c#L234)
### Description
Duplicates an existing token (primary or impersonation) into a new impersonation token and assigns it to the current thread using `ImpersonateLoggedOnUser`. Allows a thread to temporarily act on behalf of another user.
### Key Features
- Creates usable impersonation tokens
- Supports `SecurityImpersonation` level
- Full control over access rights (`MAXIMUM_ALLOWED`)
- Clean reversion via `RevertToSelf()`
### Use Cases
- Client-server impersonation (e.g., file servers)
- Service accounts acting on user behalf
- Security tool development
- Token theft research (educational)

---
## 4. [ImpersonateLoggedOnUser via System Process Token](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/Token%20Manipulation%20%26%20Privilege%20Escalation/Token_Manipulation_Escalation.c#L126)
### Description
Attempts to open a token from a high-privilege system process (`lsass.exe`, `winlogon.exe`, `services.exe`) and impersonate it. Demonstrates real-world token impersonation attacks (e.g., incognito, token stealing) in a controlled, educational context.
### Key Features
- Process snapshot via Toolhelp32
- Targets known system processes
- Graceful fallback to current token
- Shows real-world privilege escalation path
### Use Cases
- Red team simulation (authorized only)
- Incident response training
- Token impersonation research
- Understanding "pass-the-token" attacks

---
## 5. [Security Context Analysis](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/Token%20Manipulation%20%26%20Privilege%20Escalation/Token_Manipulation_Escalation.c#L298)
### Description
Compares process token vs thread token to determine if impersonation is active. Shows how Windows distinguishes between primary identity (process) and effective identity (thread during impersonation).
### Key Features
- Detects active impersonation
- Displays current effective user
- Explains process vs thread token model
### Use Cases
- Debugging impersonation issues
- Security boundary validation
- Token-aware application development

---

## Token & Privilege Flow
```
Current User Token → [DuplicateTokenEx] → Impersonation Token
                             ↓
                 [ImpersonateLoggedOnUser]
                             ↓
                   Thread assumes new identity
                             ↓
                      [RevertToSelf]
                             ↓
                  Original identity restored
```

---
## Required Privileges
| Privilege                     | Common Use Case                     |
|------------------------------|-------------------------------------|
| `SeDebugPrivilege`           | Open processes like LSASS           |
| `SeImpersonatePrivilege`     | Impersonate clients after auth      |
| `SeAssignPrimaryToken`       | Replace process user token          |
| `SeTcbPrivilege`             | Act as part of operating system     |
| `SeBackupPrivilege`          | Read any file (bypass ACLs)         |

---
## Compilation Instructions
### Visual Studio / cl.exe
```bash
cl.exe TokenDemo.c /link advapi32.lib
```

### MinGW / GCC
```bash
gcc TokenDemo.c -o TokenDemo.exe -ladvapi32
```

### Run as Administrator (Required for most features)
```cmd
TokenDemo.exe
```

---
## Required Libraries
- `advapi32.lib` – Security and token functions
- `kernel32.lib` – Process/thread APIs
- `user32.lib`   – Optional (for message boxes)

---
## API References
- [OpenProcessToken](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-openprocesstoken)
- [AdjustTokenPrivileges](https://learn.microsoft.com/en-us/windows/win32/api/securitybaseapi/nf-securitybaseapi-adjusttokenprivileges)
- [ImpersonateLoggedOnUser](https://learn.microsoft.com/en-us/windows/win32/api/securitybaseapi/nf-securitybaseapi-impersonateloggedonuser)
- [DuplicateTokenEx](https://learn.microsoft.com/en-us/windows/win32/api/securitybaseapi/nf-securitybaseapi-duplicatetokenex)
- [GetTokenInformation](https://learn.microsoft.com/en-us/windows/win32/api/securitybaseapi/nf-securitybaseapi-gettokeninformation)

---
<p align="right">
  Last Updated: November 2025
</p>
