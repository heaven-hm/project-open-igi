#pragma once

// Skinned-model palette evaluation, transcribed from retail igi.exe routine 0x0049B700
// (semantic catalog label unresolved at that VA; the body is fully decoded).
// Artifact: retail igi.EXE SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Radare2 disassembly of retail 0x0049B700 (1086 bytes) — per-bone palette stride
//                 33 floats (bone*32 + bone), position transform rows at +0x24/+0x28/+0x2C,
//                 +0x30/+0x34/+0x38, +0x3C/+0x40/+0x44 applied as M*v, translation at +0x18/+0x1C/+0x20,
//                 the record's +0x20 float kept on the x87 stack and multiplied after the transform,
//                 normals rotated by the same 3x3 without translation or scale, UV dwords copied
//                 verbatim, and the fatal "Bone model too large. Model %s has %d vertices." path at
//                 0x00543D04 through Debug_Error 0x004AF7B0 with the self-loop at 0x0049B72A.
//                 Cross-checked with the C# transcription (open-igi MefSkinner.cs / MefSkinInfluence.cs,
//                 docs/re-notes/animation.md §7-8) and REA evidence ev_7b50c35acc556348073cf6b485533b219fa3e483f5f98028979a4228a713c446.
// Behavior: the skinned vertex stream is base_vertex_count + extra_influence_count 40-byte records
//           ({f32 scale/weight, u16 target, u16 bone} tails read two ways). Base vertices are placed
//           by their single bone — position (R*v + T) * record scale, normal R*n, UVs copied — into
//           destination[vertex_index]. Extra influences add weight * (R*v + T) into their target
//           vertex position only (never the normal, never UVs) and are skipped when the target
//           index leaves the base range. More than 4000 records is the retail fatal error.
// Verification status: pending native-gate record (same tracked-pending tier as the other model
//           loader routines); the identity palette reproduces the authored bind pose. Animated
//           palettes arrive with the animation slice; the palette ring globals (0x00BCAD90/94/98)
//           and the caller-side unit scale stay outside this transcription scope.
// Usage/tests: exercised by the MEF skinner test with identity and rotated palettes; consumed by
//              Display_DecodeMeshRenderData for MefRenderFormat::skinned.

#include "igi/retail/model/mef_model_loader.hpp"
#include "igi/retail/model/mef_render_data.hpp"

#include <array>
#include <span>
#include <vector>

namespace igi::retail::model
{

// One retail palette entry: 33 floats per bone. Only the documented transform window is read by
// the retail skinner: translation at byte +0x18..+0x20 and the row-major 3x3 at +0x24..+0x44
// (rows {+0x24,+0x28,+0x2C}, {+0x30,+0x34,+0x38}, {+0x3C,+0x40,+0x44}, applied as M*v).
struct MefSkinPaletteEntry
{
    std::array<float, 33> floats{};
};

struct MefSkinPalette
{
    std::vector<MefSkinPaletteEntry> bones;
};

// Identity palette (R = I, T = 0) sized for the model's rig; reproduces the authored bind pose.
[[nodiscard]] MefSkinPalette MefSkinPalette_Identity(std::size_t bone_count);

// Port of retail 0x0049B700: evaluates the skinned vertex stream against `palette` and writes the
// base_vertex_count posed vertices (32-byte render vertices) into `destination`.
// Throws MefFormatError with the retail fatal text when the record count exceeds 4000, when the
// stream is short, or when a bone index leaves the palette.
void Display_EvaluateSkinnedPalette(const MefModel& model, const MefSkinPalette& palette,
                                    std::span<MefRenderVertex> destination);

// Highest bone index named by the model's skinned stream, or -1 when the model has no records.
[[nodiscard]] int MefSkinPalette_HighestBone(const MefModel& model);

} // namespace igi::retail::model
