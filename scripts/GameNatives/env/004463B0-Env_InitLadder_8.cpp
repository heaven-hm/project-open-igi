// IGI native provenance: Env_InitLadder_8 - 0x004463B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004463B0-Env_InitLadder_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004463B0-Env_InitLadder_8.hpp"
int Env_InitLadder_8(void* context)

{
    void item_8;
    int result = Env_TriggerAlarm_8(&item_8, context);
    int* i = *(context + 8);

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
            unsigned int data_5 = DynObj_CreateMagicObject_6();
            int temp_2 = 0;
            temp_2 = i[7];
            result = *(((data_5 * 0x180 + temp_2) << 2) + &g_a96ae0);

            if (result)
                result = result(i, &item_8);

            entity_1 = g_afa7e0 - 1;
            g_afa7e0 = entity_1;
            i = *((entity_1 << 2) + &g_afa6e0);
        } while (i);
    }

    return result;
}
