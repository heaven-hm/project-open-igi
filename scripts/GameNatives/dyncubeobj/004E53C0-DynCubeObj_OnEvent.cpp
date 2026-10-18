// IGI native provenance: DynCubeObj_OnEvent - 0x004E53C0 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004E53C0-DynCubeObj_OnEvent.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "DynCubeObj_OnEvent.hpp"
int* DynCubeObj_OnEvent(int context, int arg2, int arg3, double* arg4, float arg5)

{
    int data_2;
    int temp_val;
    temp_val = HIGHD(context * context);
    data_2 = LOWD(context * context);
    int dest_1 = (data_2 + (temp_val & 7)) >> 3;
    int* result = Mem_Alloc(dest_1 + 0xf, 4);
    result[2] = context;
    int entity_val = g_b019e8;
    int** data_7 = *(g_b019e4 + (entity_val << 2)) * g_b019f0 + g_b019e0;
    *(g_b019f4 + data_7) = entity_val;
    g_b019e8 += 1;
    *result = data_7;
    *data_7 = result;
    int eflags;
    int data_8;
    data_8 = __bsf_gprv_memv(context);
    result[1] = data_8;

    if (dest_1 > 0)
        memset(memset(&result[3], 0, dest_1 & 0xfffffffc), 0, dest_1 & 3);

    double* item_c = arg4;
    double item_44 = *arg4 - arg5;
    char temp_7 = g_a5e408;
    int item_34 = 0;
    double item_3c = arg4[1] - arg5;
    long double fpu_val_7_5 = arg5 + *arg4;
    int item_30 = 0xc1d00000;
    int item_1c = 0;
    int item_18 = 0x41d00000;
    int* result_1 = result;
    int item_10 = context;
    double item_2c = fpu_val_7_5;
    float item_8 = arg5;
    double item_24 = arg5 + arg4[1];
    DynObj_CreateMagicObject_9(arg2, temp_7, &result_1, &item_44, &item_2c, 0x20000000, arg3);
    return result;
}
