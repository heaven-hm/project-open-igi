// IGI native provenance: Camera_ApplyPostProcess_5 - 0x004A5760 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A5760-Camera_ApplyPostProcess_5.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A5760-Camera_ApplyPostProcess_5.hpp"
int Camera_ApplyPostProcess_5(long double context , long double arg2 ) {
    double item_8 = context;
    short fpu_control;
    short x87tag;
    return Camera_UpdateViewCone_5(*(&item_8 + 4), fpu_control, x87tag, context, arg2, item_8);
}
