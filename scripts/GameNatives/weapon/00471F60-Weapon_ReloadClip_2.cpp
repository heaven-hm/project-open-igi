// IGI native provenance: Weapon_ReloadClip_2 - 0x00471F60 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00471F60-Weapon_ReloadClip_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00471F60-Weapon_ReloadClip_2.hpp"
int Weapon_ReloadClip_2(void* context, int arg2, float arg3, int arg4, int* arg5, int arg6, int arg7, int arg8)

{
    int data_val = 0;
    int* entity_val = context + 0x2c44;
    int* temp_val = entity_val;
    int* source_1;

    while (true)
    {
        if (*temp_val == arg2)
        {
            source_1 = context + data_val * 0x2c + 0x2c44;

            if (source_1)
                break;
        }
        else
        {
            data_val += 1;
            temp_val = &temp_val[0xb];

            if (data_val < 0xa)
                continue;
        }

        int data_2 = 0;

        while (true)
        {
            if (!*entity_val)
            {
                source_1 = context + data_2 * 0x2c + 0x2c44;
                source_1[8] = Timer_Now();

                if (source_1)
                    break;
            }
            else
            {
                data_2 += 1;
                entity_val = &entity_val[0xb];

                if (data_2 < 0xa)
                    continue;
            }

            int entity_2 = 0;
            float* temp_3 = context + 0x2c5c;

            while (true)
            {
                long double fpu_val_7_1 = *temp_3;
                long double f0_1 = arg3;
                fpu_val_7_1 - f0_1;

                if (!(*(&((fpu_val_7_1 < f0_1 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_7_1, f0_1) ? 1 : 0) << 0xa
                    | (fpu_val_7_1 == f0_1 ? 1 : 0) << 0xe) + 1) & 0x41))
                {
                    source_1 = context + entity_2 * 0x2c + 0x2c44;
                    source_1[8] = Timer_Now();

                    if (source_1)
                        break;
                }
                else
                {
                    entity_2 += 1;
                    temp_3 = &temp_3[0xb];

                    if (entity_2 < 0xa)
                        continue;
                }

                return 0;
            }

            break;
        }

        break;
    }

    *source_1 = arg2;
    source_1[6] = arg3;
    source_1[4] = 1;
    source_1[5] = arg4;
    source_1[3] = arg6;
    source_1[7] = Timer_Now();
    source_1[2] = arg7;
    source_1[1] = arg8;
    source_1[9] = *arg5;
    source_1[0xa] = arg5[1];
    return 1;
}
