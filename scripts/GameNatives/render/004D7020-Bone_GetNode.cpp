// IGI native provenance: Bone_GetNode - 0x004D7020 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D7020-Render_SkinBones_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D7020-Bone_GetNode.hpp"
int Bone_GetNode(int context, int arg2)

{
    return **(*(*(context * 0x28 + &g_a538f4) + (arg2 << 2)) + 8);
}
