// IGI native provenance: LevelTimer_Tick - 0x0051BE10 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0051BE10-LevelTimer_Tick.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "LevelTimer_Tick.h"
int LevelTimer_Tick(long double context , void* arg2)

{
    int* i = *(arg2 + 8);

    if (*i && i)
    {
        int temp_1 = g_afa7e0;

        do
        {
            int* source_1 = *i;
            void* data_4;

            if (!source_1)
                data_4 = nullptr;
            else
            {
                int data_1 = *source_1;
                int data_2 = -(data_1);
                data_4 = (data_2 - data_2) & source_1;
            }

            *((temp_1 << 2) + &g_afa6e0) = data_4;
            int temp_2 = temp_1 + 1;
            int data_5 = 0;
            g_afa7e0 = temp_2;
            data_5 = i[7];
            int data_6 = *((data_5 << 2) + &g_a970e0);

            if (data_6)
            {
                data_6(i);
                temp_2 = g_afa7e0;
            }

            i = *((temp_2 << 2) + &g_afa6dc);
            temp_1 = temp_2 - 1;
            g_afa7e0 = temp_1;
        } while (i);
    }

    short idx;

    if (Script_FormatString_6(arg2 + 0x6c))
    {
        Script_SpawnNewTask_7(arg2 + 0x6c, arg2);
        idx = 1;
        *(arg2 + 0x60) = context;
    }

    short result = Script_FormatString_6(arg2 + 0xbc);

    if (result)
    {
        Script_SpawnNewTask_7(arg2 + 0xbc, arg2);
⋯        long double f0_1 = 0.0;
        /* unimplemented  {fcomp fp0, qword [&g_5333b0]} f- f0_1 */ - f0_1;
        bool c0_1 = /* bool c0_1 =
            unimplemented  {fcomp fp0, qword [&g_5333b0]} f< f0_1 */ < f0_1;
        bool c2_1 = FCMP_UO(/* bool c2_1 =
            is_unordered.t(unimplemented  {fcomp fp0, qword [&g_5333b0]}, f0_1) */, f0_1);
        bool c3_1 = /* bool c3_1 =
            unimplemented  {fcomp fp0, qword [&g_5333b0]} f== f0_1 */ == f0_1;
        /* unimplemented  {fcomp fp0, qword [&g_5333b0]} */
        result = (c0_1 ? 1 : 0) << 8 | (c2_1 ? 1 : 0) << 0xa | (c3_1 ? 1 : 0) << 0xe
            | ((idx + 1) & 7) << 0xb;

        if (!(*(&result + 1) & 0x40))
            *(arg2 + 0x64) = 0;
    }

    result = *(arg2 + 0x60);

    if (result)
        *(arg2 + 0x64) += 1;

    return result;
}
