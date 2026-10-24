// IGI native provenance: SCamera_Cleanup_5 - 0x00442A30 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 00442A30-SCamera_Cleanup_5.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00442A30-SCamera_Cleanup_5.hpp"
void* SCamera_Cleanup_5(void* context, int* arg2)

{
    *arg2 = Env_InitLadder_3();
    arg2[8] = 1;
    bool context_val = *(context + 0x170);
    *(arg2 + 0x25) = 1;
    arg2[9] = !context_val;
    memcpy(&arg2[2], context + 0x20, 0x18);
    return context;
}
