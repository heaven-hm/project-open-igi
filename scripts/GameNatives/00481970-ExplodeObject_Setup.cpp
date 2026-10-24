// IGI native provenance: ExplodeObject_Setup - 0x00481970 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00481970-ExplodeObject_Setup.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00481970-ExplodeObject_Setup.hpp"
void* ExplodeObject_Setup(long double context , double* arg2)

{
    int* i = arg2[1];
    void* result;

    if (*i && i)
    {
        int temp_1 = g_afa7e0;

        do
        {
            int* source_1 = *i;
            void* data_3;

            if (!source_1)
                data_3 = nullptr;
            else
            {
                int data_val = *source_1;
                int data_1 = -(data_val);
                data_3 = (data_1 - data_1) & source_1;
            }

            *((temp_1 << 2) + &g_afa6e0) = data_3;
            int temp_2 = temp_1 + 1;
            int data_4 = 0;
            g_afa7e0 = temp_2;
            data_4 = i[7];
            result = *((data_4 << 2) + &g_a970e0);

            if (result)
            {
                result = result(i);
                temp_2 = g_afa7e0;
            }

            i = *((temp_2 << 2) + &g_afa6dc);
            temp_1 = temp_2 - 1;
            g_afa7e0 = temp_1;
        } while (i);
    }

    result = arg2[0x32];

    if (!result)
    {
        if (Script_FormatString_6(arg2 + 0x194))
        {
            Script_SpawnNewTask_7(arg2 + 0x194, arg2);
            *(arg2 + 0x1e4) = context;
        }

        if (*(arg2 + 0x1e4) || !*(arg2 + 0x1f4))
        {
            FX_1009011(arg2);
            result = DynObj_Missing(&arg2[0x30]);

            if (result)
                goto label_481a3c;
        }
        else
        {
        label_481a3c:
            result = *(arg2 + 0x1f4);

            if (result > 0)
            {
                result -= 1;
                *(arg2 + 0x1f4) = result;
            }
        }
    }

    return result;
}
