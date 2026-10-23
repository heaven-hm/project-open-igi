#include "../../platform/windows/player_controller.hpp"

#include <cmath>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
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

igi::host::TerrainCollisionGrid flat_ground()
{
    using namespace igi::retail::terrain;
    TerrainDecodedMesh mesh{};
    mesh.vertices = {
        {-64.0F, -64.0F, 0.0F}, {64.0F, -64.0F, 0.0F},
        {64.0F, 64.0F, 0.0F}, {-64.0F, 64.0F, 0.0F},
    };
    mesh.batches.push_back({0U, {0U, 1U, 2U, 0U, 2U, 3U}});
    igi::host::TerrainCollisionGrid ground{};
    ground.rebuild(mesh, 0.0, 0.0);
    return ground;
}

// The shipped humanplayer.qvm DefineHumanPlayerGeneral arguments 0..3. The controller
// reads them from the host in production; the unit test installs the decoded values so
// it exercises the same numbers (docs/fidelity/human_player_movement.md).
igi::host::PlayerController::HumanPlayerParameters shipped_parameters()
{
    return {1.75, 17.5, 27.0, 0.5};
}

void test_default_eye_and_look()
{
    igi::host::PlayerController player{};
    player.reset({0.0, 0.0, 0.0}, 0.0, 0.0, {0.0, 0.0, 0.0}, 1.0 / 4096.0);
    player.set_parameters(shipped_parameters());
    const auto eye = player.eye_position();
    require(std::abs(eye[2] - 2826.24) < 0.001,
            "default HumanPlayer camera must use retail 0.69m standing eye height");

    igi::host::PlayerInputFrame look{};
    look.look_delta_x = 10.0F;
    look.look_delta_y = -20.0F;
    player.update(look, 0.0, flat_ground());
    require(std::abs(player.yaw() - 0.035) < 0.00001, "mouse look uses retail yaw scale");
    require(std::abs(player.pitch() - 0.070) < 0.00001, "mouse look uses retail pitch scale");
}

void test_unavailable_locomotion_does_not_prevent_look()
{
    const auto ground = flat_ground();
    igi::host::PlayerController player{};
    player.reset({0.0, 0.0, 0.0}, 0.0, 0.0, {0.0, 0.0, 0.0}, 1.0 / 4096.0);
    player.set_parameters(shipped_parameters());
    const auto before = player;
    igi::host::PlayerInputFrame forward{};
    forward.move_forward = true;
    bool rejected = false;
    try { player.update(forward, 1.0 / 30.0, ground); }
    catch (const std::logic_error&) { rejected = true; }
    require(rejected, "unavailable root motion must not substitute a jump-derived walking speed");
    player = before;
    igi::host::PlayerInputFrame look{};
    look.look_delta_x = 10.0F;
    look.look_delta_y = -10.0F;
    player.update_view(look);
    require(player.yaw() != before.yaw() && player.pitch() != before.pitch(),
            "look remains usable independently of unavailable ground locomotion");
    require(player.feet_position() == before.feet_position(), "look must not invent player movement");
}

void test_jump_and_land()
{
    const auto ground = flat_ground();
    igi::host::PlayerController player{};
    player.reset({0.0, 0.0, 0.0}, 0.0, 0.0, {0.0, 0.0, 0.0}, 1.0 / 4096.0);
    player.set_parameters(shipped_parameters());

    igi::host::PlayerInputFrame jump{};
    jump.jump = true;
    player.update(jump, 1.0 / 30.0, ground);
    require(!player.on_ground() && player.velocity()[2] > 0.0,
            "jump action starts the vertical impulse");
    require(std::abs(player.velocity()[2] - 1024.0) < 1e-6,
            "jump vertical speed is the shipped 27 km/h");
    igi::host::PlayerInputFrame released{};
    for (int tick = 0; tick < 40; ++tick)
    {
        player.update(released, 1.0 / 30.0, ground);
    }
    require(player.on_ground(), "player lands back on rendered flat terrain");
    require(std::abs(player.feet_position()[2]) < 0.001, "landing snaps feet to terrain height");
}

void test_update_requires_parameters()
{
    igi::host::PlayerController player{};
    player.reset({0.0, 0.0, 0.0}, 0.0, 0.0, {0.0, 0.0, 0.0}, 1.0 / 4096.0);
    bool rejected = false;
    try { player.update(igi::host::PlayerInputFrame{}, 1.0 / 30.0, flat_ground()); }
    catch (const std::logic_error&) { rejected = true; }
    require(rejected, "update refuses to run before humanplayer.qvm parameters are installed");
}

igi::host::AnimationClip synthetic_forward_clip()
{
    igi::host::AnimationClip clip{};
    clip.id = 4;
    clip.duration_ms = 320.0;
    clip.root_keys.push_back({0.0, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}});
    clip.root_keys.push_back({320.0, {0.0, 100.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}});
    return clip;
}

