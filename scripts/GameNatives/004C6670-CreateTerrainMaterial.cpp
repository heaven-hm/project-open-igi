// IGI native Terrain_CreateTerrainMaterial - 0x004C6670 - transcribed from retail igi.exe.
// Retail: cdecl (result, unused, vm). Registered by Terrain_Load (0x004C6240) as "CreateTerrainMaterial" with 0xE2 args.
// Reads material slot argument 0 from the Qvm, then 32 groups of 7 arguments starting at index 3 into the
// 900 byte record at g_c28fe0 + slot * 900 (int @+0, then 28 byte groups at +4: int, float, float, float*0.5, int, float*0.5, int).
// Writes the 6 dword result {0, ?, 0, 0, 0x567C74, ?} through the first argument.
#include <cstdint>
#include <cstring>

extern std::uint8_t g_c28fe0[];
extern const float g_533504;

int __cdecl Qvm_GetIntArg(void* vm, int index);      // 0x004B8A20
float __cdecl Qvm_GetFloatArg(void* vm, int index);  // 0x004B8A50

void __cdecl Terrain_CreateTerrainMaterial(std::uint32_t* result, int arg2, void* vm)
{
    std::uint32_t local[6];

    int slot = Qvm_GetIntArg(vm, 0);
    int offset = slot * 900;
    *reinterpret_cast<int*>(g_c28fe0 + offset) = Qvm_GetIntArg(vm, 1);

    std::uint8_t* record = g_c28fe0 + offset + 8;
    for (int index = 3; index < 0xE3; index += 7)
    {
        *reinterpret_cast<int*>(record - 4) = Qvm_GetIntArg(vm, index - 1);
        *reinterpret_cast<float*>(record) = Qvm_GetFloatArg(vm, index);
        *reinterpret_cast<float*>(record + 4) = Qvm_GetFloatArg(vm, index + 1);
        *reinterpret_cast<float*>(record + 8) = Qvm_GetFloatArg(vm, index + 2) * g_533504;
        *reinterpret_cast<int*>(record + 0xC) = Qvm_GetIntArg(vm, index + 3);
        *reinterpret_cast<float*>(record + 0x10) = Qvm_GetFloatArg(vm, index + 4) * g_533504;
        *reinterpret_cast<int*>(record + 0x14) = Qvm_GetIntArg(vm, index + 5);
        record += 0x1C;
    }

    local[0] = 0;
    local[2] = 0;
    local[3] = 0;
    local[4] = 0x567C74;
    std::memcpy(result, local, sizeof(local));
}
