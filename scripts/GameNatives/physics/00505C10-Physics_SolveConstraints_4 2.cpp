// IGI native provenance: Physics_SolveConstraints_4 - 0x00505C10 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00505C10-Physics_SolveConstraints_4.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Physics_SolveConstraints_4.h"
void* Physics_SolveConstraints_4(void* context, int arg2, int arg3, int arg4, int arg5, int arg6, char arg7, int arg8)

{
    *(context + 0x38) = arg2;
    *(context + 0x40) = arg4;
    *(context + 0x3c) = arg3;
    *(context + 0x48) = arg6;
    *(context + 0x44) = arg5;
    *(context + 0x4e) = 0;
    *(context + 0x4d) = 0;
    *(context + 0x4c) = arg7;
    *(context + 0x50) = arg8;
    return context;
}
