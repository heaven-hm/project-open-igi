#pragma once

#include "igi/retail/terrain/terrain_loaders.hpp"
#include "retail_animation.hpp"

#include <array>
#include <cstdint>
#include <vector>

// Host player controller for the retail human movement model.
//
// The port walks the human with the retail model rather than a game-feel invention: gravity,
// jump, air control, the ground query budgets, the slope-slide bands and the eye heights all
// come from the retail natives named in player_controller.cpp, and the first-person eye is
// produced by the transcribed Human_CameraEyePosition native (retail VA 0x00465820). The code
// here is host glue: it samples the terrain that is drawn, integrates the retail tick, and
// converts between retail units (1/4096 m) and the renderer's metre space. It is not a native
// transcription and does not carry a verification record.
namespace igi::host
{

// Ground surface sample in the renderer's metre space: (retail units - level origin) / 4096.
struct GroundSample
{
    bool found = false;
    double z = 0.0;
    std::array<float, 3> normal{0.0F, 0.0F, 1.0F};
};

// Triangle index over the decoded terrain mesh, used for the ground query.
//
// The grid is rebuilt from the terrain mesh the renderer builds for the frame, so the human
// stands on the surface that is actually drawn; the drawn mesh carries the fine LOD around the
// camera, which is where the human is. Queries outside the indexed window report "not found",
// which the controller reads as "no ground" and treats as airborne.
class TerrainCollisionGrid
{
  public:
    static constexpr int cells_per_axis = 24;
    static constexpr double cell_size_metres = 16.0;
    static constexpr double extent_metres = cells_per_axis * cell_size_metres;

    // HOST HELPER: indexes the terrain mesh triangles around a centre in metre space.
    void rebuild(const igi::retail::terrain::TerrainDecodedMesh& mesh, double centre_x, double centre_y);

    // HOST HELPER: topmost surface height and normal under (x, y) in metre space.
    [[nodiscard]] GroundSample sample(double x, double y) const;

    [[nodiscard]] bool empty() const noexcept { return triangles_.empty(); }

  private:
    struct Triangle
    {
        std::array<double, 3> a{};
        std::array<double, 3> b{};
        std::array<double, 3> c{};
    };

    std::vector<Triangle> triangles_;
    std::array<std::vector<std::uint32_t>, static_cast<std::size_t>(cells_per_axis * cells_per_axis)>
        cells_{};
    double origin_x_ = 0.0;
    double origin_y_ = 0.0;
    bool indexed_ = false;
};

// One frame of player input. The platform adapter resolves action keys from config.qvm;
// mouse look currently follows the retail 0.0035-radian-per-count rate.
struct PlayerInputFrame
{
    bool move_forward = false;
    bool move_backward = false;
    bool strafe_left = false;
    bool strafe_right = false;
    bool jump = false;
    bool crouch = false;
    bool walk_toggle = false;
    float look_delta_x = 0.0F;
    float look_delta_y = 0.0F;
};

// Host player controller implementing the retail human movement model.
class PlayerController
{
  public:
    // HumanPlayerParameters units (retail VA 0x00410070 converts these):
    //   units per metre 4096 (dbl 0x00533498), ticks per second 30 (dbl 0x00533488),
    //   km/h -> units/tick factor 4096000 / 108000 = 37.925926.
    static constexpr double units_per_metre = 4096.0;
    static constexpr double ticks_per_second = 30.0;
    static constexpr double units_per_tick_per_kmh = 4096000.0 / 108000.0;

    // Airborne integrator (retail VA 0x00408460): gravity 84.741692 units/tick^2 with no
    // damping and no terminal velocity.
    static constexpr double gravity_per_tick_squared = 84.741692;

    // The movement values come from humanplayer.qvm DefineHumanPlayerGeneral. The host
    // decodes them with config::decode_human_player_general and installs them before the
    // first update; the controller compiles no speed of its own.
    struct HumanPlayerParameters
    {
        double root_motion_scale = 0.0;   // argument 0, dimensionless
        double jump_horizontal_kmh = 0.0; // argument 1
        double jump_vertical_kmh = 0.0;   // argument 2
        double air_control_kmh = 0.0;     // argument 3
    };

    // HOST HELPER: installs the decoded humanplayer.qvm parameters. update() refuses to
    // run before this call, so a missing parameter file cannot leave a default speed.
    void set_parameters(const HumanPlayerParameters& parameters) noexcept;
    [[nodiscard]] const HumanPlayerParameters& parameters() const noexcept {
        return parameters_;
    }

    // HOST HELPER: installs the retail animation clips for the player's skeleton
    // (COMMON/ANIMS/000.IFF, the 000_01_1 model rig). Ground locomotion samples the
    // retail root translation track; update() refuses to walk when the clips were not
    // installed or the selected state has no root track.
    void set_locomotion_clips(std::vector<AnimationClip> clips);

