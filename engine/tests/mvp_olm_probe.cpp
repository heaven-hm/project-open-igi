#include "igi/asset_io.hpp"
#include "igi/mission_assets.hpp"
#include "igi/qvm_initializer.hpp"

#include "mef_native.h"
#include "olm_parser.h"
#include "res_parser.h"

#include <algorithm>
#include <bit>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{

void require(bool condition, const char* message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}

void write_unsigned_integer(std::vector<std::uint8_t>& bytes, std::size_t offset, std::uint32_t value)
{
    for (std::size_t byte_index = 0; byte_index < sizeof(value); ++byte_index)
    {
        bytes[offset + byte_index] = static_cast<std::uint8_t>(value >> (byte_index * 8));
    }
}

void write_unsigned_short(std::vector<std::uint8_t>& bytes, std::size_t offset, std::uint16_t value)
{
    bytes[offset] = static_cast<std::uint8_t>(value);
    bytes[offset + 1] = static_cast<std::uint8_t>(value >> 8);
}

std::vector<std::uint8_t> make_lightmap_fixture()
{
    constexpr std::size_t prefix_size = 60;
    constexpr std::size_t descriptor_size = 44;
    std::vector<std::uint8_t> bytes(prefix_size + descriptor_size * 2 + 12);
    write_unsigned_integer(bytes, 0, std::bit_cast<std::uint32_t>(0.12F));
    write_unsigned_integer(bytes, 44, 2);
    write_unsigned_short(bytes, prefix_size + 40, 2);
    write_unsigned_short(bytes, prefix_size + 42, 1);
    write_unsigned_short(bytes, prefix_size + descriptor_size + 40, 1);
    write_unsigned_short(bytes, prefix_size + descriptor_size + 42, 1);
    for (std::size_t byte_index = prefix_size + descriptor_size * 2; byte_index < bytes.size(); ++byte_index)
    {
        bytes[byte_index] = static_cast<std::uint8_t>(byte_index & 0xffU);
    }
    return bytes;
}

void verify_synthetic_lightmap()
{
    const auto fixture = make_lightmap_fixture();
    const auto lightmap = igi::assets::parse_object_lightmap(fixture);
    require(std::abs(lightmap.version - 0.12F) < 0.0001F, "OLM version must be preserved");
    require(lightmap.images.size() == 2, "OLM must parse every image descriptor");
    require(lightmap.images[0].width == 2 && lightmap.images[0].height == 1 &&
                lightmap.images[0].rgba_pixels.size() == 8,
            "First OLM image dimensions and RGBA bytes must match the fixture");
    require(lightmap.images[1].width == 1 && lightmap.images[1].height == 1 &&
                lightmap.images[1].rgba_pixels.size() == 4,
            "Second OLM image dimensions and RGBA bytes must match the fixture");
    require(lightmap.images[0].rgba_pixels.front() == fixture[148] &&
                lightmap.images[1].rgba_pixels.back() == fixture.back(),
            "OLM image payloads must remain in descriptor order");

    for (std::vector<std::uint8_t> malformed : {std::vector<std::uint8_t>(59),
                                                std::vector<std::uint8_t>(104),
                                                std::vector<std::uint8_t>(60 + 2 * 44 + 11)})
    {
        bool rejected = false;
        try
        {
            static_cast<void>(igi::assets::parse_object_lightmap(malformed));
        }
        catch (const std::exception&)
        {
            rejected = true;
        }
        require(rejected, "Truncated OLM input must be rejected");
    }

    auto trailing_bytes = fixture;
    trailing_bytes.push_back(0);
    bool rejected_trailing_data = false;
    try
    {
        static_cast<void>(igi::assets::parse_object_lightmap(trailing_bytes));
    }
    catch (const std::exception&)
    {
        rejected_trailing_data = true;
    }
    require(rejected_trailing_data, "Trailing OLM bytes must be rejected");

    std::string_view set_name;
    std::uint32_t image_index = 99;
    require(igi::assets::parse_object_lightmap_entry_name(
                "LOCAL:missions/location0/level1/lightmaps/obj00014_00000.olm", set_name, image_index),
            "RES OLM path must parse");
    require(set_name == "obj00014" && image_index == 0, "RES OLM set and image index must be preserved");
    require(!igi::assets::parse_object_lightmap_entry_name("obj00014_invalid.olm", set_name, image_index),
            "Malformed RES OLM name must be rejected");
}

