#include "igi/retail/model/mef_model_loader.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

using igi::retail::model::Ilff_ReadLittleEndian32;
using igi::retail::model::Ilff_ReadContainer;
using igi::retail::model::MefFormatError;
using igi::retail::model::MefModel;
using igi::retail::model::MefSkinnedVertex_ReadRetailRecord;

void write_u32(std::vector<std::byte>& destination, std::size_t offset, std::uint32_t value) {
    if (destination.size() < offset + sizeof(value)) {
        destination.resize(offset + sizeof(value));
    }
    for (std::size_t byte_index = 0U; byte_index < sizeof(value); ++byte_index) {
        destination[offset + byte_index] = static_cast<std::byte>((value >> (byte_index * 8U)) & 0xFFU);
    }
}

void append_chunk(
    std::vector<std::byte>& destination,
    std::uint32_t tag,
    std::span<const std::byte> payload,
    std::uint32_t alignment,
    bool last_chunk) {
    const std::size_t chunk_offset = destination.size();
    destination.resize(chunk_offset + 16U);
    write_u32(destination, chunk_offset, tag);
    write_u32(destination, chunk_offset + 4U, static_cast<std::uint32_t>(payload.size()));
    write_u32(destination, chunk_offset + 8U, alignment);

    const std::size_t payload_offset = (chunk_offset + 16U + alignment - 1U) & ~(static_cast<std::size_t>(alignment) - 1U);
    destination.resize(payload_offset);
    destination.insert(destination.end(), payload.begin(), payload.end());
    const std::size_t next_offset = destination.size() - chunk_offset;
    write_u32(destination, chunk_offset + 12U, last_chunk ? 0U : static_cast<std::uint32_t>(next_offset));
}

std::vector<std::byte> make_model_bytes(std::uint32_t render_format = 0U) {
    std::vector<std::byte> file_bytes(20U);
    write_u32(file_bytes, 0U, igi::retail::model::mef_tag::ilff);
    write_u32(file_bytes, 8U, 4U);
    write_u32(file_bytes, 16U, MefModel::form_ocem);

    std::vector<std::byte> mesh_header(MefModel::mesh_header_size);
    write_u32(mesh_header, 0U, MefModel::supported_version_bits);
    write_u32(mesh_header, 0x20U, render_format);

    append_chunk(file_bytes, igi::retail::model::mef_tag::mesh, mesh_header, 4U, false);
    const std::array<std::byte, 4> attachment_payload{
        std::byte{0x11}, std::byte{0x22}, std::byte{0x33}, std::byte{0x44}};
    append_chunk(file_bytes, igi::retail::model::mef_tag::attachments, attachment_payload, 4U, true);
    write_u32(file_bytes, 4U, static_cast<std::uint32_t>(file_bytes.size()));
    return file_bytes;
}

std::vector<std::byte> make_render_model_bytes() {
    std::vector<std::byte> file_bytes(20U);
    write_u32(file_bytes, 0U, igi::retail::model::mef_tag::ilff);
    write_u32(file_bytes, 8U, 4U);
    write_u32(file_bytes, 16U, MefModel::form_ocem);

    std::vector<std::byte> mesh_header(MefModel::mesh_header_size);
    write_u32(mesh_header, 0U, MefModel::supported_version_bits);
    append_chunk(file_bytes, igi::retail::model::mef_tag::mesh, mesh_header, 4U, false);

    const std::array<std::byte, 8> render_header_payload{};
    append_chunk(file_bytes, igi::retail::model::mef_tag::d3d_render_header, render_header_payload, 4U, false);

    const std::array<std::byte, 12> draw_record_payload{};
    append_chunk(file_bytes, igi::retail::model::mef_tag::draw_records, draw_record_payload, 4U, false);

    const std::array<std::byte, 40> vertex_payload{};
    append_chunk(file_bytes, igi::retail::model::mef_tag::vertices, vertex_payload, 4U, true);
    write_u32(file_bytes, 4U, static_cast<std::uint32_t>(file_bytes.size()));
    return file_bytes;
}

void testRetailIlffChunkAlignmentAndTagSearch() {
    const std::vector<std::byte> file_bytes = make_model_bytes();
    const auto container = Ilff_ReadContainer(file_bytes);
    const auto* mesh_chunk = container.find_first(igi::retail::model::mef_tag::mesh);
    const auto* attachment_chunk = container.find_first(igi::retail::model::mef_tag::attachments);

    if (container.form_type != MefModel::form_ocem || container.chunks.size() != 2U
        || mesh_chunk == nullptr || mesh_chunk->payload.size() != MefModel::mesh_header_size
        || attachment_chunk == nullptr || attachment_chunk->payload.size() != 4U) {
        throw std::runtime_error("Retail ILFF tag lookup or parent-relative chunk alignment failed.");
    }

    if (igi::retail::model::Ilff_ReadLittleEndian32(mesh_chunk->payload, 0U) != MefModel::supported_version_bits) {
        throw std::runtime_error("MESH version field did not retain its retail little-endian value.");
    }
}

