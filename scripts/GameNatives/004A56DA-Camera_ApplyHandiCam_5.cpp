// IGI native provenance: Camera_ApplyHandiCam_5 - 0x004A56DA - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004A56DA-Camera_ApplyHandiCam_5.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A56DA-Camera_ApplyHandiCam_5.hpp"
int Camera_ApplyHandiCam_5()

{
    short fpu_control;
    long double fpu0;
    long double fpu1;
    /* tailcall */
    return Camera_ApplyLineFilter_14(fpu_control, fpu0, fpu1);
}
