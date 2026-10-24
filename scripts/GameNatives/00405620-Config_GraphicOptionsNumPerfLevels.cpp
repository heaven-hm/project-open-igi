// IGI native Config_GraphicOptionsNumPerfLevels - 0x00405620 - transcribed from retail igi.exe.
// Retail: cdecl (result, unused, vm). Result block {7, ?, 0, 0x401C0000, 0x567C74, ?}.
#include <cstdint>
#include <cstring>


void __cdecl Config_GraphicOptionsNumPerfLevels(std::uint32_t* result, int unused, void* vm)
{
    (void)unused;
    (void)vm;
    std::uint32_t block[6];
    block[0] = 7;
    block[2] = 0;
    block[3] = 0x401C0000;
    block[4] = 0x567C74;
    std::memcpy(result, block, sizeof(block));
}
