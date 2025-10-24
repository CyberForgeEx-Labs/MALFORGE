<h1 align="center">
   <a href="https://github.com/CyberForgeEx/MALFORGE_API-ARSENAL/blob/main/Enumeration/Network%20Resource%20Enumeration/NetworkResourceEnumeration.c">Network Resource Enumeration Techniques</a>
</h1>

## Overview
Network resource enumeration is a fundamental Windows API capability for discovering network shares, adapters, connections, and mapping network topology. This documentation covers advanced APIs for enumerating network resources, managing network connections, and analyzing network infrastructure.

---

## Core APIs

| API | Purpose | Sophistication |
|-----|---------|----------------|
| `NetShareEnum` | Enumerate network shares on server | Basic |
| `NetShareGetInfo` | Get detailed share information | Basic |
| `NetShareCheck` | Check if device is shared | Basic |
| `WNetEnumResource` | Enumerate network resources | Intermediate |
| `WNetAddConnection2A` | Connect to network resource | Intermediate |
| `WNetAddConnectionA` | Make connection to network resource | Basic |
| `GetAdaptersInfo` | Get network adapter information | Intermediate |
| `GetIpNetTable` | Get ARP table entries | Intermediate |

---

## 1. [NetShareEnum](https://github.com/CyberForgeEx/MALFORGE_API-ARSENAL/blob/main/Enumeration/Network%20Resource%20Enumeration/NetworkResourceEnumeration.c#L40)
### Description
Enumerates all network shares on a server, providing information about shared resources including disk drives, printers, and IPC resources.

### Key Features
- Lists all network shares on local or remote systems
- Supports multiple information levels (0, 1, 2, 502)
- Retrieves share names, types, and remarks
- Handles large result sets with resume capability
- Can target specific servers

### Use Cases
- Network share discovery
- Share inventory management
- Security auditing
- Network resource mapping
- Administrative automation

### Example Output
```
Total Shares Found: 5

Share #1:
  Name: C$
  Type: Disk Drive
  Remark: Default share

Share #2:
  Name: IPC$
  Type: IPC
  Remark: Remote IPC
```

---

## 2. [NetShareGetInfo](https://github.com/CyberForgeEx/MALFORGE_API-ARSENAL/blob/main/Enumeration/Network%20Resource%20Enumeration/NetworkResourceEnumeration.c#L102)
### Description
Retrieves detailed information about a specific network share including path, permissions, user limits, and current connections.

### Key Features
- Multiple information levels (0, 1, 2, 502, 503)
- Detailed share configuration
- Permission information
- Current user count
- Maximum user limits
- Share paths and security descriptors

### Use Cases
- Share configuration analysis
- Permission auditing
- Connection monitoring
- Share usage tracking
- Administrative reporting

### Example Output
```
Share Information for 'C$':
  Name: C$
  Path: C:\
  Remark: Default share
  Max Users: 4294967295
  Current Users: 0
  Permissions: 0x00000001
```

---

## 3. [NetShareCheck](https://github.com/CyberForgeEx/MALFORGE_API-ARSENAL/blob/main/Enumeration/Network%20Resource%20Enumeration/NetworkResourceEnumeration.c#L134)
### Description
Determines whether a specific device (drive letter or path) is shared on the network and returns the share type.

### Share Types
| Type | Constant | Description |
|------|----------|-------------|
| Disk | STYPE_DISKTREE | Disk drive share |
| Print | STYPE_PRINTQ | Print queue share |
| Device | STYPE_DEVICE | Communication device |
| IPC | STYPE_IPC | Interprocess communication |
| Special | STYPE_SPECIAL | Special share ($ suffix) |

### Use Cases
- Quick share status verification
- Device share detection
- Security assessment
- Share type identification
- Administrative validation

### Example Output
```
Checking device 'C:':
  Status: Device IS shared
  Type: Disk Drive
```

---

## 4. [WNetEnumResource](https://github.com/CyberForgeEx/MALFORGE_API-ARSENAL/blob/main/Enumeration/Network%20Resource%20Enumeration/NetworkResourceEnumeration.c#L178)
### Description
Enumerates network resources including servers, shares, domains, and workgroups in a hierarchical manner.

