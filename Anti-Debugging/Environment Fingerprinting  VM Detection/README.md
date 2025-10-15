<h1 align="center">
    Environment Fingerprinting / VM Detection Techniques
</h1>

## Overview
Environment fingerprinting techniques identify sandbox, virtual machine, and analysis environments by examining system properties. These techniques check for telltale signs that indicate the malware is running in a controlled analysis environment rather than a real victim's machine.

## Common APIs
| API | Sophistication | 
|-----|---------------|
| `GetComputerNameA / GetUserNameA` | Basic |
| `GetLogicalProcessorInformation` | Intermediate |

---

## 1. [GetComputerNameA / GetUserNameA](https://github.com/CyberForgeEx/MalAPI-Arsenal/blob/main/Anti-Debugging/Environment%20Fingerprinting%20%20VM%20Detection/GetUserNameA_%26_GetComputerNameA.c)

### Description
The most widely used environment detection technique. Retrieves system and user names to identify common sandbox and analysis environment identifiers through blacklist matching.

### Why It's Popular
- Simple and reliable identification method
- Requires no complex checks or calculations
- Directly targets known analysis platforms

### How It Works
```c
GetComputerNameA(computerName, &size);
GetUserNameA(userName, &size);
```

The technique compares retrieved names against blacklists:

**Common Sandbox Computer Names:**
- `SANDBOX`, `MALWARE`, `VIRUS`, `MALTEST`
- `SAMPLE`, `ANALYSIS`, `CUCKOO`
- `VMWARE`, `VBOX`, `QEMU`

**Common Sandbox User Names:**
- `JOHN DOE`, `MALWARE`, `SANDBOX`
- `CURRENTUSER`, `ADMIN`, `TESTER`
- `SAMPLE`, `VIRUS`, `MALTEST`

### Detection Logic
1. Query system for computer and user names
2. Convert names to uppercase for case-insensitive comparison
3. Check if names contain blacklisted keywords
4. Flag environment as suspicious if matches found

### Key Advantages
- **Zero False Negatives**: Analysis environments often use default/obvious names
- **Low Overhead**: Single API call per check
- **Difficult to Evade**: Requires environment reconfiguration
- **No Special Privileges**: Works with standard user permissions

### Real-World Impact
- Effective against **automated analysis systems**
- Forces analysts to customize environment names
- Combined with other checks for multi-layered evasion

---

## 2. [GetLogicalProcessorInformation](https://github.com/CyberForgeEx/MalAPI-Arsenal/blob/main/Anti-Debugging/Environment%20Fingerprinting%20%20VM%20Detection/GetLogicalProcessorInformation.c)

### Description
Hardware-based VM detection that identifies virtualized environments by analyzing CPU core counts and processor characteristics.

### Why It's Popular
- Detects **resource-constrained sandboxes**
- Identifies VMs with artificially limited CPU resources
- Harder to spoof than software-based checks

### How It Works
```c
GetLogicalProcessorInformation(buffer, &size);
// Count processor cores
// Flag if cores < threshold (typically 2-4)
```

Virtual machines and sandboxes typically allocate:
- **1-3 CPU cores** (real systems: 4-16+ cores)
- Limited processor features
- Reduced cache sizes

### Detection Thresholds
- **Suspicious**: < 3 cores
- **Highly Suspicious**: 1 core + low cache
- **Legitimate Systems**: 4+ cores typical

### Key Advantages
- **Hardware-level detection**: Difficult to fake
- **Resource analysis**: Identifies cost-cutting in analysis infrastructure
- **Multi-factor**: Combines core count, cache size, and features
- **Low false positives**: Modern systems rarely have < 3 cores

---

## Comparison

| Feature | GetComputerName/UserName | GetLogicalProcessorInfo |
|---------|--------------------------|-------------------------|
| **Purpose** | Name-based Detection | Hardware Profiling |
| **Complexity** | Very Low | Medium |
| **Detection Rate** | Very High  | High  |
| **Bypass Difficulty** | Medium | Hard |
| **False Positives** | Very Low | Low-Medium |
| **Evasion Required** | Name Customization | Resource Allocation |

---

## Evasion Techniques (Analyst Perspective)

### Against Name-Based Detection
- Customize VM/sandbox names to appear legitimate
- Use real-world company naming conventions
- Randomize user and computer names per analysis
- Avoid default sandbox installation names

### Against Hardware Detection
- Allocate realistic CPU core counts (4-8 cores)
- Enable all processor features in VM settings
- Configure appropriate cache sizes
- Use nested virtualization for better emulation

---

## Educational Purpose
This documentation is provided for **educational and research purposes only**. Understanding environment fingerprinting techniques is essential for:
- Malware analysis and reverse engineering
- Sandbox development and hardening
- Threat intelligence and detection engineering
- Cybersecurity research and education
- Building robust analysis environments

**Disclaimer**: Implementing anti-analysis techniques in malicious software is illegal. This information is provided solely for defensive cybersecurity purposes.

---

## References
- MALAPI: [GetComputerNameA](https://malapi.io/winapi/GetComputerNameA)
- MALAPI: [GetUserNameA](https://malapi.io/winapi/GetUserNameA)

---

<p align="right">
  Last Updated: October 2025
</p>
