// IGI native provenance: GunM16A2_Init_2 - 0x004732F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004732F0-GunM16A2_Init_2.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "GunM16A2_Init_2.h"

int GunM16A2_Init_2(int self )

{
    int dest_ptr = *(self + 8);
    float* base_ptr = *self;
    int result_val_2;
    int scratch_val;
    scratch_val = HIGHD(Script_GetArg(*(self + 4), dest_ptr));
    result_val_2 = LOWD(Script_GetArg(*(self + 4), dest_ptr));
    int result = (result_val_2 - scratch_val) >> 1;
    long double st0;
    bool c1;
    bool c2;
    st0 = __fptan(result * 0.0174532924f);
🚫🚫🚫    /* unimplemented  {fstp st0, st0} */
    /* unimplemented  {fstp st0, st0} */
🚫🚫🚫    *base_ptr = /* *base_ptr = unimplemented  {fstp dword [base_ptr], st0} */;
    /* unimplemented  {fstp dword [base_ptr], st0} */
    *(self + 8) = dest_ptr + 1;
    return result;
}