    // Retail animation clock (0x004D2D30): the retail divides 4800.0 ticks by the
    // frame rate, i.e. 160 ms of clip time per 30 Hz tick.
    static constexpr double animation_step_ms = 4800.0 / 30.0;

    // Ground query (retail VA 0x00462390): step-down budgets 0 airborne / 2048 grounded /
    // 8192 sliding; probe origin 1638.4 standing, 819.2 crouched; roof clearance 7372.8.
    static constexpr double airborne_step_down = 0.0;
    static constexpr double grounded_step_down = 2048.0;
    static constexpr double sliding_step_down = 8192.0;
    static constexpr double standing_probe_origin = 1638.4;
    static constexpr double crouched_probe_origin = 819.2;
    static constexpr double roof_clearance = 7372.8;

    // Slope slide (retail VA 0x004614B0): four bands, the two steep ones replace the
    // accumulator, the two shallow ones add to it, and the accumulator damps by 0.9 a tick.
    static constexpr double slide_very_steep_degrees = 80.0;
    static constexpr double slide_steep_degrees = 60.0;
    static constexpr double slide_moderate_degrees = 50.0;
    static constexpr double slide_shallow_degrees = 45.0;
    static constexpr double slide_very_steep_per_tick = 1137.0;
    static constexpr double slide_steep_per_tick = 948.0;
    static constexpr double slide_moderate_per_tick = 56.888889;
    static constexpr double slide_shallow_per_tick = 18.962963;
    static constexpr double slide_damping = 0.89999998;

    // Retail look rate (HumanView), the value the look was hardcoded to.
    static constexpr float look_radians_per_count = 0.0035F;

    // Ground locomotion requires authored animation root-motion samples. No fallback
    // walking/running/crouching speed is defined; requesting it without those samples fails.

    // HOST HELPER: places the human at a feet position (retail units) and level reference.
    void reset(const std::array<double, 3>& feet_position, double yaw, double pitch,
               const std::array<double, 3>& level_origin, double world_scale);

    // HOST HELPER: advances the human by wall-clock seconds in fixed 30 Hz retail ticks.
    // Unsupported ground locomotion throws; reset/discard the controller after that
    // failure because input/timing state may already have advanced.
    void update(const PlayerInputFrame& input, double delta_seconds,
                const TerrainCollisionGrid& ground);

    // HOST HELPER: existing look integration, independent of movement availability.
    void update_view(const PlayerInputFrame& input);

    // HOST HELPER: first-person eye through the transcribed retail native (0x00465820).
    [[nodiscard]] std::array<double, 3> eye_position() const;
    [[nodiscard]] const std::array<double, 3>& feet_position() const noexcept { return position_; }
    [[nodiscard]] const std::array<double, 3>& velocity() const noexcept { return velocity_; }
    [[nodiscard]] double yaw() const noexcept { return yaw_; }
    [[nodiscard]] double pitch() const noexcept { return pitch_; }
    [[nodiscard]] bool on_ground() const noexcept { return on_ground_; }
    [[nodiscard]] bool crouched() const noexcept { return crouched_; }
    [[nodiscard]] bool run_toggled() const noexcept { return run_toggled_; }
    [[nodiscard]] bool sliding() const noexcept { return sliding_; }
    [[nodiscard]] double ground_height() const noexcept { return ground_height_; }

  private:
    void tick_step(const PlayerInputFrame& input, bool jump_pressed,
                   const TerrainCollisionGrid& ground);
    void apply_slope_slide(const std::array<float, 3>& normal);
    [[nodiscard]] int select_locomotion_state(const PlayerInputFrame& input) const noexcept;
    [[nodiscard]] const AnimationClip* find_locomotion_clip(int state) const noexcept;
    [[nodiscard]] std::array<double, 3> advance_root_motion(const AnimationClip& clip);

    std::array<double, 3> position_{};   // feet, retail units
    std::array<double, 3> velocity_{};   // retail units per tick
    std::array<double, 3> slide_{};      // slope-slide accumulator, retail units per tick
    std::array<double, 3> level_origin_{};
    double world_scale_ = 1.0 / units_per_metre;
    double yaw_ = 0.0;
    double pitch_ = 0.0;
    double ground_height_ = 0.0;
    double tick_accumulator_ = 0.0;
    bool ground_found_ = false;
    bool on_ground_ = true;
    bool crouched_ = false;
    bool run_toggled_ = true;   // retail starts in run; LSHIFT toggles to walk
    bool sliding_ = false;
    bool jump_held_ = false;
    bool crouch_held_ = false;
    bool walk_toggle_held_ = false;
    HumanPlayerParameters parameters_{};
    bool parameters_set_ = false;
    std::vector<AnimationClip> locomotion_clips_;
    int active_locomotion_state_ = -1;
    double animation_time_ms_ = 0.0;
};

} // namespace igi::host
