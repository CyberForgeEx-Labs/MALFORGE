<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/Data%20Protection%20%20%26%20Obfuscation/Data_Protection_OBF.c">Data Protection APIs Demonstration</a>
</h1>

## Overview
This program demonstrates **legitimate Windows APIs** that are frequently abused by malware, credential dumpers, red team tools, and post-exploitation frameworks for:

- Local data encryption/decryption using the Data Protection API (DPAPI)
- Obfuscation and storage of sensitive strings (passwords, tokens, C2 configs) in a way that is tied to the user/machine
- UUID string parsing and generation (commonly used for unique implant identifiers, mutexes, campaign IDs, etc.)

The code shows proper usage of `CryptProtectData` / `CryptUnprotectData` and the RPC UUID functions (`UuidFromStringA`, `UuidCreate`, `UuidToStringA`).

Purely **educational and research-oriented** — intended for defensive security engineers, malware analysts, incident responders, and blue team operators to understand how real-world threats leverage these APIs.

---
## Core APIs Demonstrated
| API                    | Purpose                                            | Sophistication | Common Malware Use Case                                      |
|------------------------|----------------------------------------------------|----------------|---------------------------------------------------------------------|
| `CryptProtectData`     | Encrypt data using user's DPAPI master key         | Intermediate   | Protect stolen credentials, C2 configs, tokens before exfil/writing |
| `CryptUnprotectData`   | Decrypt DPAPI blobs (same user/machine)            | Intermediate   | Decrypt previously stored configuration or credentials            |
| `UuidFromStringA`      | Convert UUID string → binary UUID structure        | Basic          | Parse campaign/implant IDs, mutex names, registry keys             |
| `UuidCreate` + `UuidToStringA` | Generate new random UUIDs                  | Basic          | Create unique identifiers per victim, mutexes, file names          |
---
## 1. [CryptProtectData / CryptUnprotectData](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/Data%20Protection%20%20%26%20Obfuscation/Data_Protection_OBF.c#L20)
### Description
Encrypts and decrypts arbitrary data using the Windows Data Protection API. The encryption key is derived from the user's login credentials (or machine credentials when `CRYPTPROTECT_LOCAL_MACHINE` is used).

### Key Features
- Transparent to the user — no password prompt
- Encrypted blob can only be decrypted by the **same user** on the **same machine** (by default)
- Widely used by browsers, Windows Credential Manager, RDP clients, etc.
- Frequently abused by ransomware, infostealers (Mimikatz-style tools, RedLine, Raccoon, etc.)

### Use Cases (Attack)
- Store stolen credentials or C2 addresses encrypted on disk
- Hide configuration from static analysis
- Persist data that survives reboots but remains protected

---
## 2. [UuidFromStringA](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/Data%20Protection%20%20%26%20Obfuscation/Data_Protection_OBF.c#L62)
### Description
Converts a standard UUID string (e.g., `550e8400-e29b-41d4-a716-446655440000`) into its binary `UUID` structure representation.

### Use Cases (Attack)
- Parse campaign IDs embedded in payloads
- Generate deterministic mutex or registry key names
- Validate configuration blocks

---
## 3. [UuidCreate + UuidToStringA](https://github.com/CyberForgeEx/MALFORGE/blob/main/Evasion/Data%20Protection%20%20%26%20Obfuscation/Data_Protection_OBF.c#L94)
### Description
Generates a new cryptographically random UUID (v4-like) and converts it back to string form.

### Use Cases (Attack)
- Generate unique implant ID per infection
- Create "random-looking" mutexes, pipe names, event names
- Name staged payloads or persistence artifacts

---
## Detection & Countermeasures
| Technique                    | EDR Detection | Recommended Countermeasure                                                                 |
|------------------------------|---------------|---------------------------------------------------------------------------------------------|
| `CryptProtectData` / `CryptUnprotectData` | Medium-High   | Monitor for calls with suspicious entropy or description strings; hook in EDR sensors     |
| Frequent UUID generation     | Low-Medium    | Behavioral correlation: UUID creation + network activity + file drops                      |
| DPAPI usage by non-browser   | High          | Flag non-standard processes (e.g., cmd.exe, custom binaries) calling DPAPI                 |

**Blue Team Tip**: Many modern infostealers and loaders use DPAPI to protect the next-stage payload or exfiltrated data. Look for processes that call `CryptProtectData` shortly after beaconing or credential access.

---
## Compilation Instructions
### Visual Studio / cl.exe
```bash
cl.exe Data_Protection_OBF.c /link crypt32.lib advapi32.lib rpcrt4.lib
```

### MinGW / GCC
```bash
gcc Data_Protection_OBF.c -o Data_Protection_OBF.exe -lcrypt32 -ladvapi32 -lrpcrt4
```

### Run (No admin required)
```cmd
Data_Protection_OBF.exe
```

Expected output includes:
- Original → encrypted → decrypted text (should match)
- Parsed UUID binary structure
- Newly generated UUID string

---
## Required Libraries
- `crypt32.lib`  → DPAPI functions
- `advapi32.lib` → Additional crypto services
- `rpcrt4.lib`   → UUID functions

---
## API References
- [CryptProtectData](https://learn.microsoft.com/en-us/windows/win32/api/dpapi/nf-dpapi-cryptprotectdata)
- [CryptUnprotectData](https://learn.microsoft.com/en-us/windows/win32/api/dpapi/nf-dpapi-cryptunprotectdata)
- [UuidFromStringA](https://learn.microsoft.com/en-us/windows/win32/api/rpcdce/nf-rpcdce-uuidfromstringa)
- [UuidCreate](https://learn.microsoft.com/en-us/windows/win32/api/rpcdce/nf-rpcdce-uuidcreate)
- [UuidToStringA](https://learn.microsoft.com/en-us/windows/win32/api/rpcdce/nf-rpcdce-uuidtostringa)

---
<p align="right">
  Last Updated: November 2025
</p>
