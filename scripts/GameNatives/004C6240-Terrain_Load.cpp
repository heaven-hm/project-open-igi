// IGI native Terrain_Load - 0x004C6240 - transcribed from retail igi.exe.
// Retail: cdecl (context, path). Frame 0x88: int size @+0x10, int cmdSize @+0x14, char name[128] @+0x18.
// Order of work: Render_ComputeNormals_3, then "<path>.bit" -> context+0xDCC (0xC00 byte table of 12 byte entries,
// flags at g_b81a20), "<path>.hmp" -> context+0xDD0 (flags at g_b81920, 0 when the file is missing),
// "<path>.cmd" -> context+8, "<path>.ctr" -> context+0xC, "<path>.qsc" run through the Qvm with the
// CreateTerrainMaterial (0x004C6670, 0xE2 args) and CreateTerrainTileMap (0x004C6760, 0x41 args) natives registered,
// then allocates context+0xDC0 (0x1000) and context+0xDC4 (0x10000), clears them and calls DynObj_UpdateDynTree_11.
#include <cstdint>
#include <cstring>

extern std::uint8_t g_b81a20[];
extern std::uint8_t g_b81920[];

int __cdecl Render_ComputeNormals_3(void);                                              // 0x004D11C0
int* __cdecl Resource_Load(const char* name, int* size);                                // 0x004B5F00
int __cdecl Resource_FileExists(const char* name);                                      // 0x004B5B60
void* __cdecl Mem_Alloc(int size, int tag);                                             // 0x004B0C60
void* __cdecl Game_DefineOptions(const char* name, void* fn, int arg3, int arg4);       // 0x004B8890
int* __cdecl Qvm_Load(void* context);                                                   // 0x004B80B0
int __cdecl Qvm_Execute(void* context);                                                 // 0x004B85B0
int __cdecl Qvm_Cleanup(int* context);                                                  // 0x004B83D0
int __cdecl Sys_CloseFileStream_23(const char* name);                                   // 0x004B8920
void* __cdecl DynObj_UpdateDynTree_11(void* context);                                   // 0x004CE3D0
void __cdecl Terrain_CreateTerrainMaterial(std::uint32_t* result, int arg2, void* vm);                                   // 0x004C6670
void __cdecl Terrain_CreateTerrainTileMap(std::uint32_t* result, int arg2, void* vm);                                    // 0x004C6760

