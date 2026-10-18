// IGI native provenance: SoundDefGroup_OnEvent - 0x004EB670 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004EB670-SoundDefGroup_OnEvent.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundDefGroup_OnEvent.hpp"
int SoundDefGroup_OnEvent(void* context, int* arg2)

{
    int result = arg2[1];

    if (!result)
    {
        char* data_val = *arg2;
        int context_val;
        int item_10_1 = context_val;
        char* source_1 = context + 0x22;

        while (true)
        {
            char temp_1 = *data_val;
            char context_1 = *source_1;
            bool c_1 = temp_1 < context_1;

            if (temp_1 == context_1)
            {
                if (!temp_1)
                {
                    result = 0;
                    break;
                }

                temp_1 = data_val[1];
                context_1 = source_1[1];
                c_1 = temp_1 < context_1;

                if (temp_1 == context_1)
                {
                    data_val = &data_val[2];
                    source_1 = &source_1[2];

                    if (temp_1)
                        continue;

                    result = 0;
                    break;
                }
            }

            result = data_val - data_val + 1;
            break;
        }

        if (!result)
        {
            arg2[1] = context;
            return result;
        }

        int* i = *(context + 8);

        if (*i && i)
        {
            int entity_2 = g_afa7e0;

            do
            {
                int* temp_2 = *i;
                void* data_5;

                if (!temp_2)
                    data_5 = nullptr;
                else
                {
                    int data_2 = *temp_2;
                    int data_3 = -(data_2);
                    data_5 = (data_3 - data_3) & temp_2;
                }

                *((entity_2 << 2) + &g_afa6e0) = data_5;
                g_afa7e0 = entity_2 + 1;
                unsigned int data_7 = SoundSys_StopSoundSample_5();
                int entity_4 = 0;
                entity_4 = i[7];
                result = *(((data_7 * 0x180 + entity_4) << 2) + &g_a96ae0);

                if (result)
                    result = result(i, arg2);

                entity_2 = g_afa7e0 - 1;
                g_afa7e0 = entity_2;
                i = *((entity_2 << 2) + &g_afa6e0);
            } while (i);
        }
    }

    return result;
}
