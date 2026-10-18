// IGI native provenance: SCamera_Cleanup_7 - 0x00442C60 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00442C60-SCamera_Cleanup_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SCamera_Cleanup_7.h"
int SCamera_Cleanup_7(int context, double** arg2)

{
    void* entity_val = *(context + 0x25d0);
    char result = *(entity_val + 0x170);

    if (result)
        return result;

    double* data_val = *arg2;
    float item_c = *(context + 0x20) - *data_val;
    float item_8_1 = *(context + 0x28) - data_val[1];
    float item_4_1 = *(context + 0x30) - data_val[2];
    float* data_1 = arg2[1];
    float item_18 = item_4_1 * data_1[6] + item_8_1 * data_1[3] + item_c * *data_1;
    float item_14_1 = item_4_1 * data_1[7] + item_8_1 * data_1[4] + item_c * data_1[1];
    long double fpu_val_7_17 = item_4_1 * data_1[8] + item_8_1 * data_1[5];
    long double fpu_val_6_12 = item_c * data_1[2];
    float item_8_2 = item_14_1;
    item_c = item_18;
    float item_4_2 = fpu_val_7_17 + fpu_val_6_12;
    int* source_1;
    int dest_1;

    if (!*(entity_val + 0x168))
    {
        source_1 = &g_5be230;
        dest_1 = 0;
    }
    else
    {
        dest_1 = *(context + 0x25e0);
        source_1 = &g_5be248;
    }

    AI_UpdateSensors_24(&item_18, &item_c, arg2);
    long double fpu_val_7_20 = Sys_CloseFileStream_17(source_1) * 0.5f;
    item_18 = item_18 - fpu_val_7_20;
    int data_6 = Sys_ExecuteBytecode_17(source_1);
    return Sys_AllocateMemory_15(source_1, item_18, item_14_1 - data_6 * 0.5f, 128.0f, 128.0f, 128.0f, 0x3f800000,
        0x44051eb8, dest_1, 0x400c0000, 0xffffffff);
}
