#include <windows.h>
#include <lm.h>
#include <winnetwk.h>
#include <iphlpapi.h>
#include <stdio.h>

#pragma comment(lib, "netapi32.lib")
#pragma comment(lib, "mpr.lib")
#pragma comment(lib, "iphlpapi.lib")

// Function prototypes
void DemoNetShareEnum();
void DemoNetShareGetInfo();
void DemoNetShareCheck();
void DemoWNetEnumResource();
void DemoWNetAddConnection();
void DemoGetAdaptersInfo();
void DemoGetIpNetTable();

int main() 
{

    printf("Network Resource Enumeration Demonstration\n");

    DemoNetShareEnum();
    DemoNetShareGetInfo();
    DemoNetShareCheck();
    DemoWNetEnumResource();
    DemoWNetAddConnection();
    DemoGetAdaptersInfo();
    DemoGetIpNetTable();

    printf("\n\nPress Enter to exit...");
    getchar();
    return 0;
}


// NetShareEnum - Enumerate all network shares
void DemoNetShareEnum() {
    printf("1. NetShareEnum - Enumerating Network Shares\n");
    printf("============================================\n");

    PSHARE_INFO_1 pBuf = NULL;
    PSHARE_INFO_1 pTmpBuf;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    DWORD dwResumeHandle = 0;
    NET_API_STATUS nStatus;

    // Call NetShareEnum to enumerate shares
    nStatus = NetShareEnum(
        NULL,                    // Local computer
        1,                       // Information level
        (LPBYTE*)&pBuf,         // Buffer
        MAX_PREFERRED_LENGTH,    // Max buffer size
        &dwEntriesRead,         // Entries read
        &dwTotalEntries,        // Total entries
        &dwResumeHandle         // Resume handle
    );

    if (nStatus == NERR_Success || nStatus == ERROR_MORE_DATA) {
        printf("Total Shares Found: %lu\n\n", dwTotalEntries);
        
        pTmpBuf = pBuf;
        for (DWORD i = 0; i < dwEntriesRead; i++) {
            printf("Share #%lu:\n", i + 1);
            printf("  Name: %S\n", pTmpBuf->shi1_netname);
            printf("  Type: ");
            
            switch (pTmpBuf->shi1_type & ~STYPE_SPECIAL) {
                case STYPE_DISKTREE:
                    printf("Disk Drive\n");
                    break;
                case STYPE_PRINTQ:
                    printf("Print Queue\n");
                    break;
                case STYPE_DEVICE:
                    printf("Communication Device\n");
                    break;
                case STYPE_IPC:
                    printf("IPC\n");
                    break;
                default:
                    printf("Unknown\n");
            }
            
            if (pTmpBuf->shi1_remark != NULL)
                printf("  Remark: %S\n", pTmpBuf->shi1_remark);
            
            printf("\n");
            pTmpBuf++;
        }

        NetApiBufferFree(pBuf);
    } else {
        printf("NetShareEnum failed with error: %lu\n", nStatus);
    }
}

// NetShareGetInfo - Get detailed share information
void DemoNetShareGetInfo() {
    printf("2. NetShareGetInfo - Getting Share Details\n");
    printf("===========================================\n");

    PSHARE_INFO_2 pBuf = NULL;
    NET_API_STATUS nStatus;
    LPCWSTR shareName = L"C$";  // Administrative share

    nStatus = NetShareGetInfo(
        NULL,           // Local computer
        shareName,      // Share name
        2,              // Information level
        (LPBYTE*)&pBuf // Buffer
    );

    if (nStatus == NERR_Success) {
        printf("Share Information for '%S':\n", shareName);
        printf("  Name: %S\n", pBuf->shi2_netname);
        printf("  Path: %S\n", pBuf->shi2_path);
        printf("  Remark: %S\n", pBuf->shi2_remark ? pBuf->shi2_remark : L"(none)");
        printf("  Max Users: %lu\n", pBuf->shi2_max_uses);
        printf("  Current Users: %lu\n", pBuf->shi2_current_uses);
        printf("  Permissions: 0x%lx\n", pBuf->shi2_permissions);

        NetApiBufferFree(pBuf);
    } else {
        printf("NetShareGetInfo failed with error: %lu\n", nStatus);
        printf("(Note: Administrative shares may require elevated privileges)\n");
    }
}

