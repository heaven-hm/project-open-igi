#include "igi/retail/texture/texture_loaders.hpp"

#include "igi/retail/dynobj/hash_lookup.hpp"
#include "igi/retail/dynobj/local_models_mef.hpp"
#include "igi/retail/error/error_show.hpp"
#include "igi/retail/mem/mem_alloc.hpp"
#include "igi/retail/resource/resource_lifecycle.hpp"

#include <cstring>

namespace igi::retail::texture {
namespace {

// Port-owned storage for `0x00A94A64` (active texture/palette backend id).
std::int32_t g_a94a64 = 0;

// Port-owned storage for `0x00943B3C` (texture upload state byte).
std::int8_t g_943b3c = 0;

// Port-owned storage for callback table `0x00A94E84` and indices `0x00546FA8`, `0x00546FB0`, `0x00A84A48`.
std::int32_t g_546fa8 = 0;
std::int32_t g_546fb0 = 1;
std::int32_t g_a84a48 = 2;
TextureBackendDispatchFn g_a94e84[0x10]{};

} // namespace

// Native: Texture_GetActiveBackendId() - VA 0x004B0F80 - complete port.
// Previous catalog alias: Sys_ReadFileBlock_3.
// Behavior: returns the active render backend identifier stored in `0x00A94A64` (`mov eax, [0x00A94A64]; ret`).
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE (SHA256: dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01);
//                 VA 0x004B0F80-0x004B0F86, 6 bytes, digest 29ebd30649f3b57e6ec4d50668338cbb32243052c21fe58371cadd573f365c23.
// CFG: radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Confidence: high for exact global getter semantics.
// Signature: x86 cdecl int32_t(void); ported as std::int32_t Texture_GetActiveBackendId().
// Usage/tests: queries the active texture/palette backend id for Texture_UploadMipmapPacket and Texture_UploadPalettePacket; exercised by texture_loaders_test.cpp.
std::int32_t Texture_GetActiveBackendId()
{
    return g_a94a64;
}

// Native: Texture_EnableUpload() - VA 0x004B26A0 - complete port.
// Previous catalog alias: Sys_PopStack_8.
// Behavior: sets the texture upload state byte `0x00943B3C` to `1` (`mov byte ptr [0x00943B3C], 1; ret`).
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B26A0-0x004B26A8, 8 bytes,
//                 digest 8209abbcb1723ad9145b7bfe97647edb07b2acc47ff1b02f71fa78fd9ab559e1.
// CFG: radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Confidence: high for exact byte store.
// Signature: x86 cdecl void(void); ported as void Texture_EnableUpload().
// Usage/tests: enables texture upload flag before Texture_UploadMipmapPacket; exercised by texture_loaders_test.cpp.
void Texture_EnableUpload()
{
    g_943b3c = 1;
}

// Native: Texture_DisableUpload() - VA 0x004B26B0 - complete port.
// Previous catalog alias: Sys_ResolveSymbol_8.
// Behavior: clears the texture upload state byte `0x00943B3C` to `0` (`mov byte ptr [0x00943B3C], 0; ret`).
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B26B0-0x004B26B8, 8 bytes,
//                 digest 4893ff3f0d5654223486e4c269627dbfc9c121aa464471eedbf09c918c805d04.
// CFG: radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Confidence: high for exact byte clear.
// Signature: x86 cdecl void(void); ported as void Texture_DisableUpload().
// Usage/tests: clears texture upload flag after Texture_UploadMipmapPacket; exercised by texture_loaders_test.cpp.
void Texture_DisableUpload()
{
    g_943b3c = 0;
}

// Native: Texture_UploadMipmapPacket() - VA 0x004B23B0 - complete port.
// Previous catalog alias: Sys_FreeMemory_8.
// Behavior: writes `Texture_GetActiveBackendId()` into `*arg1`, zero-initializes a 12-slot
//           dispatch packet (`slot[0]=arg1`, `slot[1]=arg2`, `slot[2]=arg7`, `slot[3]=arg3`,
//           `slot[6]=arg4`, `slot[7]=arg5`, `slot[8]=arg6`, `slot[9]=arg8`, `slot[10]=(int8_t)g_943b3c`,
//           `slot[11]=arg9`), and calls the two renderer callbacks at `0x00A94E84[g_546fa8]` and
//           `0x00A94E84[g_546fb0]` with the packet pointer.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B23B0-0x004B2446, 150 bytes,
//                 digest 65ce08ad0b3bfb6914e1cd899467c3bcc788ddb0bd6a4c1a2b476295dfe1c109;
//                 CLEAN/sys/004B23B0-Sys_FreeMemory_8.c.
// CFG: radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Confidence: high for exact 12-slot packet layout and dual backend dispatch.
// Signature: x86 cdecl int32_t(int32_t* arg1, void* arg2, int32_t arg3, int32_t arg4, int32_t arg5, int32_t arg6, int32_t arg7, int32_t arg8, void* arg9).
// Usage/tests: texture upload dispatcher called by Texture_LoadLoopTextureFile and Terrain_LoadTexturePackage; exercised by texture_loaders_test.cpp.
std::int32_t Texture_UploadMipmapPacket(std::int32_t* arg1, void* arg2, std::int32_t arg3, std::int32_t arg4,
                                        std::int32_t arg5, std::int32_t arg6, std::int32_t arg7,
                                        std::int32_t arg8, void* arg9)
{
    *arg1 = Texture_GetActiveBackendId();
    std::uintptr_t var_30[12]{};
    var_30[6] = static_cast<std::uintptr_t>(static_cast<std::uint32_t>(arg4));
    var_30[1] = reinterpret_cast<std::uintptr_t>(arg2);
    var_30[0] = reinterpret_cast<std::uintptr_t>(arg1);
    var_30[8] = static_cast<std::uintptr_t>(static_cast<std::uint32_t>(arg6));
    var_30[3] = static_cast<std::uintptr_t>(static_cast<std::uint32_t>(arg3));
    var_30[7] = static_cast<std::uintptr_t>(static_cast<std::uint32_t>(arg5));
    var_30[9] = static_cast<std::uintptr_t>(static_cast<std::uint32_t>(arg8));
    var_30[11] = reinterpret_cast<std::uintptr_t>(arg9);
    var_30[10] = static_cast<std::uintptr_t>(static_cast<std::intptr_t>(g_943b3c));
    var_30[2] = static_cast<std::uintptr_t>(static_cast<std::uint32_t>(arg7));

    if (g_546fa8 >= 0 && g_546fa8 < 0x10 && g_a94e84[g_546fa8] != nullptr)
    {
        g_a94e84[g_546fa8](var_30);
    }
    if (g_546fb0 >= 0 && g_546fb0 < 0x10 && g_a94e84[g_546fb0] != nullptr)
    {
        return g_a94e84[g_546fb0](var_30);
    }
    return 0;
}

// Native: Texture_UploadPalettePacket() - VA 0x004B7CF0 - complete port.
// Previous catalog alias: Sys_PushStack_21.
// Behavior: writes `Texture_GetActiveBackendId()` into `*arg1`, builds a 3-slot palette
//           packet (`slot[0]=arg1`, `slot[1]=arg3`, `slot[2]=arg2`), and calls the renderer callback
//           at `0x00A94E84[g_a84a48]` with the packet pointer.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B7CF0-0x004B7D28, 56 bytes,
//                 digest 91d6a4b0ea6acca264ed1285d7d516332e1293e63e09487c7fad2bcb3febac92.
// CFG: radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Confidence: high for exact 3-slot packet layout and backend dispatch.
// Signature: x86 cdecl int32_t(int32_t* arg1, void* arg2, int32_t arg3).
// Usage/tests: terrain palette upload dispatcher called by Terrain_LoadTexturePackage; exercised by texture_loaders_test.cpp.
std::int32_t Texture_UploadPalettePacket(std::int32_t* arg1, void* arg2, std::int32_t arg3)
{
    const std::int32_t eax = Texture_GetActiveBackendId();
    std::uintptr_t var_c[3]{};
    var_c[0] = reinterpret_cast<std::uintptr_t>(arg1);
    var_c[2] = reinterpret_cast<std::uintptr_t>(arg2);
    *arg1 = eax;
    var_c[1] = static_cast<std::uintptr_t>(static_cast<std::uint32_t>(arg3));

    if (g_a84a48 >= 0 && g_a84a48 < 0x10 && g_a94e84[g_a84a48] != nullptr)
    {
        return g_a94e84[g_a84a48](var_c);
    }
    return 0;
}

// Native: Texture_LoadLoopTextureFile() - VA 0x004B24C0 - complete port.
// Previous catalog alias: Sys_SIsNotAValidTextureF.
// Behavior: loads `resource_path` (`arg3`) via Resource_Load(arg3, nullptr) (0x004B5F00), checks
//           the `'LOOP'` header (`0x4C, 0x4F, 0x4F, 0x50`) and version at `+0x04` (`2` or `11`),
//           calls Texture_EnableUpload() (0x004B26A0), extracts the version-specific 16-bit/32-bit
//           header fields and pixel payload pointer (`+0x18` for v2, `+0x20` for v11), calls
//           Texture_UploadMipmapPacket(...) (0x004B23B0), Resource_Unload(arg3) (0x004B6380), and
//           Texture_DisableUpload() (0x004B26B0). On invalid header, calls
//           Error_Show("'%s' is not a valid Texture file.", arg3) (0x004AF7B0).
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B24C0-0x004B25AF, 239 bytes,
//                 digest 01d287da4b443fddcd7d6be5cdf3c1bc3df57987d4b5a46165182aa4d3b9c79c;
//                 CLEAN/sys/004B24C0-Sys_SIsNotAValidTextureF.c.
// CFG: radare2 15 blocks/22 edges; Binary Ninja 15 blocks; 100% structural agreement.
// Confidence: high for exact LOOP v2/v11 field extraction and callee sequence.
// Signature: x86 cdecl int32_t(int32_t* arg1, void* arg2, const char* arg3).
// Usage/tests: model texture loader invoked by DynObj_LoadModelTextures; exercised by texture_loaders_test.cpp.
std::int32_t Texture_LoadLoopTextureFile(std::int32_t* arg1, void* arg2, const char* arg3)
{
    auto* const data_1 = static_cast<std::int32_t*>(resource::Resource_Load(arg3, nullptr));
    auto* const bytes = reinterpret_cast<std::uint8_t*>(data_1);
    const std::uint8_t data_2 = bytes[0];

    if (data_2 == 0x4CU)
    {
        std::int32_t* item_2c = nullptr;
        void* item_28 = nullptr;
        std::int32_t item_24 = 0;
        std::int32_t item_20 = 0;
        std::int32_t item_1c = 0;
        std::int32_t item_18 = 0;
        std::int32_t item_14 = 0;
        std::int32_t item_10_1 = 0;
        void* item_c = nullptr;

        if (bytes[1] == 0x4FU && bytes[2] == 0x4FU && bytes[3] == 0x50U && data_1[1] == 2)
        {
            Sys_PopStack_8();
            {
                const std::int32_t entity_3 =
                    static_cast<std::int32_t>(*reinterpret_cast<const std::uint16_t*>(bytes + 0x08));
                const std::int32_t temp_3 =
                    static_cast<std::int32_t>(*reinterpret_cast<const std::uint16_t*>(bytes + 0x16));
                item_c = &data_1[6];
                item_10_1 = 0;
                item_14 = entity_3;
                item_18 = temp_3;
                const std::int32_t data_4 =
                    static_cast<std::int32_t>(*reinterpret_cast<const std::uint16_t*>(bytes + 0x14));
                const std::int32_t entity_4 =
                    static_cast<std::int32_t>(*reinterpret_cast<const std::uint16_t*>(bytes + 0x12));
                const std::int32_t temp_4 =
                    static_cast<std::int32_t>(*reinterpret_cast<const std::uint16_t*>(bytes + 0x10));
                item_1c = data_4;
                item_20 = entity_4;
                item_24 = temp_4;
                item_28 = arg2;
                item_2c = arg1;
            }
        label_4b252e:
            Sys_FreeMemory_8(item_2c, item_28, item_24, item_20, item_1c, item_18, item_14,
                             item_10_1, item_c);
            const std::int32_t result = resource::Resource_Unload(arg3);
            Sys_ResolveSymbol_8();
            return result;
        }

        if (data_2 == 0x4CU && bytes[1] == 0x4FU && bytes[2] == 0x4FU && bytes[3] == 0x50U
            && data_1[1] == 0x0B)
        {
            Sys_PopStack_8();
            item_c = &data_1[8];
            item_10_1 = data_1[3];
            const std::int32_t entity_6 =
                static_cast<std::int32_t>(*reinterpret_cast<const std::uint16_t*>(bytes + 0x14));
            const std::int32_t data_7 =
                static_cast<std::int32_t>(*reinterpret_cast<const std::uint16_t*>(bytes + 0x18));
            item_14 = entity_6;
            item_18 = data_1[2];
            item_1c = data_7;
            const std::int32_t temp_7 =
                static_cast<std::int32_t>(*reinterpret_cast<const std::uint16_t*>(bytes + 0x1E));
            const std::int32_t data_8 =
                static_cast<std::int32_t>(*reinterpret_cast<const std::uint16_t*>(bytes + 0x1A));
            const std::int32_t entity_7 =
                static_cast<std::int32_t>(*reinterpret_cast<const std::uint16_t*>(bytes + 0x16));
            item_20 = entity_7;
            item_24 = temp_7 * data_8;
            item_28 = arg2;
            item_2c = arg1;
            goto label_4b252e;
        }
    }

    return error::Error_Show("'%s' is not a valid Texture file.", arg3);
}

// Native: Terrain_LoadTexturePackage() - VA 0x005175A0 - complete port.
// Previous catalog alias: Terrain_TextureFileNotOfRequ.
// Behavior: calls Texture_EnableUpload() (0x004B26A0), loads `resource_path` (`arg1`) via
//           Resource_Load(arg1, nullptr) (0x004B5F00) (calling Error_Show("Couldn't load texture file '%s'.")
//           if null), checks `'LOOP'` (`0x4C, 0x4F, 0x4F, 0x50`) and version `7` or `9` at `+0x04`,
//           allocates and uploads optional palette at `header[6]` via Mem_Alloc(0x0C, 4) and
//           Texture_UploadPalettePacket (0x004B7CF0), allocates `header[8] * 0x24 + 0x30` bytes via Mem_Alloc,
//           iterates `header[8]` records (stride `0x28` for v7 at `+0x3C`, stride `0x20` for v9 at
//           `+0x40`) calling Texture_UploadMipmapPacket (0x004B23B0), then calls Texture_DisableUpload() and
//           Resource_Unload(arg1).
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x005175A0-0x005177E2, 578 bytes,
//                 digest 450ad796b5382d56bd90a518553df529c860093b5070d3b3d7af6b886a82f530;
//                 CLEAN/terrain/005175A0-Terrain_TextureFileNotOfRequ.c.
// CFG: radare2 29 blocks/42 edges; Binary Ninja 29 blocks; 100% structural agreement.
// Confidence: high for exact LOOP v7/v9 record strides, palette upload, and callee order.
// Signature: x86 cdecl int32_t*(const char* arg1); ported as std::int32_t* Terrain_LoadTexturePackage(const char* arg1).
// Usage/tests: retail terrain texture loader; exercised by texture_loaders_test.cpp.
std::int32_t* Terrain_LoadTexturePackage(const char* arg1)
{
    Texture_EnableUpload();
    auto* const data_1 = static_cast<std::int32_t*>(resource::Resource_Load(arg1, nullptr));
    std::int32_t* flagP = nullptr;

    if (data_1 == nullptr)
    {
        error::Error_Show("Couldn't load texture file '%s'.", arg1);
        return nullptr;
    }

    auto* const bytes = reinterpret_cast<std::uint8_t*>(data_1);
    const std::uint8_t data_2 = bytes[0];

    if (data_2 == 0x4CU)
    {
        if (bytes[1] == 0x4FU && bytes[2] == 0x4FU && bytes[3] == 0x50U && data_1[1] == 7)
        {
            const std::int32_t context_val = data_1[6];
            std::int32_t item_8 = 0;
            std::int32_t dest_1 = 0;

            if (context_val == 0)
            {
                item_8 = 2;
                dest_1 = 2;
            }
            else
            {
                auto* const data_3 = static_cast<std::int32_t*>(mem::Mem_Alloc(0x0C, 4));
                dest_1 = *reinterpret_cast<const std::int32_t*>(bytes + context_val + 0x0C);
                flagP = data_3;
                item_8 = dest_1;
                Sys_PushStack_21(flagP, bytes + context_val + 0x10, dest_1);
            }

            auto* const data_6 =
                static_cast<std::int32_t*>(mem::Mem_Alloc(data_1[8] * 0x24 + 0x30, 4));
            data_6[0] = dest_1;
            data_6[2] = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(flagP));
            std::int32_t i = 0;
            data_6[1] = data_1[8];

            if (data_1[8] > 0)
            {
                std::int32_t* item_4_1 = &data_6[3];
                std::int32_t* dest_2 = &data_1[0x0F];

                do
                {
                    const std::int32_t shared_param =
                        static_cast<std::int32_t>(*reinterpret_cast<const std::int16_t*>(bytes + 0x12));
                    Sys_FreeMemory_8(item_4_1, flagP, dest_2[-1], dest_2[0], dest_2[1], item_8,
                                     shared_param, 0, bytes + dest_2[-2]);
                    i += 1;
                    dest_2 = &dest_2[0x0A];
                    item_4_1 = &item_4_1[9];
                } while (i < data_1[8]);
            }

            Sys_ResolveSymbol_8();
            resource::Resource_Unload(arg1);
            return data_6;
        }

        if (data_2 == 0x4CU && bytes[1] == 0x4FU && bytes[2] == 0x4FU && bytes[3] == 0x50U
            && data_1[1] == 9)
        {
            const std::int32_t context_2 = data_1[6];
            std::int32_t dest_3 = 0;

            if (context_2 == 0)
            {
                dest_3 = 2;
            }
            else
            {
                auto* const data_14 = static_cast<std::int32_t*>(mem::Mem_Alloc(0x0C, 4));
                dest_3 = *reinterpret_cast<const std::int32_t*>(bytes + context_2 + 0x0C);
                flagP = data_14;
                Sys_PushStack_21(flagP, bytes + context_2 + 0x10, dest_3);
            }

            auto* const data_17 =
                static_cast<std::int32_t*>(mem::Mem_Alloc(data_1[8] * 0x24 + 0x30, 4));
            data_17[0] = dest_3;
            data_17[2] = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(flagP));
            std::int32_t i_1 = 0;
            data_17[1] = data_1[8];

            if (data_1[8] > 0)
            {
                std::int32_t* item_4_2 = &data_17[3];
                auto* dest_4 = reinterpret_cast<std::uint16_t*>(&data_1[0x10]);

                do
                {
                    const auto* const rec_bytes = reinterpret_cast<const std::uint8_t*>(dest_4);
                    const std::int32_t temp_10 = static_cast<std::int32_t>(dest_4[0]);
                    const std::int32_t data_20 = static_cast<std::int32_t>(dest_4[-1]);
                    const std::int32_t entity_15 = static_cast<std::int32_t>(dest_4[-2]);
                    const std::int32_t arg6_val =
                        *reinterpret_cast<const std::int32_t*>(rec_bytes - 0x08);
                    const std::int32_t data_off =
                        *reinterpret_cast<const std::int32_t*>(rec_bytes - 0x0C);
                    const std::int32_t shared_param =
                        static_cast<std::int32_t>(*reinterpret_cast<const std::int16_t*>(bytes + 0x12));
                    Sys_FreeMemory_8(item_4_2, flagP, entity_15, data_20, temp_10, arg6_val,
                                     shared_param, 0, bytes + data_off);
                    i_1 += 1;
                    dest_4 = &dest_4[0x10];
                    item_4_2 = &item_4_2[9];
                } while (i_1 < data_1[8]);
            }

            Sys_ResolveSymbol_8();
            resource::Resource_Unload(arg1);
            return data_17;
        }
    }

    error::Error_Show("Texture file not of required type: '%s'.", arg1);
    return nullptr;
}

