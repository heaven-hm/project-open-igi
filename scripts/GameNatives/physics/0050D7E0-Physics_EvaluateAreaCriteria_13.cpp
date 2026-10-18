// IGI native provenance: Physics_EvaluateAreaCriteria_13 - 0x0050D7E0 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 0050D7E0-Physics_EvaluateAreaCriteria_13.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Physics_EvaluateAreaCriteria_13.hpp"
❓  int Physics_EvaluateAreaCriteria_13(void* context)
// This function has unresolved stack usage. View graph of stack usage to resolve.

{
    int context_val;
    int item_10 = context_val;
    void* context_1 = context;
    int flagP;
    int item_14 = flagP;
    int source_1 = *(context_1 + 0x14);
    int dest_val;
    int item_1c = dest_val;
    int dest_1 = g_a7da00;
    int entity_1 = dest_1 & 0x8000003c;
    void* flagP1 = *(context_1 + 0x24);
    int entity_2 = -(entity_1);
    int temp_1 = source_1 & 0x8000003c;
    void* item_4 = *(context_1 + 0xc);
    int temp_2 = -(temp_1);
    int item_8 = source_1;

    if (entity_2 - entity_2 + 1 != temp_2 - temp_2 + 1 || (dest_1 ^ source_1) & 0x60000)
    {
        *(g_bcadac + 4) = 0xffffffff;
        source_1 = item_8;
        dest_1 = g_a7da00;
    }

    if ((dest_1 ^ source_1) & 0x8000003c)
    {
        *(g_bcad7c + 4) = 0xffffffff;
        source_1 = item_8;
        dest_1 = g_a7da00;
    }

    int data_1 = 0;
    data_1 = !g_a7da04;
    int entity_11 = 0;
    entity_11 = !item_4;

    if (data_1 != entity_11 || (dest_1 ^ source_1) & 0x1400000)
    {
        *(g_bcade8 + 4) = 0xffffffff;
        source_1 = item_8;
    }

    int entity_12 = g_a7ced8;
    g_a7da04 = item_4;
    void* data_4 = g_bcade8;
    g_a7da00 = source_1;

    if (*(data_4 + 4) != entity_12)
    {
        *(data_4 + 4) = entity_12;
        (*(g_bcade8 + (g_a7ced8 << 2) + 0x14))(&item_8);
    }

    void* data_6 = g_bcad7c;
    int entity_13 = g_a7d9f8;

    if (*(data_6 + 4) != entity_13)
    {
        *(data_6 + 4) = entity_13;
        (*(g_bcad7c + (g_a7d9f8 << 2) + 0x14))(&item_8);
    }

    void* data_8 = g_bcadac;
    int entity_14 = g_a7d9f4;

    if (*(data_8 + 4) != entity_14)
    {
        *(data_8 + 4) = entity_14;
        (*(g_bcadac + (g_a7d9f4 << 2) + 0x14))(&item_8);
    }

    void* data_10 = g_bcadc4;
    int entity_15 = g_bcacec;

    if (*(data_10 + 4) != entity_15)
    {
        *(data_10 + 4) = entity_15;
        (*(g_bcadc4 + (g_bcacec << 2) + 0x14))(&item_8);
    }

    void** item_24;

    if (item_4)
    {
        int* data_13 = g_5ca114;
        int temp_14 = *(*(context_1 + 0xc) + 0x20);
        int entity_16 = *data_13;
        item_24 = nullptr;
        (*(entity_16 + 0x8c))(data_13, item_24, temp_14);
    }

    int data_15 = *(context_1 + 0x1c);
    int result;
    void*** stack_ptr;
    void** item_30;

    if (!*(context_1 + 0x20))
    {
        int temp_21 = g_bcad08;
        int* data_26;

        if (data_15 <= 2)
        {
            bool flag_2_1 = temp_21 + 4 > g_bcad04;
            int* data_28 = g_bcad00;
            int item_20_8 = 0;
            int entity_46 = *data_28;
            item_24 = &context;
            void* temp_30;
            void** context_3;

            if (flag_2_1)
            {
                result = (*(entity_46 + 0xc))(data_28, 0x2020);
                stack_ptr = &item_24;
                context_3 = nullptr;
                g_bcad08 = 4;

                if (!result)
                {
                    temp_30 = item_4;
                label_50db91:

                    if (temp_30)
                    {
                        memcpy(temp_30, flagP1, 0x1c);
                        memcpy(item_4 + 0x54, flagP1 + 0x1c, 0x1c);
                        memcpy(item_4 + 0x1c, flagP1, 0x1c);
                        *(item_4 + 0x1c) = *(flagP1 + 0x1c);
                        *(item_4 + 0x30) = *(flagP1 + 0x30);
                        memcpy(item_4 + 0x38, flagP1, 0x1c);
                        *(item_4 + 0x3c) = *(flagP1 + 0x20);
                        *(item_4 + 0x50) = *(flagP1 + 0x34);
                        int* data_33 = g_bcad00;
                        (*(*data_33 + 0x10))(data_33);
                        data_26 = g_5ca114;
                        int item_28_12 = 0;
                        int item_2c_8 = 4;
                        item_30 = context_3;
                        result = (*(*data_26 + 0x7c))(data_26, 5, g_bcad00);
                        stack_ptr = &item_30;
                    }
                }
            }
            else
            {
                result = (*(entity_46 + 0xc))(data_28, 0x20);
                stack_ptr = &item_24;
                void** entity_47 = g_bcad08;
                context_3 = entity_47;
                g_bcad08 = &entity_47[1];
                temp_30 = item_4 + entity_47 * 0x1c;
                item_4 = temp_30;

                if (!result)
                    goto label_50db91;
            }
        }
        else
        {
            bool flag_3_1 = data_15 + temp_21 > g_bcad04;
            int* data_23 = g_bcad00;
            int item_20_7 = 0;
            int entity_33 = *data_23;
            item_24 = &context;
            void** item_14_3;
            void* temp_24;

            if (flag_3_1)
            {
                result = (*(entity_33 + 0xc))(data_23, 0x2020);
                stack_ptr = &item_24;
                item_14_3 = nullptr;
                g_bcad08 = *(context_1 + 0x1c);

                if (!result)
                {
                    temp_24 = item_4;
                label_50dad7:

                    if (temp_24)
                    {
                        int entity_39 = *(context_1 + 0x1c) * 0x1c;
                        int source_6;
                        int dest_11;
                        dest_11 = memcpy(temp_24, flagP1, entity_39 & 0xfffffffc);
                        memcpy(dest_11, source_6, entity_39 & 3);
                        int* data_25 = g_bcad00;
                        (*(*data_25 + 0x10))(data_25);
                        data_26 = g_5ca114;
                        int item_28_8 = 0;
                        int item_2c_5 = *(context_1 + 0x1c);
                        int temp_26 = *data_26;
                        item_30 = item_14_3;
                        result = (*(temp_26 + 0x7c))(data_26, 5, g_bcad00);
                        stack_ptr = &item_30;
                    }
                }
            }
            else
            {
                result = (*(entity_33 + 0xc))(data_23, 0x20);
                stack_ptr = &item_24;
                void** entity_34 = g_bcad08;
                item_14_3 = entity_34;
                temp_24 = item_4 + entity_34 * 0x1c;
                void* item_4_2 = temp_24;
                g_bcad08 = entity_34 + *(context_1 + 0x1c);

                if (!result)
                    goto label_50dad7;
            }
        }
    }
    else
    {
        bool flag_1_1 = data_15 + g_bcad08 > g_bcad04;
        int* data_17 = g_bcad00;
        int item_20_6 = 0;
        item_24 = &context;
        int entity_18 = *data_17;
        void* temp_18;

        if (flag_1_1)
        {
            result = (*(entity_18 + 0xc))(data_17, 0x2020);
            stack_ptr = &item_24;
            int item_14_2 = 0;
            g_bcad08 = *(context_1 + 0x1c);

            if (!result)
            {
                temp_18 = item_4;
            label_50d9f9:

                if (temp_18)
                {
                    int entity_24 = *(context_1 + 0x1c) * 0x1c;
                    int source_3;
                    int dest_6;
                    dest_6 = memcpy(temp_18, flagP1, entity_24 & 0xfffffffc);
                    memcpy(dest_6, source_3, entity_24 & 3);
                    int* data_19 = g_bcad00;
                    (*(*data_19 + 0x10))(data_19);
                    int* data_20 = g_5ca114;
                    int item_2c_2 = 0;
                    item_30 = *(context_1 + 0x1c);
                    int source_val;
                    int item_34_1 = source_val;
                    return (*(*data_20 + 0x7c))(data_20, 3, g_bcad00);
                }
            }
        }
        else
        {
            result = (*(entity_18 + 0xc))(data_17, 0x20);
            stack_ptr = &item_24;
            int entity_19 = g_bcad08;
            int item_14_1 = entity_19;
            temp_18 = item_4 + entity_19 * 0x1c;
            void* item_4_1 = temp_18;
            g_bcad08 = entity_19 + *(context_1 + 0x1c);

            if (!result)
                goto label_50d9f9;
        }
    }
❓️    *stack_ptr;
    stack_ptr[1];
    stack_ptr[2];
    stack_ptr[3];
    return result;
}
