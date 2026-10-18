// IGI native provenance: Display_SwapBuffers_2 - 0x00490D10 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00490D10-Display_SwapBuffers_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_SwapBuffers_2.hpp"
HRESULT Display_SwapBuffers_2(int context , int arg2 , float arg3)

{
    int entity_val;
    int temp_val;

    if (*(arg3 + 4))
    {
        int data_2 = Timer_Now();
        temp_val = *(arg3 + 8);
        entity_val = *(arg3 + 4);
        *arg3 = data_2 - temp_val - entity_val;
    }
    else
    {
        int data_1;
        data_1 = Timer_Now();
        *(arg3 + 4) = data_1;
    }

    void* data_5 = *(arg3 + 0x158);

    if (data_5)
    {
        char data_6 = Display_SyncVSync_11(data_5, temp_val, entity_val, data_5);
        *(arg3 + 0x29) = data_6;

        if (!data_6)
        {
            Display_SetDisplayMode_12(*(arg3 + 0x158));
            Display_CloseDevice_11(*(arg3 + 0x158));
            *(arg3 + 0x158) = 0;
        }
        else
        {
            int data_9;
            int temp_1;
            temp_1 = HIGHD(0x10624dd3 * *arg3 * 0x3c);
            data_9 = LOWD(0x10624dd3 * *arg3 * 0x3c);
            int entity_3 = *(arg3 + 0x14);
            unsigned int temp_2 = temp_1 >> 6;

            if (temp_2 != entity_3)
            {
                int i_1 = temp_2 - entity_3;
                int i;

                do
                {
                    void item_58;
                    Display_SetGammaRamp_12(&item_58);
                    Display_SyncVSync_12(*(arg3 + 0x158), &item_58);
                    i = i_1;
                    i_1 -= 1;
                    *(arg3 + 0x14) += 1;
                } while (i != 1);
            }
        }
    }

    int data_13 = Timer_Now();
    HRESULT data_14 = *(arg3 + 0xc);
    int entity_9 = data_13 - *(arg3 + 8) - *(arg3 + 4);
    *arg3 = entity_9;

    if (entity_9 >= data_14)
    {
        *(arg3 + 0x28) = 0;
    label_490e72:
        data_14 = *(arg3 + 0x29);

        if (!data_14)
        {
            data_14 = *(arg3 + 0x28);

            if (!data_14)
            {
            label_490e81:
                data_14 = 0;
                return data_14;
            }
        }
    }
    else
    {
        int data_17;
        int temp_8;
        temp_8 = HIGHD(0x10624dd3 * COMBINE(0, *(arg3 + 0xb8)) / *(arg3 + 0xb4) * entity_9);
        data_17 = LOWD(0x10624dd3 * COMBINE(0, *(arg3 + 0xb8)) / *(arg3 + 0xb4) * entity_9);
        data_14 = *(arg3 + 0x1c);
        unsigned int dest_2 = temp_8 >> 6;

        if (data_14 == dest_2)
            goto label_490e72;

        if (data_14 == 0xffffffff)
        {
            HRESULT data_18 = Display_ClearSurfaces_2(arg3, dest_2, 0, arg2, context);

            if (!data_18)
                return data_18;
        }
        else if (dest_2 - data_14 != 1)
        {
            do
            {
                data_14 = Display_ClearSurfaces_2(arg3, data_14 + 1, 0x80000000, arg2, context);

                if (!data_14)
                    goto label_490e81;

                data_14 = *(arg3 + 0x1c);
            } while (data_14 != dest_2);
        }
        else
        {
            HRESULT data_19 = Display_ClearSurfaces_2(arg3, dest_2, 0, arg2, context);

            if (!data_19)
                return data_19;
        }

        data_14 = Display_RestoreDevice_2(arg3);
    }

    data_14 = 1;
    return data_14;
}
