// IGI native Terrain_LoadTexturesAndLightmaps - 0x004C5F00 - transcribed from retail igi.exe.
// Retail: cdecl (context, path). Frame 0x484: int size @+0, char name[128] @+4, uint32 palette[256] @+0x84.
// Copies path to g_b97c20, loads "<path>.tex" texture list into context+0xDBC, uploads a 256 entry palette
// (index << 16) into context+0xDF4, and when terrain lightmaps are enabled loads "<path>.lmp" into context+0xE00
// and uploads each square lightmap packet into the 0x2C byte table at g_b81b20.
#include <cstdint>
#include <cstring>

extern char g_b97c20[];
extern std::uint8_t g_b81b20[];

int __cdecl Sys_PushStack_8(void);                       // 0x004B2670
int __cdecl Sys_ExecuteBytecode_8(int mode);             // 0x004B2610
int __cdecl Terrain_LoadTexFile(const char* name);       // 0x005175A0
void* __cdecl Mem_Alloc(int size, int tag);              // 0x004B0C60
int __cdecl Texture_UploadPalettePacket(void* context, std::uint32_t* palette, int count);   // 0x004B7CF0
int __cdecl AppContext_GetTerrainLightmapsUsed(void);    // 0x0048F250
int __cdecl Resource_FileExists(const char* name);       // 0x004B5B60
int* __cdecl Resource_Load(const char* name, int* size); // 0x004B5F00
int __cdecl Resource_Unload(const char* name);           // 0x004B6380
int __cdecl Texture_UploadMipmapPacket(void* context, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9); // 0x004B23B0

int __cdecl Terrain_LoadTexturesAndLightmaps(void* context, const char* path)
{
    int size;
    char name[128];
    std::uint32_t palette[256];
    std::uint8_t* ctx = static_cast<std::uint8_t*>(context);

    std::strcpy(g_b97c20, path);
    std::strcpy(name, path);
    std::strcat(name, ".tex");

    if (Sys_PushStack_8() > 1)
        Sys_ExecuteBytecode_8(0);

    *reinterpret_cast<int*>(ctx + 0xDBC) = Terrain_LoadTexFile(name);

    if (Sys_PushStack_8() > 1)
        Sys_ExecuteBytecode_8(-1);

    for (int i = 0; i < 0x100; i++)
        palette[i] = static_cast<std::uint32_t>(i) << 16;

    void* paletteBlock = Mem_Alloc(0xC, 4);
    *reinterpret_cast<void**>(ctx + 0xDF4) = paletteBlock;
    Texture_UploadPalettePacket(paletteBlock, palette, 1);

    if (!AppContext_GetTerrainLightmapsUsed())
        return 0;

    std::strcpy(name, path);
    std::strcat(name, ".lmp");
    std::strcpy(reinterpret_cast<char*>(ctx + 0xE00), name);

    if (!Resource_FileExists(name))
        return 0;

    int* data = Resource_Load(name, &size);
    *reinterpret_cast<int**>(ctx + 0xDF8) = data;

    if (data && size > 0)
    {
        int consumed = 0;
        std::uint8_t* entry = g_b81b20;
        do
        {
            int dim = *data;
            data++;
            consumed += 4;
            if (dim)
            {
                if (Sys_PushStack_8() > 1)
                    Sys_ExecuteBytecode_8(1);

                Texture_UploadMipmapPacket(entry + 8, *reinterpret_cast<int*>(ctx + 0xDF4), dim, dim, dim, 1, 0, 0x20000000,
                                           static_cast<int>(reinterpret_cast<std::intptr_t>(data)));

                if (Sys_PushStack_8() > 1)
                    Sys_ExecuteBytecode_8(-1);

                *reinterpret_cast<int*>(entry) = dim;
                *reinterpret_cast<int**>(entry + 4) = data;
            }
            int bytes = dim * dim;
            entry += 0x2C;
            data = reinterpret_cast<int*>(reinterpret_cast<std::uint8_t*>(data) + bytes);
            consumed += bytes;
        } while (consumed < size);
    }

    return Resource_Unload(name);
}
