// IGI native provenance: Symbol_CheckRegister - 0x004C0560 - CFG score 90.5% (PASS; structural only); runtime behavior not verified.
// Evidence: 004C0560-Symbol_CheckRegister.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Symbol_CheckRegister.h"
int Symbol_CheckRegister(int* context, int* arg2, int arg3)

// Sig: void* Symbol_CheckRegister(int* symTable, char* symName, int extra)
// Note: Checks a symbol and registers it if new - ERRORS via ErrorShow("Symbol
// \"%s\" already registered in hash table") if the name ALREADY exists, so it
// doubles as duplicate protection. Params: symTable, symName, extra =
// registration payload/flags. RETURNS symbol entry or triggers the error
// dialog.
{
    int* saved_source_2 = arg2;

    if (!DynObj_InsertHashItem_2(context, &arg2, saved_source_2))
    {
        int* saved_source_3 = saved_source_2;
        int i = 0xffffffff;

        while (i)
        {
            bool flag_0_1 = 0 != *saved_source_3;
            saved_source_3 += 1;
            i -= 1;

            if (!flag_0_1)
                break;
        }

        int* data_1 = Mem_Alloc(~i + 0x10, 4);
        arg2 = data_1;
        data_1[2] = &data_1[4];
        int* saved_source_4 = saved_source_2;
        arg2[3] = arg3;
        int i_1 = 0xffffffff;

        while (i_1)
        {
            bool flag_1_1 = 0 != *saved_source_4;
            saved_source_4 += 1;
            i_1 -= 1;

            if (!flag_1_1)
                break;
        }

        int entity_3 = ~i_1;
        int source_1;
        int dest_2;
        dest_2 = memcpy(arg2[2], saved_source_4 - entity_3, entity_3 & 0xfffffffc);
        memcpy(dest_2, source_1, entity_3 & 3);
        arg2[1] = 0;
        *arg2 = 0;
        unsigned int data_5 = DynObj_LookupHashItem_2(saved_source_2, context[1]);
        Camera_ApplyLineFilter_33(*context + data_5 * 0xc, arg2);
        return arg2[2];
    }

    int* saved_source_1 = saved_source_2;
    Error_Show("Symbol "%s" already registered in hash table");

    while (true)
        /* nop */
}
