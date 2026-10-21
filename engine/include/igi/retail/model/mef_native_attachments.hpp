#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace igi::retail::model
{

struct alignas(4) MefNativeAttachmentRecord
{
    std::array<std::byte, 0x44> bytes{};
};

struct MefNativeMeshHeader
{
    std::array<std::byte, 0x7E> unknown_before_attachment_count{};
    std::int16_t attachment_count = 0;
    std::array<std::byte, 0x0C> unknown_before_attachments{};
    MefNativeAttachmentRecord* attachments = nullptr;
    std::array<std::byte, 0x0C> unknown_after_attachments{};
};

struct MefNativeResourceRecord
{
    std::array<std::byte, 0x0C> unknown_before_mesh_header{};
    MefNativeMeshHeader* mesh_header = nullptr;
};

struct MefNativeModelRecord
{
    std::array<std::byte, 0x0C> unknown_before_resource_record{};
    MefNativeResourceRecord* resource_record = nullptr;
};

// Model_GetAttachment() - 0x004D0CF0 - Found via REA/Ghidra, Binary Ninja and radare2.
// Source CFG score: 1.000 (catalog); static confidence: verified instruction/offset agreement.
// Tested using Windows x86 layout/record-address tests; original x86 instruction differential passed; live retail factory replay pending.
// REA: ev_e8a6d947c3dfb54d48f4d68b285b8650bdf8e407b2a517b012e08a16efa792fd.
// Catalog name is provisional: the body retrieves an ATTA record, rather than drawing bones.
[[nodiscard]] MefNativeAttachmentRecord* Model_GetAttachment(MefNativeModelRecord* model_record,
                                                             std::int32_t attachment_index);

// Model_GetAttachmentCount() - 0x004D0D10 - Found via REA/Ghidra, Binary Ninja and radare2.
// Source CFG score: 1.000 (catalog); static confidence: verified instruction/offset agreement.
// Tested using Windows x86 positive/negative signed-count tests; original x86 instruction differential passed; live retail factory replay pending.
// REA: ev_e22c9c9465b14b72a7b37179adbb24d23e80de9f0ad4a44fdbb689de3a634460.
// Catalog name is provisional: this returns the signed 16-bit MESH attachment count.
[[nodiscard]] std::int32_t Model_GetAttachmentCount(MefNativeModelRecord* model_record);

} // namespace igi::retail::model
