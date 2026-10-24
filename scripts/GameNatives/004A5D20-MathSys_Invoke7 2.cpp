// IGI native provenance: MathSys_Invoke7 - 0x004A5D20 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A5D20-MathSys_Invoke7.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "MathSys_Invoke7.h"

int MathSys_Invoke7(int arg1, int arg2, int* arg3)

{
    if (!(*(arg1 + 4) & 6))
        return 1;

    *arg3 = arg2;
    return 3;
}
