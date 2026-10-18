// IGI native provenance: HumanPlayerInput_Setup - 0x0045DE20 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 0045DE20-HumanPlayerInput_Setup.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayerInput_Setup.hpp"
char* HumanPlayerInput_Setup(void* context)

{
    if (*(context + 0x80) && *(context + 0x190))
    {
        int item_100 = g_53f30c[0];
        short item_fc_1 = g_53f30c[4][0];
        char item_fa_1 = g_53f30c[6][0];
        void item_f9;
        *memset(&item_f9, 0, 0xf8) = 0;
        void* dest_3 = context + 0x82;
        int i = 0xffffffff;
        char* item_114_1 = &g_538354;

        while (i)
        {
            bool flag_0_1 = 0 != *dest_3;
            dest_3 += 1;
            i -= 1;

            if (!flag_0_1)
                break;
        }

        int entity_2 = ~i;
        int* dest_5 = &item_100;
        int i_1 = 0xffffffff;

        while (i_1)
        {
            bool flag_1_1 = 0 != *dest_5;
            dest_5 += 1;
            i_1 -= 1;

            if (!flag_1_1)
                break;
        }

        int source_2;
        int dest_7;
        dest_7 = memcpy(dest_5 - 1, dest_3 - entity_2, entity_2 & 0xfffffffc);
        int* item_118_1 = &item_100;
        memcpy(dest_7, source_2, entity_2 & 3);
        int* data_3 = QFile_ReadWrite(item_118_1, item_114_1);

        if (data_3)
        {
            Sys_CloseFileStream_4(data_3, *(context + 0x188), *(context + 0x190) * 0xa0);
            Device_CloseStream(data_3);
        }
    }
    else if (*(context + 0x81))
    {
        void* data_5 = *(context + 0x184);

        if (data_5)
            Sys_OpenFileStream_5(data_5);
    }

    void* data_6 = *(context + 0x188);

    if (data_6)
    {
        Sys_ExecuteBytecode_2(data_6);
        *(context + 0x188) = 0;
    }

    return Script_CallNativeHandler_6(context + 0x194);
}