// NetShareCheck - Check if device is shared
void DemoNetShareCheck() {
    printf("3. NetShareCheck - Checking Device Share Status\n");
    printf("================================================\n");

    DWORD dwType;
    NET_API_STATUS nStatus;
    LPCWSTR device = L"C:";

    nStatus = NetShareCheck(
        NULL,       // Local computer
        device,     // Device name
        &dwType     // Share type
    );

    printf("Checking device '%S':\n", device);
    
    if (nStatus == NERR_Success) {
        printf("  Status: Device IS shared\n");
        printf("  Type: ");
        
        switch (dwType) {
            case STYPE_DISKTREE:
                printf("Disk Drive\n");
                break;
            case STYPE_PRINTQ:
                printf("Print Queue\n");
                break;
            case STYPE_DEVICE:
                printf("Communication Device\n");
                break;
            case STYPE_IPC:
                printf("IPC\n");
                break;
            default:
                printf("Unknown (0x%lx)\n", dwType);
        }
    } else if (nStatus == NERR_DeviceNotShared) {
        printf("  Status: Device is NOT shared\n");
    } else {
        printf("  NetShareCheck failed with error: %lu\n", nStatus);
    }
}

// WNetEnumResource - Enumerate network resources
void DemoWNetEnumResource() {
    printf("4. WNetEnumResource - Enumerating Network Resources\n");
    printf("====================================================\n");

    HANDLE hEnum;
    DWORD dwResult;
    LPNETRESOURCE lpnrLocal;
    DWORD cbBuffer = 16384;
    DWORD cEntries = 0xFFFFFFFF;

    // Start enumeration
    dwResult = WNetOpenEnum(
        RESOURCE_GLOBALNET,  // Enumerate all network resources
        RESOURCETYPE_ANY,    // All resource types
        0,                   // Enumerate all resources
        NULL,                // NULL for top level
        &hEnum              // Enumeration handle
    );

    if (dwResult != NO_ERROR) {
        printf("WNetOpenEnum failed with error: %lu\n", dwResult);
        return;
    }

    lpnrLocal = (LPNETRESOURCE)GlobalAlloc(GPTR, cbBuffer);
    if (lpnrLocal == NULL) {
        printf("Memory allocation failed\n");
        WNetCloseEnum(hEnum);
        return;
    }

    printf("Network Resources:\n\n");

    do {
        ZeroMemory(lpnrLocal, cbBuffer);
        cEntries = 0xFFFFFFFF;
        
        dwResult = WNetEnumResource(
            hEnum,          // Enumeration handle
            &cEntries,      // Number of entries
            lpnrLocal,      // Buffer
            &cbBuffer       // Buffer size
        );

        if (dwResult == NO_ERROR) {
            for (DWORD i = 0; i < cEntries; i++) {
                printf("Resource #%lu:\n", i + 1);
                
                if (lpnrLocal[i].lpLocalName)
                    printf("  Local Name: %s\n", lpnrLocal[i].lpLocalName);
                if (lpnrLocal[i].lpRemoteName)
                    printf("  Remote Name: %s\n", lpnrLocal[i].lpRemoteName);
                if (lpnrLocal[i].lpProvider)
                    printf("  Provider: %s\n", lpnrLocal[i].lpProvider);
                
                printf("  Type: ");
                switch (lpnrLocal[i].dwType) {
                    case RESOURCETYPE_DISK:
                        printf("Disk\n");
                        break;
                    case RESOURCETYPE_PRINT:
                        printf("Print\n");
                        break;
                    case RESOURCETYPE_ANY:
                        printf("Any\n");
                        break;
                    default:
                        printf("Unknown\n");
                }
                printf("\n");
            }
        } else if (dwResult != ERROR_NO_MORE_ITEMS) {
            printf("WNetEnumResource failed with error: %lu\n", dwResult);
        }

    } while (dwResult != ERROR_NO_MORE_ITEMS);

    GlobalFree(lpnrLocal);
    WNetCloseEnum(hEnum);
}

// WNetAddConnection - Connect to network resource
void DemoWNetAddConnection() {
    printf("5. WNetAddConnection - Network Connection Demo\n");
    printf("===============================================\n");

    printf("This demo shows how to connect to network resources.\n");
    printf("Note: Requires valid network path and credentials.\n\n");

    // Example connection (will likely fail without valid credentials)
    NETRESOURCE nr;
    ZeroMemory(&nr, sizeof(NETRESOURCE));
    
    nr.dwType = RESOURCETYPE_DISK;
    nr.lpLocalName = "Z:";
    nr.lpRemoteName = "\\\\SERVER\\Share";
    nr.lpProvider = NULL;

    printf("Attempting connection:\n");
    printf("  Local: %s\n", nr.lpLocalName);
    printf("  Remote: %s\n", nr.lpRemoteName);

    DWORD dwResult = WNetAddConnection2A(
        &nr,            // Network resource
        NULL,           // Password
        NULL,           // Username
        0               // Flags
    );

    if (dwResult == NO_ERROR) {
        printf("  Status: Connection successful!\n");
        
        // Disconnect
        WNetCancelConnection2A(nr.lpLocalName, 0, TRUE);
        printf("  Disconnected.\n");
    } else {
        printf("  Status: Connection failed (error: %lu)\n", dwResult);
        printf("  (This is expected without valid credentials)\n");
    }
}

