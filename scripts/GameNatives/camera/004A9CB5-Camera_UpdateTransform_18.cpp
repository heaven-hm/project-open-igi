// IGI native provenance: Camera_UpdateTransform_18 - 0x004A9CB5 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A9CB5-Camera_UpdateTransform_18.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A9CB5-Camera_UpdateTransform_18.hpp"
int Camera_UpdateTransform_18(int context)

{
    *(&context + 2) = (context & 0x300) | 0x7f;
    short fpu_control;
    short x87status;
    fpu_control = __fldcw_memmem16(*(&context + 2));
}
