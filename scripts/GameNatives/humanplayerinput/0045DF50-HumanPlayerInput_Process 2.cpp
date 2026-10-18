// IGI native provenance: HumanPlayerInput_Process - 0x0045DF50 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 0045DF50-HumanPlayerInput_Process.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayerInput_Process.h"
int HumanPlayerInput_Process(void* context)

{
    if (*(context + 0x80))
        *(context + 0x188) = Mem_Alloc(0x15f900, 4);
    else if (*(context + 0x81))
    {
        int item_100 = g_53f30c[0];
        short item_fc_1 = g_53f30c[4][0];
        char item_fa_1 = g_53f30c[6][0];
        void item_f9;
        *memset(&item_f9, 0, 0xf8) = 0;
        void* dest_3 = context + 0x82;
        int i = 0xffffffff;

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
        int item_104;
        int* item_118_1 = &item_104;
        memcpy(dest_7, source_2, entity_2 & 3);
        void* data_3 = Resource_PackUnpack(&item_100, item_118_1);
        *(context + 0x184) = data_3;

        if (data_3)
            *(context + 0x190) = item_104 / 0xa0;
    }

    Script_Isexists(context + 0x194, context);
    return 0;
}
