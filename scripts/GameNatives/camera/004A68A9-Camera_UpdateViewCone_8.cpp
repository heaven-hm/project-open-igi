// IGI native provenance: Camera_UpdateViewCone_8 - 0x004A68A9 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A68A9-Camera_UpdateViewCone_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A68A9-Camera_UpdateViewCone_8.hpp"
void* __stdcall Camera_UpdateViewCone_8(int* context)

{
    *context;
    return Camera_SetFieldOfView_7(context[6], context[7]);
}
