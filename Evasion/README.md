<h1 align="center">
    MALFORGE / Evasion
</h1>

A comprehensive evasion techniques framework demonstrating advanced anti-analysis, anti-detection, and sandbox evasion methods for authorized security research and red team operations.

## Overview
This repository provides a structured collection of modern evasion techniques commonly employed by sophisticated malware and penetration testing tools. Each module showcases real-world methods used to bypass EDRs, AV engines, sandboxes, and forensic analysis — strictly for defensive research, malware analysis training, and authorized security testing.

## Features
- **Data Protection & Obfuscation** – String encryption, resource hiding, and anti-disassembly techniques.
- **Dynamic Code Loading & Execution** – Reflective loading, early bird APC, and runtime decryption.
- **Environment Manipulation** – VM detection, debugger checks, and human interaction simulation.
- **File System Manipulation & Anti-Forensics** – Timestomping, secure delete, and alternate data streams.
- **Process Creation & Injection** – Stealthy process creation, parent PID spoofing, and block DLL injection.
- **Time-Based Evasion & Sandbox Detection** – Delay execution, uptime checks, and sleep obfuscation.
- **Token Manipulation & Privilege Escalation** – Token stealing, duplication, and UAC bypass methods.
- **UI-Based Evasion** – Mouse movement simulation, clipboard monitoring, and UI interaction checks.

## Directory Structure
```
Evasion/
├── Data Protection & Obfuscation/          # Encryption, packing, string obfuscation
├── Dynamic Code Loading & Execution/       # Reflective DLLs, shellcode runners, early-bird APC
├── Environment Manipulation/               # VM/artifacts detection, debugger/analyzer checks
├── File System Manipulation & Anti-Forensics/ # Timestomp, ADS, secure erase, MFT manipulation
├── Process Creation & Injection/            # PPID spoofing, blockdll, phantom DLLs, indirect syscalls
├── Time-Based Evasion - Sandbox Detection/  # Long sleeps, rdtsc checks, execution timing
├── Token Manipulation & Privilege Escalation/ # Token theft, impersonation, bypass UAC
├── UI-Based Evasion/                        # Simulate user activity, detect analysis UI
└── README.md                                # This file
```

## Performance Considerations
- Use asynchronous techniques where possible to reduce detection windows.
- Combine multiple evasion layers for higher success rates.
- Prefer in-memory execution over disk-based artifacts.
- Adapt delays and checks based on target environment.
- Minimize API calls that trigger common EDR hooks.

## Limitations
- Windows-focused (primarily Windows 10/11)
- Some techniques require administrator or SYSTEM privileges.
- Effectiveness decreases against cutting-edge EDR/XDR solutions with behavioral ML.
- Certain methods may be flagged or patched in future Windows updates.
- Not all techniques work reliably in fully hardened environments.

## Security Disclaimer
This framework is intended **exclusively** for:
- Authorized red team engagements.
- Defensive security research.
- Malware analysis training.
- Penetration testing with explicit written permission.

Use only on systems you own or have written authorization to assess.

## References
- MITRE ATT&CK® – Defense Evasion (TA0005)
- Windows Internals (P. Yosifovich et al.)
- Practical Malware Analysis – M. Sikorski & A. Honig
- Red Team Field Manual (RTFM)
- Unprotect Project – Malware Evasion Techniques Database.
- Various public research from SpecterOps, MDSec, Elastic, CrowdStrike, and Microsoft.
