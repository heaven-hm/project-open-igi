#include "igi/game_simulation.hpp"
#include "igi/navigation.hpp"

#include <cmath>
#include <exception>
#include <iostream>
#include <numbers>
#include <stdexcept>

using namespace igi::gameplay;

void require(bool condition, const char* message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}

MissionScene test_scene()
{
    MissionScene scene;
    scene.collision_triangles = {{{Vector3{-100, -100, -2}, Vector3{100, -100, -2}, Vector3{100, 100, -2}}},
                                 {{Vector3{-100, -100, -2}, Vector3{100, 100, -2}, Vector3{-100, 100, -2}}}};
    SceneWeapon pistol;
    pistol.identifier = "WEAPON_ID_GLOCK";
    pistol.parameters.name = "Glock";
    pistol.parameters.magazine_capacity = 17;
    pistol.parameters.automatic = false;
    pistol.initial_reserve = 49;
    pistol.initially_owned = true;
    pistol.ammunition_identifier = "AMMO_ID_919";
    SceneWeapon rifle = pistol;
    rifle.identifier = "WEAPON_ID_MP5SD";
    rifle.parameters = {};
    SceneWeapon knife = pistol;
    knife.identifier = "WEAPON_ID_KNIFE";
    knife.parameters.melee = true;
    knife.parameters.magazine_capacity = 1;
    knife.parameters.maximum_distance = 1.8f;
    knife.ammunition_identifier = "AMMO_ID_NONE";
    SceneWeapon pickup = pistol;
    pickup.identifier = "WEAPON_ID_AK47";
    pickup.initially_owned = false;
    pickup.ammunition_identifier = "AMMO_ID_762";
    SceneWeapon sniper = pistol;
    sniper.identifier = "WEAPON_ID_DRAGUNOV";
    sniper.parameters.name = "Dragunov SVD";
    sniper.parameters.magazine_capacity = 10;
    sniper.initial_reserve = 50;
    sniper.initially_owned = false;
    sniper.ammunition_identifier = "AMMO_ID_762x54";
    scene.weapons = {pistol, rifle, knife, pickup, sniper};
    scene.weapon_categories = {{"WEAPON_ID_KNIFE"}, {"WEAPON_ID_GLOCK"}, {"WEAPON_ID_MP5SD"}};
    scene.weapon_pickups.push_back({{10, 0, -2}, "WEAPON_ID_AK47"});
    return scene;
}

