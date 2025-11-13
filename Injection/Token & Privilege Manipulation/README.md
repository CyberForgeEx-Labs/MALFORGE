<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Injection/Token%20%26%20Privilege%20Manipulation/Token_Manipulation.c">Token Security Manipulation and Inspection Techniques</a>
</h1>

## Overview
This educational tool demonstrates Windows token and privilege APIs for legitimate security research, system administration, and auditing purposes. Unlike offensive techniques, this implementation focuses on **read-only inspection** of security tokens without performing privilege escalation or unauthorized access. These APIs are essential for understanding Windows security architecture, building security tools, system administration utilities, and defensive security research.

---

## Core APIs

| API | Purpose | Sophistication |
|-----|---------|----------------|
| `OpenProcessToken` | Opens process token for inspection | Basic |
| `AdjustTokenPrivileges` | Queries privilege status | Intermediate |
| `DuplicateToken` | Creates duplicate token | Intermediate |
| `GetTokenInformation` | Retrieves token details | Basic |
| `LookupPrivilegeValue` | Converts privilege name to LUID | Basic |
| `LookupPrivilegeName` | Converts LUID to privilege name | Basic |

---

## 1. OpenProcessToken
### Description
Opens an access token associated with a process. Access tokens contain security information including user identity, group memberships, and privileges. Opening a token with TOKEN_QUERY access allows read-only inspection without modification. This is the foundation for all token inspection operations. Tokens can be opened with various access rights depending on the intended operation - query, adjust privileges, duplicate, or impersonate.

### Key Features
- Read-only token access (TOKEN_QUERY)
- Current or remote process tokens
- Access rights specification
- Safe inspection capability
- No privilege escalation
- Automatic handle cleanup

### Use Cases
- Security auditing tools
- Permission verification
- User identity inspection
- Privilege enumeration
- Group membership checking
- Compliance verification

---

## 2. AdjustTokenPrivileges
### Description
Enables or disables privileges in an access token. Privileges control what system-level operations a process can perform (e.g., backup files, debug programs, shutdown system). This demonstration shows how to QUERY privilege status without actually modifying privileges. The function can return previous privilege states, allowing safe inspection of what could be adjusted. Critical for understanding a process's security capabilities.

### Key Features
- Privilege state querying
- Previous state retrieval
- Granular privilege control
- Reversible operations
- Status verification
- Safe inspection mode

### Use Cases
- Security capability auditing
- Privilege requirement analysis
- Permission boundary testing
- Security policy validation
- Administrative tool development
- Compliance checking

---

## 3. DuplicateToken
### Description
Creates a new duplicate access token from an existing token. The duplicate can have a different impersonation level, allowing safe inspection at lower privilege levels. SecurityIdentification level produces a token that can only be used for identity inspection, not impersonation or privilege modification. Useful for creating safe copies of tokens for analysis without risking the original token's security.

### Key Features
- Token duplication capability
- Impersonation level control
- Safe inspection copies
- Identity preservation
- Isolated analysis
- Reversible operation

### Use Cases
- Token comparison analysis
- Security boundary testing
- Safe token inspection
- Identity verification
- Forensic analysis
- Educational demonstrations

---

## 4. GetTokenInformation
### Description
Retrieves various types of information from an access token. Can query user identity, group memberships, privilege lists, token type, impersonation level, and security descriptors. Requires different buffer sizes depending on information class. Essential for comprehensive token inspection and understanding security context. Works with all token information classes defined in TOKEN_INFORMATION_CLASS enumeration.

### Key Features
- Multiple information classes
- Dynamic buffer sizing
- Comprehensive data retrieval
- Type-safe queries
- Read-only operation
- Detailed security context

### Use Cases
- Complete token inspection
- User identity verification
- Group membership enumeration
- Privilege listing
- Security context analysis
- Audit trail generation

---

## 5. LookupPrivilegeValue
### Description
Retrieves the locally unique identifier (LUID) used on the local system to represent a specified privilege name. Privileges like "SeDebugPrivilege" or "SeBackupPrivilege" are referred to by name in code but identified by LUID at the system level. LUIDs are unique per system boot. Required before calling AdjustTokenPrivileges to enable/disable specific privileges. Converts human-readable privilege names to system identifiers.

### Key Features
- Name to LUID conversion
- System-specific lookup
- Privilege name validation
- Case-insensitive search
- Local system resolution
- Error handling support

