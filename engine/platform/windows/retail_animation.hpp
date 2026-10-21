#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <vector>

// Host reader for the retail animation IFF clips.
//
// The retail animation file is an IFF container whose BOAL/BOAN forms hold one
// clip per BOAN.  The FORM sizes in these files are not trustworthy; the retail
// walks the chunk tree by tag, so this reader scans by tag as well.
//
// Retail evidence for the layout (igi.exe SHA256
// dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01):
//   0x00489E70 Human_StateAnimation indexes the human animation table at
//              0x00538870 (entries {animation_id, flags}; written at 0x004104B7).
//   0x004D61D0 selects the clip and schedules it on the animation track; its
//              0x4D2D30 setter computes the per-tick clip advance as
//              4800.0 / ticks_per_second = 160 ms at the retail 30 Hz tick.
//   0x004D2FB0 / 0x004D3020 sample the root translation track; 0x004109A0
//              multiplies that sample by the humanplayer.qvm root-motion scale
//              when +0x52C is clear, rotates it by the body matrix and hands it
//              to 0x004614B0, which adds it to the per-tick velocity.
//
// The clip data layout is documented by the IGI converter
// (docs/game_file_formats.md sections 11.7-11.9): BOAH is a 12-byte
// {duration_ms, reserved, flags, animation_id}; BOTH holds a little-endian
// key count; each BOTD key is 40 bytes of {x,y,z,time,tangent_in[3],
// tangent_out[3]} in IGI world units and milliseconds.
namespace igi::host
{

struct AnimationRootKey
{
    double time_ms = 0.0;
    std::array<double, 3> position{};
    std::array<double, 3> tangent_in{};
    std::array<double, 3> tangent_out{};
};

struct AnimationClip
{
    std::uint32_t id = 0;
    double duration_ms = 0.0;
    // Root translation track in parent/local space, ordered by time.
    std::vector<AnimationRootKey> root_keys;

    // Linear interpolation between the two surrounding keys.  The retail
    // stores incoming/outgoing spline tangents per key (units per millisecond),
    // but their sign/order convention has not been established from the binary,
    // so the host samples the key positions.  The displacement over a full loop
    // (and therefore the ground speed) is independent of the interpolation;
    // only the sub-key profile differs.
    [[nodiscard]] std::array<double, 3> sample_root(double time_ms) const;
};

// HOST HELPER: reads every BOAN clip in an animation IFF.  Throws
// std::runtime_error when the file cannot be read or the clip data is
// malformed.  A clip with no BOTH/BOTD root track is still returned; its
// root_keys vector is empty.
[[nodiscard]] std::vector<AnimationClip> read_animation_clips(const std::filesystem::path& path);

// HOST HELPER: the retail human state table at 0x00538870, entries 0..16.
// The first entry is idle; 1..8 are the eight standing directions and 9..16
// their crouched counterparts.  The table itself is static retail data.
inline constexpr std::array<std::uint32_t, 17> human_locomotion_animation_ids{
    2, 4, 37, 35, 64, 70, 68, 11, 10, 57, 74, 72, 75, 77, 76, 81, 80};

} // namespace igi::host
