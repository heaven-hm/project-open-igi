// IGI native provenance: Render_SkinBones_5 - 0x004D2F80 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004D2F80-Render_SkinBones_5.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D2F80-Render_SkinBones_5.hpp"
void* Render_SkinBones_5(void* context, void* arg2)

{
    memset(context, 0, 0x84);
    *(context + 0x78) = 0;
    *(context + 0x74) = arg2;
    *(context + 0x7c) = *(arg2 + 0x78);
    *(arg2 + 0x78) = context;
    return arg2;
}
