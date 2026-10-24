// IGI native provenance: Camera_ResetMatrix_28 - 0x004ADA58 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004ADA58-Camera_ResetMatrix_28.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004ADA58-Camera_ResetMatrix_28.hpp"
int Camera_ResetMatrix_28(int context, int arg2, int* arg3)

{
    int result = 0;
    int entity_val = context + arg2;

    if (entity_val < context || entity_val < arg2)
        result = 1;

    *arg3 = entity_val;
    return result;
}
