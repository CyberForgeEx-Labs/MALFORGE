<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Enumeration/User-Account%20Enumeration/UserAccountEnumeration.c">User/Account Enumeration Techniques</a>
</h1>

## Overview
User and account enumeration is a critical Windows API capability for discovering user identity, computer identification, security identifiers (SIDs), and privilege information. This documentation covers essential APIs for querying current user context, account mappings, and privilege values used in Windows security model.

---

## Core APIs

| API | Purpose | Sophistication |
|-----|---------|----------------|
| `GetUserNameA` | Gets current user name | Basic |
| `GetComputerNameA` | Gets NetBIOS computer name | Basic |
| `LookupAccountNameA` | Maps account name to SID | Intermediate |
| `LookupPrivilegeValueA` | Gets LUID for privilege name | Intermediate |

---

## 1. [GetUserNameA](https://github.com/CyberForgeEx/MALFORGE/blob/main/Enumeration/User-Account%20Enumeration/UserAccountEnumeration.c#L44)
### Description
Retrieves the user name associated with the current thread. This function returns the name of the user account under which the current process or thread is running, providing essential identity information for the executing context.

### Key Features
- Current thread user identification
- Format includes domain/workgroup prefix
- Access token based retrieval
- Simple buffer-based API
- Works with impersonation
- Minimal overhead

### Use Cases
- User context verification
- Logging and auditing
- Application personalization
- Security context validation
- Process ownership identification
- Session management

### Example Output
```
Current User Name: WORKSTATION\JohnDoe
Username Length: 19 characters

Context:
  - This is the name of the user account running this process
  - Format is typically: DOMAIN\Username or just Username
  - Retrieved from current thread's access token
```

---

## 2. [GetComputerNameA](https://github.com/CyberForgeEx/MALFORGE/blob/main/Enumeration/User-Account%20Enumeration/UserAccountEnumeration.c#L66)
### Description
Retrieves the NetBIOS name of the local computer. This is the network identification name used for file sharing and network communication. Extended versions can retrieve DNS hostnames and fully qualified domain names.

### Key Features
- NetBIOS name retrieval (15 char limit)
- DNS hostname support (extended)
- Domain name identification
- FQDN retrieval capability
- Multiple name format support
- Network identity access

### Use Cases
- Network identification
- Computer inventory
- Remote connection setup
- License management
- System identification
- Network diagnostics

### Example Output
```
Computer Name (NetBIOS): DESKTOP-PC01
Name Length: 12 characters
Fully Qualified Domain Name: desktop-pc01.contoso.com

Context:
  - NetBIOS name is limited to 15 characters
  - Used for network identification and file sharing
  - Can be different from DNS hostname in some configurations
```

---

## 3. [LookupAccountNameA](https://github.com/CyberForgeEx/MALFORGE/blob/main/Enumeration/User-Account%20Enumeration/UserAccountEnumeration.c#L107)
### Description
Accepts an account name and retrieves the corresponding security identifier (SID), domain name, and account type. This is a fundamental API for translating human-readable names into system security identifiers used by Windows for access control.

### Key Features
- Name-to-SID translation
- Domain name resolution
- Account type identification
- SID structure analysis
- Multiple account format support

### Use Cases
- Security permission management
- Access control list (ACL) creation
- User/group identification
- Security auditing
- Account validation
- Domain trust relationships

### Example Output
```
Looking up account: WORKSTATION\JohnDoe

Account Lookup Successful:

Security Identifier (SID):
  String Format: S-1-5-21-1234567890-1234567890-1234567890-1001
  Binary Size: 28 bytes

Domain Information:
  Domain Name: WORKSTATION
  Domain Length: 11 characters

Account Type:
  SID Type: User Account
  Type Value: 1

SID Components:
  Identifier Authority: 5
  Sub-Authority Count: 5
  Sub-Authorities: 21-1234567890-1234567890-1234567890-1001

Context:
  - SIDs are unique identifiers for security principals
  - Format: S-R-I-S-S-S... (Revision-Authority-SubAuthorities)
  - Used internally by Windows for access control

Well-Known Account Lookups:
-------------------------------------
SYSTEM               S-1-5-18 (Well-Known Group)
Administrators       S-1-5-32-544 (Alias (Local Group))
Everyone             S-1-1-0 (Well-Known Group)
NETWORK SERVICE      S-1-5-20 (Well-Known Group)
```

---

## 4. [LookupPrivilegeValueA](https://github.com/CyberForgeEx/MALFORGE/blob/main/Enumeration/User-Account%20Enumeration/UserAccountEnumeration.c#L210)
### Description
Retrieves the locally unique identifier (LUID) for a specified privilege name. Privileges control what system-level operations an access token can perform. This function translates privilege constant names into their numeric identifiers.

### Key Features
- Privilege name to LUID mapping
- System-wide privilege enumeration
- Access token privilege queries
- Display name retrieval
- Privilege state checking
- 64-bit LUID structure

### Use Cases
- Token privilege management
- Security privilege checking
- Administrative operation validation
- Process elevation verification
- Service account configuration
- Security auditing

