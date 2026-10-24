// IGI native provenance: BoneDynCubeObj_Update - 0x004D94F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D94F0-BoneDynCubeObj_Update.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "BoneDynCubeObj_Update.h"
void BoneDynCubeObj_Update(int* context, int* arg2)

{
    if (!*arg2)
        DynObj_LinkDynamicCube_8(arg2[1], context, &context[8], context[0x28], arg2[2]);

    int* i = context[2];

    if (!*i || !i)
        return;

    int entity_2 = g_afa7e0;

    do
    {
        int* temp_2 = *i;
        void* data_6;

        if (!temp_2)
            data_6 = nullptr;
        else
        {
            int data_3 = *temp_2;
            int data_4 = -(data_3);
            data_6 = (data_4 - data_4) & temp_2;
        }

        *((entity_2 << 2) + &g_afa6e0) = data_6;
        g_afa7e0 = entity_2 + 1;
        unsigned int data_8 = DynObj_UpdateDynTree_12();
        int temp_3 = 0;
        temp_3 = i[7];
        int data_val = *(((data_8 * 0x180 + temp_3) << 2) + &g_a96ae0);

        if (data_val)
            data_val(i, arg2);

        entity_2 = g_afa7e0 - 1;
        g_afa7e0 = entity_2;
        i = *((entity_2 << 2) + &g_afa6e0);
    } while (i);
}
