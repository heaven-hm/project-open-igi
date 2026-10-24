// IGI native provenance: MathSys_Invoke16_4A947C - 0x004A947C - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A947C-MathSys_Invoke16_4A947C.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A947C-MathSys_Invoke16_4A947C.hpp"
int MathSys_Invoke16_4A947C(long double arg1, long double arg2) __location("st0")

{
    short x87control;
    return Camera_ApplyLineFilter_16(x87control, arg1, arg2);
}
