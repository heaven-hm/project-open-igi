// IGI native provenance: Camera_ApplyWideScreen_25 - 0x004ACAEC - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004ACAEC-Camera_ApplyWideScreen_25.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_ApplyWideScreen_25.h"
int Camera_ApplyWideScreen_25(short* context, int* arg2)

{
    return Camera_ApplyHandiCam_25(context, arg2, 0x546ac0);
}
