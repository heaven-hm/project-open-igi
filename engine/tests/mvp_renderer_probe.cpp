#include "igi/scene_renderer.hpp"

#include <algorithm>
#include <iostream>
#include <stdexcept>

using namespace igi::gameplay;

int main()
{
    try
    {
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
        std::cout << "Renderer probe passed: perspective projection, nearest depth, near-plane clipping, "
                     "invalid texture, nearest and trilinear mip filtering\n";
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Renderer probe failed: " << exception.what() << '\n';
        return 1;
    }
}
