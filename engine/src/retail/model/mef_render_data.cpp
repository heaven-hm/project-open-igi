// HOST HELPER: MEF render-data builder — no retail VA (host-side support logic).
// Artifact: not transcribed from retail igi.exe; complements native MEF loaders bound in the manifest.
// Tools/evidence: MEF chunk-format parsing cross-checked with the project-igi-converter reference parsers.
// Behavior/CFG: host-only chunk decode/layout assembly; no retail control flow is transcribed here.
// Status/limits: outside the native verification scope; correctness is covered by host unit tests.
#include "igi/retail/model/mef_render_data.hpp"

#include "igi/retail/model/mef_bone_rig.hpp"
#include "igi/retail/model/mef_skinner.hpp"

#include <algorithm>
#include <bit>
#include <cmath>

namespace igi::retail::model
{
namespace
{

std::uint16_t read_unsigned_short(std::span<const std::byte> bytes, std::size_t offset)
{
    if (offset > bytes.size() || bytes.size() - offset < 2U)
    {
        throw MefFormatError("MEF draw record extends past its chunk.");
    }
    return static_cast<std::uint16_t>(std::to_integer<unsigned int>(bytes[offset]) |
                                      (std::to_integer<unsigned int>(bytes[offset + 1U]) << 8U));
}

float read_float(std::span<const std::byte> bytes, std::size_t offset)
{
    const float value = std::bit_cast<float>(Ilff_ReadLittleEndian32(bytes, offset));
    if (!std::isfinite(value))
    {
        throw MefFormatError("MEF vertex contains a non-finite float.");
    }
    return value;
}

} // namespace

MefRenderData Display_DecodeMeshRenderData(const MefModel &model,
                                      const IffSkeletonTable &skeletons,
                                      MefBoneRig *out_rig)
{
    if (!model.render_header.has_value())
    {
        throw MefFormatError("MEF has no renderer header.");
    }
    const MefRenderChunks chunks = model.render_chunks();
    if (chunks.vertex_stream == nullptr || chunks.draw_records == nullptr ||
        chunks.d3d_render_header == nullptr)
    {
        throw MefFormatError("MEF is missing its native renderer chunks.");
    }
    const bool lightmapped = model.render_format == MefRenderFormat::lightmapped;
    const bool skinned = model.render_format == MefRenderFormat::skinned;
    // Skinned streams hold 40-byte influence records; diffuse/lightmapped hold 32/40-byte
    // plain vertices. The posed output is always the 32-byte render vertex.
    const std::size_t source_stride = skinned ? 40U : (lightmapped ? 40U : 32U);
    const std::span<const std::byte> vertex_bytes = chunks.vertex_stream->payload;
    const std::size_t vertex_count = model.render_header->vertex_count;
    if (vertex_count > vertex_bytes.size() / source_stride)
    {
        throw MefFormatError("MEF vertex count exceeds its native vertex stream.");
    }
    MefRenderData render_data;
    render_data.primitive_type = Ilff_ReadLittleEndian32(chunks.d3d_render_header->payload, 0U);
    if (render_data.primitive_type < 1U || render_data.primitive_type > 6U)
    {
        throw MefFormatError("MEF primitive type is outside the Direct3D 7 enum.");
    }
    render_data.vertices.reserve(vertex_count);
    if (skinned)
    {
        // Retail 0x0049B700: pose the base vertices through the bone palette.
        // The retail MEF carries no skeleton, so the palette comes from the
        // retail animation IFF the model package names. The skeleton that fits
        // the model's own influence records is selected, and the live
        // animation replaces the palette with the posed bones.
        const int highest_bone = MefSkinPalette_HighestBone(model);
        const std::uint32_t max_bone_index =
            highest_bone < 0 ? 0U : static_cast<std::uint32_t>(highest_bone);
        MefSkinPalette palette;
        if (!skeletons.by_object_id.empty())
        {
            const MefBoneRig rig = MefBoneRig_Select(model, skeletons);
            palette = MefBoneRig_Palette(skeletons, rig.object_id);
            if (out_rig != nullptr)
            {
                *out_rig = rig;
            }
        }
        else
        {
            palette = MefSkinPalette_Identity(
                highest_bone < 0 ? 1U : static_cast<std::size_t>(highest_bone) + 1U);
        }
        if (palette.bones.size() <= static_cast<std::size_t>(max_bone_index))
        {
            palette.bones.resize(static_cast<std::size_t>(max_bone_index) + 1U);
        }
        render_data.vertices.resize(vertex_count);
        Display_EvaluateSkinnedPalette(model, palette, render_data.vertices);
    }
    else
    {
    for (std::size_t vertex_index = 0; vertex_index < vertex_count; ++vertex_index)
    {
        const std::size_t offset = vertex_index * source_stride;
        MefRenderVertex vertex;
        for (std::size_t component_index = 0; component_index < 3U; ++component_index)
        {
            vertex.position[component_index] = read_float(vertex_bytes, offset + component_index * 4U);
            vertex.normal[component_index] = read_float(vertex_bytes, offset + 12U + component_index * 4U);
        }
        for (std::size_t coordinate_index = 0; coordinate_index < 2U; ++coordinate_index)
        {
            vertex.texture_coordinates[coordinate_index] =
                read_float(vertex_bytes, offset + 24U + coordinate_index * 4U);
            if (lightmapped)
            {
                vertex.lightmap_coordinates[coordinate_index] =
                    read_float(vertex_bytes, offset + 32U + coordinate_index * 4U);
            }
        }
        render_data.vertices.push_back(vertex);
    }
    }
    const std::span<const std::byte> draw_bytes = chunks.draw_records->payload;
    const std::size_t header_size = lightmapped ? 32U : 28U;
    std::size_t record_offset = 0;
    if (draw_bytes.empty())
    {
        if (model.render_header->submesh_count != 0U)
        {
            throw MefFormatError("MEF declares draw batches but has an empty draw chunk.");
        }
        return render_data;
    }
    for (;;)
    {
        if (record_offset > draw_bytes.size() || draw_bytes.size() - record_offset < header_size)
        {
            throw MefFormatError("MEF draw batch header is truncated.");
        }
        const std::size_t index_count = read_unsigned_short(draw_bytes, record_offset + 12U);
        const std::uint16_t next_record_bytes = read_unsigned_short(draw_bytes, record_offset + 14U);
        MefRenderBatch batch;
        batch.material_index = read_unsigned_short(draw_bytes, record_offset + 16U);
        batch.first_vertex = read_unsigned_short(draw_bytes, record_offset + (lightmapped ? 20U : 18U));
        batch.vertex_count = read_unsigned_short(draw_bytes, record_offset + (lightmapped ? 22U : 20U));
        batch.translucent = lightmapped ? Ilff_ReadLittleEndian32(draw_bytes, record_offset + 24U) != 0U
                                        : read_unsigned_short(draw_bytes, record_offset + 22U) != 0U;
        if (static_cast<std::size_t>(batch.first_vertex) + batch.vertex_count > vertex_count)
        {
            throw MefFormatError("MEF batch vertex range exceeds its vertex stream.");
        }
        const std::size_t indices_offset = record_offset + header_size;
        if (index_count > (draw_bytes.size() - indices_offset) / 2U)
        {
            throw MefFormatError("MEF batch index stream is truncated.");
        }
        batch.indices.reserve(index_count);
        for (std::size_t index_number = 0; index_number < index_count; ++index_number)
        {
            const std::uint16_t vertex_index =
                read_unsigned_short(draw_bytes, indices_offset + index_number * 2U);
            if (vertex_index >= batch.vertex_count)
            {
                throw MefFormatError("MEF batch index exceeds its local vertex range.");
            }
            batch.indices.push_back(vertex_index);
        }
        render_data.batches.push_back(std::move(batch));
        if (next_record_bytes == 0xFFFFU)
        {
            break;
        }
        if (next_record_bytes < index_count * 2U)
        {
            throw MefFormatError("MEF next batch overlaps its index stream.");
        }
        record_offset += header_size + next_record_bytes;
    }
    return render_data;
}

} // namespace igi::retail::model
