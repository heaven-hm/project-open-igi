// IGI native provenance: DynObj_RemoveSymbol_8 - 0x004C7350 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004C7350-DynObj_RemoveSymbol_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "DynObj_RemoveSymbol_8.h"
int DynObj_RemoveSymbol_8(void* context, int* arg2, double* arg3, double* arg4, double* arg5, int arg6)

{
    int data_1 = DynObj_InvalidCategory(arg6, arg2[7]);
    int* data_2 = DynObj_InsertHashItem_9(context, data_1);
    int flagP2 = *arg3 + *(context + 0xde0);
    int flagP4 = arg3[1] + *(context + 0xde4);
    void* flagP6 = arg3[2] + *(context + 0xde8);
    int flagP8 = *arg4 + *(context + 0xde0);
    int flagP10 = arg4[1] + *(context + 0xde4);
    int flagP12 = arg4[2] + *(context + 0xde8);
    int entity_5 = *arg5 + *(context + 0xde0);
    int flagP14 = arg5[1] + *(context + 0xde4);
    int temp_15 = arg5[2] + *(context + 0xde8);
    arg2[8] = *arg3;
    arg2[9] = *(arg3 + 4);
    arg2[0xa] = arg3[1];
    arg2[0xb] = *(arg3 + 0xc);
    arg2[0xc] = arg3[2];
    arg2[0xd] = *(arg3 + 0x14);
    int data_5 = 0;
    data_5 = arg2[7];
    arg2[0x11] = sqrtl(0.0) * 0.5f;
    short data_8 = *(data_5 * 0x30 + &g_af5f08);
    arg2[0x18] = 0;
    arg2[0x17] = data_8;
    DynObj_LookupHashItem_8(context, data_2, arg2,
        DynObj_LinkDynamicCube_7(flagP8, flagP10, flagP12, entity_5 - 1, flagP14 - 1, temp_15 - 1), flagP2, flagP4, flagP6);
    int result = 0;
    short* dest_1 = context + data_1 * 0x1fc + 0x118;
    int i_1 = 0xfe;
    int i;

    do
    {
        int temp_20 = 0;
        short context_1 = 0;
        temp_20 = arg2[7];
        context_1 = *(((temp_20 + result) << 2) + &g_a96ae0);
        *dest_1 += context_1;
        dest_1 = &dest_1[1];
        result += 0x180;
        i = i_1;
        i_1 -= 1;
    } while (i != 1);
    return result;
}
