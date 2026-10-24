// IGI native provenance: MathSys_Invoke16_4A8E7E - 0x004A8E7E - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A8E7E-MathSys_Invoke16_4A8E7E.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A8E7E-MathSys_Invoke16_4A8E7E.hpp"
int MathSys_Invoke16_4A8E7E(long double arg1, int80_t arg2) __location("st0")

{
    return_address = arg2;
    short x87control;
    return Camera_ApplyLineFilter_16(x87control, return_address, arg1);
}
