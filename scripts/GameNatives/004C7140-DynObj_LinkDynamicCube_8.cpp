// IGI native provenance: DynObj_LinkDynamicCube_8 - 0x004C7140 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004C7140-DynObj_LinkDynamicCube_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004C7140-DynObj_LinkDynamicCube_8.hpp"
int DynObj_LinkDynamicCube_8(void* context, int* arg2, double* arg3, float arg4, int arg5)

{
    int data_1 = DynObj_InvalidCategory(arg5, arg2[7]);
    int* data_2 = DynObj_InsertHashItem_9(context, data_1);
    int flagP1 = *arg3 + *(context + 0xde0);
    int entity_2 = arg3[1] + *(context + 0xde4);
    void* temp_1 = arg3[2] + *(context + 0xde8);
    int item_14_3 = arg4;
    int data_7 = DynObj_LinkDynamicCube_7(flagP1 - item_14_3, entity_2 - item_14_3, temp_1 - item_14_3,
        item_14_3 + flagP1, item_14_3 + entity_2, temp_1 + item_14_3);
    arg2[8] = *arg3;
    arg2[9] = *(arg3 + 4);
    arg2[0xa] = arg3[1];
    arg2[0xb] = *(arg3 + 0xc);
    arg2[0xc] = arg3[2];
    arg2[0xd] = *(arg3 + 0x14);
    int entity_9 = 0;
    arg2[0x11] = arg4;
    entity_9 = arg2[7];
    arg2[0x17] = *(entity_9 * 0x30 + &g_af5f08);
    arg2[0x18] = 0;
    DynObj_LookupHashItem_8(context, data_2, arg2, data_7, flagP1, entity_2, temp_1);
    int result = 0;
    int i_1 = 0xfe;
    short* context_1 = context + data_1 * 0x1fc + 0x118;
    int i;

    do
    {
        int entity_14 = 0;
        short temp_16 = 0;
        entity_14 = arg2[7];
        temp_16 = *(((entity_14 + result) << 2) + &g_a96ae0);
        *context_1 += temp_16;
        context_1 = &context_1[1];
        result += 0x180;
        i = i_1;
        i_1 -= 1;
    } while (i != 1);
    return result;
}
