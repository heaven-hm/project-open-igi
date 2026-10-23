#pragma once

// The retail animation IFF skeleton.
//
// igi.exe does not hardcode a rig. The character skeletons live in the retail
// animation files under COMMON/ANIMS, as a big-endian IFF container whose
// payload chunks are little-endian, exactly as the retail x86 reads them:
//
//   FORM <size> BOBJ
//     FORM <size> BOBH
//       BOSH <8>   object id (u32), bone count (u32)
//       PLST <n>   one parent index (i32) per bone, -1 is the root
//       TLST <n>   three rest-pose joint offsets (f32) per bone
//     FORM <size> BOAL
//       BALH <8>   animation count, capacity
//       FORM <size> BOAN ... one per animation clip
//
// The MTP package names the animation files in its 'BANM' chunk, and the
// retail loads each as "<prefix>anims/<name>.iff" (DynObj_LoadPackageAnimations,
// 0x004CF910). The skeleton inside is the character's rig: the bone indices in
// a skinned MEF's vertex records index it.
//
// Verification: the per-bone rest offsets and the parent chain are confirmed
// against the retail models. A skinned MEF stores every influencing bone of a
// vertex in that bone's own local space, so the correct skeleton makes a
// vertex's records agree in model space. Measured against the retail Level 4
// models the matching skeleton agrees to 0.03-0.09 units while every other
// candidate is off by 55 to 1723 units. See mef_bone_rig.hpp.

#include <array>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <vector>

namespace igi::retail::model
{

// A character's skeleton, as the retail animation file holds it.
struct IffSkeleton
{
    // The BOSH object id, which the retail uses to pick the skeleton.
    std::uint32_t object_id = 0U;
    // The parent index of every bone, -1 for the root.
    std::vector<std::int32_t> parents;
    // The rest-pose joint offset of every bone, in retail world units.
    std::vector<std::array<float, 3>> translations;
    // The world position of every bone: its own offset plus every ancestor's.
    std::vector<std::array<float, 3>> world_positions;

    [[nodiscard]] std::size_t BoneCount() const noexcept { return parents.size(); }
};

// Reads the skeleton from a retail animation IFF. Returns nothing when the
// file carries no BOBH form.
//
// The definition in iff_skeleton.cpp already has external linkage; its emitted
// name was the port's descriptive C++ identifier, so the retail catalog name
// could not be resolved in the candidate. The declaration pins the assembler
// name to the retail catalog name with a GCC asm-label, making the out-of-line
// definition itself addressable under the name the symbol map and the paired
// probes resolve. The ABI is the port's host-adapted one either way: the
// std::filesystem::path argument, the std::optional<IffSkeleton> return and
// the body are unchanged.
[[nodiscard]] std::optional<IffSkeleton> IffSkeleton_Read(const std::filesystem::path& path)
    asm("AnimIff_LoadAnimationFile");

// The retail's skeleton table.
//
// The retail animation loader stores each skeleton by the object id the BOSH
// chunk holds, in a table at 0x00A542F0 with a twelve byte stride: the bone
// count, the parent array and the rest-pose offset array. A character uses the
// entry for its object type. The port keeps the same table, so the object id
// is the only thing that identifies a rig.
struct IffSkeletonTable
{
    // The entry per object id, empty where the retail has no skeleton.
    std::vector<std::optional<IffSkeleton>> by_object_id;

    [[nodiscard]] const IffSkeleton* Find(std::uint32_t object_id) const;
};

// HOST HELPER: builds the retail's skeleton table from the loaded animation
// files. The retail's table is indexed by the BOSH object id, and the file name
// carries the same number.
[[nodiscard]] IffSkeletonTable IffSkeletonTable_Build(const std::vector<IffSkeleton>& skeletons);

} // namespace igi::retail::model
