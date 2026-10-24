// IGI native provenance: Camera_ApplyLineFilter_29 - 0x004ADB05 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004ADB05-Camera_ApplyLineFilter_29.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004ADB05-Camera_ApplyLineFilter_29.hpp"
int* Camera_ApplyLineFilter_29(int* context)

{
    int temp_val = context[2];
    int entity_val = context[1];
    context[1] = entity_val >> 1 | temp_val << 0x1f;
    int entity_5 = *context >> 1 | entity_val << 0x1f;
    context[2] = temp_val >> 1;
    *context = entity_5;
    return context;
}