int main()
{
    try
    {
        NavigationGraph graph{7,
                              {{1, {0, 6, -2}, {1}},
                               {2, {-5, 6, -2}, {0, 2}},
                               {3, {-5, 12, -2}, {1, 3}},
                               {4, {0, 12, -2}, {2}},
                               {5, {50, 50, -2}, {}}}};
        const auto route = find_navigation_path(graph, {0, 6, -2}, {0, 12, -2});
        require(route.size() == 4 && route[1].x == -5 && route[2].y == 12,
                "Pathfinding must follow graph connectivity around an obstacle");
        require(find_navigation_path(graph, {0, 6, -2}, {50, 50, -2}).empty(),
                "Disconnected graph components must not invent a route");
        graph.nodes[0].neighbors.push_back(999);
        bool rejected = false;
        try
        {
            static_cast<void>(find_navigation_path(graph, {0, 6, -2}, {0, 12, -2}));
        }
        catch (const std::runtime_error&)
        {
            rejected = true;
        }
        require(rejected, "Invalid navigation edges must produce a controlled error");
        graph.nodes[0].neighbors.pop_back();

        graph.route_table_width = 5;
        graph.route_table.assign(25, {});
        graph.route_table[1 + 4 * graph.route_table_width] = {2, 3.0f};
        graph.route_table[2 + 4 * graph.route_table_width] = {3, 2.0f};
        graph.route_table[3 + 4 * graph.route_table_width] = {4, 1.0f};
        const auto retail_route = find_navigation_path(graph, {0, 6, -2}, {0, 12, -2});
        require(retail_route.size() == 4 && retail_route[1].x == -5 && retail_route.back().x == 0,
                "Retail route table must take precedence over reconstructed graph edges");

        auto scene = test_scene();
        GameSimulation simulation(scene);
        for (int tick_index = 0; tick_index < 600; ++tick_index)
        {
            simulation.tick({});
        }
        require(simulation.health() == scene.player_parameters.maximum_health,
                "Health must not drain without a projectile hit");
        require(simulation.inventory_size() == 3 && simulation.selected_weapon_slot() == 1,
                "Initial loadout must select the MP5");
        PlayerInput input;
        input.weapon_category = 0;
        simulation.tick(input);
        require(simulation.weapon_parameters().melee, "Original weapon category 1 must select the knife");
        input.weapon_category = 2;
        simulation.tick(input);
        require(simulation.selected_weapon_slot() == 1, "Original weapon category 3 must select the MP5");
        input = {};
        input.firing = true;
        for (int tick_index = 0; tick_index < 100; ++tick_index)
        {
            simulation.tick(input);
        }
        input = {};
        input.reloading = true;
        simulation.tick(input);
        require(simulation.weapon().reload_remaining > 0, "Reload must start");
        input = {};
        input.weapon_slot = 0;
        simulation.tick(input);
        require(simulation.weapon().magazine == 17 && simulation.weapon().reload_remaining == 0,
                "Switch must equip pistol and cancel reload");
        input = {};
        input.firing = true;
        const auto shots_before = simulation.weapon().shots_fired;
        for (int tick_index = 0; tick_index < 60; ++tick_index)
        {
            simulation.tick(input);
        }
        require(simulation.weapon().shots_fired == shots_before + 1,
                "Semiautomatic pistol must require trigger release");
        input = {};
        input.reloading = true;
        simulation.tick(input);
        input = {};
        for (int tick_index = 0; tick_index < 62; ++tick_index)
        {
            simulation.tick(input);
        }
        require(simulation.weapon().reserve == 48, "Reload must consume the shared ammunition pool");
        input.weapon_slot = 1;
        simulation.tick(input);
        require(simulation.weapon().magazine == 0 && simulation.weapon().reserve == 48,
                "Switch must preserve each magazine and synchronize shared ammunition");
        input.weapon_slot = 999;
        simulation.tick(input);
        require(simulation.selected_weapon_slot() == 1, "Unowned weapon slots must be ignored");
        input = {};
        input.weapon_cycle = 1;
        simulation.tick(input);
        require(simulation.selected_weapon_slot() == 2, "Wheel cycle must select next owned weapon");
        input = {};
        input.firing = true;
        simulation.tick(input);
        require(simulation.weapon().magazine == 1, "Knife must not consume ammunition");
        simulation.teleport_player({10, 0, -2}, 0, 0);
        input = {};
        simulation.tick(input);
        require(simulation.inventory_size() == 4, "Retail automatic pickup must add the nearby weapon");
        require(simulation.take_player_weapon_pickup_sound_count() == 1,
                "Retail Weapon_GunPickup must queue one weaponpickup_1 sound event");
        simulation.tick(input);
        require(simulation.inventory_size() == 4, "A retail pickup must not be collected twice");
        require(simulation.take_player_weapon_pickup_sound_count() == 0,
                "A collected pickup must not emit a second weaponpickup_1 sound event");

        auto ammunition_pickup_scene = test_scene();
        ammunition_pickup_scene.weapon_pickups.clear();
        ammunition_pickup_scene.weapon_pickups.push_back(
            {{10.0f, 0.0f, -2.0f}, "AMMO_ID_919", {1.0f, 1.0f, 1.0f}, 1.0f, 16});
        GameSimulation ammunition_pickup_simulation(ammunition_pickup_scene);
        ammunition_pickup_simulation.teleport_player({10.0f, 0.0f, -2.0f}, 0.0f, 0.0f);
        ammunition_pickup_simulation.tick({});
        require(ammunition_pickup_simulation.weapon().reserve == 65,
                "AmmoPickup must apply the Int16 count from the retail QVM task parameters");
        require(ammunition_pickup_simulation.take_player_weapon_pickup_sound_count() == 0,
                "AmmoPickup must not emit the Weapon_GunPickup weaponpickup_1 sound");

        simulation.restart();
        require(simulation.inventory_size() == 3 && simulation.weapon().magazine == 32 &&
                    simulation.health() == scene.player_parameters.maximum_health,
                "Restart must restore the original loadout and health");

        // Verify weapon zoom differentiation: MP5 iron sights vs Dragunov scope
        input = {};
        input.aiming = true;
        for (int i = 0; i < 15; ++i)
        {
            simulation.tick(input);
        }
        require(!simulation.is_scoped(), "MP5SD iron sights must not engage sniper scope mode");
        require(simulation.zoom_factor() > 1.2f, "MP5SD aiming must provide iron-sight zoom");

        // The original player-only developer shortcut must not refill or heal an AI soldier.
        auto cheat_scope_scene = test_scene();
        cheat_scope_scene.actors.push_back({{0, 6, -2}, std::numbers::pi_v<float>, 2201, 1, {}});
        GameSimulation player_only_cheat(cheat_scope_scene);
        const float enemy_health_before_cheat = player_only_cheat.actors().front().health;
        const std::uint32_t enemy_ammunition_before_cheat =
            player_only_cheat.actors().front().ammunition_in_clip;
        player_only_cheat.toggle_developer_mode();
        require(player_only_cheat.actors().front().health == enemy_health_before_cheat &&
                    player_only_cheat.actors().front().ammunition_in_clip == enemy_ammunition_before_cheat,
                "Player unlimited-health/ammo mode must not change an enemy's health or ammunition");
        PlayerInput player_only_fire;
        player_only_fire.firing = true;
        for (int tick_index = 0; tick_index < 120; ++tick_index)
        {
            player_only_cheat.tick(player_only_fire);
        }
        const auto enemy_weapon = std::find_if(
            cheat_scope_scene.weapons.begin(),
            cheat_scope_scene.weapons.end(),
            [&](const auto& weapon) { return weapon.identifier == cheat_scope_scene.actors.front().weapon_identifier; });
        const std::uint32_t enemy_magazine_capacity =
            enemy_weapon == cheat_scope_scene.weapons.end()
                ? cheat_scope_scene.weapon_parameters.magazine_capacity
                : enemy_weapon->parameters.magazine_capacity;
        require(player_only_cheat.actors().front().health < enemy_health_before_cheat &&
                    player_only_cheat.actors().front().ammunition_in_clip <= enemy_magazine_capacity &&
                    player_only_cheat.actors().front().ammunition_in_clip != 999 &&
                    player_only_cheat.health() == cheat_scope_scene.player_parameters.maximum_health,
                "Player-only cheats must leave enemies vulnerable and their ammunition finite");
        require(player_only_cheat.actors().front().shots_fired > 0,
                "An enemy must continue firing with normal combat behavior while player cheats are active");

        auto medipack_scene = test_scene();
        medipack_scene.weapon_pickups.clear();
        SceneWeapon medipack_weapon;
        medipack_weapon.identifier = "WEAPON_ID_MEDIPACK";
        medipack_weapon.parameters.name = "Medipack";
        medipack_weapon.parameters.category = WeaponCategory::medipack;
        medipack_weapon.parameters.magazine_capacity = 1;
        medipack_weapon.ammunition_identifier = "AMMO_ID_MEDIPACK";
        medipack_scene.weapons.push_back(medipack_weapon);
        medipack_scene.collision_triangles.push_back(
            {{Vector3{-2.0f, -2.0f, 14.0f}, Vector3{2.0f, -2.0f, 14.0f}, Vector3{2.0f, 2.0f, 14.0f}}});
        medipack_scene.collision_triangles.push_back(
            {{Vector3{-2.0f, -2.0f, 14.0f}, Vector3{2.0f, 2.0f, 14.0f}, Vector3{-2.0f, 2.0f, 14.0f}}});
        const Vector3 medipack_test_root_motion_derivative{0.0f, 12288.0f / 4800.0f, 0.0f};
        medipack_scene.root_motion_clips[4] = {
            4800,
            {{0, {}, medipack_test_root_motion_derivative, medipack_test_root_motion_derivative},
             {4800,
              {0, 12288, 0},
              medipack_test_root_motion_derivative,
              medipack_test_root_motion_derivative}}};
        medipack_scene.weapon_pickups.push_back({{40.0f, 0.0f, -2.0f}, medipack_weapon.identifier});
        medipack_scene.weapon_pickups.push_back(
            {{40.0f, 0.0f, -2.0f}, "AMMO_ID_MEDIPACK", {1.0f, 1.0f, 1.0f}, 1.0f, 50});
        GameSimulation medipack_pickup_simulation(medipack_scene);
        medipack_pickup_simulation.teleport_player({0.0f, 0.0f, 14.0f}, 0.0f, 0.0f);
        PlayerInput walk_off_fall_test_platform;
        walk_off_fall_test_platform.forward = 1.0f;
        for (int walk_tick_index = 0; walk_tick_index < 60; ++walk_tick_index)
        {
            medipack_pickup_simulation.tick(walk_off_fall_test_platform);
        }
        for (int fall_tick_index = 0; fall_tick_index < 180; ++fall_tick_index)
        {
            medipack_pickup_simulation.tick({});
        }
        const float health_before_medipack_pickup = medipack_pickup_simulation.health();
        require(health_before_medipack_pickup > 0.0f &&
                    health_before_medipack_pickup < medipack_scene.player_parameters.maximum_health,
                "The setup fall must leave the player alive with verifiable fall damage");
        medipack_pickup_simulation.teleport_player({40.0f, 0.0f, -2.0f}, 0.0f, 0.0f);
        medipack_pickup_simulation.tick({});
        require(medipack_pickup_simulation.inventory_size() == 4,
                "Retail Weapon_GunPickup must add the medipack weapon task to the human inventory");
        require(std::abs(medipack_pickup_simulation.health() - health_before_medipack_pickup) < 0.0001f,
                "Retail Weapon_GunPickup must not heal the human while adding a medipack");
        PlayerInput select_medipack;
        select_medipack.weapon_slot = 3;
        medipack_pickup_simulation.tick(select_medipack);
        require(medipack_pickup_simulation.weapon().magazine == 1 &&
                    medipack_pickup_simulation.weapon().reserve == 50,
                "Retail AmmoPickup must place its 50-round payload in the medipack ammo pool");
        require(std::abs(medipack_pickup_simulation.health() - health_before_medipack_pickup) < 0.0001f,
                "Picking up medipack charges must not heal before the medipack use state");

        // Verify developer-mode gameplay behavior
        simulation.toggle_developer_mode();
        require(simulation.developer_mode(), "Developer mode must be active after toggle");
        require(simulation.inventory_size() == scene.weapons.size(),
                "Developer mode must grant all weapons in mission scene");
        require(simulation.weapon().reserve == 999, "Developer mode must grant 999 unlimited reserve ammo");
        require(simulation.health() == scene.player_parameters.maximum_health,
                "Developer mode must restore maximum health");

        // Find and select Dragunov in inventory
        int dragunov_slot = -1;
        for (std::size_t slot = 0; slot < simulation.inventory_size(); ++slot)
        {
            input = {};
            input.weapon_slot = static_cast<int>(slot);
            simulation.tick(input);
            const auto* def = simulation.weapon_definition();
            if (def && def->identifier == "WEAPON_ID_DRAGUNOV")
            {
                dragunov_slot = static_cast<int>(slot);
                break;
            }
        }
        require(dragunov_slot >= 0, "Dragunov must be present in developer mode inventory");
        input = {};
        input.aiming = true;
        for (int i = 0; i < 20; ++i)
        {
            simulation.tick(input);
        }
        require(simulation.is_scoped(), "Aiming with Dragunov must engage high-magnification sniper scope");
        require(simulation.zoom_factor() >= 4.0f, "Dragunov scope must provide telescopic magnification >= 4x");
        simulation.adjust_zoom(1);
        require(simulation.is_scoped(), "Dragunov must remain scoped after adjust_zoom");

        // Toggle developer mode off
        simulation.toggle_developer_mode();
        require(!simulation.developer_mode(), "Developer mode must toggle off cleanly");

        simulation.restart();

        scene.navigation_graphs.push_back(graph);
        scene.patrol_paths.push_back({2400, {{2, 4}}});
        SceneActor soldier;
        soldier.position = {0, 6, -2};
        soldier.identifier = 2000;
        soldier.navigation_graph_identifier = 7;
        soldier.patrol_path_identifier = 2400;
        scene.actors.push_back(soldier);
        scene.collision_triangles.push_back({{Vector3{-3, 8, -2}, Vector3{3, 8, -2}, Vector3{3, 8, 3}}});
        scene.collision_triangles.push_back({{Vector3{-3, 8, -2}, Vector3{3, 8, 3}, Vector3{-3, 8, 3}}});
        GameSimulation patrol(scene);
        for (int tick_index = 0; tick_index < 500; ++tick_index)
        {
            patrol.tick({});
        }
        require(patrol.actors().front().position.y > 11 &&
                    patrol.actors().front().behavior == ActorBehavior::guarding,
                "Authored patrol must navigate around a wall while ignoring an unseen player");
        std::cout << "Navigation/inventory probe passed: connected paths, disconnection, bounds, health, "
                     "switching, ammo, pickups, patrol\n";
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Navigation/inventory probe failed: " << exception.what() << '\n';
        return 1;
    }
}
