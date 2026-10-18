// IGI native provenance: MathSys_Invoke16_4A9216 - 0x004A9216 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A9216-MathSys_Invoke16_4A9216.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A9216-MathSys_Invoke16_4A9216.hpp"
int MathSys_Invoke16_4A9216(int80_t arg1, long double arg2)

{
    return_address = arg1;
    int result;
    short x87control;
    int80_t st0;
    st0 = Camera_ApplyLineFilter_16(x87control, return_address, arg2);
    return result;
}
