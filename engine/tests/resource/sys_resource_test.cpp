#include "igi/retail/resource/sys_resource.hpp"

#include "igi/retail/resource/is_resource_loaded.hpp"
#include "igi/retail/resource/resource_lifecycle.hpp"

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

void test_camera_apply_line_filter_21()
{
    require(igi::retail::resource::Camera_ApplyLineFilter_21("RESOURCE_EXTRA", "RESOURCE", 8) == 0,
            "Camera_ApplyLineFilter_21 returns 0 on matching 8-byte prefix");
    require(igi::retail::resource::Camera_ApplyLineFilter_21("ILFF_DATA", "RESOURCE", 8) != 0,
            "Camera_ApplyLineFilter_21 returns non-zero on differing prefix");
}

void test_sys_resource_ilff_ires_and_unload()
{
    igi::retail::resource::Resource_InitPoolForTest(32U);

    // Construct an ILFF/IRES container with 1 child: NAME ("LOCAL:models/cube.mef") + BODY (8 bytes).
    // Header:
    //   +0x00: 'ILFF' (0x46464C49)
    //   +0x04: total_size (0x60)
    //   +0x08: alignment (4)
    //   +0x0C: next (0)
    //   +0x10: 'IRES' (0x53455249)
    //   +0x14: first child (NAME)
    // NAME chunk at +0x14:
    //   +0x00: 'NAME' (0x454D414E)
    //   +0x04: chunk_size (0x28)
    //   +0x08: alignment (4)
    //   +0x0C: next_offset (0x28 -> +0x3C)
    //   +0x10..: "LOCAL:models\\cube.mef\0"
    // BODY chunk at +0x3C:
    //   +0x00: 'BODY' (0x59444F42)
    //   +0x04: payload_size (8)
    //   +0x08: alignment (4)
    //   +0x0C: next_offset (0)
    //   +0x10..+0x17: payload bytes
    std::vector<std::uint8_t> ires(0x60U, 0U);
    write_u32_le(ires, 0x00U, 0x46464C49U); // 'ILFF'
    write_u32_le(ires, 0x04U, 0x60U);
    write_u32_le(ires, 0x08U, 4U);
    write_u32_le(ires, 0x0CU, 0U);
    write_u32_le(ires, 0x10U, 0x53455249U); // 'IRES'

    write_u32_le(ires, 0x14U, 0x454D414EU); // 'NAME'
    write_u32_le(ires, 0x18U, 0x28U);
    write_u32_le(ires, 0x1CU, 4U);
    write_u32_le(ires, 0x20U, 0x28U);
    std::strcpy(reinterpret_cast<char*>(ires.data() + 0x24U), "LOCAL:models\\cube.mef");

    write_u32_le(ires, 0x3CU, 0x59444F42U); // 'BODY'
    write_u32_le(ires, 0x40U, 8U);
    write_u32_le(ires, 0x44U, 4U);
    write_u32_le(ires, 0x48U, 0U);
    write_u32_le(ires, 0x4CU, 0xCAFEBABEU);
    write_u32_le(ires, 0x50U, 0xDEADBEEFU);

    void* const ret = igi::retail::resource::Sys_Resource(
        "LOCAL:models/level1.res", ires.data(), static_cast<std::int32_t>(ires.size()));
    require(ret == ires.data(), "Sys_Resource returns container pointer for ILFF/IRES");
    require(igi::retail::resource::Resource_GetActiveCount() == 2,
            "Sys_Resource allocates 1 parent node + 1 child node");

    igi::retail::resource::ResourceHashNode* child_node = nullptr;
    require(igi::retail::resource::Is_ResourceLoaded("LOCAL:models/cube.mef", &child_node) == 1,
            "Child resource registered by Sys_ErrorInResourceFileS is found via Is_ResourceLoaded");
    require(child_node != nullptr && child_node->kind == 1 && child_node->size == 8,
            "Child node records kind=1 and size=8");

    std::int32_t loaded_size = 0;
    void* const payload = igi::retail::resource::Resource_Load("LOCAL:models/cube.mef", &loaded_size);
    require(payload == ires.data() + 0x4CU && loaded_size == 8,
            "Resource_Load returns child BODY payload pointer and size");

    // Unloading the parent container recursively frees child nodes via Sys_FreeMemory_16.
    igi::retail::resource::Resource_Unload("LOCAL:models/level1.res");
    require(igi::retail::resource::Resource_GetActiveCount() == 0,
            "Resource_Unload -> Sys_FreeMemory_16 frees parent and child nodes");
    require(igi::retail::resource::Is_ResourceLoaded("LOCAL:models/cube.mef", nullptr) == 0,
            "Child resource is unlinked after container unload");
}

void test_sys_resource_packed_archive()
{
    igi::retail::resource::Resource_InitPoolForTest(32U);

    // Construct a "RESOURCE" packed archive with 1 entry ("LOCAL:tex/a.tex", 4 bytes payload).
    // Header:
    //   +0x00..+0x07: "RESOURCE"
    //   +0x08 (dwords[2]): count = 1
    // Entry 0 at +0x20 (&dwords[8]):
    //   +0x04: temp_1 (payload size = 4)
    //   +0x08: data_7 (name length = 15)
    //   +0x0C..: "LOCAL:tex/a.tex"
    //   payload offset = (15 + 0x2B) & ~0x1F = 0x20 -> entry + 0x20 = +0x40
    std::vector<std::uint8_t> archive(0x60U, 0U);
    std::memcpy(archive.data(), "RESOURCE", 8U);
    write_u32_le(archive, 0x08U, 1U);
    write_u32_le(archive, 0x20U + 0x04U, 4U);
    write_u32_le(archive, 0x20U + 0x08U, 15U);
    std::memcpy(archive.data() + 0x20U + 0x0CU, "LOCAL:tex/a.tex", 15U);
    write_u32_le(archive, 0x40U, 0x11223344U);

    igi::retail::resource::Sys_Resource(
        "LOCAL:tex/pack.res", archive.data(), static_cast<std::int32_t>(archive.size()));

    std::int32_t size = 0;
    void* const data = igi::retail::resource::Resource_Load("LOCAL:tex/a.tex", &size);
    require(data == archive.data() + 0x40U && size == 4,
            "Sys_Resource unpacks 'RESOURCE' archive entries at aligned offsets");

    igi::retail::resource::Resource_Unload("LOCAL:tex/pack.res");
    require(igi::retail::resource::Resource_GetActiveCount() == 0,
            "Unloading 'RESOURCE' archive frees parent and child entries");
}

} // namespace

int main()
{
    test_camera_apply_line_filter_21();
    test_sys_resource_ilff_ires_and_unload();
    test_sys_resource_packed_archive();
    std::cout << "All retail Sys_Resource tests passed.\n";
    return EXIT_SUCCESS;
}
