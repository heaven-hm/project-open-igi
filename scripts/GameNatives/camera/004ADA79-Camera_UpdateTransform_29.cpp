// IGI native provenance: Camera_UpdateTransform_29 - 0x004ADA79 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004ADA79-Camera_UpdateTransform_29.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004ADA79-Camera_UpdateTransform_29.hpp"
int Camera_UpdateTransform_29(int* context, int* arg2)

{
    if (Camera_ResetMatrix_28(*context, *arg2, context) && Camera_ResetMatrix_28(context[1], 1, &context[1]))
        context[2] += 1;

    if (Camera_ResetMatrix_28(context[1], arg2[1], &context[1]))
        context[2] += 1;

    return Camera_ResetMatrix_28(context[2], arg2[2], &context[2]);
}
