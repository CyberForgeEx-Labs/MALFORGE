#include <windows.h>
#include <stdio.h>
#include <time.h>

#pragma comment(lib, "user32.lib")

// Global variables for demonstrations
HWND g_hwndDemo = NULL;
int g_messageCount = 0;
BOOL g_running = TRUE;
BOOL g_classRegistered = FALSE;

// Custom message types
#define WM_CUSTOM_COMMAND (WM_USER + 1)
#define WM_CUSTOM_DATA (WM_USER + 2)
#define WM_STOP_DEMO (WM_USER + 100)

// Helper function to get message name
const char* GetMessageName(UINT msg) {
    switch(msg) {
        case WM_NULL: return "WM_NULL";
        case WM_CREATE: return "WM_CREATE";
        case WM_DESTROY: return "WM_DESTROY";
        case WM_MOVE: return "WM_MOVE";
        case WM_SIZE: return "WM_SIZE";
        case WM_PAINT: return "WM_PAINT";
        case WM_CLOSE: return "WM_CLOSE";
        case WM_QUIT: return "WM_QUIT";
        case WM_KEYDOWN: return "WM_KEYDOWN";
        case WM_KEYUP: return "WM_KEYUP";
        case WM_CHAR: return "WM_CHAR";
        case WM_LBUTTONDOWN: return "WM_LBUTTONDOWN";
        case WM_LBUTTONUP: return "WM_LBUTTONUP";
        case WM_RBUTTONDOWN: return "WM_RBUTTONDOWN";
        case WM_MOUSEMOVE: return "WM_MOUSEMOVE";
        case WM_TIMER: return "WM_TIMER";
        case WM_COMMAND: return "WM_COMMAND";
        case WM_CUSTOM_COMMAND: return "WM_CUSTOM_COMMAND";
        case WM_CUSTOM_DATA: return "WM_CUSTOM_DATA";
        case WM_STOP_DEMO: return "WM_STOP_DEMO";
        default: return "UNKNOWN";
    }
}

// Window procedure for our demo window
LRESULT CALLBACK DemoWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
        case WM_CUSTOM_COMMAND:
            printf("  [WindowProc] Received WM_CUSTOM_COMMAND: wParam=%llu, lParam=%lld\n", 
                   wParam, lParam);
            return 100; // Custom return value
            
        case WM_CUSTOM_DATA:
            printf("  [WindowProc] Received WM_CUSTOM_DATA: Data=%s\n", (char*)lParam);
            return 200;
            
        case WM_STOP_DEMO:
            printf("  [WindowProc] Received WM_STOP_DEMO\n");
            g_running = FALSE;
            PostQuitMessage(0);
            return 0;
            
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// Create a demo window
HWND CreateDemoWindow() {
    // Only register the class once
    if (!g_classRegistered) {
        WNDCLASSA wc = {0};
        wc.lpfnWndProc = DemoWindowProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = "MessageDemoClass";
        
        if (!RegisterClassA(&wc)) {
            DWORD error = GetLastError();
            if (error != ERROR_CLASS_ALREADY_EXISTS) {
                printf("[-] Failed to register window class (Error: %lu)\n", error);
                return NULL;
            }
        }
        g_classRegistered = TRUE;
    }
    
    HWND hwnd = CreateWindowExA(
        0, "MessageDemoClass", "Message Demo Window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
        NULL, NULL, GetModuleHandle(NULL), NULL
    );
    
    if (!hwnd) {
        printf("[-] Failed to create window (Error: %lu)\n", GetLastError());
        return NULL;
    }
    
    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);
    
    return hwnd;
}

