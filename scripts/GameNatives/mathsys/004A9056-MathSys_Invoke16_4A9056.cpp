// IGI native provenance: MathSys_Invoke16_4A9056 - 0x004A9056 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A9056-MathSys_Invoke16_4A9056.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A9056-MathSys_Invoke16_4A9056.hpp"
int MathSys_Invoke16_4A9056(long double arg1, int80_t arg2)

{
    return_address = arg2;
    int result;
    short x87control;
    int80_t st0;
    st0 = Camera_ApplyLineFilter_16(x87control, return_address, arg1);
    return result;
}
