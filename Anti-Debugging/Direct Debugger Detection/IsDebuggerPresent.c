#include <windows.h>
#include <stdio.h>

void checkIsDebuggerPresent()
{
    if(IsDebuggerPresent())
    {
        printf("Debugger Present\n");
        ExitProcess(1);
    }
    else
    {
        printf("No Debugger Detected\n");
    }
}

int main()
{
    printf("Result -> ");
    checkIsDebuggerPresent();
    return 0;
}