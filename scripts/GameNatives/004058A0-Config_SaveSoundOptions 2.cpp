// IGI native provenance: Config_SaveSoundOptions - 0x004058A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004058A0-Config_SaveSoundOptions.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Config_SaveSoundOptions.h"
int Config_SaveSoundOptions()

{
    int item_4 = 0x1b;
    int source_val = Config_SetResolution();
    int* flagP = Config_SetGamma();
    int data_2 = 0;
    int* item_c = flagP;
    int item_10 = 0;

    while (true)
    {
        int dest_1 = 0;
        int item_8;
        Config_Moveleft(data_2, &item_8, nullptr);
        int context_1 = item_8;
        int data_3 = 0;

        if (source_val > 0)
        {
            int* entity_1 = flagP;

            do
            {
                if (dest_1)
                    goto label_405953;

                if (*entity_1 == context_1)
                    dest_1 = 1;

                data_3 += 1;
                entity_1 = &entity_1[3];
            } while (data_3 < source_val);
        }

        if (source_val <= 0 || !dest_1)
        {
            int i = 0;
            int* entity_2 = &flagP[source_val * 3];
            int* data_5 = &g_536410;

            while (!i)
            {
                if (*data_5 == context_1)
                {
                    int* dest_2 = entity_2;
                    source_val += 1;
                    entity_2 = &entity_2[3];
                    *dest_2 = *data_5;
                    dest_2[1] = data_5[1];
                    dest_2[2] = data_5[2];
                    i = 1;
                }

                data_5 = &data_5[3];

                if (data_5 >= 0x536554)
                    break;
            }
        }

    label_405953:
        data_2 = item_10 + 1;
        item_10 = data_2;

        if (data_2 >= item_4)
            break;

        flagP = item_c;
    }

    return Config_SetVolume(source_val);
}
