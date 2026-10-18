// IGI native provenance: MathSys_Invoke12 - 0x004A7FA0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A7FA0-MathSys_Invoke12.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "MathSys_Invoke12.h"

🌐  int __stdcall __allmul(int arg1, int arg2, int arg3, int arg4) __pure

{
    if (!(arg4 | arg2))
        return arg1 * arg3;

    int result;
    int data_val;
    data_val = HIGHD(arg1 * arg3);
    result = LOWD(arg1 * arg3);
    return result;
}
