// IGI native provenance: MathSys_Invoke18 - 0x004A9C05 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A9C05-MathSys_Invoke18.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A9C05-MathSys_Invoke18.hpp"
long double MathSys_Invoke18(short arg1, short arg2, short arg3, long double arg4, long double arg5)

{
    short x87control;
    short x87status;
    short x87tag;
    uint864_t temp0;
    temp0 = __fnsave_memmem108(arg1, arg3, arg2);
    double var_78;
    int result_val;
    char context_val;
    result_val = Camera_ResetMatrix_18(arg5, arg4, &var_78);
    __frstor_memmem108(temp0);

    if (!result_val)
        return var_78;

    void* frame_ptr;
    /* tailcall */
    return Camera_SetFieldOfView_16(context_val, frame_ptr);
}
