// IGI native provenance: Camera_UpdateViewCone_25 - 0x004ACB18 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004ACB18-Camera_UpdateViewCone_25.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004ACB18-Camera_UpdateViewCone_25.hpp"
int Camera_UpdateViewCone_25(int* context, int arg2)

{
    void item_10;
    Camera_ApplyHandiCam_29(&item_10, &arg2, arg2, 0, 0, 0, 0);
    return Camera_ApplyWideScreen_25(&item_10, context);
}
