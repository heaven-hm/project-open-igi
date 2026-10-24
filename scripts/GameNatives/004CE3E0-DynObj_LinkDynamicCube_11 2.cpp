// IGI native provenance: DynObj_LinkDynamicCube_11 - 0x004CE3E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004CE3E0-DynObj_LinkDynamicCube_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "DynObj_LinkDynamicCube_11.h"
int DynObj_LinkDynamicCube_11(int context, int arg2, int arg3)

{
    *(context + (arg2 << 2) + 0xd3c) = arg3;
    return arg3;
}
