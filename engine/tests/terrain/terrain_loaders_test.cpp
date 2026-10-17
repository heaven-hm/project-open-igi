#include "igi/retail/terrain/terrain_loaders.hpp"

#include <array>
#include <cassert>
#include <filesystem>
#include <iostream>
#include <stdexcept>
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
    for (const auto b : dummy_ctx)
    {
        require(b == 0, "Terrain_CreateMaterial zeroes 0x18 context bytes");
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
    for (const auto b : dummy_ctx)
    {
        require(b == 0, "Terrain_CreateTileMap zeroes 0x18 context bytes");
    }
}

void test_modifier_and_heightmap_stack_natives()
{
    using namespace igi::retail::terrain;
    std::array<std::uint8_t, 512> dummy_ctx{};
    *reinterpret_cast<std::int32_t*>(dummy_ctx.data() + 0xBC) = 0; // counter >= 0
    *reinterpret_cast<std::uint32_t*>(dummy_ctx.data() + 0xE4) = 5; // scratch val

    const auto push_res = TextureModifier_Push(dummy_ctx.data());
    require(push_res != nullptr, "TextureModifier_Push returns active pointer/index");

    *reinterpret_cast<std::int32_t*>(dummy_ctx.data() + 0xBC) = 0;
    *reinterpret_cast<std::uint32_t*>(dummy_ctx.data() + 0xF8) = 7;
    const auto hmp_res = HeightMap_Enter(dummy_ctx.data());
    require(hmp_res != nullptr, "HeightMap_Enter returns heightmap index");

    *reinterpret_cast<std::int32_t*>(dummy_ctx.data() + 0x108) = 1;
    *reinterpret_cast<std::uint32_t*>(dummy_ctx.data() + 0xD8) = 3;
    const auto lmp_res = TerrainLightMap_Init_2(dummy_ctx.data());
    require(lmp_res != nullptr, "TerrainLightMap_Init_2 returns lightmap index");
}

void test_level1_terrain_loading_if_present()
{
    using namespace igi::retail::terrain;
    const std::filesystem::path level1_dir = "/Users/haseeb-mir/Downloads/IGI/MISSIONS/location0/LEVEL1/TERRAIN";
    if (std::filesystem::exists(level1_dir))
    {
        TerrainLevelData level_data{};
        std::vector<std::shared_ptr<igi::retail::script::QvmObjectCall>> empty_calls;
        const bool loaded = level_data.Host_LoadLevelTerrain(level1_dir, empty_calls);
        require(loaded, "Level 1 terrain loads successfully from retail directory");
        require(level_data.contour_nodes.size() > 1000, "Level 1 CTR contains contour nodes");
        require(level_data.texture_layers.size() == 18, "Level 1 TERRAIN.TEX has 18 texture layers");
        require(level_data.texture_layers[0].width == 128 && level_data.texture_layers[0].height == 128,
                "Texture layer 0 dimension is 128x128");

        // Test render mesh building
        const std::array<double, 3> origin = {-24373846.0, -35335356.0, 181129328.0};
        const float cam_pos[3] = {0.0f, -100.0f, 50.0f};
        const float cam_target[3] = {0.0f, 0.0f, 0.0f};
        const auto mesh = level_data.Host_BuildRenderMesh(origin, 1.0f / 4096.0f, cam_pos, cam_target,
                                                         1.0f, 1.333f, 500.0f);
        require(!mesh.vertices.empty(), "Mesh has decoded vertices");
        require(!mesh.batches.empty(), "Mesh has render batches");
        std::cout << "Level 1 terrain mesh generated: " << mesh.vertices.size() << " vertices, "
                  << mesh.batches.size() << " batches.\n";
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