// GetMessageA Blocking message retrieval
void Demo_GetMessage() {
    printf("\n[+] GetMessageA (Blocking Retrieval)\n");
    printf("[+] Sending 3 custom messages...\n\n");
    
    g_hwndDemo = CreateDemoWindow();
    if (!g_hwndDemo) {
        printf("[-] ERROR: Failed to create demo window\n");
        return;
    }
    
    // Post some messages to the queue
    PostMessageA(g_hwndDemo, WM_CUSTOM_COMMAND, 1, 100);
    PostMessageA(g_hwndDemo, WM_CUSTOM_COMMAND, 2, 200);
    PostMessageA(g_hwndDemo, WM_CUSTOM_COMMAND, 3, 300);
    PostMessageA(g_hwndDemo, WM_STOP_DEMO, 0, 0);
    
    MSG msg;
    int count = 0;
    
    printf("[+] Starting message loop with GetMessageA...\n");
    
    // GetMessageA blocks until a message arrives
    while (GetMessageA(&msg, NULL, 0, 0) > 0) {
        count++;
        printf("[Msg #%d] Intercepted: %s (0x%04X) - wParam=%llu, lParam=%lld\n",
               count, GetMessageName(msg.message), msg.message, 
               msg.wParam, msg.lParam);
        
        TranslateMessage(&msg);
        DispatchMessageA(&msg); // Send to window procedure
        
        if (msg.message == WM_STOP_DEMO) {
            printf("\n [+] Stop message received. Exiting loop.\n");
            break;
        }
    }
    
    printf("[+] Total messages processed: %d\n", count);
    DestroyWindow(g_hwndDemo);
    g_hwndDemo = NULL;
}

// PeekMessageA Non-blocking message checking.
void Demo_PeekMessage() {
    printf("\n[+] PeekMessageA (Non-blocking Checking)\n");
    printf("[+] PeekMessageA checks for messages without blocking.\n");
    
    g_hwndDemo = CreateDemoWindow();
    if (!g_hwndDemo) {
        printf("[-] ERROR: Failed to create demo window\n");
        return;
    }
    
    // Post messages with delays
    printf("[+] Posting messages at intervals...\n");
    PostMessageA(g_hwndDemo, WM_CUSTOM_COMMAND, 1, 1000);
    
    MSG msg;
    int iterations = 0;
    int messagesFound = 0;
    
    printf("\n[+] Checking message queue (non-blocking)...\n");
    
    for (int i = 0; i < 10; i++) {
        iterations++;
        
        // PM_REMOVE - Remove message from queue
        // Returns TRUE if message available, FALSE otherwise
        if (PeekMessageA(&msg, g_hwndDemo, 0, 0, PM_REMOVE)) {
            messagesFound++;
            printf("[Iteration %d] Message found: %s\n", 
                   iterations, GetMessageName(msg.message));
            DispatchMessageA(&msg);
        } else {
            printf("[Iteration %d] No message available, continuing work...\n", iterations);
        }
        
        // Post another message mid-loop
        if (i == 5) {
            printf("  -> Posting another message...\n");
            PostMessageA(g_hwndDemo, WM_CUSTOM_COMMAND, 2, 2000);
        }
        
        Sleep(100); // Simulate doing other work
    }
    
    printf("\n[+] Iterations: %d, Messages found: %d\n", iterations, messagesFound);
    DestroyWindow(g_hwndDemo);
    g_hwndDemo = NULL;
}

// PostMessageA vs SendMessageA
void Demo_PostVsSend() {
    printf("\n[+] PostMessageA vs SendMessageA \n");
    printf("[+] PostMessageA: Asynchronous (queued)\n");
    printf("[+] SendMessageA: Synchronous (immediate)\n\n");
    
    g_hwndDemo = CreateDemoWindow();
    if (!g_hwndDemo) {
        printf("[-] ERROR: Failed to create demo window\n");
        return;
    }
    
    // PostMessageA - Asynchronous
    printf("[1] PostMessageA (Asynchronous):\n");
    printf("   Posting message...\n");
    BOOL postResult = PostMessageA(g_hwndDemo, WM_CUSTOM_COMMAND, 10, 1000);
    printf("   PostMessageA returned immediately: %s\n", postResult ? "Success" : "Failed");
    printf("   (Message queued, not processed yet)\n\n");
    
    // SendMessageA - Synchronous
    printf("[2] SendMessageA (Synchronous):\n");
    printf("   Sending message...\n");
    LRESULT sendResult = SendMessageA(g_hwndDemo, WM_CUSTOM_COMMAND, 20, 2000);
    printf("   SendMessageA returned: %lld (message processed immediately)\n", sendResult);
    printf("[+]Return value came from WindowProc\n\n");
    
    // Process the posted message
    printf("[+] Now processing the posted message:\n");
    MSG msg;
    if (PeekMessageA(&msg, g_hwndDemo, 0, 0, PM_REMOVE)) {
        DispatchMessageA(&msg);
    }
    
    DestroyWindow(g_hwndDemo);
    g_hwndDemo = NULL;
}