int __cdecl Terrain_Load(void* context, const char* path)
{
    int size;
    int cmdSize;
    char name[128];
    std::uint8_t* ctx = static_cast<std::uint8_t*>(context);

    Render_ComputeNormals_3();

    std::strcpy(name, path);
    std::strcat(name, ".bit");
    std::uint8_t* bit = reinterpret_cast<std::uint8_t*>(Resource_Load(name, &size));
    *reinterpret_cast<std::uint8_t**>(ctx + 0xDCC) = bit;
    std::uint8_t* pool = bit + 0xC00;
    if (static_cast<unsigned>(size) < 0xC00u)
    {
        pool = static_cast<std::uint8_t*>(Mem_Alloc(0xC00, 4));
        std::memset(pool, 0, 0xC00);
        *reinterpret_cast<std::uint8_t**>(ctx + 0xDCC) = pool;
        std::memset(g_b81a20, 0, 0x100);
    }
    else
    {
        int offset = 0;
        int index = 0;
        do
        {
            g_b81a20[index] = 1;
            std::uint8_t* table = *reinterpret_cast<std::uint8_t**>(ctx + 0xDCC);
            size = *reinterpret_cast<int*>(table + offset + 8);
            if (size)
            {
                *reinterpret_cast<std::uint8_t**>(table + offset) = pool;
                int bytes = size * size;
                bytes = (bytes + ((bytes >> 31) & 7)) >> 3;
                pool += bytes;
            }
            table = *reinterpret_cast<std::uint8_t**>(ctx + 0xDCC);
            offset += 0xC;
            index++;
            table[offset - 8] = 0;
        } while (offset < 0xC00);
    }

    std::strcpy(name, path);
    std::strcat(name, ".hmp");
    if (Resource_FileExists(name))
    {
        std::uint8_t* hmp = reinterpret_cast<std::uint8_t*>(Resource_Load(name, &size));
        *reinterpret_cast<std::uint8_t**>(ctx + 0xDD0) = hmp;
        pool = hmp + 0xC00;
        if (static_cast<unsigned>(size) < 0xC00u)
        {
            pool = static_cast<std::uint8_t*>(Mem_Alloc(0xC00, 4));
            std::memset(pool, 0, 0xC00);
            *reinterpret_cast<std::uint8_t**>(ctx + 0xDD0) = pool;
            std::memset(g_b81920, 0, 0x100);
        }
        else
        {
            int offset = 0;
            int index = 0;
            do
            {
                g_b81920[index] = 1;
                std::uint8_t* entry = *reinterpret_cast<std::uint8_t**>(ctx + 0xDD0) + offset;
                size = *reinterpret_cast<int*>(entry + 8);
                if (size)
                {
                    *reinterpret_cast<std::uint8_t**>(entry) = pool;
                    int side = size + 1;
                    pool += side * side;
                }
                std::uint8_t* table = *reinterpret_cast<std::uint8_t**>(ctx + 0xDD0);
                offset += 0xC;
                index++;
                table[offset - 8] = 0;
            } while (offset < 0xC00);
        }
    }
    else
    {
        *reinterpret_cast<int*>(ctx + 0xDD0) = 0;
    }

    std::strcpy(name, path);
    std::strcat(name, ".cmd");
    *reinterpret_cast<int**>(ctx + 8) = Resource_Load(name, nullptr);

    std::strcpy(name, path);
    std::strcat(name, ".ctr");
    *reinterpret_cast<int**>(ctx + 0xC) = Resource_Load(name, &cmdSize);

    std::strcpy(name, path);
    std::strcat(name, ".qsc");
    Game_DefineOptions("CreateTerrainMaterial", reinterpret_cast<void*>(&Terrain_CreateTerrainMaterial), 0, 0xE2);
    Game_DefineOptions("CreateTerrainTileMap", reinterpret_cast<void*>(&Terrain_CreateTerrainTileMap), 0, 0x41);
    int* vm = Qvm_Load(name);
    Qvm_Execute(vm);
    Qvm_Cleanup(vm);
    Sys_CloseFileStream_23("CreateTerrainMaterial");
    Sys_CloseFileStream_23("CreateTerrainTileMap");

    int count = static_cast<int>(static_cast<unsigned>(cmdSize) >> 5);
    if (count > 0)
    {
        int offset = 0;
        std::uint8_t* base = *reinterpret_cast<std::uint8_t**>(ctx + 0xC);
        std::uint8_t* reloc = *reinterpret_cast<std::uint8_t**>(ctx + 8);
        do
        {
            int* slot = reinterpret_cast<int*>(base + offset + 0x1C);
            *slot = *slot + static_cast<int>(reinterpret_cast<std::intptr_t>(reloc));
            offset += 0x20;
            count--;
        } while (count);
    }

    std::uint8_t* tree = static_cast<std::uint8_t*>(Mem_Alloc(0x1000, 4));
    *reinterpret_cast<std::uint8_t**>(ctx + 0xDC0) = tree;
    std::uint8_t* grid = static_cast<std::uint8_t*>(Mem_Alloc(0x10000, 4));
    *reinterpret_cast<std::uint8_t**>(ctx + 0xDC4) = grid;

    for (int row = 0; row < 0x10000; row += 0x100)
    {
        int col = 0;
        do
        {
            std::uint8_t* base = *reinterpret_cast<std::uint8_t**>(ctx + 0xDC4);
            col++;
            base[row + col - 1] = 0;
        } while (col < 0x100);
    }

    int offset = 0;
    do
    {
        std::uint8_t* base = *reinterpret_cast<std::uint8_t**>(ctx + 0xDC0);
        offset += 0x10;
        *reinterpret_cast<int*>(base + offset - 0x10) = 0;
    } while (offset < 0x1000);

    return static_cast<int>(reinterpret_cast<std::intptr_t>(DynObj_UpdateDynTree_11(context)));
}