void verify_level_one_lightmap(const std::filesystem::path& game_directory)
{
    const auto archive_path = game_directory / "missions/location0/level1/lightmaps/lightmaps.res";
    std::size_t parsed_lightmap_count = 0;
    std::string archive_error;
    const bool archive_valid = RES_ForEachEntry(
        archive_path.string(),
        [&](const std::string& entry_name, const std::uint8_t* entry_bytes, std::size_t entry_size)
        {
            std::string_view set_name;
            std::uint32_t image_index = 0;
            if (!igi::assets::parse_object_lightmap_entry_name(entry_name, set_name, image_index))
            {
                return;
            }
            const auto entry_span = std::span<const std::uint8_t>(entry_bytes, entry_size);
            const auto lightmap = igi::assets::parse_object_lightmap(entry_span);
            require(!set_name.empty(), "Retail OLM resource set name must not be empty");
            require(std::abs(lightmap.version - 0.12F) < 0.0001F,
                    "Retail Level 1 OLM versions must match converter inspection");
            require(!lightmap.images.empty(), "Retail Level 1 OLM resource must contain image data");
            ++parsed_lightmap_count;
        },
        archive_error);
    require(archive_valid,
            archive_error.empty() ? "Level 1 lightmaps.res must be readable" : archive_error.c_str());
    require(parsed_lightmap_count > 0, "Level 1 lightmaps.res must contain OLM resources");

    const auto sample_bytes =
        RES_Extract(archive_path.string(), "LOCAL:missions/location0/level1/lightmaps/obj00014_00000.olm");
    require(!sample_bytes.empty(), "Level 1 lightmaps.res must contain obj00014_00000.olm");
    const auto sample_lightmap = igi::assets::parse_object_lightmap(sample_bytes);
    require(sample_lightmap.images.size() == 1, "Retail Level 1 sample OLM must contain one image");
    require(sample_lightmap.images.front().width == 256 && sample_lightmap.images.front().height == 64,
            "Retail Level 1 sample OLM dimensions must match converter inspection");
    require(sample_lightmap.images.front().rgba_pixels.size() == 256U * 64U * 4U,
            "Retail Level 1 sample OLM payload size must match its RGBA dimensions");
    std::cout << "[PASS] Retail Level 1 OLM: " << archive_path << ", parsed " << parsed_lightmap_count
              << " resources; sample image " << sample_lightmap.images.front().width << 'x'
              << sample_lightmap.images.front().height << " RGBA\n";

    const auto model_archive = game_directory / "missions/location0/level1/MODELS/LEVEL1.RES";
    const auto model_bytes = RES_Extract(model_archive.string(), "LOCAL:models/435_01_1.mef");
    require(!model_bytes.empty(), "Level 1 model archive must contain lightmapped model 435_01_1.mef");
    const auto model_directory = std::filesystem::temp_directory_path() / "project-open-igi-olm-probe";
    std::filesystem::create_directories(model_directory);
    const auto model_path = model_directory / "435_01_1.mef";
    {
        std::ofstream model_output(model_path, std::ios::binary);
        require(model_output
                    .write(reinterpret_cast<const char*>(model_bytes.data()),
                           static_cast<std::streamsize>(model_bytes.size()))
                    .good(),
                "Could not write extracted Level 1 MEF fixture");
    }
    const auto geometry = ParseMefFile(model_path.string());
    require(geometry.modelType == 3, "Retail 435_01_1.mef must use the lightmapped MEF model type");
    require(geometry.vertices.size() == 701 && geometry.triangles.size() == 360 &&
                geometry.renderBlocks.size() == 33,
            "Retail 435_01_1.mef geometry must match converter inspection");
    require(std::all_of(geometry.vertices.begin(),
                        geometry.vertices.end(),
                        [](const RenderVertex& vertex)
                        { return std::isfinite(vertex.uv2.x) && std::isfinite(vertex.uv2.y); }),
            "Retail 435_01_1.mef lightmap UV coordinates must be finite");
    require(std::any_of(geometry.vertices.begin(),
                        geometry.vertices.end(),
                        [](const RenderVertex& vertex)
                        { return vertex.uv2.x != 0.0f || vertex.uv2.y != 0.0f; }),
            "Retail 435_01_1.mef must carry non-zero lightmap UV coordinates");
    require(std::all_of(geometry.renderBlocks.begin(),
                        geometry.renderBlocks.end(),
                        [](const ParsedGeometry::RenderBlock& block) { return block.lightmapPage == 0; }),
            "Retail 435_01_1.mef render blocks must select lightmap page zero");

    const auto mission_directory = game_directory / "missions/location0/level1";
    igi::assets::MissionAssets mission_assets(game_directory, mission_directory, model_directory / "cache");
    const auto scene_mesh = mission_assets.model("435_01_1");
    require(scene_mesh != nullptr && !scene_mesh->triangles.empty(),
            "Mission asset adapter must emit the retail model triangles");
    require(scene_mesh->attachment_count > 0 && scene_mesh->maximum_attachment_depth > 0,
            "Retail Level 1 model must resolve MEF attachments through magicobj.qvm");
    const auto model_lightmap_layout = mission_assets.model_lightmap_layout("435_01_1");
    require(model_lightmap_layout.size() > 1 && model_lightmap_layout.front().model_name == "435_01_1" &&
                model_lightmap_layout.front().lightmap_index == 0 &&
                model_lightmap_layout.front().lightmapped,
            "Retail model lightmap layout must start with its lightmapped LOD at index zero");
    std::size_t expected_lod_count = 0;
    for (std::size_t lod_index = 0; lod_index < 5; ++lod_index)
    {
        const std::string expected_lod_name = "435_01_" + std::to_string(lod_index + 1);
        if (!mission_assets.has_model(expected_lod_name))
        {
            break;
        }
        require(lod_index < model_lightmap_layout.size() &&
                    model_lightmap_layout[lod_index].model_name == expected_lod_name &&
                    model_lightmap_layout[lod_index].lightmap_index == static_cast<std::int64_t>(lod_index),
                "Retail model LODs must use incremental names and consecutive lightmap indices");
        ++expected_lod_count;
    }
    require(expected_lod_count > 0, "Retail model must resolve at least its authored first LOD");
    const auto magic_script_bytes = igi::assets::read_file(game_directory / "magicobj/magicobj.qvm");
    const igi::assets::QvmInitializer magic_script{igi::assets::BinaryView(magic_script_bytes)};
    const MefAttachment* attached_geometry = nullptr;
    std::string attached_model_name;
    for (const auto& attachment : geometry.mefAttachments)
    {
        const auto attachment_name_end =
            std::find(std::begin(attachment.name), std::end(attachment.name), '\0');
        const std::string attachment_name(std::begin(attachment.name), attachment_name_end);
        for (const auto& call : magic_script.calls())
        {
            if (call.function == "DefineMagicObj" && call.arguments.size() >= 3 &&
                call.arguments[0].text == attachment_name && call.arguments[2].text != "TASKTYPE_DEATHZONE" &&
                mission_assets.has_model(call.arguments[1].text))
            {
                attached_geometry = &attachment;
                attached_model_name = call.arguments[1].text;
                break;
            }
        }
        if (attached_geometry != nullptr)
        {
            break;
        }
    }
    require(attached_geometry != nullptr && !attached_model_name.empty(),
            "Retail MEF attachment must resolve to a drawable magic-object model");
    require(std::any_of(model_lightmap_layout.begin(),
                        model_lightmap_layout.end(),
                        [&](const igi::assets::ModelLightmapEntry& entry)
                        { return entry.model_name == igi::assets::normalize_name(attached_model_name); }),
            "Retail lightmap layout must include the attachment's magic-object model");
    const auto repeated_root_layout =
        mission_assets.model_lightmap_layout(std::vector<std::string>{"435_01_1", "435_01_1"});
    require(repeated_root_layout.size() == model_lightmap_layout.size() * 2,
            "BuildAll-style model layout must preserve both root subtrees");
    std::int32_t next_owner_lightmap_index = 0;
    for (const auto& entry : model_lightmap_layout)
    {
        if (entry.lightmap_index >= next_owner_lightmap_index)
        {
            next_owner_lightmap_index = static_cast<std::int32_t>(entry.lightmap_index + 1);
        }
    }
    require(repeated_root_layout[model_lightmap_layout.size()].lightmap_index == next_owner_lightmap_index,
            "BuildAll-style model layout must continue lightmap indices across root placements");
    const auto attached_mesh = mission_assets.model(attached_model_name);
    require(scene_mesh->triangles.size() == geometry.triangles.size() + attached_mesh->triangles.size(),
            "Flattened Level 1 MEF must include the mapped child model triangles exactly once");
    const auto& child_position = attached_mesh->triangles.front().vertices.front().position;
    const auto& flattened_position =
        scene_mesh->triangles[geometry.triangles.size()].vertices.front().position;
    const igi::gameplay::Vector3 expected_attached_position{
        attached_geometry->r00 * child_position.x + attached_geometry->r03 * child_position.y +
            attached_geometry->r06 * child_position.z + attached_geometry->px / 4096.0f,
        attached_geometry->r01 * child_position.x + attached_geometry->r04 * child_position.y +
            attached_geometry->r07 * child_position.z + attached_geometry->py / 4096.0f,
        attached_geometry->r02 * child_position.x + attached_geometry->r05 * child_position.y +
            attached_geometry->r08 * child_position.z + attached_geometry->pz / 4096.0f};
    require(std::abs(flattened_position.x - expected_attached_position.x) < 0.00001f &&
                std::abs(flattened_position.y - expected_attached_position.y) < 0.00001f &&
                std::abs(flattened_position.z - expected_attached_position.z) < 0.00001f,
            "Flattened MEF child position and orientation must match the retail ATTA transform");
    require(std::all_of(scene_mesh->triangles.begin(),
                        scene_mesh->triangles.end(),
                        [](const igi::gameplay::SceneTriangle& triangle)
                        {
                            return triangle.lightmap_page_index == 0 &&
                                   std::all_of(triangle.vertices.begin(),
                                               triangle.vertices.end(),
                                               [](const igi::gameplay::SceneVertex& vertex) {
                                                   return std::isfinite(vertex.lightmap_u) &&
                                                          std::isfinite(vertex.lightmap_v);
                                               });
                        }),
            "Mission asset adapter must preserve MEF lightmap pages and UV coordinates");
    const auto lightmap_texture_indices = mission_assets.lightmap_textures("obj00014", 0);
    require(lightmap_texture_indices.size() == sample_lightmap.images.size(),
            "Mission asset adapter must load all pages from the indexed retail OLM resource");
    const auto cached_lightmap_texture_indices = mission_assets.lightmap_textures("obj00014", 0);
    require(cached_lightmap_texture_indices == lightmap_texture_indices,
            "Repeated object lightmap requests must reuse cached texture indices");
    auto loaded_textures = mission_assets.take_textures();
    require(lightmap_texture_indices.front() < loaded_textures.size(),
            "Loaded object lightmap texture index must refer to the mission texture collection");
    const auto& loaded_lightmap_texture = loaded_textures[lightmap_texture_indices.front()];
    require(loaded_lightmap_texture.width == sample_lightmap.images.front().width &&
                loaded_lightmap_texture.height == sample_lightmap.images.front().height,
            "Mission asset adapter must preserve retail OLM texture dimensions");
    const auto& sample_pixel = sample_lightmap.images.front().rgba_pixels;
    const std::uint32_t expected_pixel = (static_cast<std::uint32_t>(sample_pixel[3]) << 24) |
                                         (static_cast<std::uint32_t>(sample_pixel[0]) << 16) |
                                         (static_cast<std::uint32_t>(sample_pixel[1]) << 8) |
                                         static_cast<std::uint32_t>(sample_pixel[2]);
    require(!loaded_lightmap_texture.pixels.empty() && loaded_lightmap_texture.pixels.front() == expected_pixel,
            "Mission asset adapter must preserve the first retail OLM RGBA pixel");
    std::cout << "[PASS] Retail OLM pages load as mission textures with stable cache indices and exact pixel "
                 "bytes\n";

    std::cout << "[PASS] Retail lightmapped model 435_01_1.mef: " << geometry.vertices.size() << " vertices, "
              << geometry.triangles.size() << " triangles, " << geometry.renderBlocks.size()
              << " draw blocks; scene mesh resolves " << scene_mesh->attachment_count
              << " MEF attachment(s) to depth " << scene_mesh->maximum_attachment_depth
              << "; UV2/page metadata reaches scene mesh\n";
}

} // namespace

int main(int argument_count, char** arguments)
{
    try
    {
        verify_synthetic_lightmap();
        std::cout << "[PASS] Synthetic OLM parsing, malformed input rejection, and RES entry naming\n";
        if (argument_count == 2)
        {
            verify_level_one_lightmap(arguments[1]);
        }
        else if (argument_count != 1)
        {
            throw std::runtime_error("Usage: mvp_olm_probe [IGI game directory]");
        }
    }
    catch (const std::exception& exception)
    {
        std::cerr << "[FAIL] OLM probe: " << exception.what() << '\n';
        return 1;
    }
    return 0;
}