### Use Cases
- Privilege manipulation setup
- Security requirement verification
- Privilege name validation
- Administrative tool development
- Permission boundary definition
- Security policy implementation

---

## 6. LookupPrivilegeName
### Description
Retrieves the name that represents a specified privilege LUID. Reverse operation of LookupPrivilegeValue - converts system identifiers back to human-readable names. Essential for displaying privilege information to users and generating audit logs. Supports all standard Windows privileges including debug, backup, restore, shutdown, and administrative privileges. Buffer size automatically managed through size parameter.

### Key Features
- LUID to name conversion
- Human-readable output
- Buffer size management
- All privilege support
- Localization support
- Display-friendly results

### Use Cases
- Audit log generation
- User interface display
- Token information reporting
- Security analysis output
- Documentation generation
- Forensic reporting

---

## Token Information Classes

### Available Query Types
```c
// Common TOKEN_INFORMATION_CLASS values
TokenUser                    // User account SID
TokenGroups                  // Group memberships
TokenPrivileges              // Available privileges
TokenOwner                   // Default owner SID
TokenPrimaryGroup            // Primary group SID
TokenDefaultDacl             // Default DACL
TokenSource                  // Token source
TokenType                    // Primary or impersonation
TokenImpersonationLevel      // Impersonation level
TokenStatistics              // Token statistics
TokenRestrictedSids          // Restricted SIDs
TokenSessionId               // Session identifier
TokenGroupsAndPrivileges     // Combined groups and privileges
TokenElevationType           // Token elevation type
TokenLinkedToken             // Linked token (UAC)
TokenElevation               // Elevation status
TokenIntegrityLevel          // Integrity level SID
```

---

## Windows Privileges

### Common Privilege Names
```c
// System privileges (require appropriate access)
SE_DEBUG_NAME                "SeDebugPrivilege"          // Debug programs
SE_BACKUP_NAME               "SeBackupPrivilege"         // Backup files/directories
SE_RESTORE_NAME              "SeRestorePrivilege"        // Restore files/directories
SE_SHUTDOWN_NAME             "SeShutdownPrivilege"       // Shutdown system
SE_TAKE_OWNERSHIP_NAME       "SeTakeOwnershipPrivilege"  // Take ownership
SE_LOAD_DRIVER_NAME          "SeLoadDriverPrivilege"     // Load device drivers
SE_SYSTEM_PROFILE_NAME       "SeSystemProfilePrivilege"  // Profile system
SE_SYSTEMTIME_NAME           "SeSystemtimePrivilege"     // Change system time
SE_INCREASE_PRIORITY_NAME    "SeIncBasePriorityPrivilege"// Increase scheduling priority
SE_CREATE_PAGEFILE_NAME      "SeCreatePagefilePrivilege" // Create pagefile
SE_CREATE_PERMANENT_NAME     "SeCreatePermanentPrivilege"// Create permanent objects
```

### Privilege States
```c
// Privilege attributes
SE_PRIVILEGE_ENABLED_BY_DEFAULT  0x00000001  // Enabled by default
SE_PRIVILEGE_ENABLED             0x00000002  // Currently enabled
SE_PRIVILEGE_REMOVED             0x00000004  // Removed from token
SE_PRIVILEGE_USED_FOR_ACCESS     0x80000000  // Used for access check
```

---

## Token Access Rights

### Standard Token Access
```c
// For OpenProcessToken / OpenThreadToken
TOKEN_ASSIGN_PRIMARY        0x0001    // Assign as primary token
TOKEN_DUPLICATE            0x0002    // Duplicate token
TOKEN_IMPERSONATE          0x0004    // Impersonate using token
TOKEN_QUERY                0x0008    // Query token information
TOKEN_QUERY_SOURCE         0x0010    // Query token source
TOKEN_ADJUST_PRIVILEGES    0x0020    // Adjust privileges
TOKEN_ADJUST_GROUPS        0x0040    // Adjust groups
TOKEN_ADJUST_DEFAULT       0x0080    // Adjust default DACL
TOKEN_ADJUST_SESSIONID     0x0100    // Adjust session ID
TOKEN_ALL_ACCESS           0x000F01FF // All possible access

// Safe read-only access
DWORD dwSafeAccess = TOKEN_QUERY;

// For privilege adjustment (inspection only)
DWORD dwInspectAccess = TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES;

// For duplication
DWORD dwDuplicateAccess = TOKEN_DUPLICATE | TOKEN_QUERY;
```

