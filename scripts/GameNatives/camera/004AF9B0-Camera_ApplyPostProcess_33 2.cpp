// IGI native provenance: Camera_ApplyPostProcess_33 - 0x004AF9B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004AF9B0-Camera_ApplyPostProcess_33.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_ApplyPostProcess_33.h"
int Camera_ApplyPostProcess_33(int* context)

{
    int result = 0;
    int* i = **context;

    while (i)
    {
        i = *i;
        result += 1;
    }

    return result;
}
