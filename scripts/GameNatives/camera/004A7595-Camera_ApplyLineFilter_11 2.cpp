// IGI native provenance: Camera_ApplyLineFilter_11 - 0x004A7595 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A7595-Camera_ApplyLineFilter_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_ApplyLineFilter_11.h"
char* Camera_ApplyLineFilter_11(double* context, char* arg2, int arg3, int arg4, int arg5)

{
    if (arg3 == 0x65 || arg3 == 0x45)
        return Camera_E000(context, arg2, arg4, arg5);

    if (arg3 != 0x66)
        return Camera_ResetMatrix_10(context, arg2, arg4, arg5);

    return Camera_UpdateViewCone_10(context, arg2, arg4);
}
