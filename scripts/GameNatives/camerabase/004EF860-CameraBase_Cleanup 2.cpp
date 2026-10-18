// IGI native provenance: CameraBase_Cleanup - 0x004EF860 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004EF860-CameraBase_Cleanup.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "CameraBase_Cleanup.h"
int CameraBase_Cleanup(float* context, int* arg2)

{
    float* dest_val = context;
    Script_TooFewParametersInLi(arg2, 0xb);
    int flagP = *arg2;

    for (int i = 0; i < 0xb; i += 1)
    {
        Profile_ValueSet(arg2[1], i + flagP);
🚫🚫🚫        *dest_val =
        /* unimplemented  {fstp dword [dest_val], fp0} */
        short idx = idx + 1;
        dest_val = &dest_val[1];
    }

    int result = *arg2 + 0xb;
    *arg2 = result;
    return result;
}
