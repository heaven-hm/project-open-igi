// IGI native Qvm_SetOpcodeHandler - 0x004BD3D0 - transcribed from retail igi.exe.
// Retail: cdecl (group, index, handler). Stores handler at g_a96108[index + group * 6] and returns it.
#include <cstdint>
#include <cstring>

extern std::int32_t g_a96108[];

int __cdecl Qvm_SetOpcodeHandler(int group, int index, int handler)
{
    g_a96108[index + group * 6] = handler;
    return handler;
}
