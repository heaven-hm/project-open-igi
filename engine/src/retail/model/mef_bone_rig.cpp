// HOST HELPER: the rig a skinned MEF model uses. No retail VA: the retail takes the
// skeleton from the character it builds, while the port renders a model on its own.
// Artifact: retail IGI.EXE SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; the skeleton data comes from the retail animation files under COMMON/ANIMS.
// Tools/evidence: the influence agreement is measured against the retail models and the retail skeletons in docs/fidelity/mef_bone_rig.md; the palette is the retail's at 0x0049B700.
// Behavior/CFG: A skinned MEF stores every influencing bone of a vertex in that bone's own local space, so the records of one vertex have to place it on the same point in model space. The selection measures that agreement and takes the skeleton that agrees; the palette is the identity rotation and the bone world position.
// Status/limits: host glue for a model that the retail only renders as part of a character; the matching skeleton agrees to 0.03-0.10 retail units and every other candidate is off by 55 to 1723.
#include "igi/retail/model/mef_bone_rig.hpp"

#include "igi/retail/model/mef_model_loader.hpp"

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <limits>
#include <span>
#include <vector>

namespace igi::retail::model
{
namespace
{

constexpr std::size_t kRecordBytes = 40U;

// The retail skinned record: the position at +0x00, the weight at +0x20, the
// target vertex at +0x24 and the bone index at +0x26.
struct InfluenceRecord
{
    std::array<float, 3> position{};
    float weight = 0.0F;
    std::uint16_t target = 0U;
    std::uint16_t bone = 0U;
};

[[nodiscard]] std::uint16_t ReadUnsignedShort(std::span<const std::byte> bytes, std::size_t offset)
{
    return static_cast<std::uint16_t>(std::to_integer<unsigned int>(bytes[offset]) |
                                      (std::to_integer<unsigned int>(bytes[offset + 1U]) << 8U));
}

[[nodiscard]] float ReadFloat(std::span<const std::byte> bytes, std::size_t offset)
{
    const std::uint32_t bits = Ilff_ReadLittleEndian32(bytes, offset);
    float value = 0.0F;
    static_assert(sizeof(value) == sizeof(bits), "the float and the bit pattern must match");
    std::memcpy(&value, &bits, sizeof(value));
    return value;
}

// Reads the model's influence records, base records first.
[[nodiscard]] std::vector<InfluenceRecord> ReadInfluences(const MefModel& model, std::size_t& base_count)
{
    std::vector<InfluenceRecord> records;
    base_count = 0U;
    if (!model.render_header.has_value())
    {
        return records;
    }
    const MefRenderChunks chunks = model.render_chunks();
    if (chunks.vertex_stream == nullptr)
    {
        return records;
    }
    const std::span<const std::byte> stream = chunks.vertex_stream->payload;
    base_count = model.render_header->base_vertex_count;
    const std::size_t total = base_count + model.render_header->extra_influence_count;
    records.reserve(total);
    for (std::size_t index = 0; index < total; ++index)
    {
        const std::size_t offset = index * kRecordBytes;
        if (offset + kRecordBytes > stream.size())
        {
            break;
        }
        InfluenceRecord record;
        for (std::size_t axis = 0; axis < 3U; ++axis)
        {
            record.position[axis] = ReadFloat(stream, offset + axis * 4U);
        }
        record.weight = ReadFloat(stream, offset + 0x20U);
        record.target = ReadUnsignedShort(stream, offset + 0x24U);
        record.bone = ReadUnsignedShort(stream, offset + 0x26U);
        records.push_back(record);
    }
    return records;
}

// The median distance, in retail units, between a vertex's base record and its
// extra influence records once both are placed in model space.
//
// A skinned MEF stores every influencing bone of a vertex in that bone's own
// local space, so the correct skeleton places every record of a vertex on the
// same point. A skeleton that does not match the model moves the records
// apart, and the distance says by how much.
[[nodiscard]] double MeasureDisagreement(const std::vector<InfluenceRecord>& records,
                                          std::size_t base_count,
                                          const IffSkeleton& skeleton)
{
    if (base_count == 0U || skeleton.BoneCount() == 0U)
    {
        return std::numeric_limits<double>::infinity();
    }
    std::vector<std::array<float, 3>> base_points(base_count);
    std::vector<bool> has_base(base_count, false);
    for (std::size_t index = 0; index < base_count && index < records.size(); ++index)
    {
        const InfluenceRecord& record = records[index];
        if (record.bone >= skeleton.BoneCount())
        {
            return std::numeric_limits<double>::infinity();
        }
        const auto& world = skeleton.world_positions[record.bone];
        for (std::size_t axis = 0; axis < 3U; ++axis)
        {
            base_points[index][axis] = record.position[axis] + world[axis];
        }
        has_base[index] = true;
    }

    std::vector<double> distances;
    for (std::size_t index = base_count; index < records.size(); ++index)
    {
        const InfluenceRecord& record = records[index];
        if (record.target >= base_count || !has_base[record.target])
        {
            continue;
        }
        if (record.bone >= skeleton.BoneCount())
        {
            return std::numeric_limits<double>::infinity();
        }
        const auto& world = skeleton.world_positions[record.bone];
        double distance = 0.0;
        for (std::size_t axis = 0; axis < 3U; ++axis)
        {
            const double difference =
                static_cast<double>(record.position[axis] + world[axis]) -
                static_cast<double>(base_points[record.target][axis]);
            distance = std::max(distance, std::abs(difference));
        }
        distances.push_back(distance);
    }
    if (distances.empty())
    {
        // A model whose vertices have a single influence cannot be told apart
        // by the records, so the fit is reported as perfect.
        return 0.0;
    }
    std::sort(distances.begin(), distances.end());
    return distances[distances.size() / 2U];
}

} // namespace

MefBoneRig MefBoneRig_Select(const MefModel& model, const IffSkeletonTable& skeletons)
{
    std::size_t base_count = 0U;
    const std::vector<InfluenceRecord> records = ReadInfluences(model, base_count);

    MefBoneRig best;
    best.disagreement = std::numeric_limits<double>::infinity();
    for (std::size_t object_id = 0; object_id < skeletons.by_object_id.size(); ++object_id)
    {
        const IffSkeleton* skeleton = skeletons.Find(static_cast<std::uint32_t>(object_id));
        if (skeleton == nullptr)
        {
            continue;
        }
        const double score = MeasureDisagreement(records, base_count, *skeleton);
        if (score < best.disagreement)
        {
            best.disagreement = score;
            best.object_id = static_cast<std::uint32_t>(object_id);
        }
    }
    return best;
}

MefSkinPalette MefBoneRig_Palette(const IffSkeletonTable& skeletons, std::uint32_t object_id)
{
    MefSkinPalette palette;
    const IffSkeleton* skeleton = skeletons.Find(object_id);
    if (skeleton == nullptr)
    {
        return palette;
    }
    palette.bones.resize(skeleton->BoneCount());
    for (std::size_t index = 0; index < skeleton->BoneCount(); ++index)
    {
        MefSkinPaletteEntry& entry = palette.bones[index];
        // The identity rotation, at the palette's +0x24 row.
        entry.floats[9] = 1.0F;
        entry.floats[13] = 1.0F;
        entry.floats[17] = 1.0F;
        // The bone world position, at the palette's +0x18 translation.
        entry.floats[6] = skeleton->world_positions[index][0];
        entry.floats[7] = skeleton->world_positions[index][1];
        entry.floats[8] = skeleton->world_positions[index][2];
    }
    return palette;
}

} // namespace igi::retail::model
