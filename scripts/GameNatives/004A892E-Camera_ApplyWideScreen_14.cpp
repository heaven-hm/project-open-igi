// IGI native provenance: Camera_ApplyWideScreen_14 - 0x004A892E - CFG score 68.2% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004A892E-Camera_ApplyWideScreen_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A892E-Camera_ApplyWideScreen_14.hpp"
int Camera_ApplyWideScreen_14()

{
    g_9361d8 = 0;
    void* flagP;
    long double fpu0;
    /* tailcall */
    return Camera_ApplyPostProcess_14(flagP, fpu0);
}