### Key Features
- Hierarchical resource enumeration
- Multiple resource types support
- Provider-specific enumeration
- Network topology discovery
- Domain and workgroup listing
- Recursive enumeration capability

### Use Cases
- Network topology mapping
- Resource discovery
- Network browser implementation
- Domain enumeration
- Enterprise network analysis

### Example Output
```
Network Resources:

Resource #1:
  Remote Name: \\SERVER01
  Provider: Microsoft Windows Network
  Type: Disk

Resource #2:
  Remote Name: \\SERVER01\Share
  Type: Disk
```

---

## 5. [WNetAddConnection2A / WNetAddConnectionA](https://github.com/CyberForgeEx/MALFORGE_API-ARSENAL/blob/main/Enumeration/Network%20Resource%20Enumeration/NetworkResourceEnumeration.c#L260)
### Description
Creates a connection to a network resource, optionally mapping it to a local drive letter, with support for authentication and persistent connections.

### Key Features
- Drive letter mapping
- UNC path connections
- Authentication support
- Persistent connection option
- Interactive credential prompting
- Multiple provider support

### Connection Flags
| Flag | Purpose |
|------|---------|
| CONNECT_UPDATE_PROFILE | Make connection persistent |
| CONNECT_INTERACTIVE | Allow UI for credentials |
| CONNECT_PROMPT | Always prompt for credentials |
| CONNECT_REDIRECT | Redirect local device |

### Use Cases
- Network drive mapping
- Automated connections
- Script-based mounting
- User profile connections
- Application resource access

### Example Output
```
Attempting connection:
  Local: Z:
  Remote: \\SERVER\Share
  Status: Connection successful!
  Disconnected.
```

---

## 6. [GetAdaptersInfo](https://github.com/CyberForgeEx/MALFORGE_API-ARSENAL/blob/main/Enumeration/Network%20Resource%20Enumeration/NetworkResourceEnumeration.c#L300)
### Description
Retrieves comprehensive information about all network adapters on the local system including IP addresses, MAC addresses, DHCP configuration, and gateway information.

### Key Features
- Complete adapter enumeration
- MAC address retrieval
- IP configuration details
- DHCP information
- DNS server information
- Gateway configuration
- Multiple IP address support

### Use Cases
- Network configuration analysis
- MAC address collection
- IP address inventory
- DHCP status monitoring
- Network troubleshooting
- Asset management

### Example Output
```
Adapter #1:
  Name: {E8B3C6F7-4F2A-4B9E-8D1C-3A5F6B2E9C1D}
  Description: Intel(R) Ethernet Connection
  MAC Address: 00-1A-2B-3C-4D-5E
  IP Address: 192.168.1.100
  Subnet Mask: 255.255.255.0
  Gateway: 192.168.1.1
  DHCP Enabled: Yes
  DHCP Server: 192.168.1.1
```

---

## 7. [GetIpNetTable](https://github.com/CyberForgeEx/MALFORGE_API-ARSENAL/blob/main/Enumeration/Network%20Resource%20Enumeration/NetworkResourceEnumeration.c#L365)
### Description
Retrieves the IP-to-physical address mapping table (ARP table), showing the relationship between IP addresses and MAC addresses on the local network.

### Key Features
- Complete ARP table enumeration
- IP to MAC address mapping
- Entry type identification (static/dynamic)
- Interface index information
- Real-time ARP cache view
- Network neighbor discovery

### Entry Types
| Type | Constant | Description |
|------|----------|-------------|
| Other | MIB_IPNET_TYPE_OTHER | Other type |
| Invalid | MIB_IPNET_TYPE_INVALID | Invalid entry |
| Dynamic | MIB_IPNET_TYPE_DYNAMIC | Dynamic entry |
| Static | MIB_IPNET_TYPE_STATIC | Static entry |

### Use Cases
- Network troubleshooting
- ARP cache analysis
- IP-MAC mapping
- Network discovery
- Security monitoring
- Duplicate IP detection

