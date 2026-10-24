// IGI native AppContext_GetIsActive - 0x0048F170 - transcribed from retail igi.exe.
// Retail: cdecl, no arguments. Returns the is-active flag dword at 0x5C8BFC.
#include <cstdint>
#include <cstring>

extern std::int32_t g_5c8bfc;

int __cdecl AppContext_GetIsActive(void)
{
    return g_5c8bfc;
}
