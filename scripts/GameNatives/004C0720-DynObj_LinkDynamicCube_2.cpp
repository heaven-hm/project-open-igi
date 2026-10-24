// IGI native provenance: DynObj_LinkDynamicCube_2 - 0x004C0720 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004C0720-DynObj_LinkDynamicCube_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004C0720-DynObj_LinkDynamicCube_2.hpp"
int* DynObj_LinkDynamicCube_2(int* context, int arg2, int arg3)

{
    *context = arg2;
    context[1] = arg3;
    context[4] = &context[2];
    context[2] = &context[3];
    context[3] = 0;
    return context;
}
