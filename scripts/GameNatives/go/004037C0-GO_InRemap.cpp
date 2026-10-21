// IGI native GO_InRemap - 0x004037C0 - transcribed from retail igi.exe.
// Retail: cdecl (result, unused, vm). Appends a 12-byte entry (arg0, arg1, arg2 & 0x3FFFFFFF) to the remap table at 0xBC25BC for the current group g_bc2380; the per-group counter lives at 0xBC2358 + 1380 * g_bc2380.
#include <cstdint>
#include <cstring>

int __cdecl Qvm_GetIntArg(void* vm, int index); // 0x004B8A20
void* __cdecl Config_GetActiveGraphicOptions(void); // 0x00404590
extern std::int32_t g_537b74[7];
extern std::int32_t g_bc2380;
extern std::uint8_t g_bc2358[];
extern std::uint8_t g_bc25bc[];

void __cdecl GO_InRemap(std::uint32_t* result, int unused, void* vm)
{
    (void)unused;
    std::uint32_t block[6];
    std::int32_t* counter = reinterpret_cast<std::int32_t*>(g_bc2358 + g_bc2380 * 1380);
    std::int32_t* entry = reinterpret_cast<std::int32_t*>(g_bc25bc + 12 * (g_bc2380 * 115 + *counter - 115));
    entry[0] = Qvm_GetIntArg(vm, 0);
    entry[1] = Qvm_GetIntArg(vm, 1);
    entry[2] = Qvm_GetIntArg(vm, 2) & 0x3FFFFFFF;
    *counter = *counter + 1;
    block[0] = 0;
    block[2] = 0;
    block[3] = 0;
    block[4] = 0x567C74;
    std::memcpy(result, block, sizeof(block));
}
