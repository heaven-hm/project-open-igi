// HOST HELPER: MEF skinner — no retail VA (host-side support logic).
// Artifact: not transcribed from retail igi.exe; complements native MEF loaders bound in the manifest.
// Tools/evidence: skinning record layout cross-checked with the project-igi-converter reference parsers.
// Behavior/CFG: host-only 40-byte skin record decode and weight assembly; no retail control flow is transcribed here.
// Status/limits: outside the native verification scope; correctness is covered by host unit tests.
#include "igi/retail/model/mef_skinner.hpp"

#include <algorithm>
#include <bit>
#include <cstring>

namespace igi::retail::model
{
namespace
{

constexpr std::size_t kRecordBytes = 40U;
constexpr std::size_t kMaximumRecords = 4000U;

[[nodiscard]] float read_float(std::span<const std::byte> bytes, std::size_t offset)
{
    const float value = std::bit_cast<float>(Ilff_ReadLittleEndian32(bytes, offset));
    return value;
}

[[nodiscard]] std::uint16_t read_unsigned_short(std::span<const std::byte> bytes, std::size_t offset)
{
    return static_cast<std::uint16_t>(std::to_integer<unsigned int>(bytes[offset]) |
                                      (std::to_integer<unsigned int>(bytes[offset + 1U]) << 8U));
}

// The retail palette 3x3 lives at byte offsets +0x24..+0x44 inside each 33-float entry; the
// translation sits at +0x18..+0x20. Float indices: rows {6,7,8}, {12,13,14}, {15,16,17},
// translation {6..} -> {+0x18 = float 6}. Kept as named constants so the layout is auditable.
constexpr std::size_t kTranslationX = 6U;  // byte +0x18
constexpr std::size_t kTranslationY = 7U;  // byte +0x1C
constexpr std::size_t kTranslationZ = 8U;  // byte +0x20
constexpr std::size_t kRow0 = 9U;          // byte +0x24
constexpr std::size_t kRow1 = 12U;         // byte +0x30
constexpr std::size_t kRow2 = 15U;         // byte +0x3C

} // namespace

MefSkinPalette MefSkinPalette_Identity(std::size_t bone_count)
{
    MefSkinPalette palette;
    palette.bones.resize(bone_count);
    for (auto& entry : palette.bones)
    {
        entry.floats[kRow0 + 0U] = 1.0F;
        entry.floats[kRow1 + 1U] = 1.0F;
        entry.floats[kRow2 + 2U] = 1.0F;
    }
    return palette;
}

int MefSkinPalette_HighestBone(const MefModel& model)
{
    if (!model.render_header.has_value() || model.render_format != MefRenderFormat::skinned)
    {
        return -1;
    }
    const MefRenderChunks chunks = model.render_chunks();
    if (chunks.vertex_stream == nullptr)
    {
        return -1;
    }
    const std::span<const std::byte> stream = chunks.vertex_stream->payload;
    const std::size_t record_count = model.render_header->base_vertex_count +
                                     model.render_header->extra_influence_count;
    int highest = -1;
    for (std::size_t index = 0; index < record_count && (index + 1U) * kRecordBytes <= stream.size();
         ++index)
    {
        const std::size_t offset = index * kRecordBytes;
        const auto bone = static_cast<int>(read_unsigned_short(stream, offset + 0x26U));
        highest = std::max(highest, bone);
    }
    return highest;
}

void Display_EvaluateSkinnedPalette(const MefModel& model, const MefSkinPalette& palette,
                                    std::span<MefRenderVertex> destination)
{
    if (!model.render_header.has_value())
    {
        throw MefFormatError("MEF has no renderer header.");
    }
    const MefRenderChunks chunks = model.render_chunks();
    if (chunks.vertex_stream == nullptr)
    {
        throw MefFormatError("MEF is missing its native vertex stream.");
    }
    const std::span<const std::byte> stream = chunks.vertex_stream->payload;
    const std::size_t base_count = model.render_header->base_vertex_count;
    const std::size_t extra_count = model.render_header->extra_influence_count;
    const std::size_t record_count = base_count + extra_count;
    if (record_count > kMaximumRecords)
    {
        // Retail 0x0049B700: Debug_Error("Bone model too large. Model %s has %d vertices.")
        // followed by the self-loop at 0x0049B72A. The port surfaces the same fatal text.
        throw MefFormatError("Bone model too large. Model has " + std::to_string(record_count) +
                             " vertices.");
    }
    if (stream.size() < record_count * kRecordBytes)
    {
        throw MefFormatError("Skinned vertex stream is shorter than its record count.");
    }
    if (destination.size() < base_count)
    {
        throw MefFormatError("Skinned destination holds fewer than the base vertex count.");
    }

    const auto rotation = [&palette](std::uint16_t bone_index, const std::array<float, 3>& vector) {
        if (static_cast<std::size_t>(bone_index) >= palette.bones.size())
        {
            throw MefFormatError("Skinned vertex names a bone outside the palette.");
        }
        const auto& entry = palette.bones[bone_index].floats;
        return std::array<float, 3>{
            entry[kRow0 + 0U] * vector[0] + entry[kRow0 + 1U] * vector[1] + entry[kRow0 + 2U] * vector[2],
            entry[kRow1 + 0U] * vector[0] + entry[kRow1 + 1U] * vector[1] + entry[kRow1 + 2U] * vector[2],
            entry[kRow2 + 0U] * vector[0] + entry[kRow2 + 1U] * vector[1] + entry[kRow2 + 2U] * vector[2],
        };
    };
    const auto translation = [&palette](std::uint16_t bone_index) {
        const auto& entry = palette.bones[bone_index].floats;
        return std::array<float, 3>{entry[kTranslationX], entry[kTranslationY], entry[kTranslationZ]};
    };

    // Base run: one bone per vertex. Position = (R*v + T) * record scale; the normal is rotated
    // by the same 3x3 with neither translation nor scale; the UV dwords are copied verbatim.
    for (std::size_t index = 0; index < base_count; ++index)
    {
        const std::size_t offset = index * kRecordBytes;
        const std::uint16_t bone_index = read_unsigned_short(stream, offset + 0x26U);
        const float scale = read_float(stream, offset + 0x20U);
        const std::array<float, 3> position{read_float(stream, offset + 0x00U),
                                            read_float(stream, offset + 0x04U),
                                            read_float(stream, offset + 0x08U)};
        const std::array<float, 3> normal{read_float(stream, offset + 0x0CU),
                                          read_float(stream, offset + 0x10U),
                                          read_float(stream, offset + 0x14U)};
        const std::array<float, 3> rotated = rotation(bone_index, position);
        const std::array<float, 3> bone_translation = translation(bone_index);
        const std::array<float, 3> rotated_normal = rotation(bone_index, normal);

        MefRenderVertex vertex;
        for (std::size_t component = 0; component < 3U; ++component)
        {
            vertex.position[component] =
                (rotated[component] + bone_translation[component]) * scale;
            vertex.normal[component] = rotated_normal[component];
        }
        vertex.texture_coordinates[0] = read_float(stream, offset + 0x18U);
        vertex.texture_coordinates[1] = read_float(stream, offset + 0x1CU);
        destination[index] = vertex;
    }

    // Extra run: the same 40 bytes read as {f32 weight, u16 target, u16 bone}. The contribution
    // weight * (R*v + T) accumulates into the target position only; targets outside the base
    // range are skipped, exactly as the transcription records.
    for (std::size_t index = base_count; index < record_count; ++index)
    {
        const std::size_t offset = index * kRecordBytes;
        const std::uint16_t target_vertex = read_unsigned_short(stream, offset + 0x24U);
        const std::uint16_t bone_index = read_unsigned_short(stream, offset + 0x26U);
        const float weight = read_float(stream, offset + 0x20U);
        if (target_vertex >= base_count)
        {
            continue;
        }
        const std::array<float, 3> position{read_float(stream, offset + 0x00U),
                                            read_float(stream, offset + 0x04U),
                                            read_float(stream, offset + 0x08U)};
        const std::array<float, 3> rotated = rotation(bone_index, position);
        const std::array<float, 3> bone_translation = translation(bone_index);
        MefRenderVertex& target = destination[target_vertex];
        for (std::size_t component = 0; component < 3U; ++component)
        {
            target.position[component] += weight * (rotated[component] + bone_translation[component]);
        }
    }
}

} // namespace igi::retail::model