void test_root_motion_walks_from_clip()
{
    const auto ground = flat_ground();
    igi::host::PlayerController player{};
    player.reset({0.0, 0.0, 0.0}, 0.0, 0.0, {0.0, 0.0, 0.0}, 1.0 / 4096.0);
    player.set_parameters(shipped_parameters());
    std::vector<igi::host::AnimationClip> clips;
    clips.push_back(synthetic_forward_clip());
    player.set_locomotion_clips(std::move(clips));

    igi::host::PlayerInputFrame forward{};
    forward.move_forward = true;
    player.update(forward, 1.0 / 30.0, ground);
    // 160 ms of a 0..320 ms clip with zero tangents is half of the 100-unit track;
    // the retail root-motion scale is 1.75.
    require(std::abs(player.feet_position()[1] - 87.5) < 1e-6,
            "ground movement follows the retail clip root track");
    require(player.on_ground(), "clip-driven movement stays on flat ground");

    player.update(forward, 1.0 / 30.0, ground);
    require(std::abs(player.feet_position()[1] - 175.0) < 1e-6,
            "the next tick advances another 160 ms of the same clip");

    // Yaw pi/2 maps the clip's local +Y (forward) onto world +X.
    igi::host::PlayerController rotated{};
    rotated.reset({0.0, 0.0, 0.0}, 1.5707963267948966, 0.0, {0.0, 0.0, 0.0}, 1.0 / 4096.0);
    rotated.set_parameters(shipped_parameters());
    std::vector<igi::host::AnimationClip> rotated_clips;
    rotated_clips.push_back(synthetic_forward_clip());
    rotated.set_locomotion_clips(std::move(rotated_clips));
    rotated.update(forward, 1.0 / 30.0, ground);
    require(std::abs(rotated.feet_position()[0] - 87.5) < 1e-6 &&
                std::abs(rotated.feet_position()[1]) < 1e-6,
            "root motion is rotated by the human yaw");
}

void write_tag(std::vector<std::uint8_t>& bytes, const char* tag)
{
    for (int index = 0; index < 4; ++index)
    {
        bytes.push_back(static_cast<std::uint8_t>(tag[index]));
    }
}

void write_be32(std::vector<std::uint8_t>& bytes, std::uint32_t value)
{
    bytes.push_back(static_cast<std::uint8_t>(value >> 24U));
    bytes.push_back(static_cast<std::uint8_t>(value >> 16U));
    bytes.push_back(static_cast<std::uint8_t>(value >> 8U));
    bytes.push_back(static_cast<std::uint8_t>(value));
}

void write_le32(std::vector<std::uint8_t>& bytes, std::uint32_t value)
{
    bytes.push_back(static_cast<std::uint8_t>(value));
    bytes.push_back(static_cast<std::uint8_t>(value >> 8U));
    bytes.push_back(static_cast<std::uint8_t>(value >> 16U));
    bytes.push_back(static_cast<std::uint8_t>(value >> 24U));
}

void write_float(std::vector<std::uint8_t>& bytes, float value)
{
    std::uint32_t raw = 0;
    std::memcpy(&raw, &value, sizeof(raw));
    write_le32(bytes, raw);
}

void write_root_key(std::vector<std::uint8_t>& bytes, float x, float y, float z, float time)
{
    write_float(bytes, x);
    write_float(bytes, y);
    write_float(bytes, z);
    write_float(bytes, time);
    for (int tangent = 0; tangent < 6; ++tangent)
    {
        write_float(bytes, 0.0F);
    }
}

void test_animation_clip_reader()
{
    std::vector<std::uint8_t> bytes;
    write_tag(bytes, "BOAH");
    write_be32(bytes, 12U);
    write_float(bytes, 320.0F);
    write_le32(bytes, 0U); // reserved u16 + flags u16
    write_le32(bytes, 4U); // animation id
    write_tag(bytes, "BOTH");
    write_be32(bytes, 4U);
    write_le32(bytes, 2U);
    write_tag(bytes, "BOTD");
    write_be32(bytes, 80U);
    write_root_key(bytes, 0.0F, 0.0F, 0.0F, 0.0F);
    write_root_key(bytes, 0.0F, 100.0F, 0.0F, 320.0F);

    const auto path =
        std::filesystem::temp_directory_path() / "open-igi-animation-clip-test.iff";
    {
        std::ofstream output(path, std::ios::binary);
        output.write(reinterpret_cast<const char*>(bytes.data()),
                     static_cast<std::streamsize>(bytes.size()));
    }

    const auto clips = igi::host::read_animation_clips(path);
    std::filesystem::remove(path);
    require(clips.size() == 1U, "clip reader finds the BOAN clip");
    require(clips.front().id == 4U, "clip reader reads the animation id");
    require(std::abs(clips.front().duration_ms - 320.0) < 1e-6,
            "clip reader reads the BOAH duration");
    require(clips.front().root_keys.size() == 2U, "clip reader reads both root keys");
    const auto midpoint = clips.front().sample_root(160.0);
    require(std::abs(midpoint[1] - 50.0) < 1e-6, "root sampling interpolates the clip track");
    const auto end = clips.front().sample_root(320.0);
    require(std::abs(end[1] - 100.0) < 1e-6, "root sampling clamps at the clip end");
}

} // namespace

int main()
{
    try
    {
        test_default_eye_and_look();
        test_unavailable_locomotion_does_not_prevent_look();
        test_jump_and_land();
        test_update_requires_parameters();
        test_root_motion_walks_from_clip();
        test_animation_clip_reader();
        std::cout << "Player controller scene-regression tests passed.\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << "Player controller test error: " << error.what() << '\n';
        return 1;
    }
}
