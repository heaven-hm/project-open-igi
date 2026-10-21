// IGI native Config_GraphicOptionsGetPerformanceFlags - 0x00404F60 - transcribed from retail igi.exe.
// Retail: cdecl (result, unused, vm). Result block {active+0xC, ?, (double)(active+0xC), 0x567C74, ?}.
#include <cstdint>
#include <cstring>

int __cdecl Qvm_GetIntArg(void* vm, int index); // 0x004B8A20
void* __cdecl Config_GetActiveGraphicOptions(void); // 0x00404590
extern std::int32_t g_537b74[7];

void __cdecl Config_GraphicOptionsGetPerformanceFlags(std::uint32_t* result, int unused, void* vm)
{
    (void)unused;
    (void)vm;
    std::uint32_t block[6];
    block[0] = static_cast<std::uint32_t>(*reinterpret_cast<std::int32_t*>(static_cast<std::uint8_t*>(Config_GetActiveGraphicOptions()) + 0xC));
    double value = static_cast<double>(*reinterpret_cast<std::int32_t*>(static_cast<std::uint8_t*>(Config_GetActiveGraphicOptions()) + 0xC));
    std::memcpy(&block[2], &value, sizeof(value));
    block[4] = 0x567C74;
    std::memcpy(result, block, sizeof(block));
}
