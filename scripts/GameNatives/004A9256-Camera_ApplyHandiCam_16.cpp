// IGI native provenance: Camera_ApplyHandiCam_16 - 0x004A9256 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A9256-Camera_ApplyHandiCam_16.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A9256-Camera_ApplyHandiCam_16.hpp"
int Camera_ApplyHandiCam_16(long double context , long double arg2 ) {
    short fpu_control;
    return Camera_ApplyLineFilter_16(fpu_control, context, arg2);
}
