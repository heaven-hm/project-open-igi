// IGI native provenance: StatusMessageScreen_OnEvent - 0x00485B80 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00485B80-StatusMessageScreen_OnEvent.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "StatusMessageScreen_OnEvent.hpp"
int StatusMessageScreen_OnEvent(int context, int arg2)

{
    void* data_val = FX_FragmentSimulate_6(context, arg2);

    if (!data_val)
        return FX_SmokeEmit_6(context, arg2);

    int data_2 = arg2 * 3;
    void* dest_val;

    if (*(data_val + 0x1c6))
    {
        int entity_2 = ((data_2 << 3) - arg2) * 0x2d;
        dest_val = context + (entity_2 << 2);

        if (!*(context + (entity_2 << 2) + 0x28))
        {
            int item_1c_2 = entity_2;
            FX_DebrisSpawn_6(context, arg2, g_c28b44 * 0.96875f);
        }

        if (*(dest_val + 0x28) > *(data_val + 0x1d8) * 30.0f)
            FX_SmokeEmit_6(context, arg2);

        *(dest_val + 0x28) += 1;
    }
    else
    {
        int data_6 = ((data_2 << 3) - arg2) * 0x2d;
        dest_val = context + (data_6 << 2);
        int data_7 = *(context + (data_6 << 2) + 0x24);

        if (data_7 < 9)
        {
            if (data_7 >= 2)
            {
                *(dest_val + 0x44) = *(dest_val + 0x44) + 0.142857149f;
                int data_11 = *(dest_val + 0x24) + 1;
                *(dest_val + 0x24) = data_11;
                return data_11;
            }

            *(dest_val + 0x48) = *(dest_val + 0x48) + 0.5f;
            int data_9 = *(dest_val + 0x24) + 1;
            *(dest_val + 0x24) = data_9;
            return data_9;
        }

        char* entity_val = *(dest_val + 0x2c);

        if (!*entity_val)
        {
            int temp_2 = *(dest_val + 0x28) + 1;
            *(dest_val + 0x28) = temp_2;

            if (temp_2 > *(data_val + 0x1d8) * 30.0f)
            {
                FX_SmokeEmit_6(context, arg2);
                int data_21 = *(dest_val + 0x24) + 1;
                *(dest_val + 0x24) = data_21;
                return data_21;
            }
        }
        else
        {
            int data_13 = (data_7 - 9) & 0x80000001;
            bool flag_2_1 = data_13;

            if (data_13 < 0)
                flag_2_1 = ((data_13 - 1) | 0xfffffffe) != 0xffffffff;

            if (!flag_2_1)
            {
                *(dest_val + 0x2c) = &entity_val[1];
                FX_DebrisSpawn_6(context, arg2, 229.0f);
                int data_18 = *(dest_val + 0x24) + 1;
                *(dest_val + 0x24) = data_18;
                return data_18;
            }
        }
    }

    int data_28 = *(dest_val + 0x24) + 1;
    *(dest_val + 0x24) = data_28;
    return data_28;
}
