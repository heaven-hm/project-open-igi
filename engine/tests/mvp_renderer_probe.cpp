#include "igi/scene_renderer.hpp"

#include <algorithm>
#include <iostream>
#include <stdexcept>

using namespace igi::gameplay;

int main()
{
    try
    {
        bool rejected_invalid_anisotropy = false;
        try
        {
            SceneRenderer invalid_anisotropy_renderer(
                640, 480, 1, TextureFilterMode::trilinear_mipmap, 3);
            (void)invalid_anisotropy_renderer;
        }
        catch (const std::runtime_error&)
        {
            rejected_invalid_anisotropy = true;
        }
        if (!rejected_invalid_anisotropy)
        {
            throw std::runtime_error("Renderer must reject unsupported anisotropy levels");
        }

        MissionScene scene;
        scene.collision_triangles = {{{Vector3{-10, -10, 0}, Vector3{10, -10, 0}, Vector3{10, 10, 0}}},
                                     {{Vector3{-10, -10, 0}, Vector3{10, 10, 0}, Vector3{-10, 10, 0}}}};
        scene.textures = {{1, 1, {0xffff0000u}}, {1, 1, {0xff00ff00u}}};
        scene.static_triangles.push_back(
            {{SceneVertex{{-4, 4, -1}, 0, 0}, SceneVertex{{4, 4, -1}, 1, 0}, SceneVertex{{0, 4, 5}, 0.5f, 1}},
             0});
        scene.static_triangles.push_back(
            {{SceneVertex{{-8, 8, -1}, 0, 0}, SceneVertex{{8, 8, -1}, 1, 0}, SceneVertex{{0, 8, 8}, 0.5f, 1}},
             1});
        GameSimulation simulation(scene);
        SceneRenderer serial_renderer(641, 481, 1);
        SceneRenderer parallel_renderer(641, 481, 3);
        const auto verify_parallel_pixels = [&]
        {
            serial_renderer.render(scene, simulation);
            parallel_renderer.render(scene, simulation);
            if (!std::equal(serial_renderer.pixels().begin(), serial_renderer.pixels().end(),
                            parallel_renderer.pixels().begin()) ||
                serial_renderer.visible_triangles() != parallel_renderer.visible_triangles())
            {
                throw std::runtime_error("Parallel raster bands changed pixels or visible triangle counts");
            }
        };
        SceneRenderer renderer(640, 480);
        renderer.render(scene, simulation);
        verify_parallel_pixels();
        const auto center_color = renderer.pixels()[250 * 640 + 340];
        if (((center_color >> 16) & 255) < 40 || ((center_color >> 8) & 255) != 0)
        {
            throw std::runtime_error(
                "Depth buffer must keep the nearer red triangle over the farther green triangle");
        }

        MissionScene object_lightmap_scene;
        object_lightmap_scene.collision_triangles = scene.collision_triangles;
        object_lightmap_scene.textures = {{1, 1, {0xfffafafaU}}, {1, 1, {0xff804020U}}};
        SceneTriangle lightmapped_triangle{
            {SceneVertex{{-4, 4, -1}, 0, 0}, SceneVertex{{4, 4, -1}, 1, 0}, SceneVertex{{0, 4, 5}, 0.5f, 1}},
            0};
        for (auto& vertex : lightmapped_triangle.vertices)
        {
            vertex.lightmap_u = 0.5f;
            vertex.lightmap_v = 0.5f;
        }
        lightmapped_triangle.lightmap_texture_index = 1;
        object_lightmap_scene.static_triangles.push_back(lightmapped_triangle);
        GameSimulation object_lightmap_simulation(object_lightmap_scene);
        SceneRenderer object_lightmap_renderer(640, 480);
        object_lightmap_renderer.render(object_lightmap_scene, object_lightmap_simulation);
        const std::uint32_t lightmapped_pixel = object_lightmap_renderer.pixels()[250 * 640 + 340];
        object_lightmap_scene.static_triangles.front().lightmap_texture_index = -1;
        object_lightmap_renderer.render(object_lightmap_scene, object_lightmap_simulation);
        const std::uint32_t diffuse_only_pixel = object_lightmap_renderer.pixels()[250 * 640 + 340];
        for (const int channel_shift : {0, 8, 16})
        {
            const std::uint32_t diffuse_channel = (diffuse_only_pixel >> channel_shift) & 255U;
            const std::uint32_t lightmap_channel =
                (object_lightmap_scene.textures[1].pixels.front() >> channel_shift) & 255U;
            const std::uint32_t expected_channel = (diffuse_channel * lightmap_channel + 127U) / 255U;
            const std::uint32_t actual_channel = (lightmapped_pixel >> channel_shift) & 255U;
            if (actual_channel > expected_channel + 1U || expected_channel > actual_channel + 1U)
            {
                throw std::runtime_error("Object lightmap channel mismatch: expected " +
                                         std::to_string(expected_channel) + ", got " +
                                         std::to_string(actual_channel));
            }
        }
        object_lightmap_scene.static_triangles.front().lightmap_texture_index = 99;
        bool rejected_missing_lightmap = false;
        try
        {
            object_lightmap_renderer.render(object_lightmap_scene, object_lightmap_simulation);
        }
        catch (const std::runtime_error&)
        {
            rejected_missing_lightmap = true;
        }
        if (!rejected_missing_lightmap)
        {
            throw std::runtime_error("Renderer must reject unavailable object lightmap texture indices");
        }

        scene.terrain_triangle_count = scene.static_triangles.size();
        scene.terrain_fog = {{0.0f, 0.0f, 1.0f}, 0.5f};
        renderer.render(scene, simulation);
        verify_parallel_pixels();
        const auto terrain_fog_pixel = renderer.pixels()[250 * 640 + 340];
        if (((terrain_fog_pixel >> 16) & 255u) >= ((center_color >> 16) & 255u) ||
            (terrain_fog_pixel & 255u) <= ((center_color & 255u) + 40u))
        {
            throw std::runtime_error("Terrain geometry must blend toward its authored per-vertex fog colour");
        }
        scene.terrain_triangle_count = 0;
        scene.object_fog = {{0.0f, 1.0f, 0.0f}, 0.5f};
        renderer.render(scene, simulation);
        verify_parallel_pixels();
        const auto object_fog_pixel = renderer.pixels()[250 * 640 + 340];
        if (((object_fog_pixel >> 8) & 255u) <= ((center_color >> 8) & 255u) + 40u ||
            ((object_fog_pixel >> 16) & 255u) >= ((center_color >> 16) & 255u))
        {
            throw std::runtime_error("Object geometry must use its separate authored fog band");
        }
        scene.static_triangles[0].vertices[0].position.y = -1;
        renderer.render(scene, simulation);
        verify_parallel_pixels();
        if (renderer.visible_triangles() == 0)
        {
            throw std::runtime_error("Near-plane clipping discarded all visible geometry");
        }
        scene.static_triangles[0].texture_index = 400;
        bool rejected_missing_texture = false;
        try
        {
            renderer.render(scene, simulation);
        verify_parallel_pixels();
        }
        catch (const std::runtime_error&)
        {
            rejected_missing_texture = true;
        }
        if (!rejected_missing_texture)
        {
            throw std::runtime_error("Renderer must reject unavailable texture indices");
        }
        scene.static_triangles.clear();
        scene.weapon_mesh = std::make_shared<SceneMesh>();
        scene.weapon_mesh->triangles.push_back(
            {{SceneVertex{{-1, 2, -1}, 0, 0}, SceneVertex{{1, 2, -1}, 1, 0}, SceneVertex{{0, 2, 1}, 0.5f, 1}},
             0,
             true});
        renderer.render(scene, simulation);
        verify_parallel_pixels();
        if (renderer.visible_triangles() != 0)
        {
            throw std::runtime_error("Gun-flame attachments must be hidden while idle");
        }
        PlayerInput firing;
        firing.firing = true;
        simulation.tick(firing);
        renderer.render(scene, simulation);
        verify_parallel_pixels();
        if (renderer.visible_triangles() == 0)
        {
            throw std::runtime_error("A shot must expose its gun-flame attachment");
        }
        for (int tick_index = 0; tick_index < 3; ++tick_index)
        {
            simulation.tick({});
        }
        renderer.render(scene, simulation);
        verify_parallel_pixels();
        if (renderer.visible_triangles() != 0)
        {
            throw std::runtime_error("Gun-flame attachments must disappear after the shot");
        }

        MissionScene mipmap_scene;
        mipmap_scene.collision_triangles = {
            {{Vector3{-10, -10, 0}, Vector3{10, -10, 0}, Vector3{10, 10, 0}}},
            {{Vector3{-10, -10, 0}, Vector3{10, 10, 0}, Vector3{-10, 10, 0}}}};
        SceneTexture mipmap_texture;
        mipmap_texture.width = 8;
        mipmap_texture.height = 8;
        mipmap_texture.pixels.resize(64);
        for (std::size_t pixel_index = 0; pixel_index < mipmap_texture.pixels.size(); ++pixel_index)
        {
            const auto row = pixel_index / mipmap_texture.width;
            const auto column = pixel_index % mipmap_texture.width;
            mipmap_texture.pixels[pixel_index] = ((row + column) & 1u) == 0 ? 0xff000000u : 0xffffffffu;
        }
        mipmap_texture.mipmaps.push_back(
            {4, 4, std::vector<std::uint32_t>(16, 0xff00ff00u), false});
        mipmap_scene.textures.push_back(std::move(mipmap_texture));
        mipmap_scene.static_triangles.push_back(
            {{SceneVertex{{-4, 10, -2.32f}, 0, 0},
              SceneVertex{{4, 10, -2.32f}, 256, 0},
              SceneVertex{{0, 10, 5.68f}, 128, 256}},
             0});
        GameSimulation mipmap_simulation(mipmap_scene);
        SceneRenderer nearest_mipmap_renderer(640, 480, 1, TextureFilterMode::nearest_mipmap);
        SceneRenderer trilinear_mipmap_renderer(640, 480, 1, TextureFilterMode::trilinear_mipmap);
        SceneRenderer parallel_trilinear_mipmap_renderer(640, 480, 3, TextureFilterMode::trilinear_mipmap);
        nearest_mipmap_renderer.render(mipmap_scene, mipmap_simulation);
        trilinear_mipmap_renderer.render(mipmap_scene, mipmap_simulation);
        parallel_trilinear_mipmap_renderer.render(mipmap_scene, mipmap_simulation);
        const auto nearest_mipmap_pixel = nearest_mipmap_renderer.pixels()[240 * 640 + 320];
        const auto trilinear_mipmap_pixel = trilinear_mipmap_renderer.pixels()[240 * 640 + 320];
        if (nearest_mipmap_pixel == trilinear_mipmap_pixel ||
            ((trilinear_mipmap_pixel >> 8) & 255u) <= ((trilinear_mipmap_pixel >> 16) & 255u) ||
            !std::equal(trilinear_mipmap_renderer.pixels().begin(),
                        trilinear_mipmap_renderer.pixels().end(),
                        parallel_trilinear_mipmap_renderer.pixels().begin()))
        {
            throw std::runtime_error(
                "Trilinear filtering must use generated mip levels and match serial raster output");
        }
        mipmap_scene.textures[0].mipmaps[0].authored = true;
        nearest_mipmap_renderer.render(mipmap_scene, mipmap_simulation);
        const auto authored_nearest_mipmap_pixel = nearest_mipmap_renderer.pixels()[240 * 640 + 320];
        if (((authored_nearest_mipmap_pixel >> 8) & 255u) <=
            ((authored_nearest_mipmap_pixel >> 16) & 255u))
        {
            throw std::runtime_error("Nearest mip filtering must use authored TEX mip levels");
        }

        MissionScene anisotropic_scene = mipmap_scene;
        anisotropic_scene.textures[0].mipmaps[0].authored = false;
        anisotropic_scene.textures[0].mipmaps[0].pixels.assign(16, 0xff00ff00u);
        anisotropic_scene.textures[0].mipmaps.push_back(
            {2, 2, std::vector<std::uint32_t>(4, 0xff0000ffu), false});
        anisotropic_scene.textures[0].mipmaps.push_back(
            {1, 1, std::vector<std::uint32_t>(1, 0xffff0000u), false});
        anisotropic_scene.static_triangles[0].vertices[1].texture_u = 1024;
        anisotropic_scene.static_triangles[0].vertices[2].texture_u = 512;
        anisotropic_scene.static_triangles[0].vertices[2].texture_v = 2;
        GameSimulation anisotropic_simulation(anisotropic_scene);
        SceneRenderer isotropic_renderer(640, 480, 1, TextureFilterMode::trilinear_mipmap, 1);
        SceneRenderer anisotropic_renderer(640, 480, 1, TextureFilterMode::trilinear_mipmap, 4);
        SceneRenderer parallel_anisotropic_renderer(640, 480, 3, TextureFilterMode::trilinear_mipmap, 4);
        isotropic_renderer.render(anisotropic_scene, anisotropic_simulation);
        anisotropic_renderer.render(anisotropic_scene, anisotropic_simulation);
        parallel_anisotropic_renderer.render(anisotropic_scene, anisotropic_simulation);
        const auto isotropic_pixel = isotropic_renderer.pixels()[240 * 640 + 320];
        const auto anisotropic_pixel = anisotropic_renderer.pixels()[240 * 640 + 320];
        if (((isotropic_pixel >> 16) & 255u) <= ((anisotropic_pixel >> 16) & 255u) ||
            !std::equal(anisotropic_renderer.pixels().begin(),
                        anisotropic_renderer.pixels().end(),
                        parallel_anisotropic_renderer.pixels().begin()))
        {
            throw std::runtime_error("Anisotropic filtering must retain detail along a stretched texture "
                                     "footprint and match parallel rendering");
        }
        std::cout << "Renderer probe passed: perspective projection, nearest depth, near-plane clipping, "
                     "diffuse/lightmap modulation, invalid texture references, mip filtering and anisotropic "
                     "filtering\n";
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Renderer probe failed: " << exception.what() << '\n';
        return 1;
    }
}
