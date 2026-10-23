#pragma once

// The rig for a skinned MEF model.
//
// The retail MEF files carry no skeleton: there is no chunk with bone names,
// no parent-child linkage and no rest-pose offsets. igi.exe gets the rig from
// the character's animation file, where the retail animation IFF holds the
// bone count, the parent chain and the rest-pose joint offsets. The bone
// indices in a skinned MEF's vertex records index that skeleton.
//
// The port therefore selects the skeleton from the retail animation files
// rather than hardcoding one. A skinned MEF stores every influencing bone of a
// vertex in that bone's own local space, so the correct skeleton makes the
// records of one vertex agree in model space. The selection measures that
// agreement.
//
// Measured against the retail Level 4 models, the matching skeleton agrees to
// 0.03-0.09 units while every other candidate is off by 55 to 1723 units, so
// the choice is decided by the model data alone.
//
// This matches the retail, which does not hardcode a rig either: the Level
// Editor's converter hardcodes four archetypes only because it cannot ship
// the retail animation files, and its archetypes are the rest offsets of the
// retail COMMON/ANIMS skeletons.

#include "igi/retail/model/iff_skeleton.hpp"
#include "igi/retail/model/mef_skinner.hpp"

#include <cstdint>
#include <string_view>
#include <vector>

namespace igi::retail::model
{

// The rig a skinned model uses, and how well it fits.
struct MefBoneRig
{
    // The object id of the skeleton the model uses, the retail's key.
    std::uint32_t object_id = 0U;
    // The median distance, in retail units, between a vertex's base record and
    // its extra influence records once both are placed in model space. The
    // matching skeleton scores near zero.
    double disagreement = 0.0;
};

// HOST HELPER: selects the object id of the skeleton that fits a skinned model.
//
// The model must be the retail MefModel of a skinned mesh and the table the
// retail's skeleton table. The object id whose skeleton makes the model's
// influence records agree wins.
[[nodiscard]] MefBoneRig MefBoneRig_Select(const MefModel& model,
                                            const IffSkeletonTable& skeletons);

// HOST HELPER: the rest-pose palette for an object id: the rotation is the
// identity and the translation is the bone's world position, which is the
// transform the retail skinner at 0x0049B700 applies as (R*v + T).
[[nodiscard]] MefSkinPalette MefBoneRig_Palette(const IffSkeletonTable& skeletons,
                                                 std::uint32_t object_id);

} // namespace igi::retail::model
