// IGI native provenance: ConditionalContainer_OnEvent - 0x00487860 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00487860-ConditionalContainer_OnEvent.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "ConditionalContainer_OnEvent.hpp"
int ConditionalContainer_OnEvent(void* context)

{
    int result;
    result = *(context + 0x112);

    if (!result)
    {
        int item_c = 0;
        result = Level_StartMission_4();
        int* i = *(context + 8);
        int result_1 = result;
        int item_4_1 = 2;

        if (*i && i)
        {
            int entity_1 = g_afa7e0;

            do
            {
                int* temp_1 = *i;
                void* data_3;

                if (!temp_1)
                    data_3 = nullptr;
                else
                {
                    int data_val = *temp_1;
                    int data_1 = -(data_val);
                    data_3 = (data_1 - data_1) & temp_1;
                }

                *((entity_1 << 2) + &g_afa6e0) = data_3;
                g_afa7e0 = entity_1 + 1;
                unsigned int data_5 = DynObj_UpdateDynTree_12();
                int entity_3 = 0;
                entity_3 = i[7];
                result = *(((data_5 * 0x180 + entity_3) << 2) + &g_a96ae0);

                if (result)
                    result = result(i, &item_c);

                entity_1 = g_afa7e0 - 1;
                g_afa7e0 = entity_1;
                i = *((entity_1 << 2) + &g_afa6e0);
            } while (i);
        }

        *(context + 0x112) = 1;
    }

    return result;
}
