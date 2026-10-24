// IGI native provenance: FX_DebrisSpawn_13 - 0x004896C0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004896C0-FX_DebrisSpawn_13.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004896C0-FX_DebrisSpawn_13.hpp"
char* FX_DebrisSpawn_13(int* context, int arg2, int arg3, int arg4, int arg5, int arg6)

{
    *context = arg2;
    context[4] = arg3;
    context[5] = arg4;
    context[3] = g_5c8b08;
    int data_4 = (g_5c8b08 + 1) & 0x80000003;

    if (data_4 < 0)
        data_4 = ((data_4 - 1) | 0xfffffffc) + 1;

    g_5c8b08 = data_4;
    int data_8;
    int temp_2;
    temp_2 = HIGHD(arg5);
    data_8 = LOWD(arg5);
    context[7] = 0;
    context[0xa] = 0;
    context[1] = (data_8 + (temp_2 & 3)) >> 2;
    int data_12;
    int temp_4;
    temp_4 = HIGHD(arg6);
    data_12 = LOWD(arg6);
    context[2] = (data_12 + (temp_4 & 3)) >> 2;
    context[6] = &context[7];
    context[8] = &context[6];
    context[0xb] = &context[9];
    context[9] = &context[0xa];
    char* result = context + 0x5a;
    int i_1 = 0xc0;
    int i;

    do
    {
        *result = 0;
        result = &result[0x30];
        i = i_1;
        i_1 -= 1;
    } while (i != 1);
    return result;
}
