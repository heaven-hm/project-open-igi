#include "igi/retail/terrain/terrain_loaders.hpp"
#include "igi/retail/resource/resource_cache.hpp"
#include "igi/retail/script/qvm_loader.hpp"

#include <array>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace
{

std::vector<std::byte> read_binary_file(const std::filesystem::path& path)
{
    std::ifstream stream(path, std::ios::binary);
    if (!stream)
    {
        return {};
    }
    const auto u8_bytes = std::vector<std::uint8_t>(std::istreambuf_iterator<char>(stream), {});
    std::vector<std::byte> result(u8_bytes.size());
    std::memcpy(result.data(), u8_bytes.data(), u8_bytes.size());
    return result;
}

void require(bool condition, const char* message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}

void test_create_material_native()
{
    using namespace igi::retail::terrain;
    std::vector<float> args(227, 0.0f);
    args[0] = 1.0f; // group_idx
    args[1] = 42.0f; // material id
    // LOD 0 entries
    args[2] = 1.0f; // active
    args[3] = 10.0f; // v1
    args[4] = 20.0f; // v2
    args[5] = 256.0f; // f_tex2
    args[6] = 2.0f; // tex2 group
    args[7] = 512.0f; // f_tex1
    args[8] = 3.0f; // tex1 group

    std::array<std::uint8_t, 0x18> dummy_ctx{};
    dummy_ctx.fill(0xFF);
    const auto result = Terrain_CreateMaterial(dummy_ctx.data(), args.data());
    require(result == dummy_ctx.data(), "Terrain_CreateMaterial returns context");
    for (const std::size_t offset : {0U, 8U, 12U, 16U})
    {
        std::uint32_t word = 0;
        std::memcpy(&word, dummy_ctx.data() + offset, sizeof(word));
        require(word == (offset == 16U ? 0x00567C74U : 0U),
                "Terrain_CreateMaterial returns retail zero value and descriptor");
    }
}

void test_create_tilemap_native()
{
    using namespace igi::retail::terrain;
    std::vector<float> args(65, 0.0f);
    args[0] = 2.0f; // group_idx
    for (std::size_t i = 1; i <= 64; ++i)
    {
        args[i] = static_cast<float>(i % 18);
    }

    std::array<std::uint8_t, 0x18> dummy_ctx{};
    dummy_ctx.fill(0xFF);
    const auto result = Terrain_CreateTileMap(dummy_ctx.data(), args.data());
    require(result == dummy_ctx.data(), "Terrain_CreateTileMap returns context");
    for (const std::size_t offset : {0U, 8U, 12U, 16U})
    {
        std::uint32_t word = 0;
        std::memcpy(&word, dummy_ctx.data() + offset, sizeof(word));
        require(word == (offset == 16U ? 0x00567C74U : 0U),
                "Terrain_CreateTileMap returns retail zero value and descriptor");
    }
}

void test_modifier_and_heightmap_stack_natives()
{
    using namespace igi::retail::terrain;
    Host_ResetTerrainGlobals();
    std::array<std::uint8_t, 512> dummy_ctx{};
    std::array<std::uint8_t, 0xDD4> source_table{};
    std::array<std::uint32_t, 3> source_row{1U, 0U, 0U};
    const auto* source_row_ptr = source_row.data();
    const auto* source_table_ptr = source_table.data();
    std::memcpy(source_table.data() + 0xDCC, &source_row_ptr, sizeof(source_row_ptr));
    std::memcpy(dummy_ctx.data() + 0x68, &source_table_ptr, sizeof(source_table_ptr));
    *reinterpret_cast<std::int32_t*>(dummy_ctx.data() + 0xBC) = 0; // counter >= 0
    *reinterpret_cast<std::uint32_t*>(dummy_ctx.data() + 0xE4) = 5; // scratch val

    const auto push_res = TextureModifier_Push(dummy_ctx.data());
    require(push_res != nullptr, "TextureModifier_Push returns active pointer/index");

    // Separate native fixtures keep adjacent retail x86 pointer slots from
    // overlapping when the regression test runs on a 64-bit host.
    std::array<std::uint8_t, 0xDD8> heightmap_table{};
    std::array<std::uint32_t, 3> heightmap_row{1U, 0U, 0U};
    const auto* heightmap_row_ptr = heightmap_row.data();
    const auto* heightmap_table_ptr = heightmap_table.data();
    std::memcpy(heightmap_table.data() + 0xDD0, &heightmap_row_ptr, sizeof(heightmap_row_ptr));
    std::memcpy(dummy_ctx.data() + 0x68, &heightmap_table_ptr, sizeof(heightmap_table_ptr));
    *reinterpret_cast<std::int32_t*>(dummy_ctx.data() + 0xBC) = 0;
    *reinterpret_cast<std::uint32_t*>(dummy_ctx.data() + 0xF8) = 7;
    const auto hmp_res = HeightMap_Enter(dummy_ctx.data());
    require(reinterpret_cast<std::uintptr_t>(hmp_res) == 8U, "HeightMap_Enter returns incremented index");
    require(*reinterpret_cast<std::uint32_t*>(dummy_ctx.data() + 0xE8) == 1U,
            "HeightMap_Enter sets the complete DWORD flag");
    heightmap_row[0] = 0U;
    *reinterpret_cast<std::uint32_t*>(dummy_ctx.data() + 0xE8) = 0xA5A5A5A5U;
    require(HeightMap_Enter(dummy_ctx.data()) == dummy_ctx.data(), "HeightMap_Enter rejects a disabled row");
    require(*reinterpret_cast<std::uint32_t*>(dummy_ctx.data() + 0xE8) == 0U,
            "HeightMap_Enter clears the complete DWORD flag");

    *reinterpret_cast<std::int32_t*>(dummy_ctx.data() + 0x108) = 1;
    *reinterpret_cast<std::uint32_t*>(dummy_ctx.data() + 0xD8) = 3;
    const auto lmp_res = TerrainLightMap_Init_2(dummy_ctx.data());
    require(lmp_res != nullptr, "TerrainLightMap_Init_2 returns lightmap index");
}

void test_texture_modifier_bitmap_coverage()
{
    using namespace igi::retail::terrain;
    std::array<std::uint8_t, 8> bitmap{};
    // Eight pixels wide, one byte per row.  Enable (x=2, y=3) only.
    bitmap[3] = 1U << 2U;
    TerrainTextureModifierTask modifier{};
    modifier.bitmap_item = bitmap.data();
    modifier.bitmap_size = 8U;
    modifier.bitmap_line_width_shift = 0U;
    modifier.local_pos_to_bitmap_pos = 1.0F;
    modifier.cube_min_x = 100.0;
    modifier.cube_min_y = 200.0;

    require(Host_TextureModifierCoversPoint(modifier, 102.25, 203.75),
            "Texture modifier applies where its packed bitmap bit is set");
    require(!Host_TextureModifierCoversPoint(modifier, 103.25, 203.75),
            "Texture modifier does not paint unset bitmap pixels");
    require(!Host_TextureModifierCoversPoint(modifier, 99.0, 203.75),
            "Texture modifier rejects points outside its bitmap extent");
}

void test_lod_does_not_use_near_distance_band()
{
    using namespace igi::retail::terrain;
    TerrainLevelData terrain;
    terrain.loaded = true;
    terrain.material_groups.resize(1);
    terrain.contour_nodes.resize(17);
    // A single positive-octant branch: the shifted distance is 100 at LOD 13,
    // 200 at LOD 14. The host threshold 163 must stop at 14 even within 250 m.
    for (unsigned int lod = 0; lod < 15; ++lod)
    {
        auto& node = terrain.contour_nodes[lod + 1];
        node.children_mask = 1U << 7U;
        node.children[7] = static_cast<std::int16_t>(lod + 2);
    }
    const TerrainCommandHeader header{1, 4, 3, 0};
    const std::array<std::uint32_t, 4> commands{
        0x00020100U,
        (24U << 26U) | (24U << 20U) | (24U << 14U),
        (25U << 26U) | (24U << 20U) | (24U << 14U),
        (24U << 26U) | (25U << 20U) | (24U << 14U)};
    terrain.cmd_file_bytes.resize(4 + sizeof(header) + sizeof(commands));
    std::memcpy(terrain.cmd_file_bytes.data() + 4, &header, sizeof(header));
    std::memcpy(terrain.cmd_file_bytes.data() + 4 + sizeof(header), commands.data(), sizeof(commands));
    terrain.contour_nodes[15].commands.file_offset = 4;
    // LOD 15 has no command payload: taking the former threshold-320 near band
    // would incorrectly descend and produce an empty mesh.
    constexpr double half_size = static_cast<double>((1 << 30) >> 14);
    constexpr double centre = static_cast<double>(1 << 30) - half_size;
    const std::array<double, 3> origin{centre + half_size + 200.0 * (1 << 11), centre, centre};
    const float position[3]{};
    const float target[3]{0.0F, 1.0F, 0.0F};
    const auto mesh = terrain.Host_BuildRenderMesh(origin, 1.0F / 4096.0F, position, target,
                                                  1.0F, 1.3333333F, 0.0F);
    require(mesh.vertices.size() == 3 && mesh.batches.size() == 1,
            "LOD 14 must be selected at shifted distance 200 without a near-distance override");
}

void test_level1_terrain_loading_if_present()
{
    using namespace igi::retail::terrain;
    const std::filesystem::path level1_dir = "/Users/haseeb-mir/Downloads/IGI/MISSIONS/location0/LEVEL1/TERRAIN";
    if (std::filesystem::exists(level1_dir))
    {
        TerrainLevelData level_data{};
        igi::retail::resource::ResourceCache cache;
        const auto script = igi::retail::script::Host_LoadQvmScript(cache, level1_dir.parent_path() / "objects.qsc");
        const auto calls = igi::retail::script::Qvm_ReadObjectCalls(script.program());
        const bool loaded = level_data.Host_LoadLevelTerrain(level1_dir, calls);
        require(loaded, "Level 1 terrain loads successfully from retail directory");
        require(level_data.contour_nodes.size() > 1000, "Level 1 CTR contains contour nodes");
        require(level_data.texture_layers.size() == 18, "Level 1 TERRAIN.TEX has 18 texture layers");
        require(level_data.texture_layers[0].width == 128 && level_data.texture_layers[0].height == 128,
                "Texture layer 0 dimension is 128x128");

        // Use the level's authored HumanPlayer position rather than a hand-entered
        // diagnostic camera transform.  This exercises the same local origin used
        // by the Windows scene loader without claiming camera parity.
        std::array<double, 3> origin{};
        bool player_spawn_found = false;
        for (const auto& call : calls)
        {
            if (call->function_name != "Task_New" || call->arguments.size() <= 5U)
            {
                continue;
            }
            const auto* type = std::get_if<std::string>(&call->arguments[1]);
            if (type == nullptr || *type != "HumanPlayer")
            {
                continue;
            }
            for (std::size_t axis = 0U; axis < origin.size(); ++axis)
            {
                if (const auto* value = std::get_if<float>(&call->arguments[3U + axis]))
                {
                    origin[axis] = *value;
                }
                else if (const auto* value = std::get_if<std::int32_t>(&call->arguments[3U + axis]))
                {
                    origin[axis] = *value;
                }
                else
                {
                    throw std::runtime_error("Level 1 HumanPlayer spawn has a non-numeric position");
                }
            }
            player_spawn_found = true;
            break;
        }
        require(player_spawn_found, "Level 1 OBJECTS.QVM contains a HumanPlayer spawn");
        const float cam_pos[3] = {0.0f, 0.0f, 0.0f};
        const float cam_target[3] = {0.0f, 0.0f, 0.0f};
        const auto mesh = level_data.Host_BuildRenderMesh(origin, 1.0f / 4096.0f, cam_pos, cam_target,
                                                         1.0f, 1.333f, 4000.0f);
        require(!mesh.vertices.empty(), "Mesh has decoded vertices");
        require(!mesh.batches.empty(), "Mesh has render batches");
        for (const auto& batch : mesh.batches)
        {
            require(batch.texture_index < level_data.texture_layers.size(),
                    "Rendered terrain batch resolves to a loaded texture layer");
            require(!batch.indices.empty(), "Rendered terrain batch contains geometry indices");
        }
        require(level_data.tile_maps[1].tex_indices[0] > 0,
                "Tile map 1 has valid non-zero texture indices from TERRAIN.QVM");
        std::cout << "Level 1 terrain mesh generated: " << mesh.vertices.size() << " vertices, "
                  << mesh.batches.size() << " batches.\n";
        for (std::size_t b = 0; b < mesh.batches.size(); ++b)
        {
            std::cout << "  Batch " << b << ": texture_index=" << mesh.batches[b].texture_index
                      << " indices=" << mesh.batches[b].indices.size() << "\n";
        }
    }
    else
    {
        std::cout << "Skipping retail Level 1 file loading test (directory not found).\n";
    }
}

} // namespace

int main()
{
    try
    {
        test_create_material_native();
        test_create_tilemap_native();
        test_modifier_and_heightmap_stack_natives();
        test_texture_modifier_bitmap_coverage();
        test_lod_does_not_use_near_distance_band();
        test_level1_terrain_loading_if_present();

        std::cout << "All terrain native loader tests passed successfully.\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << "Terrain loaders test error: " << error.what() << '\n';
        return 1;
    }
}