void testRetailMefLoaderBindsMeshAndKnownChunks() {
    const std::vector<std::byte> file_bytes = make_model_bytes();
    const MefModel model = MefModel::DynObj_LocalModelsSMef(file_bytes);

    if (model.version != 0.1F || model.mesh_header.size() != MefModel::mesh_header_size
        || !model.attachments.has_value() || model.attachments->payload.size() != 4U
        || model.legacy_vertices.has_value() || model.portals.has_value()
        || model.portal_faces.has_value() || model.portal_vertices.has_value()) {
        throw std::runtime_error("DynObj_LocalModelsSMef did not bind the retail model resource fields.");
    }
}

void testRetailMefLoaderAcceptsOnlyRetailRenderFormats() {
    const std::array<std::pair<std::uint32_t, igi::retail::model::MefRenderFormat>, 3> supported_formats{{
        {0U, igi::retail::model::MefRenderFormat::diffuse},
        {1U, igi::retail::model::MefRenderFormat::skinned},
        {3U, igi::retail::model::MefRenderFormat::lightmapped},
    }};
    for (const auto& [raw_format, expected_format] : supported_formats) {
        const std::vector<std::byte> file_bytes = make_model_bytes(raw_format);
        const MefModel model = MefModel::DynObj_LocalModelsSMef(file_bytes);
        if (model.render_format != expected_format) {
            throw std::runtime_error("FUN_0049FB10 render format was not retained from MESH+0x20.");
        }
    }

    bool rejected_unsupported_format = false;
    try {
        const std::vector<std::byte> file_bytes = make_model_bytes(2U);
        static_cast<void>(MefModel::DynObj_LocalModelsSMef(file_bytes));
    } catch (const MefFormatError& exception) {
        rejected_unsupported_format = std::string(exception.what()) == "Mesh format not supported";
    }
    if (!rejected_unsupported_format) {
        throw std::runtime_error("MefModel did not preserve the retail unsupported-format failure.");
    }
}

void testRetailMefLoaderExposesRetailRenderChunks() {
    const std::vector<std::byte> file_bytes = make_render_model_bytes();
    const MefModel model = MefModel::DynObj_LocalModelsSMef(file_bytes);
    const igi::retail::model::MefRenderChunks render_chunks = model.render_chunks();

    if (render_chunks.d3d_render_header == nullptr
        || render_chunks.draw_records == nullptr
        || render_chunks.vertex_stream == nullptr
        || render_chunks.d3d_render_header->payload.size() != 8U
        || render_chunks.draw_records->payload.size() != 12U
        || render_chunks.vertex_stream->payload.size() != 40U) {
        throw std::runtime_error("Retail MEF render chunks were not exposed from their original ILFF records.");
    }
}

void testRetailMefLoaderRejectsUnsupportedVersion() {
    std::vector<std::byte> file_bytes = make_model_bytes();
    constexpr std::size_t mesh_payload_offset = 36U;
    write_u32(file_bytes, mesh_payload_offset, std::bit_cast<std::uint32_t>(0.2F));

    bool rejected_version = false;
    try {
        static_cast<void>(MefModel::DynObj_LocalModelsSMef(file_bytes));
    } catch (const MefFormatError&) {
        rejected_version = true;
    }

    if (!rejected_version) {
        throw std::runtime_error("MefModel loader accepted a version that retail rejects.");
    }
}

void testRetailMefLoaderRejectsTruncation() {
    std::vector<std::byte> file_bytes = make_model_bytes();
    file_bytes.resize(32U);

    bool rejected_file = false;
    try {
        static_cast<void>(MefModel::DynObj_LocalModelsSMef(file_bytes));
    } catch (const MefFormatError&) {
        rejected_file = true;
    }

    if (!rejected_file) {
        throw std::runtime_error("MefModel loader accepted a truncated ILFF resource.");
    }
}

void testRetailSkinnedVertexUsesNativeRecordOffsets() {
    std::vector<std::byte> record_bytes(40U);
    const std::array<float, 9> float_fields{
        1.0F, 2.0F, 3.0F,
        0.0F, 0.0F, 1.0F,
        0.25F, 0.75F,
        0.5F,
    };
    for (std::size_t field_index = 0U; field_index < float_fields.size(); ++field_index) {
        write_u32(
            record_bytes,
            field_index * sizeof(float),
            std::bit_cast<std::uint32_t>(float_fields[field_index]));
    }
    record_bytes[36U] = std::byte{0x34};
    record_bytes[37U] = std::byte{0x12};
    record_bytes[38U] = std::byte{0x78};
    record_bytes[39U] = std::byte{0x56};

    const igi::retail::model::MefSkinnedVertex vertex =
        MefSkinnedVertex_ReadRetailRecord(record_bytes);
    if (vertex.position_x != 1.0F || vertex.position_y != 2.0F || vertex.position_z != 3.0F
        || vertex.normal_z != 1.0F || vertex.diffuse_u != 0.25F || vertex.diffuse_v != 0.75F
        || vertex.influence_weight != 0.5F || vertex.target_vertex_index != 0x1234U
        || vertex.bone_index != 0x5678U) {
        throw std::runtime_error("FUN_0049b700 skinned vertex fields were read at incorrect record offsets.");
    }

    bool rejected_wrong_record_size = false;
    try {
        static_cast<void>(MefSkinnedVertex_ReadRetailRecord(
            std::span<const std::byte>(record_bytes).first(39U)));
    } catch (const MefFormatError&) {
        rejected_wrong_record_size = true;
    }
    if (!rejected_wrong_record_size) {
        throw std::runtime_error("Retail skinned vertex reader accepted a record with the wrong stride.");
    }
}

