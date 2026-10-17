#include "igi/retail/dynobj/local_models_mef.hpp"

#include "igi/retail/mem/mem_alloc.hpp"
#include "igi/retail/render/backend.hpp"
#include "igi/retail/resource/is_resource_loaded.hpp"
#include "igi/retail/sys/resource_lookup.hpp"

#include <array>
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

void write_u32(std::vector<std::uint8_t>& buf, std::size_t offset, std::uint32_t value)
{
    if (buf.size() < offset + 4U)
    {
        buf.resize(offset + 4U, 0U);
    }
    buf[offset + 0U] = static_cast<std::uint8_t>(value & 0xFFU);
    buf[offset + 1U] = static_cast<std::uint8_t>((value >> 8U) & 0xFFU);
    buf[offset + 2U] = static_cast<std::uint8_t>((value >> 16U) & 0xFFU);
    buf[offset + 3U] = static_cast<std::uint8_t>((value >> 24U) & 0xFFU);
}

void append_ilff_chunk(std::vector<std::uint8_t>& buf, std::uint32_t tag,
                       const std::vector<std::uint8_t>& payload, bool last_chunk)
{
    const std::size_t chunk_offset = buf.size();
    buf.resize(chunk_offset + 16U, 0U);
    write_u32(buf, chunk_offset + 0U, tag);
    write_u32(buf, chunk_offset + 4U, static_cast<std::uint32_t>(payload.size()));
    write_u32(buf, chunk_offset + 8U, 4U);
    buf.insert(buf.end(), payload.begin(), payload.end());
    while ((buf.size() & 3U) != 0U)
    {
        buf.push_back(0U);
    }
    const std::uint32_t next_rel =
        last_chunk ? 0U : static_cast<std::uint32_t>(buf.size() - chunk_offset);
    write_u32(buf, chunk_offset + 12U, next_rel);
}

void* g_backend_container = nullptr;
void* g_backend_mesh = nullptr;
void fake_backend_init(void** pair)
{
    g_backend_container = pair[0];
    g_backend_mesh = pair[1];
}

struct alignas(void*) TestNodeBuffer {
    std::uint8_t bytes[0x100]{};
};

void test_dynobj_local_models_smef()
{
    igi::retail::mem::Mem_ResetGlobalsForTest();

    std::vector<std::uint8_t> container(20U, 0U);
    write_u32(container, 0U, 0x46464C49U);  // ILFF
    write_u32(container, 8U, 4U);           // align 4
    write_u32(container, 16U, 0x4D45434FU); // OCEM

    std::vector<std::uint8_t> mesh_payload(0x9CU, 0U);
    write_u32(mesh_payload, 0U, 0x3DCCCCCDU); // version 0.1f
    append_ilff_chunk(container, 0x4D455348U, mesh_payload, false);

    std::vector<std::uint8_t> atta_payload(8U, 0xAAU);
    append_ilff_chunk(container, 0x41545441U, atta_payload, false);

    std::vector<std::uint8_t> naxt_payload(16U, 0U);
    write_u32(naxt_payload, 0U, 10U); // frame rate / field 0 -> +0x08
    write_u32(naxt_payload, 4U, 3U);  // frame count / field 1 -> +0x00
    write_u32(naxt_payload, 8U, 0x12345678U);
    append_ilff_chunk(container, 0x4E415854U, naxt_payload, true);

    write_u32(container, 4U, static_cast<std::uint32_t>(container.size()));

    // Register "LOCAL:MODELS/CRATE.MEF" in the retail Is_ResourceLoaded hash chain.
    igi::retail::resource::ResourceHashNode node{};
    igi::retail::resource::ResourceHashNode sentinel{};
    node.next = &sentinel;
    const char* const canonical = "LOCAL:MODELS/CRATE.MEF";
    node.hash = static_cast<std::int32_t>(igi::retail::sys::Sys_CheckHeap_15(canonical));
    std::strcpy(node.name(), canonical);
    node.size = static_cast<std::int32_t>(container.size());
    node.data = container.data();

    auto& root = igi::retail::resource::Resource_GetHashRoot();
    root.next = &node;
    root.hash = -1;

    igi::retail::render::Host_SetRenderBackendId(2);
    igi::retail::dynobj::Host_SetModelBackendInit(2, &fake_backend_init);
    g_backend_container = nullptr;
    g_backend_mesh = nullptr;

    auto** const result =
        static_cast<void**>(igi::retail::dynobj::DynObj_LocalModelsSMef("crate"));
    require(result != nullptr, "DynObj_LocalModelsSMef returns allocated model record");
    require(std::strcmp(igi::retail::dynobj::DynObj_GetModelPathBuffer(),
                        "LOCAL:models/crate.mef")
                == 0,
            "DynObj_LocalModelsSMef formats LOCAL:models/%s.mef into 0x00A4EC6C");
    require(result[2] == container.data(), "result[2] holds container pointer from Resource_Load");
    require(result[3] != nullptr, "result[3] holds MESH payload pointer");
    require(reinterpret_cast<std::intptr_t>(result[4]) == 1, "result[4] records 1 NAXT chunk");
    require(result[5] != nullptr, "result[5] holds allocated NAXT array");

    const auto* const naxt_rec = static_cast<const std::int32_t*>(result[5]);
    require(naxt_rec[0] == 3, "NAXT record +0x00 holds data_32[1]");
    require(naxt_rec[1] == 0, "NAXT record +0x04 is cleared to 0");
    require(naxt_rec[2] == 10, "NAXT record +0x08 holds data_32[0]");

    require(g_backend_container == result[2], "backend callback receives container pointer");
    require(g_backend_mesh == result[3], "backend callback receives MESH payload pointer");
}

} // namespace

int main()
{
    test_dynobj_local_models_smef();
    std::cout << "All retail DynObj_LocalModelsSMef tests passed.\n";
    return EXIT_SUCCESS;
}