// PostThreadMessageA Demonstration.
DWORD WINAPI ThreadMessageReceiver(LPVOID param) {
    printf("  [Thread %lu] Started, creating message queue...\n", GetCurrentThreadId());
    
    // Force message queue creation
    MSG msg;
    PeekMessageA(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
    
    printf("  [Thread %lu] Ready to receive messages\n", GetCurrentThreadId());
    
    int count = 0;
    while (GetMessageA(&msg, NULL, 0, 0)) {
        count++;
        printf("  [Thread %lu] Received message #%d: %s - wParam=%llu\n",
               GetCurrentThreadId(), count, GetMessageName(msg.message), msg.wParam);
        
        if (msg.message == WM_QUIT || count >= 5) {
            printf("  [Thread %lu] Exiting...\n", GetCurrentThreadId());
            break;
        }
    }
    
    return 0;
}

void Demo_PostThreadMessage() {
    printf("\n[+] PostThreadMessageA\n");
    printf("[+] Send messages directly to a thread (no window needed).\n\n");
    
    HANDLE hThread = CreateThread(NULL, 0, ThreadMessageReceiver, NULL, 0, NULL);
    if (!hThread) {
        printf("[-] Failed to create thread\n");
        return;
    }
    
    DWORD threadId = GetThreadId(hThread);
    Sleep(100); // Let thread initialize.
    
    printf("[Main] Posting messages to thread %lu...\n", threadId);
    
    for (int i = 1; i <= 5; i++) {
        BOOL result = PostThreadMessageA(threadId, WM_CUSTOM_COMMAND, i, i * 100);
        printf("[Main] Posted message #%d: %s\n", i, result ? "Success" : "Failed");
        Sleep(200);
    }
    
    PostThreadMessageA(threadId, WM_QUIT, 0, 0);
    
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
}

// Callback for SendMessageCallbackA
void CALLBACK MyMessageCallback(HWND hwnd, UINT msg, ULONG_PTR data, LRESULT result) {
    printf("  [Callback] Message processed! Custom data=%llu, Result=%lld\n", data, result);
}

// SendMessageCallbackA Demonstration.
void Demo_SendMessageCallback() {
    printf("\n[+] SendMessageCallbackA\n");
    printf("[+] Asynchronous SendMessage with callback notification.\n\n");
    
    g_hwndDemo = CreateDemoWindow();
    if (!g_hwndDemo) {
        printf("[-] ERROR: Failed to create demo window\n");
        return;
    }
    
    printf("[+] Sending message with callback...\n");
    
    BOOL result = SendMessageCallbackA(
        g_hwndDemo,
        WM_CUSTOM_COMMAND,
        42,
        9999,
        MyMessageCallback,
        12345 // Custom data passed to callback
    );
    
    printf("[+] SendMessageCallbackA returned: %s\n", result ? "Queued" : "Failed");
    printf("[+] Waiting for callback...\n");
    
    // Process messages to trigger callback
    MSG msg;
    for (int i = 0; i < 5; i++) {
        if (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
            DispatchMessageA(&msg);
        }
        Sleep(100);
    }
    
    DestroyWindow(g_hwndDemo);
    g_hwndDemo = NULL;
}

// SendMessageTimeoutA Deomstration.
void Demo_SendMessageTimeout() {
    printf("\n[+] SendMessageTimeoutA \n");
    printf("[+] SendMessage with timeout protection.\n\n");
    
    g_hwndDemo = CreateDemoWindow();
    if (!g_hwndDemo) {
        printf("[-] ERROR: Failed to create demo window\n");
        return;
    }
    
    DWORD_PTR result;
    LRESULT lResult;
    
    printf("[+] Sending message with 2-second timeout...\n");
    
    lResult = SendMessageTimeoutA(
        g_hwndDemo,
        WM_CUSTOM_COMMAND,
        99,
        8888,
        SMTO_NORMAL,      // Flags
        2000,             // 2 second timeout
        &result           // Result pointer
    );
    
    if (lResult) {
        printf("[+] Message processed successfully\n");
        printf(" -> Return value: %llu\n", result);
    } else {
        DWORD error = GetLastError();
        printf("[-] Message failed or timed out\n");
        printf(" -> Error code: %lu\n", error);
        if (error == ERROR_TIMEOUT) {
            printf("[+]  Reason: TIMEOUT\n");
        }
    }
    
    DestroyWindow(g_hwndDemo);
    g_hwndDemo = NULL;
}

// SendNotifyMessageA Demonstration.
void Demo_SendNotifyMessage() {
    printf("\n[+] SendNotifyMessageA \n");
    
    g_hwndDemo = CreateDemoWindow();
    if (!g_hwndDemo) {
        printf("[-] ERROR: Failed to create demo window\n");
        return;
    }
    
    printf("[+] Sending notify message...\n");
    
    BOOL result = SendNotifyMessageA(g_hwndDemo, WM_CUSTOM_COMMAND, 77, 7777);
    
    printf("[+] SendNotifyMessageA returned: %s\n", result ? "Success" : "Failed");
    
    // Process the message
    MSG msg;
    if (PeekMessageA(&msg, g_hwndDemo, 0, 0, PM_REMOVE)) {
        DispatchMessageA(&msg);
    }
    
    DestroyWindow(g_hwndDemo);
    g_hwndDemo = NULL;
}

// Message Intelligence - Traffic Analysis
void Demo_MessageIntelligence() {
    printf("\n[+] Message Intelligence & Traffic Analysis.\n");

    g_hwndDemo = CreateDemoWindow();
    if (!g_hwndDemo) {
        printf("[-] ERROR: Failed to create demo window\n");
        return;
    }
    
    // Send various messages
    printf("[+] Generating message traffic...\n");
    for (int i = 0; i < 10; i++) {
        PostMessageA(g_hwndDemo, WM_CUSTOM_COMMAND, i, i * 10);
    }
    PostMessageA(g_hwndDemo, WM_CUSTOM_DATA, 0, (LPARAM)"Test Data");
    PostMessageA(g_hwndDemo, WM_TIMER, 1, 0);
    PostMessageA(g_hwndDemo, WM_PAINT, 0, 0);
    
    // Analyze message queue
    MSG msg;
    int totalMessages = 0;
    int customCommands = 0;
    int systemMessages = 0;
    DWORD startTime = GetTickCount();
    
    printf("\n[+] Intercepting and analyzing messages...\n");
    
    while (PeekMessageA(&msg, g_hwndDemo, 0, 0, PM_REMOVE)) {
        totalMessages++;
        
        if (msg.message == WM_CUSTOM_COMMAND) customCommands++;
        else if (msg.message < WM_USER) systemMessages++;
        
        printf("[%04d] %s (0x%04X) wParam=%llu lParam=%lld\n",
               totalMessages, GetMessageName(msg.message), msg.message,
               msg.wParam, msg.lParam);
        
        DispatchMessageA(&msg);
    }
    
    DWORD elapsed = GetTickCount() - startTime;
    
    printf("\n[*] Traffic Analysis Status\n");
    printf("[+] Total messages intercepted: %d\n", totalMessages);
    printf("[+] Custom commands: %d\n", customCommands);
    printf("[+] System messages: %d\n", systemMessages);
    printf("[+] Processing time: %lu ms\n", elapsed);
    printf("[+] Average time per message: %.2f ms\n", 
           totalMessages > 0 ? (float)elapsed / totalMessages : 0);
    
    DestroyWindow(g_hwndDemo);
    g_hwndDemo = NULL;
}

int main() 
{
    printf("[*] Message Interception & Intelligence Demonstration\n");
    
    printf("\n[+] Press Enter to start demonstrations...");
    getchar();
    
    Demo_GetMessage();
    printf("\n[+] Press Enter for next demo...");
    getchar();
    
    Demo_PeekMessage();
    printf("\n[+] Press Enter for next demo...");
    getchar();
    
    Demo_PostVsSend();
    printf("\n[+] Press Enter for next demo...");
    getchar();
    
    Demo_PostThreadMessage();
    printf("\n[+] Press Enter for next demo...");
    getchar();
    
    Demo_SendMessageCallback();
    printf("\n[+] Press Enter for next demo...");
    getchar();
    
    Demo_SendMessageTimeout();
    printf("\n[+] Press Enter for next demo...");
    getchar();
    
    Demo_SendNotifyMessage();
    printf("\n[+] Press Enter for next demo...");
    getchar();
    
    Demo_MessageIntelligence();
    
    printf("\n\n[+] All Demonstrations Complete.\n");
    
    printf("\nPress Enter to exit...");
    getchar();
    
    return 0;
}