MefModel loadMefAsset(const std::filesystem::path& model_path) {
    std::ifstream model_stream(model_path, std::ios::binary);
    if (!model_stream) {
        throw std::runtime_error("Could not open MEF asset: " + model_path.string());
    }

    std::vector<char> file_characters(
        (std::istreambuf_iterator<char>(model_stream)),
        std::istreambuf_iterator<char>());
    std::vector<std::byte> file_bytes(file_characters.size());
    for (std::size_t byte_index = 0U; byte_index < file_characters.size(); ++byte_index) {
        file_bytes[byte_index] = static_cast<std::byte>(static_cast<unsigned char>(file_characters[byte_index]));
    }

    const MefModel model = MefModel::DynObj_LocalModelsSMef(file_bytes);
    if (model.mesh_header.size() != MefModel::mesh_header_size) {
        throw std::runtime_error("MESH header length differs from the retail loader's accessed layout.");
    }
    const igi::retail::model::MefRenderChunks render_chunks = model.render_chunks();
    if (render_chunks.d3d_render_header == nullptr
        || render_chunks.draw_records == nullptr
        || render_chunks.vertex_stream == nullptr) {
        throw std::runtime_error("MEF asset is missing an observed D3DR, DNER, or XTRV chunk.");
    }
    return model;
}

void testMefAssetsAtPath(const std::filesystem::path& requested_path) {
    std::error_code filesystem_error;
    if (!std::filesystem::is_directory(requested_path, filesystem_error)) {
        const MefModel model = loadMefAsset(requested_path);
        const igi::retail::model::MefRenderChunks render_chunks = model.render_chunks();
        std::cout << "Loaded retail MEF resource: " << requested_path
                  << " (MESH version " << model.version
                  << "; D3DR " << render_chunks.d3d_render_header->payload.size()
                  << " bytes; DNER " << render_chunks.draw_records->payload.size()
                  << " bytes; XTRV " << render_chunks.vertex_stream->payload.size()
                  << " bytes)\n";
        return;
    }

    std::size_t tested_asset_count = 0U;
    std::size_t failed_asset_count = 0U;
    std::size_t render_vertex_bytes = 0U;
    for (std::filesystem::recursive_directory_iterator iterator(requested_path, filesystem_error), end;
         !filesystem_error && iterator != end;
         iterator.increment(filesystem_error)) {
        if (!iterator->is_regular_file(filesystem_error) || filesystem_error
            || iterator->path().extension().string() != ".mef") {
            continue;
        }

        try {
            const MefModel model = loadMefAsset(iterator->path());
            render_vertex_bytes += model.render_chunks().vertex_stream->payload.size();
            ++tested_asset_count;
        } catch (const std::exception& exception) {
            ++failed_asset_count;
            std::cerr << "MEF asset failure: " << iterator->path() << ": "
                      << exception.what() << '\n';
        }
    }

    if (filesystem_error) {
        throw std::runtime_error(
            "Could not enumerate MEF assets beneath " + requested_path.string()
            + ": " + filesystem_error.message());
    }

    std::cout << "Level MEF assets tested: " << tested_asset_count << '\n'
              << "Models with D3DR, DNER, and XTRV chunks: " << tested_asset_count << '\n'
              << "XTRV vertex-stream bytes inspected: " << render_vertex_bytes << '\n'
              << "Failed MEF assets: " << failed_asset_count << '\n';
    if (tested_asset_count == 0U || failed_asset_count != 0U) {
        throw std::runtime_error("Level MEF asset verification did not pass.");
    }
}

} // namespace

int main(int argument_count, char** arguments) {
    try {
        testRetailIlffChunkAlignmentAndTagSearch();
        testRetailMefLoaderBindsMeshAndKnownChunks();
        testRetailMefLoaderAcceptsOnlyRetailRenderFormats();
        testRetailMefLoaderExposesRetailRenderChunks();
        testRetailMefLoaderRejectsUnsupportedVersion();
        testRetailMefLoaderRejectsTruncation();
        testRetailSkinnedVertexUsesNativeRecordOffsets();

        if (argument_count > 1) {
            testMefAssetsAtPath(arguments[1]);
        }

        std::cout << "MEF loader checks passed.\n";
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "MEF loader test failure: " << exception.what() << '\n';
        return 1;
    }
}
