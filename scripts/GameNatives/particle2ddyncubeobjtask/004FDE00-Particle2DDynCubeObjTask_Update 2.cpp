// IGI native provenance: Particle2DDynCubeObjTask_Update - 0x004FDE00 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004FDE00-Particle2DDynCubeObjTask_Update.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Particle2DDynCubeObjTask_Update.h"
int Particle2DDynCubeObjTask_Update(float context, void* arg2)

{
    int* source_val = context;
    context = Script_ParseEnumReal32_30(source_val);
    double item_18[0x3];
    Script_ParseEnumString_30(&item_18, source_val);
    int result = DynObj_LinkDynamicCube_8(*(arg2 + 4), source_val, &item_18, context, 2);
    int* i = source_val[2];

    if (*i && i)
    {
        int entity_1 = g_afa7e0;

        do
        {
            int* temp_1 = *i;
            void* data_4;

            if (!temp_1)
                data_4 = nullptr;
            else
            {
                int data_1 = *temp_1;
                int data_2 = -(data_1);
                data_4 = (data_2 - data_2) & temp_1;
            }

            *((entity_1 << 2) + &g_afa6e0) = data_4;
            g_afa7e0 = entity_1 + 1;
            unsigned int data_6 = DynObj_UpdateDynTree_12();
            int temp_2 = 0;
            temp_2 = i[7];
            result = *(((data_6 * 0x180 + temp_2) << 2) + &g_a96ae0);

            if (result)
                result = result(i, arg2);

            entity_1 = g_afa7e0 - 1;
            g_afa7e0 = entity_1;
            i = *((entity_1 << 2) + &g_afa6e0);
        } while (i);
    }

    return result;
}
