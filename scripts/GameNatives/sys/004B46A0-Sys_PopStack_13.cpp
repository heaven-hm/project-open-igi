// IGI native provenance: Sys_PopStack_13 - 0x004B46A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B46A0-Sys_PopStack_13.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B46A0-Sys_PopStack_13.hpp"
int Sys_PopStack_13()

{
    int entity_val;
    int item_4_1 = entity_val;
    int i = 0;
    int i_2 = 0;
    short top_1;

    do
    {
        long double fpu_val_7_1 = i_2;
        i += 1;
        i_2 = i;
        long double fpu_val_7_2 = fpu_val_7_1 * 0.00153398083f;
        long double fp0_1;
        bool c1_1;
        bool c2_1;
        fp0_1 = __fsin(fpu_val_7_2);
        *((i << 2) + &g_a88a5c) = fp0_1;
        long double fp0_2;
        bool c1_2;
        bool c2_2;
        fp0_2 = __fcos(fpu_val_7_2);
        *((i << 2) + &g_a84a5c) = fp0_2;
        top_1 = 0;
    } while (i < 0x1000);

    int item_4 = 0;
    int result;

    for (float* i_1 = &g_a8ca60; i_1 < &g_a94a64; )
    {
🚫🚫        /* unimplemented  {fild fp0, dword [stack_ptr+0x4]} */
        int item_c_1 = entity_val;
🚫🚫        /* unimplemented  {fmul fp0, dword [&g_5333e8]} */
🚫🚫        /* unimplemented  {fsub fp0, dword [&g_5333e0]} */
🚫🚫🚫        float item_c_2 = /* float item_c_2 = unimplemented  {fstp dword [stack_ptr], fp0} */;
        /* unimplemented  {fstp dword [stack_ptr], fp0} */
🚫🚫        result = Sys_PushStack_9(item_c_2);
        /* unimplemented  {call Sys_PushStack_9} */
🚫🚫🚫        *i_1 =
        /* unimplemented  {fstp dword [source_val], fp0} */
        top_1 += 1;
        i_1 = &i_1[1];
        item_4 += 1;
    }

    return result;
}
