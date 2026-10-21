#include "igi/retail/dynobj/mtp_package_loader.hpp"

#include "igi/retail/dynobj/hash_lookup.hpp"
#include "igi/retail/dynobj/local_models_mef.hpp"
#include "igi/retail/error/error_show.hpp"
#include "igi/retail/mem/heap_list.hpp"
#include "igi/retail/mem/mem_alloc.hpp"
#include "igi/retail/resource/resource_lifecycle.hpp"
#include "igi/retail/texture/texture_loaders.hpp"

#include <cstring>

namespace igi::retail::dynobj {
namespace {

// Port-owned storage for shared package pointer `0x00A4EC60`.
void* g_a4ec60 = nullptr;

// Port-owned storage for package path prefix `0x00A4ED6C` and anim override prefix `0x00A4EDF8`.
char g_a4ed6c[0x8C] = "LOCAL:";
char g_a4edf8[0x8C]{};

// Port-owned storage for global formatting buffer `0x00A4EC6C`.
char g_a4ec6c[0x100]{};

PaletteLoaderFn g_palette_loader = nullptr;
AnimIffLoaderFn g_anim_iff_loader = nullptr;
LovsEntryLoaderFn g_lovs_entry_loader = nullptr;

} // namespace

// Native: DynObj_LoadPackageAnimations; retail VA 0x004CF910; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE (SHA256: dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01); VA 0x004CF910-0x004CF9CA, 186 bytes, digest 9bc4b0a10b59709630cf47788d176a5bcca0360df50b4e8349e0edafa1a0d524; CLEAN/dynobj/004CF910-DynObj_LoadPackageAnimations.c.
// Behavior/CFG: locates the `'BANM'` (`0x42414E4D`) chunk inside `*(context + 0x08)` via Ilff_FindFormChunk (0x004CEAA0); when present and `*banm > 0`, formats `"%sanims/%s.iff"` using either `g_a4edf8` (when `g_a4edf8[0] != 0`) or `g_a4ed6c` via sprintf (0x004A53B3) and calls Render_FileSIsNotIffFile (0x004D67A0) for each NUL-terminated animation name. radare2 10 blocks/14 edges; Binary Ninja 10 blocks; 100% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact BANM chunk tag, prefix selection (`0x00A4EDF8` vs `0x00A4ED6C`), and loop.
void* DynObj_LoadPackageAnimations(void* context)
{
    auto* const ctx_bytes = static_cast<std::uint8_t*>(context);
    void* const form_ptr = *reinterpret_cast<void**>(ctx_bytes + 0x08);
    auto* const result_1 =
        static_cast<std::int32_t*>(Ilff_FindFormChunk(form_ptr, 0x42414E4D, nullptr));
    void* result = result_1;

    if (result_1 != nullptr)
    {
        const std::int32_t count = *result_1;
        result = reinterpret_cast<void*>(static_cast<std::intptr_t>(count));
        const char* source_val = reinterpret_cast<const char*>(&result_1[1]);
        char item_40[0x80]{};

        if (g_a4edf8[0] != '\0')
        {
            if (count > 0)
            {
                std::int32_t context_2 = count;
                std::int32_t i = 0;

                do
                {
                    error::sprintf(item_40, "%sanims/%s.iff", g_a4edf8, source_val);
                    if (g_anim_iff_loader != nullptr)
                    {
                        g_anim_iff_loader(item_40);
                    }

                    const char* dest_2 = source_val;
                    std::uint32_t j = 0xFFFFFFFFU;
                    result = nullptr;

                    while (j != 0U)
                    {
                        const bool flag_0_1 = (*dest_2 != '\0');
                        ++dest_2;
                        --j;
                        if (!flag_0_1)
                        {
                            break;
                        }
                    }

                    i = context_2;
                    context_2 -= 1;
                    source_val = source_val + (~j);
                } while (i != 1);
            }
        }
        else if (count > 0)
        {
            std::int32_t context_1 = count;
            std::int32_t i_1 = 0;

            do
            {
                error::sprintf(item_40, "%sanims/%s.iff", g_a4ed6c, source_val);
                if (g_anim_iff_loader != nullptr)
                {
                    g_anim_iff_loader(item_40);
                }

                const char* dest_1 = source_val;
                std::uint32_t j_1 = 0xFFFFFFFFU;

                while (j_1 != 0U)
                {
                    const bool flag_2_1 = (*dest_1 != '\0');
                    ++dest_1;
                    --j_1;
                    if (!flag_2_1)
                    {
                        break;
                    }
                }

                i_1 = context_1;
                context_1 -= 1;
                source_val = source_val + (~j_1);
            } while (i_1 != 1);
            return nullptr;
        }
    }

    return result;
}

// Native: DynObj_LoadPackagePalettes; retail VA 0x004CF870; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: allocates `count * 0x0C` bytes via Mem_Alloc (0x004B0C60) into `*(context + 0x3C)` when Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004CF870-0x004CF909, 153 bytes, digest 246a42f51d3979bfd848ccefcef6fd53d81b2373c741392b5d719b6b46f29e09; CLEAN/dynobj/004CF870-DynObj_LoadPackagePalettes.c.
// Behavior/CFG: locates the `'PALF'` (`0x50414C46`) chunk inside `*(context + 0x08)` via Ilff_FindFormChunk (0x004CEAA0), stores `count = *palf` at `*(context + 0x44)`, `count > 0` (or clears `*(context + 0x3C) = 0`), and iterates each NUL-terminated palette name formatting `"%spalettes/%s.pal"` into `0x00A4EC6C` via sprintf and calling Sys_SIsNotAValidPaletteF (0x004B7D30). radare2 7 blocks/8 edges; Binary Ninja 7 blocks; 100% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact PALF chunk tag, allocation stride `0x0C`, and callee sequence.
void* DynObj_LoadPackagePalettes(void* context)
{
    auto* const ctx_bytes = static_cast<std::uint8_t*>(context);
    std::size_t offset = 0U;
    void* const form_ptr = *reinterpret_cast<void**>(ctx_bytes + 0x08);
    auto* data_1 =
        static_cast<std::int32_t*>(Ilff_FindFormChunk(form_ptr, 0x50414C46, nullptr));
    std::int32_t* const esi = data_1;
    const std::int32_t count = *esi;
    *reinterpret_cast<std::int32_t*>(ctx_bytes + 0x44) = count;

    if (count <= 0)
    {
        void* const saved_38 = *reinterpret_cast<void**>(ctx_bytes + 0x38);
        *reinterpret_cast<void**>(ctx_bytes + 0x3C) = nullptr;
        if constexpr (sizeof(void*) > 4)
        {
            *reinterpret_cast<void**>(ctx_bytes + 0x38) = saved_38;
        }
    }
    else
    {
        auto* const pal_array = static_cast<std::uint8_t*>(mem::Mem_Alloc(count * 0x0C, 4));
        const char* source_1 = reinterpret_cast<const char*>(&esi[1]);
        void* const saved_38 = *reinterpret_cast<void**>(ctx_bytes + 0x38);
        *reinterpret_cast<void**>(ctx_bytes + 0x3C) = pal_array;
        if constexpr (sizeof(void*) > 4)
        {
            *reinterpret_cast<void**>(ctx_bytes + 0x38) = saved_38;
        }
        *reinterpret_cast<std::int32_t*>(ctx_bytes + 0x44) = count;
        data_1 = reinterpret_cast<std::int32_t*>(pal_array);

        std::int32_t rem = count;
        std::int32_t data_3 = 0;
        bool flag_3_1 = false;

        do
        {
            error::sprintf(g_a4ec6c, "%spalettes/%s.pal", g_a4ed6c, source_1);
            if (g_palette_loader != nullptr)
            {
                g_palette_loader(pal_array + offset, g_a4ec6c);
            }

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

            offset += 0x0CU;
            data_3 = rem - 1;
            flag_3_1 = rem != 1;
            rem = data_3;
            source_1 = source_1 + (~i);
        } while (flag_3_1);

        return reinterpret_cast<void*>(static_cast<std::intptr_t>(data_3));
    }

    return data_1;
}

// Native: DynObj_LoadPackageMeshes; retail VA 0x004CF330; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004CF330-0x004CF381, 81 bytes, digest 1f15034786839dfd674728a70a7502c4ea2ab4ff265eefdbd0012b2948a9460a; CLEAN/dynobj/004CF330-DynObj_LoadPackageMeshes.c.
// Behavior/CFG: locates the `'MODS'` (`0x4D4F4453`) chunk inside `*(context + 0x08)` via Ilff_FindFormChunk (0x004CEAA0) and iterates `*mods` NUL-terminated model names, calling DynObj_LoadModelMesh (0x004CF390) and appending each returned record to the list at `context + 0x10` via HeapList_Insert (0x004B0E60). radare2 5 blocks/6 edges; Binary Ninja 5 blocks; 100% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact MODS tag, DynObj_LoadModelMesh call, and list insertion at `+0x10`.
void* DynObj_LoadPackageMeshes(void* context)
{
    auto* const ctx_bytes = static_cast<std::uint8_t*>(context);
    void* const form_ptr = *reinterpret_cast<void**>(ctx_bytes + 0x08);
    auto* result =
        static_cast<std::int32_t*>(Ilff_FindFormChunk(form_ptr, 0x4D4F4453, nullptr));
    std::int32_t i_1 = *result;
    const char* source_1 = reinterpret_cast<const char*>(&result[1]);

    if (i_1 > 0)
    {
        std::int32_t i = 0;

        do
        {
            auto** const model_node = static_cast<void**>(DynObj_LocalModelsSMef(source_1));
            Camera_SetFieldOfView_33(ctx_bytes + 0x10, model_node);

            const char* dest_1 = source_1;
            std::uint32_t j = 0xFFFFFFFFU;
            result = nullptr;

            while (j != 0U)
            {
                const bool flag_0_1 = (*dest_1 != '\0');
                ++dest_1;
                --j;
                if (!flag_0_1)
                {
                    break;
                }
            }

            i = i_1;
            i_1 -= 1;
            source_1 = source_1 + (~j);
        } while (i != 1);
    }

    return result;
}

// Native: DynObj_LoadPackageLovs; retail VA 0x004CF630; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004CF630-0x004CF685, 85 bytes, digest e7cb705e985c39decbb0b12edbef88c179529f6dab08e9c9b276e7256ac30c9b; CLEAN/dynobj/004CF630-DynObj_LoadPackageLovs.c.
// Behavior/CFG: locates the optional `'LOVS'` (`0x53564F4C`) chunk inside `*(context + 0x08)` via Ilff_FindFormChunk (0x004CEAA0); when present, iterates `*lovs` NUL-terminated names calling DynObj_UpdateDynTree_14 (0x004CF690) and appending each returned node to the list at `context + 0x1C` via HeapList_Insert (0x004B0E60). radare2 6 blocks/8 edges; Binary Ninja 6 blocks; 100% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact LOVS tag, null-chunk guard, and list insertion at `+0x1C`.
void* DynObj_LoadPackageLovs(void* context)
{
    auto* const ctx_bytes = static_cast<std::uint8_t*>(context);
    void* const form_ptr = *reinterpret_cast<void**>(ctx_bytes + 0x08);
    auto* result =
        static_cast<std::int32_t*>(Ilff_FindFormChunk(form_ptr, 0x53564F4C, nullptr));

    if (result != nullptr)
    {
        std::int32_t i_1 = *result;
        const char* source_1 = reinterpret_cast<const char*>(&result[1]);

        if (i_1 > 0)
        {
            std::int32_t i = 0;

            do
            {
                if (g_lovs_entry_loader != nullptr)
                {
                    Camera_SetFieldOfView_33(ctx_bytes + 0x1C, g_lovs_entry_loader(source_1));
                }

                const char* dest_1 = source_1;
                std::uint32_t j = 0xFFFFFFFFU;
                result = nullptr;

                while (j != 0U)
                {
                    const bool flag_0_1 = (*dest_1 != '\0');
                    ++dest_1;
                    --j;
                    if (!flag_0_1)
                    {
                        break;
                    }
                }

                i = i_1;
                i_1 -= 1;
                source_1 = source_1 + (~j);
            } while (i != 1);
        }
    }

    return result;
}

// Native: DynObj_LoadModelTexturePackage; retail VA 0x004CF080; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: instance record (`(mat_count << 3) + 0x1C` bytes) via Mem_Alloc (0x004B0C60), links it Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004CF080-0x004CF2F3, 627 bytes, digest 2cf87f358150e1e4f5d5b5d2b7fbdf9597d8faddeaa1e0cb25e06f434d1e50e5; CLEAN/dynobj/004CF080-DynObj_LoadModelTexturePackage.c.
// Behavior/CFG: loads an MTP package into `context`: calls DynObj_LoadPackageAnimations(context) (0x004CF910); when `load_textures != 0` (`arg3`), loads `"%stextures/%s.res"` via Resource_Load, calls DynObj_LoadModelTextures(context) (0x004CF790), DynObj_LoadPackagePalettes(context) (0x004CF870), and Resource_Unload; loads `"%smodels/%s.res"` into `*(context + 0x0C)`, calls DynObj_LoadPackageMeshes(context) (0x004CF330) and DynObj_LoadPackageLovs(context) (0x004CF630); resolves `'VNAM'` (`0x564E414D`), `'INST'` (`0x494E5354`), and `'GTT '` (`0x47545420`) chunks via Ilff_FindFormChunk (0x004CEAA0), allocates each into `context + 0x28` via HeapList_Insert (0x004B0E60), resolves local/shared model (`< 0x2000` vs `>= 0x2000` in `g_a4ec60`) via DynObj_LookupHashItem (0x004CFA30), and populates the instance texture/palette pointer tables. radare2 28 blocks/43 edges; Binary Ninja 28 blocks; 100% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact VNAM/INST/GTT chunk tags, 0x2000 shared-package threshold, and offsets.
void* DynObj_LoadModelTexturePackage(void* context, const char* arg2, std::int32_t arg3)
{
    auto* const data_val = static_cast<std::uint8_t*>(g_a4ec60);
    auto* dest_val = static_cast<std::uint8_t*>(context);
    DynObj_LoadPackageAnimations(dest_val);
    char item_100[0x100]{};

    if (arg3 != 0)
    {
        error::sprintf(item_100, "%stextures/%s.res", g_a4ed6c, arg2);
        resource::Resource_Load(item_100, nullptr);
        texture::DynObj_LocalTexturesSTex(dest_val);
        DynObj_LoadPackagePalettes(dest_val);
        resource::Resource_Unload(item_100);
    }

    error::sprintf(item_100, "%smodels/%s.res", g_a4ed6c, arg2);
    void* const saved_form_ptr = *reinterpret_cast<void**>(dest_val + 0x08);
    void* const saved_model_list = *reinterpret_cast<void**>(dest_val + 0x10);
    *reinterpret_cast<void**>(dest_val + 0x0C) = resource::Resource_Load(item_100, nullptr);
    if constexpr (sizeof(void*) > 4)
    {
        *reinterpret_cast<void**>(dest_val + 0x08) = saved_form_ptr;
        *reinterpret_cast<void**>(dest_val + 0x10) = saved_model_list;
    }
    DynObj_LoadPackageMeshes(dest_val);
    DynObj_LoadPackageLovs(dest_val);

    void* const form_ptr = *reinterpret_cast<void**>(dest_val + 0x08);
    auto* const data_6 =
        static_cast<std::int32_t*>(Ilff_FindFormChunk(form_ptr, 0x564E414D, nullptr));
    const std::int32_t result_3 = *data_6;
    const char* item_104 = reinterpret_cast<const char*>(&data_6[result_3 + 1]);
    auto* const inst_ptr =
        static_cast<std::int32_t*>(Ilff_FindFormChunk(form_ptr, 0x494E5354, nullptr));
    const std::int32_t* result_2 = inst_ptr;
    void* result = inst_ptr;

    if (result_3 > 0)
    {
        std::int32_t result_1 = result_3;

        while (true)
        {
            const std::int32_t data_7 = *result_2;
            HashItem** item_12c_1 = nullptr;
            std::int32_t item_128_3 = 0;

            if (data_7 < 0x2000)
            {
                item_128_3 = data_7;
                item_12c_1 = reinterpret_cast<HashItem**>(dest_val + 0x10);
            }
            else
            {
                item_128_3 = data_7 - 0x2000;
                item_12c_1 = data_val != nullptr ? reinterpret_cast<HashItem**>(data_val + 0x10) : nullptr;
            }


            HashItem* const data_11 = DynObj_LookupHashItem(item_12c_1, item_128_3);
            const std::int32_t context_1 = result_2[1];
            result_2 = &result_2[2];

            // Allocate 7 header slots + 2 * context_1 material pointer slots.
            const std::int32_t alloc_bytes =
                sizeof(void*) == 4
                    ? ((context_1 << 3) + 0x1C)
                    : ((7 + 2 * context_1) * static_cast<std::int32_t>(sizeof(void*)));
            auto** const data_12 = static_cast<void**>(mem::Mem_Alloc(alloc_bytes, 4));
            data_12[5] = &data_12[context_1 + 7];
            data_12[4] = &data_12[7];
            data_12[6] = reinterpret_cast<void*>(static_cast<std::intptr_t>(context_1));
            data_12[1] = nullptr;
            data_12[0] = nullptr;
            Camera_SetFieldOfView_33(static_cast<std::uint8_t*>(context) + 0x28, data_12);
            data_12[3] = data_11;

            const char* dest_3 = item_104;
            std::uint32_t i = 0xFFFFFFFFU;
            data_12[2] = const_cast<char*>(item_104);

            while (i != 0U)
            {
                const bool flag_1_1 = (*dest_3 != '\0');
                ++dest_3;
                --i;
                if (!flag_1_1)
                {
                    break;
                }
            }

            item_104 = item_104 + (~i);
            auto* const gtt_chunk = static_cast<std::uint8_t*>(
                Ilff_FindFormChunk(*reinterpret_cast<void**>(static_cast<std::uint8_t*>(context) + 0x08),
                                            0x47545420, nullptr));
            const std::uint8_t* const data_17 = gtt_chunk + 4U;
            std::int32_t item_110_1 = 0;

            if (context_1 > 0)
            {
                bool flag_3_1 = false;
                auto* const ctx_u8 = static_cast<std::uint8_t*>(context);

                do
                {
                    const std::int32_t dest_4 = *result_2;
                    const std::int32_t entity_9 =
                        *reinterpret_cast<const std::int32_t*>(data_17 + (dest_4 << 3));
                    void* flagP2 = nullptr;

                    if (entity_9 < 0x2000)
                    {
                        auto* const temp_10 = *reinterpret_cast<std::uint8_t**>(ctx_u8 + 0x34);
                        flagP2 = temp_10 == nullptr ? nullptr : (temp_10 + entity_9 * 0x24);
                    }
                    else
                    {
                        auto* const temp_8 =
                            data_val != nullptr ? *reinterpret_cast<std::uint8_t**>(data_val + 0x34) : nullptr;
                        flagP2 = temp_8 == nullptr ? nullptr : (temp_8 + ((entity_9 * 9 - 0x12000) << 2));
                    }

                    const std::int32_t entity_12 =
                        *reinterpret_cast<const std::int32_t*>(data_17 + (dest_4 << 3) + 4);
                    void* entity_14 = nullptr;

                    if (entity_12 == -1)
                    {
                        auto* const temp_12 = *reinterpret_cast<std::uint8_t**>(ctx_u8 + 0x38);
                        entity_14 = temp_12 == nullptr ? nullptr : (temp_12 + entity_12 * 0x0C);
                    }
                    else if (entity_12 < 0x2000)
                    {
                        auto* const temp_16 = *reinterpret_cast<std::uint8_t**>(ctx_u8 + 0x3C);
                        entity_14 = temp_16 == nullptr ? nullptr : (temp_16 + entity_12 * 0x0C);
                    }
                    else
                    {
                        auto* const temp_14 =
                            data_val != nullptr ? *reinterpret_cast<std::uint8_t**>(data_val + 0x38) : nullptr;
                        entity_14 = temp_14 == nullptr ? nullptr : (temp_14 + ((entity_12 * 3 - 0x6000) << 2));
                    }

                    static_cast<void**>(data_12[4])[item_110_1] = flagP2;
                    result_2 = &result_2[1];
                    static_cast<void**>(data_12[5])[item_110_1] = entity_14;
                    flag_3_1 = (item_110_1 + 1) < context_1;
                    item_110_1 += 1;
                } while (flag_3_1);
            }

            const std::int32_t next_rem = result_1 - 1;
            const bool flag_0_1 = result_1 != 1;
            result = reinterpret_cast<void*>(static_cast<std::intptr_t>(next_rem));
            result_1 = next_rem;

            if (!flag_0_1)
            {
                break;
            }

            dest_val = static_cast<std::uint8_t*>(context);
        }
    }

    return result;
}

void DynObj_SetSharedPackageContext(void* shared_pkg) noexcept
{
    g_a4ec60 = shared_pkg;
}

void DynObj_SetPackagePrefix(const char* prefix) noexcept
{
    if (prefix != nullptr)
    {
        std::strncpy(g_a4ed6c, prefix, sizeof(g_a4ed6c) - 1U);
        g_a4ed6c[sizeof(g_a4ed6c) - 1U] = '\0';
    }
}

void DynObj_SetAnimOverridePrefix(const char* prefix) noexcept
{
    if (prefix != nullptr)
    {
        std::strncpy(g_a4edf8, prefix, sizeof(g_a4edf8) - 1U);
        g_a4edf8[sizeof(g_a4edf8) - 1U] = '\0';
    }
    else
    {
        g_a4edf8[0] = '\0';
    }
}

void DynObj_SetPaletteLoader(PaletteLoaderFn fn) noexcept
{
    g_palette_loader = fn;
}

void DynObj_SetAnimIffLoader(AnimIffLoaderFn fn) noexcept
{
    g_anim_iff_loader = fn;
}

void DynObj_SetLovsEntryLoader(LovsEntryLoaderFn fn) noexcept
{
    g_lovs_entry_loader = fn;
}

} // namespace igi::retail::dynobj
