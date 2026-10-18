// IGI native provenance: DynCubeObj_OnTick - 0x004E5550 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E5550-DynCubeObj_OnTick.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "DynCubeObj_OnTick.h"
int DynCubeObj_OnTick(void* context, int arg2, int arg3)

{
    int result = ((arg3 << (*(context + 4) & 0x1f)) + (arg2 & 0xfffffff8)) >> 3;
    *(result + context + 0xc) |= 1 << (arg2 & 7);
    return result;
}