### Example Output
```
ARP Table Entries: 12

IP Address           Physical Address     Type
--------------------------------------------------------
192.168.1.1          00-1A-2B-3C-4D-5E    Dynamic
192.168.1.50         11-22-33-44-55-66    Dynamic
192.168.1.100        AA-BB-CC-DD-EE-FF    Static
```

---

## Comparison

| Feature | NetShare APIs | WNet APIs | Adapter/ARP Info |
|---------|---------------|-----------|------------------|
| **Speed** | Fast | Moderate | Fast |
| **Memory Overhead** | Low | Moderate | Moderate |
| **Detail Level** | Share Info | Resources | Network Config |
| **Network Traffic** | Low | Moderate | None (Local) |
| **Privileges Required** | User/Admin | User | User |
| **Primary Use** | Share Management | Connections | Configuration |

---


## Required Libraries
- `netapi32.lib` - Network Management API for share enumeration
- `mpr.lib` - Multiple Provider Router for network connections
- `iphlpapi.lib` - IP Helper API for network adapter and ARP information
- `ws2_32.lib` - Windows Sockets for network address conversion

---

## Common Applications

### Network Management Tools
- Network inventory systems
- Asset management software
- Network monitoring tools
- Configuration management systems

### Security Tools
- Network scanners
- Vulnerability assessment tools
- Penetration testing frameworks
- Security audit tools

### Administrative Tools
- Network drive mapping utilities
- Share management tools
- Network configuration tools
- System administration scripts

---

## Security Considerations

### Defensive Perspective
- Monitor for unusual network enumeration activity
- Implement least-privilege access to shares
- Use strong authentication for network resources
- Audit administrative share access
- Monitor ARP table for anomalies

### Offensive Perspective (Ethical Testing)
- Network reconnaissance
- Share enumeration for pentesting
- Lateral movement discovery
- Network topology mapping
- Credential validation

---

## API References
- MSDN: [NetShareEnum](https://docs.microsoft.com/en-us/windows/win32/api/lmshare/nf-lmshare-netshareenum)
- MSDN: [NetShareGetInfo](https://docs.microsoft.com/en-us/windows/win32/api/lmshare/nf-lmshare-netsharegetinfo)
- MSDN: [NetShareCheck](https://docs.microsoft.com/en-us/windows/win32/api/lmshare/nf-lmshare-netsharecheck)
- MSDN: [WNetEnumResource](https://docs.microsoft.com/en-us/windows/win32/api/winnetwk/nf-winnetwk-wnetenumresourcea)
- MSDN: [WNetAddConnection2](https://docs.microsoft.com/en-us/windows/win32/api/winnetwk/nf-winnetwk-wnetaddconnection2a)
- MSDN: [GetAdaptersInfo](https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-getadaptersinfo)
- MSDN: [GetIpNetTable](https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-getipnettable)
- MSDN: [WNetCloseEnum](https://docs.microsoft.com/en-us/windows/win32/api/winnetwk/nf-winnetwk-wnetcloseenum)

---

### Performance Metrics

| Operation | Typical Time | Network Impact |
|-----------|--------------|----------------|
| NetShareEnum (local) | < 10ms | None |
| NetShareEnum (remote) | 50-500ms | Low |
| WNetEnumResource | 100-2000ms | Moderate |
| GetAdaptersInfo | < 5ms | None |
| GetIpNetTable | < 10ms | None |
| WNetAddConnection2 | 500-5000ms | High |

---

### Common Error Codes

| Error Code | Constant | Meaning | Solution |
|------------|----------|---------|----------|
| 5 | ERROR_ACCESS_DENIED | Insufficient permissions | Run as administrator |
| 53 | ERROR_BAD_NETPATH | Network path not found | Check network path |
| 67 | ERROR_BAD_NET_NAME | Invalid network name | Verify server name |
| 1219 | ERROR_SESSION_CREDENTIAL_CONFLICT | Multiple credentials | Disconnect existing connections |
| 2138 | NERR_DeviceNotShared | Device not shared | Verify share exists |
| 2250 | NERR_NetworkError | Network error | Check network connectivity |

---

<p align="right">
  Last Updated: October 2025
</p>
