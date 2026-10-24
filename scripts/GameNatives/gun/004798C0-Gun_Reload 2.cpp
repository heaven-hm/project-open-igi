// IGI native provenance: Gun_Reload - 0x004798C0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004798C0-Gun_MagFill.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Gun_MagFill.h"
char* Gun_Reload(int* context, char* arg2)

{
    int* source_val = context;
    bool flag_0 = !*arg2;
    void* flagP = source_val[0x4a];
    int* context_val = source_val[0x4b];
    context = nullptr;

    if (!flag_0)
    {
        source_val[0x51] = *(flagP + 0x648);
        return arg2;
    }

    void* dest_val = source_val[5];
    unsigned int data_1 = Weapon_ReloadClip_8();
    int data_2 = 0;
    data_2 = *(dest_val + 0x1c);
    int data_3 = *(((data_1 * 0x180 + data_2) << 2) + &g_a96ae0);

    if (data_3)
        data_3(dest_val, &context);

    int* data_5 = Weapon_UpdateMuzzleFlash_8(context, *context_val);

    if (data_5)
    {
        int entity_2 = data_5[1];

        if (entity_2)
        {
            int dest_1 = source_val[0x51];
            int temp_4 = *(flagP + 0x648);
            int temp_5 = temp_4 - dest_1;

            if (temp_4 != dest_1)
            {
                if (entity_2 >= temp_5)
                {
                    data_5[1] = entity_2 - temp_5;
                    int data_7 = source_val[0x51] + temp_5;
                    source_val[0x51] = data_7;
                    return data_7;
                }

                source_val[0x51] = dest_1 + entity_2;
                data_5[1] = 0;
            }
        }
    }

    return data_5;
}
