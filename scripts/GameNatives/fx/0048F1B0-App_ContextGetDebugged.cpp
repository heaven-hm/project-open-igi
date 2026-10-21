// IGI native App_ContextGetDebugged - 0x0048F1B0 - transcribed from retail igi.exe.
// Retail: cdecl, no arguments. Returns the debugged flag dword at 0x5C8BF8.
#include <cstdint>
#include <cstring>

extern std::int32_t g_5c8bf8;

int __cdecl App_ContextGetDebugged(void)
{
    return g_5c8bf8;
}
