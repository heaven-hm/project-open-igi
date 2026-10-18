// IGI native provenance: Camera_ApplyWideScreen_33 - 0x004AF990 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004AF990-Camera_ApplyWideScreen_33.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004AF990-Camera_ApplyWideScreen_33.hpp"
void* Camera_ApplyWideScreen_33(void* context, void** arg2)

{
    **(context + 4) = arg2;
    arg2[1] = *(context + 4);
    *(context + 4) = arg2;
    *arg2 = context;
    return context;
}
