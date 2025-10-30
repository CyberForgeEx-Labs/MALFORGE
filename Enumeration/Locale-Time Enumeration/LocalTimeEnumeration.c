#include <windows.h>
#include <stdio.h>
#include <time.h>

// Locale enumeration callback count
static int g_LocaleCount = 0;

/*
 * Demonstrates GetSystemTime API usage
 * Retrieves and displays the current system time in UTC
 */
void DemoGetSystemTime(void) {
    SYSTEMTIME st = {0};
    
    printf("\n1. GetSystemTime Demo\n");
    
    // Get current system time (UTC)
    GetSystemTime(&st);
    
    printf("Current System Time (UTC):\n");
    printf("----------------------------\n");
    printf("Date: %04d-%02d-%02d\n", st.wYear, st.wMonth, st.wDay);
    printf("Time: %02d:%02d:%02d.%03d\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    printf("Day of Week: %d ", st.wDayOfWeek);
    
    // Day of week name
    const char* days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    printf("(%s)\n", days[st.wDayOfWeek]);
    
    printf("\nSTRUCTURE DETAILS:\n");
    printf("  wYear:         %u\n", st.wYear);
    printf("  wMonth:        %u\n", st.wMonth);
    printf("  wDayOfWeek:    %u\n", st.wDayOfWeek);
    printf("  wDay:          %u\n", st.wDay);
    printf("  wHour:         %u\n", st.wHour);
    printf("  wMinute:       %u\n", st.wMinute);
    printf("  wSecond:       %u\n", st.wSecond);
    printf("  wMilliseconds: %u\n", st.wMilliseconds);
    
    printf("  - SYSTEMTIME structure size: %zu bytes\n", sizeof(SYSTEMTIME));
    
    printf("\n");
}

/*
 * Demonstrates GetSystemTimeAsFileTime API usage
 * Retrieves system time as FILETIME and performs conversions
 */
void DemoGetSystemTimeAsFileTime(void) {
    FILETIME ft = {0};
    ULARGE_INTEGER uli = {0};
    SYSTEMTIME st = {0};
    
    printf("\n2. GetSystemTimeAsFileTime Demo\n");
    
    // Get system time as FILETIME (most efficient)
    GetSystemTimeAsFileTime(&ft);
    
    printf("FILETIME Structure:\n");
    printf("----------------------------\n");
    printf("dwLowDateTime:  0x%08lX (%lu)\n", ft.dwLowDateTime, ft.dwLowDateTime);
    printf("dwHighDateTime: 0x%08lX (%lu)\n", ft.dwHighDateTime, ft.dwHighDateTime);
    
    // Convert FILETIME to 64-bit integer
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    
    printf("\n64-bit FILETIME Value:\n");
    printf("----------------------------\n");
    printf("Decimal: %llu\n", uli.QuadPart);
    printf("Hex:     0x%016llX\n", uli.QuadPart);
    
    // FILETIME represents 100-nanosecond intervals since Jan 1, 1601
    printf("\nTime Calculations:\n");
    printf("----------------------------\n");
    printf("100-nanosecond intervals since Jan 1, 1601: %llu\n", uli.QuadPart);
    printf("Seconds since Jan 1, 1601:                   %llu\n", uli.QuadPart / 10000000ULL);
    printf("Days since Jan 1, 1601:                      %llu\n", uli.QuadPart / 10000000ULL / 86400ULL);
    
    // Convert to UNIX timestamp (seconds since Jan 1, 1970)
    // Difference between 1601 and 1970 is 11644473600 seconds
    ULONGLONG unixTime = (uli.QuadPart / 10000000ULL) - 11644473600ULL;
    printf("UNIX Timestamp (seconds since Jan 1, 1970): %llu\n", unixTime);
    
    // Convert FILETIME to SYSTEMTIME for readable format
    if (FileTimeToSystemTime(&ft, &st)) {
        printf("\nConverted to SYSTEMTIME:\n");
        printf("----------------------------\n");
        printf("Date: %04d-%02d-%02d\n", st.wYear, st.wMonth, st.wDay);
        printf("Time: %02d:%02d:%02d.%03d UTC\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    }
    
    printf("  - FILETIME structure size: %zu bytes\n", sizeof(FILETIME));
    
    printf("\n");
}

/*
 * Demonstrates GetThreadLocale API usage
 * Retrieves and analyzes the current thread's locale identifier
 */
void DemoGetThreadLocale(void) {
    LCID lcid = 0;
    char buffer[256] = {0};
    int result = 0;
    
    printf("3. GetThreadLocale Demo\n");
    
    // Get current thread's locale identifier
    lcid = GetThreadLocale();
    
    printf("Thread Locale Information:\n");
    printf("---------------------------\n");
    printf("LCID (Locale ID): 0x%08lX (%lu)\n", lcid, lcid);
    
    
    printf("\nDate & Time Format:\n");
    printf("----------------------------\n");
    
    result = GetLocaleInfoA(lcid, LOCALE_SSHORTDATE, buffer, sizeof(buffer));
    if (result > 0) {
        printf("Short Date Format:     %s\n", buffer);
    }
    
    result = GetLocaleInfoA(lcid, LOCALE_SLONGDATE, buffer, sizeof(buffer));
    if (result > 0) {
        printf("Long Date Format:      %s\n", buffer);
    }
    
    result = GetLocaleInfoA(lcid, LOCALE_STIMEFORMAT, buffer, sizeof(buffer));
    if (result > 0) {
        printf("Time Format:           %s\n", buffer);
    }
    
    printf("\n");
}


int main(void) 
{

    printf("\nLOCALE/TIME ENUMERATION DEMONSTRATION\n");
    
    DemoGetSystemTime();
    DemoGetSystemTimeAsFileTime();
    DemoGetThreadLocale();
    
    printf("Demonstration Complete\n");
    
    return 0;
}