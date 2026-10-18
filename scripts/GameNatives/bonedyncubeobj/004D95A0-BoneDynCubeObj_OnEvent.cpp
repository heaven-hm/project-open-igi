// IGI native provenance: BoneDynCubeObj_OnEvent - 0x004D95A0 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004D95A0-BoneDynCubeObj_OnEvent.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D95A0-BoneDynCubeObj_OnEvent.hpp"
int BoneDynCubeObj_OnEvent(void* context, double* arg2)

{
    int result = WorldObject_CommitTransform(g_a44344, context, arg2, *(context + 0xa0));
    memcpy(context + 0x78, &arg2[3], 0x28);
    return result;
}
