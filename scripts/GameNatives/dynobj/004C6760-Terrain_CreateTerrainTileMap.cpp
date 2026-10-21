// IGI native Terrain_CreateTerrainTileMap - 0x004C6760 - transcribed from retail igi.exe.
// Retail: cdecl (result, unused, vm). Registered by Terrain_Load (0x004C6240) as "CreateTerrainTileMap" with 0x41 args.
// Reads tile map slot argument 0 from the Qvm, then 64 byte arguments (indices 1..64) into g_c30080 + slot * 64.
// Writes the 6 dword result {0, ?, 0, 0, 0x567C74, ?} through the first argument.
#include <cstdint>
#include <cstring>

extern std::uint8_t g_c30080[];

int __cdecl Qvm_GetIntArg(void* vm, int index);      // 0x004B8A20

void __cdecl Terrain_CreateTerrainTileMap(std::uint32_t* result, int arg2, void* vm)
{
    std::uint32_t local[6];

    int slot = Qvm_GetIntArg(vm, 0);
    std::uint8_t* tiles = g_c30080 + (slot << 6);
    int index = 0;
    do
    {
        int next = index + 1;
        tiles[index] = static_cast<std::uint8_t>(Qvm_GetIntArg(vm, next));
        index = next;
    } while (index < 0x40);

    local[0] = 0;
    local[2] = 0;
    local[3] = 0;
    local[4] = 0x567C74;
    std::memcpy(result, local, sizeof(local));
}
