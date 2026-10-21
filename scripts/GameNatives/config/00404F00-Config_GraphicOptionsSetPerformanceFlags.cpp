// IGI native Config_GraphicOptionsSetPerformanceFlags - 0x00404F00 - transcribed from retail igi.exe.
// Retail: cdecl (result, unused, vm). Stores arg 0 at active+0xC, then writes the 6-dword result block {1, ?, 0, 0x3FF00000, 0x567C74, ?}.
#include <cstdint>
#include <cstring>

int __cdecl Qvm_GetIntArg(void* vm, int index); // 0x004B8A20
void* __cdecl Config_GetActiveGraphicOptions(void); // 0x00404590
extern std::int32_t g_537b74[7];

void __cdecl Config_GraphicOptionsSetPerformanceFlags(std::uint32_t* result, int unused, void* vm)
{
    (void)unused;
    std::uint32_t block[6];
    int flags = Qvm_GetIntArg(vm, 0);
    *reinterpret_cast<std::int32_t*>(static_cast<std::uint8_t*>(Config_GetActiveGraphicOptions()) + 0xC) = flags;
    block[0] = 1;
    block[2] = 0;
    block[3] = 0x3FF00000;
    block[4] = 0x567C74;
    std::memcpy(result, block, sizeof(block));
}