### Example Output
```
Looking up common Windows privileges:

Privilege Name                      LUID                 Description
----------------------------------- -------------------- ----------------------------------------
SeDebugPrivilege                    0x0000000000000014   Debug programs
SeShutdownPrivilege                 0x0000000000000013   Shut down the system
SeTcbPrivilege                      0x0000000000000007   Act as part of the operating system
SeBackupPrivilege                   0x0000000000000011   Back up files and directories
SeRestorePrivilege                  0x0000000000000012   Restore files and directories
SeSecurityPrivilege                 0x0000000000000008   Manage auditing and security log
SeSystemtimePrivilege               0x000000000000000C   Change the system time
SeLoadDriverPrivilege               0x000000000000000A   Load and unload device drivers
SeTakeOwnershipPrivilege            0x0000000000000009   Take ownership of files or objects
SeIncreaseBasePriorityPrivilege     0x000000000000000E   Increase scheduling priority

LUID Details:
  - LUID = Locally Unique Identifier
  - 64-bit value (HighPart + LowPart)
  - Unique on the local system until reboot
  - Used to represent privileges in access tokens

Current Process Token Privileges:
-------------------------------------
Total Privileges: 5

Privilege                                State      LUID
---------------------------------------- ---------- --------------------
SeShutdownPrivilege                      Disabled   0x0000000000000013
SeChangeNotifyPrivilege                  Default    0x0000000000000017
SeUndockPrivilege                        Disabled   0x0000000000000019
SeIncreaseWorkingSetPrivilege            Disabled   0x0000000000000021
SeTimeZonePrivilege                      Disabled   0x0000000000000022
```

---

## SID Structure Explained

### Format
```
S-R-I-S-S-S-S-S...
│ │ │ └─────────── Sub-authorities (variable count)
│ │ └──────────────── Identifier Authority (48-bit)
│ └─────────────────── Revision (always 1)
└───────────────────── Literal "S"
```

### Well-Known SIDs

| SID | Name | Description |
|-----|------|-------------|
| S-1-0-0 | Null Authority | Nobody |
| S-1-1-0 | World Authority | Everyone |
| S-1-5-18 | Local System | NT AUTHORITY\SYSTEM |
| S-1-5-19 | Local Service | NT AUTHORITY\LOCAL SERVICE |
| S-1-5-20 | Network Service | NT AUTHORITY\NETWORK SERVICE |
| S-1-5-32-544 | Administrators | BUILTIN\Administrators |
| S-1-5-32-545 | Users | BUILTIN\Users |

### Account Types (SID_NAME_USE)

| Value | Name | Description |
|-------|------|-------------|
| 1 | SidTypeUser | User account |
| 2 | SidTypeGroup | Global group |
| 3 | SidTypeDomain | Domain object |
| 4 | SidTypeAlias | Local group (alias) |
| 5 | SidTypeWellKnownGroup | Well-known group |
| 6 | SidTypeDeletedAccount | Deleted account |
| 7 | SidTypeInvalid | Invalid SID |
| 8 | SidTypeUnknown | Unknown type |
| 9 | SidTypeComputer | Computer account |

---

## Privilege Names and Constants

### Privilege Attributes

| Flag | Name | Description |
|------|------|-------------|
| 0x00000000 | SE_PRIVILEGE_DISABLED | Privilege is disabled |
| 0x00000001 | SE_PRIVILEGE_ENABLED_BY_DEFAULT | Enabled by default |
| 0x00000002 | SE_PRIVILEGE_ENABLED | Currently enabled |
| 0x00000004 | SE_PRIVILEGE_REMOVED | Removed from token |
| 0x80000000 | SE_PRIVILEGE_USED_FOR_ACCESS | Used in access check |

---

## Required Libraries
- `kernel32.lib` - For GetUserNameA, GetComputerNameA
- `advapi32.lib` - For LookupAccountNameA, LookupPrivilegeValueA, token operations.

---

## Related APIs

### Extended Functionality
- `GetUserNameExA` - Extended name formats (UPN, DNS)
- `ConvertSidToStringSidA` - SID to string conversion
- `ConvertStringSidToSidA` - String to SID conversion
- `LookupAccountSidA` - Reverse lookup (SID to name)
- `LookupPrivilegeNameA` - LUID to privilege name
- `LookupPrivilegeDisplayNameA` - User-friendly privilege name

### Token Management
- `OpenProcessToken` - Get process token handle
- `GetTokenInformation` - Query token details
- `AdjustTokenPrivileges` - Enable/disable privileges
- `DuplicateToken` - Create token copy
- `ImpersonateLoggedOnUser` - Impersonate user context

### Security Descriptor APIs
- `GetSecurityInfo` - Get object security
- `SetSecurityInfo` - Set object security
- `GetNamedSecurityInfoA` - Get security by name
- `SetNamedSecurityInfoA` - Set security by name

---

## API References
- MSDN: [GetUserNameA](https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getusernamea)
- MSDN: [GetComputerNameA](https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getcomputernamea)
- MSDN: [LookupAccountNameA](https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-lookupaccountnamea)
- MSDN: [LookupPrivilegeValueA](https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-lookupprivilegevaluea)
- MSDN: [Security Identifiers](https://docs.microsoft.com/en-us/windows/win32/secauthz/security-identifiers)
- MSDN: [Privileges](https://docs.microsoft.com/en-us/windows/win32/secauthz/privileges)
- MSDN: [Access Tokens](https://docs.microsoft.com/en-us/windows/win32/secauthz/access-tokens)

---

### Required Compiler Flags
- C99 or later standard
- Link against `kernel32.lib` and `advapi32.lib`
- Windows SDK headers required
- No special optimization flags needed.

---

<p align="right">
  Last Updated: October 2025
</p>
