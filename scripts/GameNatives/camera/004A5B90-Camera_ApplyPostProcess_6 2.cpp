// IGI native provenance: Camera_ApplyPostProcess_6 - 0x004A5B90 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A5B90-Camera_ApplyPostProcess_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_ApplyPostProcess_6.h"
void Camera_ApplyPostProcess_6(int* context, int arg2)

{
    for (int* i = context; i < arg2; i = &i[1])
    {
        int data_val = *i;

        if (data_val)
            data_val();
    }
}
