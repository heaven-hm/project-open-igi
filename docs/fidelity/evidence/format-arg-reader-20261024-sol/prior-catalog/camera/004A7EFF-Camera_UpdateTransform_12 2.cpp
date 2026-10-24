// IGI native provenance: Camera_UpdateTransform_12 - 0x004A7EFF - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A7EFF-Camera_UpdateTransform_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_UpdateTransform_12.h"
int Camera_UpdateTransform_12(int* context)

{
    *context += 4;
    return *(*context - 4);
}
