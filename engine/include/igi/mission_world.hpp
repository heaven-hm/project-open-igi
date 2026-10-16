#pragma once

#include "igi/ai_navigation.hpp"
#include "igi/game_assets.hpp"
#include "igi/human_motion.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace igi::gameplay {

constexpr float kUnitsPerMeter = 4096.0f;

struct TextureImage {
    std::string name;
    std::uint32_t width{};
    std::uint32_t height{};
    bool has_alpha{};
    std::vector<std::uint32_t> argb_pixels;

    [[nodiscard]] std::uint32_t sample_bilinear(float u, float v) const noexcept;
    [[nodiscard]] std::uint32_t sample_nearest(float u, float v) const noexcept;
};

struct MeshVertex {
    Vector3 position{};
    Vector3 normal{0.0f, 0.0f, 1.0f};
    float u{};
    float v{};
    std::uint16_t bone_index{};
};

struct MeshSubBlock {
    std::size_t triangle_start{};
    std::size_t triangle_count{};
    int material_slot{};
    float opacity{1.0f};
};

struct CollisionTriangle {
    Vector3 v0{};
    Vector3 v1{};
    Vector3 v2{};
    Vector3 unit_normal{0.0f, 0.0f, 1.0f};
    std::int16_t material_id{-1};
    bool is_portal{};
};

struct MeshBone {
    std::string name;
    int parent{-1};
    Vector3 pivot{};
    Vector3 rest_world_position{};
};

struct ModelMesh {
    std::string model_id;
    std::vector<MeshVertex> vertices;
    std::vector<std::array<std::uint32_t, 3>> triangles;
    std::vector<MeshSubBlock> sub_blocks;
    std::vector<std::string> slot_texture_names;
    std::vector<CollisionTriangle> collision_triangles_set0;
    std::vector<CollisionTriangle> collision_triangles_set1;
    std::vector<MeshBone> bones;
    Vector3 min_bounds{};
    Vector3 max_bounds{};
    float bounding_radius{};
};

enum class PlacedObjectKind : std::uint8_t {
    StaticBuilding,
    RigidProp,
    Door,
    Fence,
    Switch,
    Terminal,
    SecurityCamera,
    ExplosiveProp,
    GunPickup,
    AmmoPickup,
    Vehicle,
    ClimbLadder,
    SplineProp,
};

struct PlacedObject {
    std::int32_t task_id{-1};
    std::string class_name;
    std::string instance_name;
    std::string model_id;
    PlacedObjectKind kind{PlacedObjectKind::StaticBuilding};
    NavigationPosition position{};
    NavigationPosition closed_position{};
    float alpha{};
    float beta{};
    float gamma{};
    Basis3x3 basis{};
    bool is_interactive{};
    bool is_open{};
    float open_progress{};
    bool is_destructible{};
    bool is_destroyed{};
    float health{100.0f};
    bool is_picked_up{};
    std::string pickup_weapon_id;
    int pickup_ammo_count{};
};

struct PatrolCommand {
    int opcode{};
    int argument{};
};

struct SoldierSpawn {
    std::int32_t task_id{-1};
    std::string name;
    std::string model_id;
    NavigationPosition position{};
    float gamma{};
    int team{1};
    int bone_hierarchy{0};
    int stand_animation{0};
    std::string ai_type;
    int graph_id{1};
    std::string weapon_id{"WEAPON_ID_AK47"};
    float view_length{122880.0f};
    float secondary_view_length{40960.0f};
    std::vector<PatrolCommand> patrol_commands;
};

struct TerrainTriangle {
    Vector3 v0{};
    Vector3 v1{};
    Vector3 v2{};
    Vector3 normal{0.0f, 0.0f, 1.0f};
    Vector3 n0{0.0f, 0.0f, 1.0f};
    Vector3 n1{0.0f, 0.0f, 1.0f};
    Vector3 n2{0.0f, 0.0f, 1.0f};
    float u0{}, v0_uv{};
    float u1{}, v1_uv{};
    float u2{}, v2_uv{};
    int tile_index{};
};

struct HeightMapPatch {
    int level{11};
    int grid_size{128};
    int bitmap_id{0};
    double world_x{};
    double world_y{};
    double world_z{};
    std::vector<std::int8_t> samples;
};

struct SkeletalKeyRotation {
    float qx{}, qy{}, qz{}, qw{1.0f};
    float time{};
};

struct SkeletalClip {
    int animation_id{};
    float duration{1.0f};
    std::vector<std::vector<SkeletalKeyRotation>> bone_rotations;
};

struct SkeletalAnimationBank {
    int hierarchy_id{};
    std::vector<int> parents;
    std::vector<Vector3> bone_translations;
    std::vector<SkeletalClip> clips;
};

class MissionWorld final {
public:
    [[nodiscard]] bool load_from_retail_install(
        const std::filesystem::path& game_root,
        int mission_number = 1);

    [[nodiscard]] bool is_loaded() const noexcept { return is_loaded_; }
    [[nodiscard]] const std::string& status_message() const noexcept { return status_message_; }
    [[nodiscard]] const std::filesystem::path& game_root() const noexcept { return game_root_; }
    [[nodiscard]] const std::filesystem::path& level_root() const noexcept { return level_root_; }

