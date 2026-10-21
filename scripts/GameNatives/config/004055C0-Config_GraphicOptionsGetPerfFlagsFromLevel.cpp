// IGI native Config_GraphicOptionsGetPerfFlagsFromLevel - 0x004055C0 - transcribed from retail igi.exe.
// Retail: cdecl (result, unused, vm). Reads the flags table at 0x537B74 at index arg 0 twice (no bounds check).
#include <cstdint>
#include <cstring>

int __cdecl Qvm_GetIntArg(void* vm, int index); // 0x004B8A20
void* __cdecl Config_GetActiveGraphicOptions(void); // 0x00404590
extern std::int32_t g_537b74[7];

void __cdecl Config_GraphicOptionsGetPerfFlagsFromLevel(std::uint32_t* result, int unused, void* vm)
{
    (void)unused;
    std::uint32_t block[6];
    block[0] = static_cast<std::uint32_t>(g_537b74[Qvm_GetIntArg(vm, 0)]);
    double value = static_cast<double>(g_537b74[Qvm_GetIntArg(vm, 0)]);
    std::memcpy(&block[2], &value, sizeof(value));
    block[4] = 0x567C74;
    std::memcpy(result, block, sizeof(block));
}
