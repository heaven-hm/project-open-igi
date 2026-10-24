// IGI native provenance: Camera_ApplyPostProcess_16 - 0x004A9491 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A9491-Camera_ApplyPostProcess_16.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_ApplyPostProcess_16.h"
int Camera_ApplyPostProcess_16(long double context , long double arg2 ) {
    short fpu_control;
    return Camera_ApplyLineFilter_16(fpu_control, arg2, context);
}