// GetAdaptersInfo - Get network adapter information
void DemoGetAdaptersInfo() {
    printf("6. GetAdaptersInfo - Network Adapter Information\n");
    printf("=================================================\n");

    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;
    DWORD dwRetVal = 0;
    ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);

    pAdapterInfo = (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));
    if (pAdapterInfo == NULL) {
        printf("Error allocating memory\n");
        return;
    }

    // Make an initial call to get the necessary size
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        free(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);
        if (pAdapterInfo == NULL) {
            printf("Error allocating memory\n");
            return;
        }
    }

    dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);
    
    if (dwRetVal == NO_ERROR) {
        pAdapter = pAdapterInfo;
        int adapterNum = 1;
        
        while (pAdapter) {
            printf("Adapter #%d:\n", adapterNum++);
            printf("  Name: %s\n", pAdapter->AdapterName);
            printf("  Description: %s\n", pAdapter->Description);
            
            printf("  MAC Address: ");
            for (UINT i = 0; i < pAdapter->AddressLength; i++) {
                if (i == (pAdapter->AddressLength - 1))
                    printf("%.2X\n", (int)pAdapter->Address[i]);
                else
                    printf("%.2X-", (int)pAdapter->Address[i]);
            }
            
            printf("  IP Address: %s\n", pAdapter->IpAddressList.IpAddress.String);
            printf("  Subnet Mask: %s\n", pAdapter->IpAddressList.IpMask.String);
            printf("  Gateway: %s\n", pAdapter->GatewayList.IpAddress.String);
            printf("  DHCP Enabled: %s\n", pAdapter->DhcpEnabled ? "Yes" : "No");
            
            if (pAdapter->DhcpEnabled) {
                printf("  DHCP Server: %s\n", pAdapter->DhcpServer.IpAddress.String);
            }
            
            printf("\n");
            pAdapter = pAdapter->Next;
        }
    } else {
        printf("GetAdaptersInfo failed with error: %lu\n", dwRetVal);
    }

    if (pAdapterInfo)
        free(pAdapterInfo);
}

// GetIpNetTable - Get ARP table
void DemoGetIpNetTable() {
    printf("7. GetIpNetTable - IP-to-Physical Address Mapping\n");
    printf("==================================================\n");

    PMIB_IPNETTABLE pIpNetTable;
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;

    // Get the size needed
    GetIpNetTable(NULL, &dwSize, FALSE);
    
    pIpNetTable = (MIB_IPNETTABLE*)malloc(dwSize);
    if (pIpNetTable == NULL) {
        printf("Error allocating memory\n");
        return;
    }

    dwRetVal = GetIpNetTable(pIpNetTable, &dwSize, FALSE);
    
    if (dwRetVal == NO_ERROR) {
        printf("ARP Table Entries: %lu\n\n", pIpNetTable->dwNumEntries);
        printf("%-20s %-20s %-10s\n", "IP Address", "Physical Address", "Type");
        printf("--------------------------------------------------------\n");
        
        for (DWORD i = 0; i < pIpNetTable->dwNumEntries; i++) {
            MIB_IPNETROW ipRow = pIpNetTable->table[i];
            
            // Print IP address
            IN_ADDR ipAddr;
            ipAddr.S_un.S_addr = (u_long)ipRow.dwAddr;
            printf("%-20s ", inet_ntoa(ipAddr));
            
            // Print physical address
            if (ipRow.dwPhysAddrLen > 0) {
                for (DWORD j = 0; j < ipRow.dwPhysAddrLen; j++) {
                    if (j == (ipRow.dwPhysAddrLen - 1))
                        printf("%.2X ", (int)ipRow.bPhysAddr[j]);
                    else
                        printf("%.2X-", (int)ipRow.bPhysAddr[j]);
                }
            } else {
                printf("N/A              ");
            }
            
            // Print type
            switch (ipRow.dwType) {
                case MIB_IPNET_TYPE_OTHER:
                    printf("Other\n");
                    break;
                case MIB_IPNET_TYPE_INVALID:
                    printf("Invalid\n");
                    break;
                case MIB_IPNET_TYPE_DYNAMIC:
                    printf("Dynamic\n");
                    break;
                case MIB_IPNET_TYPE_STATIC:
                    printf("Static\n");
                    break;
                default:
                    printf("Unknown\n");
            }
        }
    } else {
        printf("GetIpNetTable failed with error: %lu\n", dwRetVal);
    }

    if (pIpNetTable)
        free(pIpNetTable);
}