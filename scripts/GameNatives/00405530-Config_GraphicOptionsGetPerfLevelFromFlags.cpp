// IGI native Config_GraphicOptionsGetPerfLevelFromFlags - 0x00405530 - transcribed from retail igi.exe.
// Retail: cdecl (result, unused, vm). Picks the table entry (7 dwords at 0x537B74) with the smallest Hamming distance to arg 0. The index variable starts as the vm argument slot, as in retail.
#include <cstdint>
#include <cstring>

int __cdecl Qvm_GetIntArg(void* vm, int index); // 0x004B8A20
void* __cdecl Config_GetActiveGraphicOptions(void); // 0x00404590
extern std::int32_t g_537b74[7];

void __cdecl Config_GraphicOptionsGetPerfLevelFromFlags(std::uint32_t* result, int unused, void* vm)
{
    (void)unused;
    std::uint32_t block[6];
    int bestDistance = 0x1F;
    int bestIndex = static_cast<int>(reinterpret_cast<std::intptr_t>(vm));
    int flags = Qvm_GetIntArg(vm, 0);
    for (int i = 0; i < 7; ++i)
    {
        std::int32_t diff = g_537b74[i] ^ flags;
        int distance = 0;
        for (int bit = 32; bit != 0; --bit)
        {
            distance += diff & 1;
            diff >>= 1;
        }
        if (!(distance >= bestDistance))
        {
            bestDistance = distance;
            bestIndex = i;
        }
    }
    block[0] = static_cast<std::uint32_t>(bestIndex);
    double value = static_cast<double>(bestIndex);
    std::memcpy(&block[2], &value, sizeof(value));
    block[4] = 0x567C74;
    std::memcpy(result, block, sizeof(block));
}
