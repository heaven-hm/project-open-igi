#include "igi/retail/texture/texture_loaders.hpp"

#include "igi/retail/dynobj/hash_lookup.hpp"
#include "igi/retail/error/error_show.hpp"
#include "igi/retail/mem/mem_alloc.hpp"
#include "igi/retail/resource/is_resource_loaded.hpp"
#include "igi/retail/sys/resource_lookup.hpp"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string_view>
#include <vector>

namespace {

void require(bool condition, std::string_view message)
{
    if (!condition)
    {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

void write_u16_le(std::vector<std::uint8_t>& buf, std::size_t off, std::uint16_t val)
{
    if (buf.size() < off + 2U)
    {
        buf.resize(off + 2U, 0U);
    }
    buf[off + 0U] = static_cast<std::uint8_t>(val & 0xFFU);
    buf[off + 1U] = static_cast<std::uint8_t>((val >> 8U) & 0xFFU);
}

void write_u32_le(std::vector<std::uint8_t>& buf, std::size_t off, std::uint32_t val)
{
    if (buf.size() < off + 4U)
    {
        buf.resize(off + 4U, 0U);
    }
    buf[off + 0U] = static_cast<std::uint8_t>(val & 0xFFU);
    buf[off + 1U] = static_cast<std::uint8_t>((val >> 8U) & 0xFFU);
    buf[off + 2U] = static_cast<std::uint8_t>((val >> 16U) & 0xFFU);
    buf[off + 3U] = static_cast<std::uint8_t>((val >> 24U) & 0xFFU);
}

void write_u32_be(std::vector<std::uint8_t>& buf, std::size_t off, std::uint32_t val)
{
    if (buf.size() < off + 4U)
    {
        buf.resize(off + 4U, 0U);
    }
    buf[off + 0U] = static_cast<std::uint8_t>((val >> 24U) & 0xFFU);
    buf[off + 1U] = static_cast<std::uint8_t>((val >> 16U) & 0xFFU);
    buf[off + 2U] = static_cast<std::uint8_t>((val >> 8U) & 0xFFU);
    buf[off + 3U] = static_cast<std::uint8_t>(val & 0xFFU);
}

std::uintptr_t g_last_tex_packet[12]{};
std::int32_t g_primary_calls = 0;
std::int32_t g_secondary_calls = 0;

std::int32_t fake_primary_cb(void* pkt)
{
    std::memcpy(g_last_tex_packet, pkt, sizeof(g_last_tex_packet));
    ++g_primary_calls;
    return 1;
}

std::int32_t fake_secondary_cb(void* /*pkt*/)
{
    ++g_secondary_calls;
    return 77;
}

std::uintptr_t g_last_pal_packet[3]{};
std::int32_t g_palette_calls = 0;

std::int32_t fake_palette_cb(void* pkt)
{
    std::memcpy(g_last_pal_packet, pkt, sizeof(g_last_pal_packet));
    ++g_palette_calls;
    return 1;
}

void register_cached_resource(igi::retail::resource::ResourceHashNode& node,
                              igi::retail::resource::ResourceHashNode& sentinel,
                              const char* canonical_name, void* data, std::int32_t size)
{
    node = igi::retail::resource::ResourceHashNode{};
    sentinel = igi::retail::resource::ResourceHashNode{};
    node.next = &sentinel;
    node.hash = static_cast<std::int32_t>(igi::retail::sys::Sys_CheckHeap_15(canonical_name));
    std::strcpy(node.name(), canonical_name);
    node.kind = 1;
    node.size = size;
    node.data = data;

    auto& root = igi::retail::resource::Resource_GetHashRoot();
    root.next = &node;
    root.hash = -1;
}

void test_model_texture_loader_v2_and_v11()
{
    igi::retail::texture::Host_SetTextureBackendId(3);
    igi::retail::texture::Host_SetTextureCallbackSlots(0, 1, 2);
    igi::retail::texture::Host_SetTextureBackendCallback(0, &fake_primary_cb);
    igi::retail::texture::Host_SetTextureBackendCallback(1, &fake_secondary_cb);

    std::vector<std::uint8_t> v2(0x20U, 0U);
    write_u32_le(v2, 0U, 0x504F4F4CU); // LOOP
    write_u32_le(v2, 4U, 2U);          // version 2
    write_u16_le(v2, 0x08U, 256U);     // arg7
    write_u16_le(v2, 0x10U, 0x1111U);  // arg3
    write_u16_le(v2, 0x12U, 0x2222U);  // arg4
    write_u16_le(v2, 0x14U, 0x3333U);  // arg5
    write_u16_le(v2, 0x16U, 0x4444U);  // arg6

    igi::retail::resource::ResourceHashNode node{};
    igi::retail::resource::ResourceHashNode sentinel{};
    register_cached_resource(node, sentinel, "LOCAL:TEXTURES/CRATE.TEX", v2.data(),
                             static_cast<std::int32_t>(v2.size()));

    std::int32_t tex_slot[9]{};
    std::int32_t pal_slot[3]{};
    g_primary_calls = 0;
    g_secondary_calls = 0;

    const std::int32_t res = igi::retail::texture::Sys_SIsNotAValidTextureF(
        tex_slot, pal_slot, "LOCAL:textures/crate.tex");
    require(res == 1, "Sys_SIsNotAValidTextureF returns Resource_Unload result");
    require(tex_slot[0] == 3, "Sys_FreeMemory_8 writes Sys_ReadFileBlock_3() to *arg1");
    require(g_primary_calls == 1 && g_secondary_calls == 1, "both texture backend callbacks run");
    require(g_last_tex_packet[3] == 0x1111U && g_last_tex_packet[6] == 0x2222U
                && g_last_tex_packet[7] == 0x3333U && g_last_tex_packet[8] == 0x4444U
                && g_last_tex_packet[2] == 256U && g_last_tex_packet[10] == 1U,
            "Sys_FreeMemory_8 packet matches LOOP v2 fields and upload flag 1");
    require(igi::retail::texture::Texture_GetUploadFlag() == 0,
            "Sys_ResolveSymbol_8 clears upload flag after Sys_FreeMemory_8");
}

void test_terrain_texture_loader_v7()
{
    igi::retail::mem::Mem_ResetGlobalsForTest();
    igi::retail::texture::Host_SetTextureBackendId(4);
    igi::retail::texture::Host_SetTextureCallbackSlots(0, 1, 2);
    igi::retail::texture::Host_SetTextureBackendCallback(0, &fake_primary_cb);
    igi::retail::texture::Host_SetTextureBackendCallback(1, &fake_secondary_cb);
    igi::retail::texture::Host_SetTextureBackendCallback(2, &fake_palette_cb);

    std::vector<std::uint8_t> v7(0x90U, 0U);
    write_u32_le(v7, 0U, 0x504F4F4CU);     // LOOP
    write_u32_le(v7, 4U, 7U);              // version 7
    write_u16_le(v7, 0x12U, 0x0055U);      // shared param
    write_u32_le(v7, 0x18U, 0x60U);        // palette offset
    write_u32_le(v7, 0x20U, 1U);           // 1 texture record
    write_u32_le(v7, 0x34U, 0x80U);        // data offset (dest_2[-2])
    write_u32_le(v7, 0x38U, 0x11111111U);  // dest_2[-1]
    write_u32_le(v7, 0x3CU, 0x22222222U);  // dest_2[0]
    write_u32_le(v7, 0x40U, 0x33333333U);  // dest_2[1]
    write_u32_le(v7, 0x6CU, 16U);          // palette size at palette_offset + 0x0C

    igi::retail::resource::ResourceHashNode node{};
    igi::retail::resource::ResourceHashNode sentinel{};
    register_cached_resource(node, sentinel, "LOCAL:TERRAIN/TERRAIN.TEX", v7.data(),
                             static_cast<std::int32_t>(v7.size()));

    g_primary_calls = 0;
    g_palette_calls = 0;
    std::int32_t* const table =
        igi::retail::texture::Terrain_TextureFileNotOfRequ("LOCAL:terrain/terrain.tex");
    require(table != nullptr, "Terrain_TextureFileNotOfRequ allocates and returns record table");
    require(table[0] == 16, "table[0] stores palette byte size");
    require(table[1] == 1, "table[1] stores texture count");
    require(g_palette_calls == 1, "Sys_PushStack_21 uploads palette");
    require(g_primary_calls == 1, "Sys_FreeMemory_8 uploads v7 texture record");
    require(g_last_tex_packet[3] == 0x11111111U && g_last_tex_packet[6] == 0x22222222U
                && g_last_tex_packet[7] == 0x33333333U && g_last_tex_packet[2] == 0x0055U,
            "v7 record arguments match retail Terrain_TextureFileNotOfRequ");
}

void test_dynobj_local_textures_stex()
{
    igi::retail::mem::Mem_ResetGlobalsForTest();
    igi::retail::texture::Host_SetTextureBackendId(2);
    igi::retail::texture::Host_SetTextureCallbackSlots(0, 1, 2);
    igi::retail::texture::Host_SetTextureBackendCallback(0, &fake_primary_cb);
    igi::retail::texture::Host_SetTextureBackendCallback(1, &fake_secondary_cb);

    // Build a minimal IFF/FORM buffer with a 'TEXF' chunk containing 1 texture name: "wall".
    std::vector<std::uint8_t> form(0x20U, 0U);
    write_u32_be(form, 0U, 0x464F524DU);  // 'FORM'
    write_u32_be(form, 4U, 20U);          // form size
    write_u32_be(form, 8U, 0x4D545020U);  // 'MTP '
    write_u32_be(form, 12U, 0x54455846U); // 'TEXF'
    write_u32_be(form, 16U, 9U);          // payload size
    write_u32_le(form, 20U, 1U);          // count = 1
    std::memcpy(form.data() + 24U, "wall", 5U);

    // Cache "LOCAL:TEXTURES/WALL.TEX" as a valid LOOP v2 texture.
    std::vector<std::uint8_t> v2(0x20U, 0U);
    write_u32_le(v2, 0U, 0x504F4F4CU);
    write_u32_le(v2, 4U, 2U);
    igi::retail::resource::ResourceHashNode node{};
    igi::retail::resource::ResourceHashNode sentinel{};
    register_cached_resource(node, sentinel, "LOCAL:TEXTURES/WALL.TEX", v2.data(),
                             static_cast<std::int32_t>(v2.size()));

    std::uint8_t pkg_ctx[0x50]{};
    *reinterpret_cast<void**>(pkg_ctx + 0x08) = form.data();

    g_primary_calls = 0;
    igi::retail::texture::DynObj_LocalTexturesSTex(pkg_ctx);
    require(*reinterpret_cast<std::int32_t*>(pkg_ctx + 0x40) == 1,
            "DynObj_LocalTexturesSTex stores count at context+0x40");
    require(*reinterpret_cast<void**>(pkg_ctx + 0x34) != nullptr,
            "DynObj_LocalTexturesSTex allocates texture array at context+0x34");
    require(*reinterpret_cast<void**>(pkg_ctx + 0x38) != nullptr,
            "DynObj_LocalTexturesSTex allocates palette array at context+0x38");
    require(g_primary_calls == 1,
            "DynObj_LocalTexturesSTex loads and uploads LOCAL:textures/wall.tex");
}

} // namespace

int main()
{
    test_model_texture_loader_v2_and_v11();
    test_terrain_texture_loader_v7();
    test_dynobj_local_textures_stex();
    std::cout << "All retail texture loader tests passed.\n";
    return EXIT_SUCCESS;
}
