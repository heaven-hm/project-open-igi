// IGI native provenance: AnimScaleTask_Setup - 0x0052AF10 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0052AF10-AnimScaleTask_Setup.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0052AF10-AnimScaleTask_Setup.hpp"
char AnimScaleTask_Setup(long double context , int arg2)

{
    int context_val = 0;
    short idx;

    if (Script_FormatString_6(arg2 + 0x200f8))
    {
        Script_SpawnNewTask_7(arg2 + 0x200f8, arg2);
        idx = 1;
        *(arg2 + 0x200f4) = context;
    }

    short data_3;
    data_3 = *(arg2 + 0x200f4);

    if (data_3)
    {
🚫🚫        /* unimplemented  {fld fp0, dword [source_val+0x20160]} */
🚫🚫        /* unimplemented  {fadd dword [source_val+0x20164]} */
🚫🚫        *(arg2 + 0x20164) =
⋯        long double f0_1 = 0.100000001f;
        /* unimplemented  {fcomp fp0, dword [&g_5333c8]} f- f0_1 */ - f0_1;
        bool c0_1 = /* bool c0_1 =
            unimplemented  {fcomp fp0, dword [&g_5333c8]} f< f0_1 */ < f0_1;
        bool c2_1 = FCMP_UO(/* bool c2_1 =
            is_unordered.t(unimplemented  {fcomp fp0, dword [&g_5333c8]}, f0_1) */, f0_1);
        bool c3_1 = /* bool c3_1 =
            unimplemented  {fcomp fp0, dword [&g_5333c8]} f== f0_1 */ == f0_1;
        /* unimplemented  {fcomp fp0, dword [&g_5333c8]} */

        if (!(*(&((c0_1 ? 1 : 0) << 8 | (c2_1 ? 1 : 0) << 0xa | (c3_1 ? 1 : 0) << 0xe
            | (idx & 7) << 0xb) + 1) & 1))
        {
            bool c0_2;
            bool c2_2;
            bool c3_2;

            do
            {
🚫🚫                /* unimplemented  {fld fp0, dword [source_val+0x20164]} */
🚫🚫                /* unimplemented  {fsub fp0, dword [&g_5333c8]} */
                context_val += 1;
🚫🚫                *(arg2 + 0x20164) = /* *(arg2 + 0x20164) =
                    unimplemented  {fst dword [source_val+0x20164], fp0} */;
⋯                long double f1_1 = 0.100000001f;
                /* unimplemented  {fcomp fp0, dword [&g_5333c8]} f- f1_1 */ - f1_1;
                c0_2 = /* c0_2 =
                    unimplemented  {fcomp fp0, dword [&g_5333c8]} f< f1_1 */ < f1_1;
                c2_2 = FCMP_UO(/* c2_2 =
                    is_unordered.t(unimplemented  {fcomp fp0, dword [&g_5333c8]}, f1_1) */,
                    f1_1);
                c3_2 = /* c3_2 =
                    unimplemented  {fcomp fp0, dword [&g_5333c8]} f== f1_1 */ == f1_1;
                /* unimplemented  {fcomp fp0, dword [&g_5333c8]} */
            } while (!(*(&((c0_2 ? 1 : 0) << 8 | (c2_2 ? 1 : 0) << 0xa | (c3_2 ? 1 : 0) << 0xe
                | (idx & 7) << 0xb) + 1) & 1));
        }

        data_3 = Script_FormatString_14(arg2, *(arg2 + 0x20154) + context_val);
    }

    return data_3;
}
