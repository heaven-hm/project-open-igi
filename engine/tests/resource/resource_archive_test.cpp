#include "igi/retail/model/mef_model_loader.hpp"
#include "igi/retail/resource/resource_archive.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

using igi::retail::resource::ResourceArchive;
using igi::retail::resource::ResourceFormatError;
using igi::retail::resource::Resource_Load;
using igi::retail::resource::Sys_Resource;

void write_u32(std::vector<std::byte>& bytes, std::size_t offset, std::uint32_t value)
{
    if (bytes.size() < offset + sizeof(value)) {
        bytes.resize(offset + sizeof(value));
    }
    for (std::size_t byte_index = 0U; byte_index < sizeof(value); ++byte_index) {
        bytes[offset + byte_index] = static_cast<std::byte>((value >> (byte_index * 8U)) & 0xFFU);
    }
}

void append_chunk(
    std::vector<std::byte>& bytes,
    std::uint32_t tag,
    std::span<const std::byte> payload,
    bool final_chunk)
{
    const std::size_t chunk_offset = bytes.size();
    bytes.resize(chunk_offset + 16U);
    write_u32(bytes, chunk_offset, tag);
    write_u32(bytes, chunk_offset + 4U, static_cast<std::uint32_t>(payload.size()));
    write_u32(bytes, chunk_offset + 8U, 4U);
    const std::size_t payload_offset = (chunk_offset + 19U) & ~std::size_t{3U};
    bytes.resize(payload_offset);
    bytes.insert(bytes.end(), payload.begin(), payload.end());
    const std::size_t sibling_distance = bytes.size() - chunk_offset;
    write_u32(bytes, chunk_offset + 12U, final_chunk ? 0U : static_cast<std::uint32_t>(sibling_distance));
}

std::vector<std::byte> make_ires_archive()
{
    std::vector<std::byte> bytes(20U);
    write_u32(bytes, 0U, ResourceArchive::ilff_signature);
    write_u32(bytes, 8U, 4U);
    write_u32(bytes, 16U, ResourceArchive::ires_form);

    constexpr std::array<std::byte, 26U> resource_name{
        std::byte{'L'}, std::byte{'O'}, std::byte{'C'}, std::byte{'A'}, std::byte{'L'}, std::byte{':'},
        std::byte{'m'}, std::byte{'o'}, std::byte{'d'}, std::byte{'e'}, std::byte{'l'}, std::byte{'s'},
        std::byte{'/'}, std::byte{'t'}, std::byte{'e'}, std::byte{'s'}, std::byte{'t'}, std::byte{'.'},
        std::byte{'m'}, std::byte{'e'}, std::byte{'f'}, std::byte{0}, std::byte{0}, std::byte{0},
        std::byte{0}, std::byte{0},
    };
    constexpr std::array<std::byte, 4U> resource_body{
        std::byte{0x49}, std::byte{0x4C}, std::byte{0x46}, std::byte{0x46},
    };

    append_chunk(bytes, ResourceArchive::name_chunk, resource_name, false);
    append_chunk(bytes, ResourceArchive::body_chunk, resource_body, true);
    write_u32(bytes, 4U, static_cast<std::uint32_t>(bytes.size()));
    return bytes;
}

std::vector<std::byte> make_legacy_resource_archive()
{
    std::vector<std::byte> bytes(128U);
    constexpr std::string_view signature = "RESOURCE";
    for (std::size_t byte_index = 0U; byte_index < signature.size(); ++byte_index) {
        bytes[byte_index] = static_cast<std::byte>(signature[byte_index]);
    }
    write_u32(bytes, 8U, 1U);
    write_u32(bytes, 36U, 3U);
    write_u32(bytes, 40U, 4U);
    bytes[44U] = std::byte{'t'};
    bytes[45U] = std::byte{'e'};
    bytes[46U] = std::byte{'s'};
    bytes[47U] = std::byte{'t'};
    bytes[64U] = std::byte{0x11};
    bytes[65U] = std::byte{0x22};
    bytes[66U] = std::byte{0x33};
    return bytes;
}

void require(bool condition, std::string_view message)
{
    if (!condition) {
        throw std::runtime_error(std::string(message));
    }
}

void test_sys_resource_reads_ires_name_body_pairs()
{
    const std::vector<std::byte> input_bytes = make_ires_archive();
    const ResourceArchive archive = Sys_Resource(input_bytes, "fixture.res");
    const auto* entry = archive.find("LOCAL:models/test.mef");
    require(archive.form_type == ResourceArchive::ires_form, "IRES form type is retained");
    require(archive.entries.size() == 1U, "one NAME/BODY pair is exposed");
    require(entry != nullptr && entry->body.size() == 4U, "named body is available");
    require(entry->body[0U] == std::byte{0x49}, "body bytes are preserved");
}

void test_sys_resource_reads_legacy_resource_records()
{
    const std::vector<std::byte> input_bytes = make_legacy_resource_archive();
    const ResourceArchive archive = Sys_Resource(input_bytes, "legacy.res");
    const auto* entry = archive.find("test");
    require(archive.entries.size() == 1U, "legacy resource count is read");
    require(entry != nullptr && entry->body.size() == 3U, "legacy resource payload is extracted");
    require(entry->body[2U] == std::byte{0x33}, "legacy payload bytes are preserved");
}