---

## Impersonation Levels

### Security Impersonation Levels
```c
// For DuplicateToken and token creation
SecurityAnonymous           // No client identification
SecurityIdentification      // Query identity only (SAFEST for inspection)
SecurityImpersonation       // Impersonate client
SecurityDelegation          // Impersonate on remote systems

// Safe inspection level (demonstration use)
SECURITY_IMPERSONATION_LEVEL safeLevel = SecurityIdentification;
```

---


### Token Comparison Analysis
```
Original Token                Duplicate Token
┌──────────────┐             ┌──────────────┐
│ User: DOMAIN\│             │ User: DOMAIN\│
│      Alice   │             │      Alice   │
├──────────────┤             ├──────────────┤
│ Groups:      │             │ Groups:      │
│  - Admin     │             │  - Admin     │
│  - Users     │             │  - Users     │
├──────────────┤             ├──────────────┤
│ Privileges:  │             │ Privileges:  │
│  - SeDebug   │             │  - SeDebug   │
│  - SeBackup  │             │  - SeBackup  │
├──────────────┤             ├──────────────┤
│ Level:       │             │ Level:       │
│ SecurityImp. │             │ SecurityId.  │ <- Lower level
└──────────────┘             └──────────────┘
        │                            │
        └────────── Compare ─────────┘
```

---

## Elevated vs Non-Elevated Tokens

### UAC Token Elevation
```
Non-Elevated Token (Standard User)
┌────────────────────────────────┐
│ User: DOMAIN\Alice             │
│ Groups:                        │
│   - Users                      │
│   - Authenticated Users        │
│ Privileges (Limited):          │
│   - SeChangeNotifyPrivilege    │
│   - SeUndockPrivilege          │
│ Integrity: Medium              │
└────────────────────────────────┘

Elevated Token (Administrator)
┌────────────────────────────────┐
│ User: DOMAIN\Alice             │
│ Groups:                        │
│   - Administrators (Enabled)   │
│   - Users                      │
│ Privileges (Extended):         │
│   - SeDebugPrivilege           │
│   - SeBackupPrivilege          │
│   - SeRestorePrivilege         │
│   - SeTakeOwnershipPrivilege   │
│ Integrity: High                │
└────────────────────────────────┘
```

---

## Common Inspection Failures

### Error Scenarios
| Error | Cause | Solution |
|-------|-------|----------|
| Access Denied | Insufficient rights | Request only TOKEN_QUERY |
| Invalid Handle | Token already closed | Check handle validity |
| Insufficient Buffer | Buffer too small | Call twice: size query, then data |
| Not All Assigned | Privilege not held | Check privilege existence first |
| Invalid Parameter | Bad TOKEN_INFORMATION_CLASS | Verify class value |
| Privilege Not Held | Token lacks privilege | Normal for standard users |


---


## Required Libraries
- `advapi32.lib` - Advanced Windows 32 Base API (all token functions)
- `kernel32.lib` - Kernel functions (process handles)

---

## Compilation Instructions

### Microsoft Visual C++
```console
cl.exe Token_Security_Audit.c advapi32.lib
```

### MinGW GCC
```console
gcc Token_Security_Audit.c -o Token_Security_Audit.exe -ladvapi32
```

---

## API References
- MSDN: [OpenProcessToken](https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-openprocesstoken)
- MSDN: [AdjustTokenPrivileges](https://docs.microsoft.com/en-us/windows/win32/api/securitybaseapi/nf-securitybaseapi-adjusttokenprivileges)
- MSDN: [DuplicateToken](https://docs.microsoft.com/en-us/windows/win32/api/securitybaseapi/nf-securitybaseapi-duplicatetoken)
- MSDN: [GetTokenInformation](https://docs.microsoft.com/en-us/windows/win32/api/securitybaseapi/nf-securitybaseapi-gettokeninformation)
- MSDN: [LookupPrivilegeValue](https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-lookupprivilegevaluea)
- MSDN: [Access Tokens](https://docs.microsoft.com/en-us/windows/win32/secauthz/access-tokens)
- MSDN: [Privileges](https://docs.microsoft.com/en-us/windows/win32/secauthz/privileges)


---

<p align="right">
  Last Updated: November 2025
</p>
