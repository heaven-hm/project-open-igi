// IGI native provenance: AI_AlertAllies_19 - 0x00463220 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00463220-AI_AlertAllies_19.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_AlertAllies_19.hpp"
int AI_AlertAllies_19(void* context, float arg2)

{
    *(context + 0x50c) = arg2;
    *(context + 0x508) = arg2;
    Sys_DispatchOpcode_12(context + 0x78, arg2);
    return Render_DrawBoneMesh_7(context + 0x78, context + 0x68);
}
