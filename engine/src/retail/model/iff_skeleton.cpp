// Native: AnimIff_LoadAnimationFile; retail VA 0x004D67A0; signature/calling convention x86 cdecl int32_t(int32_t* context).
// Artifact: retail IGI.EXE SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; implementation engine/src/retail/model/iff_skeleton.cpp.
// Tools/evidence: Binary Ninja basic blocks and Radare2 CFG in docs/fidelity/evidence/0x004D67A0-binaryninja.json and 0x004D67A0-radare2.json; paired retail and candidate execution in scripts/native_iff_diff_runner.py.
// Behavior/CFG: The retail loads the file through the resource loader, walks the chunk tree to the BOBJ form and then to the BOBH form, and reads the skeleton from three chunks: BOSH holds the object id and the bone count, PLST the parent index of every bone and TLST the rest-pose joint offset of every bone. The port reads the same three chunks and builds the same world positions.
// Status/limits: STRUCTURALLY_VERIFIED for the skeleton read; the retail also parses the animation clips in the same file, which the port does not play yet, so the port reads the skeleton only.
#include "igi/retail/model/iff_skeleton.hpp"

#include "igi/retail/model/iff_chunk_tree.hpp"

#include <cstddef>
#include <cstring>
#include <memory>
#include <algorithm>
#include <optional>
#include <vector>

namespace igi::retail::model
{
namespace
{

// The chunk tags the retail's animation loader compares, as the retail's
// four byte constants.
constexpr std::uint32_t tag_bobj = 0x424F424AU;  // "BOBJ"
constexpr std::uint32_t tag_bobh = 0x424F4248U;  // "BOBH"
constexpr std::uint32_t tag_bosh = 0x424F5348U;  // "BOSH"
constexpr std::uint32_t tag_plst = 0x504C5354U;  // "PLST"
constexpr std::uint32_t tag_tlst = 0x544C5354U;  // "TLST"

std::uint32_t ReadLittleEndian32(const std::byte* bytes)
{
    const auto* raw = reinterpret_cast<const unsigned char*>(bytes);
    return static_cast<std::uint32_t>(raw[0]) | (static_cast<std::uint32_t>(raw[1]) << 8) |
           (static_cast<std::uint32_t>(raw[2]) << 16) | (static_cast<std::uint32_t>(raw[3]) << 24);
}

std::int32_t ReadLittleEndianSigned32(const std::byte* bytes)
{
    return static_cast<std::int32_t>(ReadLittleEndian32(bytes));
}

float ReadLittleEndianFloat(const std::byte* bytes)
{
    const std::uint32_t bits = ReadLittleEndian32(bytes);
    float value = 0.0F;
    static_assert(sizeof(value) == sizeof(bits), "the float and the bit pattern must match");
    std::memcpy(&value, &bits, sizeof(value));
    return value;
}

} // namespace

std::optional<IffSkeleton> IffSkeleton_Read(const std::filesystem::path& path)
{
    // Transcribed from retail 0x004D67A0, the retail's animation file loader.
    // The retail loads the file through the resource loader, walks the chunk
    // tree to the "BOBJ" form and then to the "BOBH" form, and reads the
    // skeleton from three chunks: "BOSH" holds the object id and the bone
    // count, "PLST" holds the parent index of every bone and "TLST" the
    // rest-pose joint offset of every bone.
    const std::vector<std::unique_ptr<IffChunkNode>> roots = IffChunkTree_Parse(path);
    if (roots.empty())
    {
        return std::nullopt;
    }

    IffChunkNode* body = IffChunk_FindByTag(roots.front().get(), tag_bobj);
    if (body == nullptr)
    {
        return std::nullopt;
    }
    IffChunkNode* bone_header = IffChunk_FindByTag(body, tag_bobh);
    if (bone_header == nullptr)
    {
        return std::nullopt;
    }
    IffChunkNode* counts = IffChunk_FindChildByTag(bone_header, tag_bosh);
    IffChunkNode* parents = IffChunk_FindChildByTag(bone_header, tag_plst);
    IffChunkNode* translations = IffChunk_FindChildByTag(bone_header, tag_tlst);
    if (counts == nullptr || parents == nullptr || translations == nullptr)
    {
        return std::nullopt;
    }

    const std::byte* count_bytes = IffChunk_GetPayload(counts);
    if (count_bytes == nullptr)
    {
        return std::nullopt;
    }
    IffSkeleton skeleton;
    skeleton.object_id = ReadLittleEndian32(count_bytes);
    const std::size_t bone_count = ReadLittleEndian32(count_bytes + 4U);
    if (bone_count == 0U)
    {
        return std::nullopt;
    }

    const std::byte* parent_bytes = IffChunk_GetPayload(parents);
    const std::byte* translation_bytes = IffChunk_GetPayload(translations);
    if (parent_bytes == nullptr || translation_bytes == nullptr)
    {
        return std::nullopt;
    }

    skeleton.parents.resize(bone_count);
    for (std::size_t index = 0; index < bone_count; ++index)
    {
        skeleton.parents[index] = ReadLittleEndianSigned32(parent_bytes + index * 4U);
    }
    skeleton.translations.resize(bone_count);
    skeleton.world_positions.resize(bone_count);
    for (std::size_t index = 0; index < bone_count; ++index)
    {
        for (std::size_t axis = 0; axis < 3U; ++axis)
        {
            skeleton.translations[index][axis] =
                ReadLittleEndianFloat(translation_bytes + index * 12U + axis * 4U);
            skeleton.world_positions[index][axis] = skeleton.translations[index][axis];
        }
        // The world position is the bone's own offset plus every ancestor's.
        std::int32_t parent = skeleton.parents[index];
        while (parent >= 0 && static_cast<std::size_t>(parent) < bone_count)
        {
            for (std::size_t axis = 0; axis < 3U; ++axis)
            {
                skeleton.world_positions[index][axis] +=
                    skeleton.translations[static_cast<std::size_t>(parent)][axis];
            }
            parent = skeleton.parents[static_cast<std::size_t>(parent)];
        }
    }
    return skeleton;
}

const IffSkeleton* IffSkeletonTable::Find(std::uint32_t object_id) const
{
    if (object_id >= by_object_id.size())
    {
        return nullptr;
    }
    const std::optional<IffSkeleton>& entry = by_object_id[object_id];
    return entry.has_value() ? &*entry : nullptr;
}

IffSkeletonTable IffSkeletonTable_Build(const std::vector<IffSkeleton>& skeletons)
{
    IffSkeletonTable table;
    std::uint32_t highest = 0U;
    for (const IffSkeleton& skeleton : skeletons)
    {
        highest = std::max(highest, skeleton.object_id);
    }
    table.by_object_id.resize(static_cast<std::size_t>(highest) + 1U);
    for (const IffSkeleton& skeleton : skeletons)
    {
        table.by_object_id[skeleton.object_id] = skeleton;
    }
    return table;
}

} // namespace igi::retail::model
