#include "igi/retail/dynobj/mtp_package_loader.hpp"

#include "igi/retail/dynobj/hash_lookup.hpp"
#include "igi/retail/mem/mem_alloc.hpp"
#include "igi/retail/resource/is_resource_loaded.hpp"
#include "igi/retail/sys/resource_lookup.hpp"
#include "igi/retail/texture/texture_loaders.hpp"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
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

void append_form_chunk(std::vector<std::uint8_t>& form, std::uint32_t tag_be,
                       const std::vector<std::uint8_t>& payload)
{
    const std::size_t off = form.size();
    form.resize(off + 8U, 0U);
    write_u32_be(form, off + 0U, tag_be);
    write_u32_be(form, off + 4U, static_cast<std::uint32_t>(payload.size()));
    form.insert(form.end(), payload.begin(), payload.end());
    while ((form.size() & 3U) != 0U)
    {
        form.push_back(0U);
    }
}

std::string g_loaded_anim_path;
void* fake_anim_loader(const char* path)
{
    g_loaded_anim_path = path;
    return nullptr;
}

std::string g_loaded_pal_path;
std::int32_t fake_pal_loader(void* /*dst*/, const char* path)
{
    g_loaded_pal_path = path;
    return 1;
}

void test_mtp_package_loader()
{
    igi::retail::mem::Mem_ResetGlobalsForTest();
    igi::retail::dynobj::Host_SetPackagePrefix("LOCAL:");
    igi::retail::dynobj::Host_SetAnimOverridePrefix(nullptr);
    igi::retail::dynobj::Host_SetAnimIffLoader(&fake_anim_loader);
    igi::retail::dynobj::Host_SetPaletteLoader(&fake_pal_loader);

    // Build an IFF FORM containing BANM (1 anim: "walk"), TEXF (0), PALF (1: "base"),
    // MODS (0), VNAM (1 instance: "inst_a"), INST (1 instance: model 0, 1 mat -> GTT 0),
    // and GTT (1 entry: tex 0, pal 0).
    std::vector<std::uint8_t> form(12U, 0U);
    write_u32_be(form, 0U, 0x464F524DU); // FORM
    write_u32_be(form, 8U, 0x4D545020U); // MTP

    std::vector<std::uint8_t> banm(9U, 0U);
    write_u32_le(banm, 0U, 1U);
    std::memcpy(banm.data() + 4U, "walk", 5U);
    append_form_chunk(form, 0x42414E4DU, banm);

    std::vector<std::uint8_t> texf(4U, 0U);
    write_u32_le(texf, 0U, 0U);
    append_form_chunk(form, 0x54455846U, texf);

    std::vector<std::uint8_t> palf(9U, 0U);
    write_u32_le(palf, 0U, 1U);
    std::memcpy(palf.data() + 4U, "base", 5U);
    append_form_chunk(form, 0x50414C46U, palf);

    std::vector<std::uint8_t> mods(4U, 0U);
    write_u32_le(mods, 0U, 0U);
    append_form_chunk(form, 0x4D4F4453U, mods);

    std::vector<std::uint8_t> vnam(15U, 0U);
    write_u32_le(vnam, 0U, 1U);
    write_u32_le(vnam, 4U, 0U); // index table entry
    std::memcpy(vnam.data() + 8U, "inst_a", 7U);
    append_form_chunk(form, 0x564E414DU, vnam);

    std::vector<std::uint8_t> inst(12U, 0U);
    write_u32_le(inst, 0U, 0U); // model index 0
    write_u32_le(inst, 4U, 1U); // 1 material ref
    write_u32_le(inst, 8U, 0U); // GTT index 0
    append_form_chunk(form, 0x494E5354U, inst);

    std::vector<std::uint8_t> gtt(12U, 0U);
    write_u32_le(gtt, 0U, 1U);  // 1 GTT entry
    write_u32_le(gtt, 4U, 0U);  // tex index 0
    write_u32_le(gtt, 8U, 0U);  // pal index 0
    append_form_chunk(form, 0x47545420U, gtt);

    write_u32_be(form, 4U, static_cast<std::uint32_t>(form.size() - 8U));

    // Cache "LOCAL:TEXTURES/LEVEL.RES" and "LOCAL:MODELS/LEVEL.RES".
    std::uint8_t dummy_res[8]{};
    igi::retail::resource::ResourceHashNode tex_res{};
    igi::retail::resource::ResourceHashNode mod_res{};
    igi::retail::resource::ResourceHashNode sentinel{};
    tex_res.next = &mod_res;
    tex_res.hash = static_cast<std::int32_t>(
        igi::retail::sys::Sys_CheckHeap_15("LOCAL:TEXTURES/LEVEL.RES"));
    std::strcpy(tex_res.name(), "LOCAL:TEXTURES/LEVEL.RES");
    tex_res.kind = 1;
    tex_res.size = 8;
    tex_res.data = dummy_res;

    mod_res.next = &sentinel;
    mod_res.hash = static_cast<std::int32_t>(
        igi::retail::sys::Sys_CheckHeap_15("LOCAL:MODELS/LEVEL.RES"));
    std::strcpy(mod_res.name(), "LOCAL:MODELS/LEVEL.RES");
    mod_res.kind = 1;
    mod_res.size = 8;
    mod_res.data = dummy_res;

    auto& root = igi::retail::resource::Resource_GetHashRoot();
    root.next = &tex_res;
    root.hash = -1;

    // Set up package context:
    // +0x08 = form.data()
    // +0x10 = model list head (with a dummy HashItem + sentinel so DynObj_LookupHashItem_14(0) returns &model_item)
    // +0x28 = instance list header (with +0x04 head anchor and +0x08 tail pointer)
    alignas(void*) std::uint8_t pkg_ctx[0x100]{};
    *reinterpret_cast<void**>(pkg_ctx + 0x08) = form.data();

    igi::retail::dynobj::HashItem model_sentinel{};
    igi::retail::dynobj::HashItem model_item{&model_sentinel};
    *reinterpret_cast<igi::retail::dynobj::HashItem**>(pkg_ctx + 0x10) = &model_item;

    // Initialize list header at pkg_ctx + 0x28 so +0x08 points to +0x04.
    *reinterpret_cast<void**>(pkg_ctx + 0x28 + 0x08) = pkg_ctx + 0x28 + 0x04;

    g_loaded_anim_path.clear();
    g_loaded_pal_path.clear();

    igi::retail::dynobj::DynObj_InsertHashItem_14(pkg_ctx, "level", 1);

    require(g_loaded_anim_path == "LOCAL:anims/walk.iff",
            "DynObj_RemoveSymbol_14 formats and loads BANM animation");
    require(g_loaded_pal_path == "LOCAL:palettes/base.pal",
            "DynObj_LinkDynamicCube_14 formats and loads PALF palette");
    require(mod_res.loaded_flag == 1,
            "DynObj_InsertHashItem_14 loads models.res via Resource_Load");

    auto** const inst_rec = *reinterpret_cast<void***>(pkg_ctx + 0x28 + 0x08);
    require(inst_rec != nullptr && reinterpret_cast<std::uint8_t*>(inst_rec) != (pkg_ctx + 0x2C),
            "DynObj_InsertHashItem_14 links allocated instance record into context+0x28");
    require(std::strcmp(static_cast<const char*>(inst_rec[2]), "inst_a") == 0,
            "instance record +0x08 (slot 2) points to VNAM instance name");
    require(inst_rec[3] == &model_item,
            "instance record +0x0C (slot 3) points to resolved model item");
    require(reinterpret_cast<std::intptr_t>(inst_rec[6]) == 1,
            "instance record +0x18 (slot 6) stores material count");
}

} // namespace

int main()
{
    test_mtp_package_loader();
    std::cout << "All retail DynObj_InsertHashItem_14 tests passed.\n";
    return EXIT_SUCCESS;
}