// Native: DynObj_LoadModelTextures() - VA 0x004CF790 - complete port.
// Previous catalog alias: DynObj_LocalTexturesSTex.
// Behavior: locates the `'TEXF'` (`0x54455846`) chunk inside `*(context + 0x08)` via
//           Ilff_FindFormChunk (0x004CEAA0), stores `count = *texf` at `*(context + 0x40)`,
//           allocates `count * 0x24` bytes at `*(context + 0x34)` and `count * 0x0C` bytes at
//           `*(context + 0x38)` via Mem_Alloc (0x004B0C60) when `count > 0` (or clears both to 0),
//           and iterates each NUL-terminated texture name formatting `"LOCAL:textures/%s.tex"` into
//           `0x00A4EC6C` via Game_DataSymbolLoad (0x004A53B3) and calling
//           Texture_LoadLoopTextureFile (0x004B24C0).
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004CF790-0x004CF865, 213 bytes,
//                 digest 75a65077e61f7571a67d6370e3eec880541dbaafcea3aed7af6dec07cf49002f;
//                 CLEAN/dynobj/004CF790-DynObj_LocalTexturesSTex.c.
// CFG: radare2 7 blocks/8 edges; Binary Ninja 7 blocks; 100% structural agreement.
// Confidence: high for exact TEXF chunk parsing, allocation sizes, and loop stride (+0x24 / +0x0C).
// Signature: x86 cdecl void*(void* context); ported as void* DynObj_LoadModelTextures(void* context).
// Usage/tests: MTP package texture loader called by DynObj_LoadModelTexturePackage; exercised by texture_loaders_test.cpp.
void* DynObj_LoadModelTextures(void* context)
{
    auto* const context_bytes = static_cast<std::uint8_t*>(context);
    std::size_t flagP = 0U;
    void* const form_ptr = *reinterpret_cast<void**>(context_bytes + 0x08);
    auto* data_1 = static_cast<std::int32_t*>(
        dynobj::DynObj_CreateMagicObject_13(form_ptr, 0x54455846, nullptr));
    std::int32_t* const source_val = data_1;
    const std::int32_t saved_count_2 = *source_val;
    *reinterpret_cast<std::int32_t*>(context_bytes + 0x40) = saved_count_2;

    if (saved_count_2 <= 0)
    {
        void* const saved_30 = *reinterpret_cast<void**>(context_bytes + 0x30);
        *reinterpret_cast<void**>(context_bytes + 0x34) = nullptr;
        if constexpr (sizeof(void*) > 4)
        {
            *reinterpret_cast<void**>(context_bytes + 0x30) = saved_30;
        }
        *reinterpret_cast<void**>(context_bytes + 0x38) = nullptr;
        *reinterpret_cast<std::int32_t*>(context_bytes + 0x40) = saved_count_2;
    }
    else
    {
        auto* const tex_records = static_cast<std::uint8_t*>(mem::Mem_Alloc(saved_count_2 * 0x24, 4));
        auto* const pal_records = static_cast<std::uint8_t*>(mem::Mem_Alloc(saved_count_2 * 0x0C, 4));
        void* const saved_30 = *reinterpret_cast<void**>(context_bytes + 0x30);
        *reinterpret_cast<void**>(context_bytes + 0x34) = tex_records;
        if constexpr (sizeof(void*) > 4)
        {
            *reinterpret_cast<void**>(context_bytes + 0x30) = saved_30;
        }
        *reinterpret_cast<void**>(context_bytes + 0x38) = pal_records;
        *reinterpret_cast<std::int32_t*>(context_bytes + 0x40) = saved_count_2;

        const char* source_1 = reinterpret_cast<const char*>(&source_val[1]);

        std::size_t tex_offset = 0U;
        std::int32_t saved_ctx = saved_count_2;
        std::int32_t saved_count_1 = 0;
        bool flag_3_1 = false;
        char path_buf[0x44]{};

        do
        {
            *reinterpret_cast<std::int32_t*>(pal_records + flagP + 4U) = 0;
            error::Game_DataSymbolLoad(path_buf, "LOCAL:textures/%s.tex", source_1);
            Sys_SIsNotAValidTextureF(
                reinterpret_cast<std::int32_t*>(tex_records + tex_offset),
                pal_records + flagP,
                path_buf);

            const char* dest_1 = source_1;
            std::uint32_t i = 0xFFFFFFFFU;
            while (i != 0U)
            {
                const bool flag_2_1 = (*dest_1 != '\0');
                ++dest_1;
                --i;
                if (!flag_2_1)
                {
                    break;
                }
            }

            flagP += 0x0CU;
            source_1 = source_1 + (~i);
            saved_count_1 = saved_ctx - 1;
            flag_3_1 = saved_ctx != 1;
            tex_offset += 0x24U;
            saved_ctx = saved_count_1;
        } while (flag_3_1);

        return reinterpret_cast<void*>(static_cast<std::intptr_t>(saved_count_1));
    }

    return data_1;
}

std::int8_t Texture_GetUploadFlag() noexcept
{
    return g_943b3c;
}

void Host_SetTextureBackendId(std::int32_t id) noexcept
{
    g_a94a64 = id;
}

void Host_SetTextureCallbackSlots(std::int32_t slot_primary, std::int32_t slot_secondary,
                                  std::int32_t slot_palette) noexcept
{
    g_546fa8 = slot_primary;
    g_546fb0 = slot_secondary;
    g_a84a48 = slot_palette;
}

void Host_SetTextureBackendCallback(std::int32_t slot, TextureBackendDispatchFn fn) noexcept
{
    if (slot >= 0 && slot < 0x10)
    {
        g_a94e84[slot] = fn;
    }
}

} // namespace igi::retail::texture
