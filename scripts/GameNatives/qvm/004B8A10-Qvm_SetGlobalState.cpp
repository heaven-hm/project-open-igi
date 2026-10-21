// IGI native Qvm_SetGlobalState - 0x004B8A10 - transcribed from retail igi.exe.
// Retail: cdecl, one argument. Stores it in g_a965a0 and returns it.
#include <cstdint>
#include <cstring>

extern std::int32_t g_a965a0;

int __cdecl Qvm_SetGlobalState(int state)
{
    g_a965a0 = state;
    return state;
}
