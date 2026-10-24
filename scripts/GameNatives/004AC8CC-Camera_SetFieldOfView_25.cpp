// IGI native provenance: Camera_SetFieldOfView_25 - 0x004AC8CC - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004AC8CC-Camera_SetFieldOfView_25.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004AC8CC-Camera_SetFieldOfView_25.hpp"
int Camera_SetFieldOfView_25(int* context)

{
    *context = 0;
    int* dest_1 = &context[1];
    *dest_1 = 0;
    dest_1[1] = 0;
    return 0;
}
