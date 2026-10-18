// IGI native provenance: FX_DebrisSpawn_6 - 0x00485DB0 - CFG score 85.2% (PASS; structural only); runtime behavior not verified.
// Evidence: 00485DB0-FX_DebrisSpawn_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00485DB0-FX_DebrisSpawn_6.hpp"
❓  char* __stdcall FX_DebrisSpawn_6(int context , int arg2, float arg3)
// This function has unresolved stack usage. View graph of stack usage to resolve.

{
    Camera_UpdateTransform_5(0x2818);
    void* data_val = FX_Font4Fnt(context, arg2);
    char* flagP = nullptr;
    void* arg_c = 3;
    int arg_4 = 0;
    void* source_1 = context + arg2 * 0x102c + 0x20;
    void* arg_10 = source_1;
    char* data_6 = *(source_1 + 0xc);
    char* context_val = source_1 + 0x2c;
    char* dest_1 = context_val;
    *data_6 = 0;
    *(&return_addr + 3) = *data_6;
    int arg_8 = source_1 + 0x10;
    bool flag_0_1;

    do
    {
        Sys_FreeMemory_18(*arg_8, &g_567c74);
        flag_0_1 = arg_c != 1;
        arg_8 += 4;
        arg_c -= 1;
    } while (flag_0_1);
    char data_11 = *context_val;
    *(&return_addr + 2) = data_11;
    int data_17;

    if (!data_11)
    {
    label_485f0e:
        data_17 = arg_4;
    }
    else
    {
        void arg_18;
        arg_c = &arg_18;

        while (true)
        {
            data_17 = arg_4;

            if (data_17 >= 9)
                break;

            *context_val = 0;
            long double fpu_val_7_1 = Sys_PopStack_18(data_val, dest_1);
            long double f1_1 = arg3;
            fpu_val_7_1 - f1_1;
            void* data_14;

            data_14 = !(*(&((fpu_val_7_1 < f1_1 ? 1 : 0) << 8
                | (FCMP_UO(fpu_val_7_1, f1_1) ? 1 : 0) << 0xa
                | (fpu_val_7_1 == f1_1 ? 1 : 0) << 0xe) + 1) & 1) || *(&return_addr + 2) == 0xa ? 1
                : nullptr;

            *context_val = *(&return_addr + 2);

            if (data_14)
            {
                if (!flagP)
                    flagP = &context_val[0xffffffff];

                char temp_4 = *flagP;
                int i = 0xffffffff;
                *flagP = 0;

                while (i)
                {
                    bool flag_4_1 = 0 != *dest_1;
                    dest_1 = &dest_1[1];
                    i -= 1;

                    if (!flag_4_1)
                        break;
                }

                *(&return_addr + 2) = temp_4;
                int entity_6 = ~i;
                int source_3;
                int dest_4;
                dest_4 = memcpy(arg_c, dest_1 - entity_6, entity_6 & 0xfffffffc);
                memcpy(dest_4, source_3, entity_6 & 3);
                source_1 = arg_10;
                *flagP = *(&return_addr + 2);
                dest_1 = &flagP[1];
                arg_4 += 1;
                arg_c += 0x400;
            }

            if (*context_val == 0x20)
                flagP = context_val;

            char data_16 = context_val[1];
            context_val = &context_val[1];
            *(&return_addr + 2) = data_16;

            if (!data_16)
                goto label_485f0e;
        }
    }

    int saved_target;

    if (context_val != dest_1)
    {
        int i_1 = 0xffffffff;

        while (i_1)
        {
            bool flag_2_1 = 0 != *dest_1;
            dest_1 = &dest_1[1];
            i_1 -= 1;

            if (!flag_2_1)
                break;
        }

        int entity_12 = ~i_1;
        int source_5;
        int dest_7;
        dest_7 = memcpy(&(&saved_target)[data_17 * 0x100 + 0xa], dest_1 - entity_12,
            entity_12 & 0xfffffffc);
        data_17 = arg_4 + 1;
        memcpy(dest_7, source_5, entity_12 & 3);
        source_1 = arg_10;
        arg_4 = data_17;
    }

    int entity_16;

    entity_16 = data_17 <= 3 ? 0 : data_17 - 3;

    if (entity_16 >= data_17)
    {
        char* data_24 = *(source_1 + 0xc);
        *data_24 = *(&return_addr + 3);
        return data_24;
    }

    int* context_1 = source_1 + 0x10;
    char* dest_8 = &(&saved_target)[entity_16 * 0x100 + 0xa];
    int i_3 = data_17 - entity_16;
    void* data_23;
    int i_2;

    do
    {
        data_23 = Sys_FreeMemory_18(*context_1, dest_8);
        dest_8 = &dest_8[0x400];
        context_1 = &context_1[1];
        i_2 = i_3;
        i_3 -= 1;
    } while (i_2 != 1);
    **(source_1 + 0xc) = *(&return_addr + 3);
    return data_23;
}