void test_sys_resource_rejects_unpaired_name_chunks()
{
    std::vector<std::byte> input_bytes = make_ires_archive();
    input_bytes.resize(20U);
    constexpr std::array<std::byte, 2U> name_payload{std::byte{'x'}, std::byte{0}};
    append_chunk(input_bytes, ResourceArchive::name_chunk, name_payload, true);
    write_u32(input_bytes, 4U, static_cast<std::uint32_t>(input_bytes.size()));

    bool rejected = false;
    try {
        static_cast<void>(Sys_Resource(input_bytes, "bad.res"));
    } catch (const ResourceFormatError&) {
        rejected = true;
    }
    require(rejected, "an IRES archive with a NAME lacking BODY is rejected");
}

void test_level_one_resource_archive(const std::filesystem::path& resource_path)
{
    const ResourceArchive archive = Resource_Load(resource_path);
    require(archive.form_type == ResourceArchive::ires_form, "Level 1 archive has IRES form");
    require(archive.entries.size() == 415U, "Level 1 model archive exposes all 415 resource pairs");

    std::size_t model_resource_count = 0U;
    std::size_t model_with_render_chunks_count = 0U;
    std::size_t diffuse_model_count = 0U;
    std::size_t skinned_model_count = 0U;
    std::size_t lightmapped_model_count = 0U;
    std::size_t empty_format_three_model_count = 0U;
    for (const igi::retail::resource::ResourceEntry& entry : archive.entries) {
        if (!entry.name.ends_with(".mef")) {
            continue;
        }

        const igi::retail::model::MefModel model =
            igi::retail::model::MefModel::DynObj_LocalModelsSMef(entry.body);
        const igi::retail::model::MefRenderChunks render_chunks = model.render_chunks();
        ++model_resource_count;
        switch (model.render_format) {
        case igi::retail::model::MefRenderFormat::diffuse:
            ++diffuse_model_count;
            break;
        case igi::retail::model::MefRenderFormat::skinned:
            ++skinned_model_count;
            break;
        case igi::retail::model::MefRenderFormat::lightmapped:
            ++lightmapped_model_count;
            break;
        }
        if (render_chunks.d3d_render_header != nullptr
            && render_chunks.draw_records != nullptr
            && render_chunks.vertex_stream != nullptr) {
            ++model_with_render_chunks_count;

            // FUN_0049B4B0 copies format-0 records with stride 0x20 and format-3
            // records with stride 0x28. FUN_0049B700 reads format-1 source records
            // with stride 0x28. Static evidence IDs:
            // ev_9b668df201c621c75f6217a02978f1ca7db796fba2784227a7ddbe490e0ef2e5
            // and ev_e831fe78437d49cf51225556ea082e70d37f8bfbbcfc05e4ba18ba1e0a5ff8bc.
            const std::size_t expected_vertex_stride =
                model.render_format == igi::retail::model::MefRenderFormat::diffuse ? 32U : 40U;
            if (model.render_header->vertex_count == 0U) {
                require(render_chunks.vertex_stream->payload.empty()
                        && model.render_format == igi::retail::model::MefRenderFormat::lightmapped,
                    "only zero-count format-3 Level 1 models may have empty vertex streams");
                ++empty_format_three_model_count;
            } else {
                require(render_chunks.vertex_stream->payload.size()
                            % model.render_header->vertex_count == 0U,
                    "each Level 1 MEF vertex stream divides evenly by its retail vertex count");
                require(render_chunks.vertex_stream->payload.size()
                            / model.render_header->vertex_count == expected_vertex_stride,
                    "each Level 1 MEF uses the vertex stride selected by the retail renderer path");
            }
        } else {
            std::cerr << "Level 1 MEF lacks a complete observed render chunk set: "
                      << entry.name << '\n';
        }
    }
    require(model_resource_count > 0U, "Level 1 model RES contains embedded MEF resources");
    require(diffuse_model_count + skinned_model_count + lightmapped_model_count == model_resource_count,
        "every Level 1 MEF uses a renderer format accepted by FUN_0049FB10");
    require(model_with_render_chunks_count == model_resource_count,
        "every Level 1 MEF has D3DR, REND, and VRTX chunks for the retail render path");
    require(empty_format_three_model_count == 4U,
        "Level 1 retains the four observed empty format-3 model resources");

    const auto* model_entry = archive.find("LOCAL:models/210_01_1.mef");
    require(model_entry != nullptr, "Level 1 archive resolves the first model resource");

    const auto model = igi::retail::model::MefModel::DynObj_LocalModelsSMef(model_entry->body);
    require(model.version == 0.1F, "embedded MEF version is accepted by the retail MEF routine");
    std::cout << "Level 1 Resource_Load/Sys_Resource test passed: "
              << archive.entries.size() << " entries; "
              << model_resource_count << " MEFs with D3DR/REND/VRTX chunks; "
              << "formats 0/1/3=" << diffuse_model_count << "/" << skinned_model_count
              << "/" << lightmapped_model_count
              << "; XTRV strides 0/1/3=32/40/40; empty format-3="
              << empty_format_three_model_count << "; "
              << model_entry->name << "; "
              << model_entry->body.size() << " bytes; "
              << model.container.chunks.size() << " MEF chunks.\n";
}

} // namespace

int main(int argument_count, char** arguments)
{
    try {
        test_sys_resource_reads_ires_name_body_pairs();
        test_sys_resource_reads_legacy_resource_records();
        test_sys_resource_rejects_unpaired_name_chunks();

        if (argument_count == 2) {
            test_level_one_resource_archive(arguments[1]);
        } else if (argument_count != 1) {
            throw std::runtime_error("Expected zero arguments or one Level 1 RES file path.");
        }

        std::cout << "All retail resource archive tests passed.\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Retail resource archive test failed: " << error.what() << '\n';
        return 1;
    }
}
