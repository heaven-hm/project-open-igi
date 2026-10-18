// IGI native provenance: Camera_SetFieldOfView_29 - 0x004ADAD7 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004ADAD7-Camera_SetFieldOfView_29.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_SetFieldOfView_29.h"
int* Camera_SetFieldOfView_29(int* context)

{
    int source_val = *context;
    int dest_val = context[1];
    *context = source_val * 2;
    int entity_2 = context[2];
    context[1] = (dest_val * 2) | source_val >> 0x1f;
    context[2] = entity_2 << 1 | dest_val >> 0x1f;
    return context;
}
