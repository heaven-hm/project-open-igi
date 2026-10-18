// IGI native provenance: AI_SelectTarget_7 - 0x00455BA0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00455BA0-AI_SelectTarget_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00455BA0-AI_SelectTarget_7.hpp"
int AI_SelectTarget_7(void* context, void* arg2)

{
    *(arg2 + 0x33d) = 0;
    *(arg2 + 0x696) = 0;
    Render_TransformVertices_10(arg2 + 0x68, 0.0f);
    int result = Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
    *(context + 0x14) = sub_455be0;
    return result;
}
