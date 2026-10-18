// IGI native provenance: AI_UpdatePatrol_9 - 0x00456770 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00456770-AI_UpdatePatrol_9.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_UpdatePatrol_9.hpp"
int AI_UpdatePatrol_9(void* context, void* arg2)

{
    *(arg2 + 0x33d) = 0;
    *(arg2 + 0x696) = 1;
    int result =
        Bones_PlayAnimation(arg2 + 0x68, 0, *(FX_FragmentSimulate_14(arg2 + 0x254, *(arg2 + 0x2e8)) + 4), 0, 0.0f);
    *(context + 0x14) = sub_4561a0;
    return result;
}
