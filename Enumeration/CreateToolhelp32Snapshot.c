/*
 Demostrtion of the CreateToolhelp32Snapshot function in C.
*/

#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>


// Function to list all processes and save them to a file.
void listProcessesToFile(const char* filename) {
    FILE *file = fopen(filename, "w");  // Open file in write mode
    if (!file) {
        printf("Failed to create file: %s\n", filename);
        return;
    }

    // Take a snapshot of all processes in the system.
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        fprintf(file, "Failed to take snapshot of processes.\n");
        fclose(file);
        return;
    }
    // Initialize the PROCESSENTRY32 structure.
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Retrieve information about the first process.
    // And then continue to retrieve information about the rest of the processes.
    if (Process32First(hSnapshot, &pe32)) {
        do {
            fprintf(file, "Process ID: %u, Name: %s\n", pe32.th32ProcessID, pe32.szExeFile);
        } while (Process32Next(hSnapshot, &pe32));
    } else {
        fprintf(file, "Failed to retrieve process information.\n");
    }

    // Clean up: close the snapshot handle and the file.
    CloseHandle(hSnapshot);
    fclose(file);
    printf("Process list saved to: %s\n", filename);
}

// Main function to execute the process listing.
int main() {
    const char* filename = "processes.txt";
    listProcessesToFile(filename);
    return 0;
}

//Execution instructions:
// 1. Compile the code using a C compiler (e.g., gcc main.c -o process_list.exe).
// 2. Run the compiled executable (process_list.exe).
// 3. The process list will be saved to a file named "processes.txt" in the current directory.
