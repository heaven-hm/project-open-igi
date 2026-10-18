// IGI native provenance: MenuItem_LoadParameters - 0x00505710 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00505710-MenuItem_LoadParameters.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00505710-MenuItem_LoadParameters.hpp"
int MenuItem_LoadParameters(void* context, int arg2)

{
    char data_1 = 1;

    if (!*(context + 0x4d))
        data_1 = *(context + 0x4e);

    int* i = *(context + 8);
    *(context + 0x4e) = data_1;

    if (!*i || !i)
    {
        *(context + 0x4d) = 0;
        return data_1;
    }

    int entity_val = g_afa7e0;
    int data_11;

    do
    {
        int* temp_1 = *i;
        void* data_5;

        if (!temp_1)
            data_5 = nullptr;
        else
        {
            int data_2 = *temp_1;
            int data_3 = -(data_2);
            data_5 = (data_3 - data_3) & temp_1;
        }

        *((entity_val << 2) + &g_afa6e0) = data_5;
        g_afa7e0 = entity_val + 1;
        unsigned int data_7 = Physics_EvaluateAreaCriteria_5();
        int entity_2 = 0;
        entity_2 = i[7];
        data_11 = *(((data_7 * 0x180 + entity_2) << 2) + &g_a96ae0);

        if (data_11)
            data_11 = data_11(i, arg2);

        entity_val = g_afa7e0 - 1;
        g_afa7e0 = entity_val;
        i = *((entity_val << 2) + &g_afa6e0);
    } while (i);

    *(context + 0x4d) = 0;
    return data_11;
}