    [[nodiscard]] NavigationPosition player_spawn_position() const noexcept { return player_spawn_position_; }
    [[nodiscard]] float player_spawn_gamma() const noexcept { return player_spawn_gamma_; }
    [[nodiscard]] const std::vector<std::string>& player_initial_weapons() const noexcept { return player_initial_weapons_; }

    [[nodiscard]] const std::vector<PlacedObject>& placed_objects() const noexcept { return placed_objects_; }
    [[nodiscard]] std::vector<PlacedObject>& placed_objects_mutable() noexcept { return placed_objects_; }
    [[nodiscard]] const std::vector<SoldierSpawn>& soldier_spawns() const noexcept { return soldier_spawns_; }
    [[nodiscard]] const std::vector<TerrainTriangle>& terrain_triangles() const noexcept { return terrain_triangles_; }
    [[nodiscard]] const std::unordered_map<int, igi::assets::NavigationGraph>& world_navigation_graphs() const noexcept {
        return world_navigation_graphs_;
    }

    [[nodiscard]] const ModelMesh* find_model(const std::string& model_id) const noexcept;
    [[nodiscard]] const TextureImage* find_texture(const std::string& texture_name) const noexcept;
    [[nodiscard]] const TextureImage* terrain_tile_texture(int tile_index) const noexcept;

    [[nodiscard]] bool sample_terrain_height(
        double world_x,
        double world_y,
        float& out_ground_z,
        Vector3& out_ground_normal) const noexcept;

    [[nodiscard]] bool sample_terrain_height(
        double world_x,
        double world_y,
        double minimum_ground_z,
        double maximum_ground_z,
        float& out_ground_z,
        Vector3& out_ground_normal) const noexcept;

    void evaluate_skeletal_pose(
        int hierarchy_id,
        int clip_id,
        float time_seconds,
        const ModelMesh& mesh,
        std::vector<Vector3>& out_bone_world_offsets) const noexcept;

    struct InteractionResult {
        bool interacted{};
        PlacedObjectKind kind{};
        int object_index{-1};
        std::string message;
        std::string weapon_id;
        int ammo_count{};
        std::string script_variable;
    };

    // Attempts to interact with the nearest interactive object within interaction_radius_units.
    // Default 8192 units (2 m) matches the HUD hint range in get_interaction_hint.
    [[nodiscard]] InteractionResult try_interact(
        NavigationPosition player_pos,
        float player_yaw,
        float interaction_radius_units = 8192.0f) noexcept;

    // Advances animated doors (open/close movement).
    void update_doors(float delta_seconds) noexcept;

    // Applies damage to destructible placed objects (security cameras, explosive props)
    [[nodiscard]] bool damage_object(int object_index, float damage) noexcept;

    [[nodiscard]] std::string get_interaction_hint(
        NavigationPosition eye,
        float yaw) const noexcept;

    void set_alarm_active(bool active) noexcept { is_alarm_active_ = active; }
    [[nodiscard]] bool is_alarm_active() const noexcept { return is_alarm_active_; }

    void extract_nearby_terrain_mesh(double center_x, double center_y, double radius);
    [[nodiscard]] static Basis3x3 basis_from_euler(float alpha, float beta, float gamma) noexcept;
    [[nodiscard]] static Vector3 transform_local_to_world_offset(
        Vector3 local_point,
        const Basis3x3& basis) noexcept;
    [[nodiscard]] static Vector3 transform_world_to_local_offset(
        Vector3 world_offset,
        const Basis3x3& basis) noexcept;

private:
    bool parse_qsc_scene(const std::string& qsc_text);
    void load_mtp_packages();
    void load_referenced_models_and_textures();
    void load_terrain_octree_and_heightmaps();
    void load_animation_banks();

    std::filesystem::path game_root_;
    std::filesystem::path level_root_;
    bool is_loaded_{};
    bool is_alarm_active_{false};
    std::string status_message_;

    NavigationPosition player_spawn_position_{24578514.0, -55852816.0, 174421680.0};
    float player_spawn_gamma_{3.095987f};
    std::vector<std::string> player_initial_weapons_;

    std::unordered_map<std::string, std::vector<std::string>> mtp_model_textures_;
    std::unordered_map<std::string, ModelMesh> models_;
    std::unordered_map<std::string, TextureImage> textures_;
    std::vector<TextureImage> terrain_tiles_;

    std::vector<PlacedObject> placed_objects_;
    std::vector<SoldierSpawn> soldier_spawns_;
    std::vector<std::vector<PatrolCommand>> all_patrol_paths_;
    std::unordered_map<int, NavigationPosition> ai_graph_offsets_;
    std::unordered_map<int, igi::assets::NavigationGraph> world_navigation_graphs_;

    std::vector<std::uint8_t> ctr_bytes_;
    std::vector<std::uint8_t> cmd_bytes_;
    std::vector<HeightMapPatch> height_maps_;
    std::vector<TerrainTriangle> terrain_triangles_;

    std::unordered_map<int, SkeletalAnimationBank> animation_banks_;
};

} // namespace igi::gameplay
