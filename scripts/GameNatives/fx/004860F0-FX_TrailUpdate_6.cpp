// IGI native provenance: FX_TrailUpdate_6 - 0x004860F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004860F0-FX_TrailUpdate_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004860F0-FX_TrailUpdate_6.hpp"
int FX_TrailUpdate_6(int context, int arg2, float arg3, float arg4)

{
    float item_4 = arg3;
    float item_8 = arg4;
    int temp_val = arg2 * 0x40b;
    void* source_val = context + (temp_val << 2);
    int* data_6 = *(context + (temp_val << 2) + 0x3c);

    if (data_6 && *(source_val + 0x24) > 9)
        Sys_ResolveSymbol_14(data_6, arg3, arg4, 128.0f, 128.0f, 128.0f, 0, 0, 0);

    void* data_7 = *(source_val + 0x3c);

    if (data_7)
        arg3 = Sys_CloseFileStream_17(data_7) + arg3;

    int* source_1 = source_val + 0x30;
    int i_1 = 3;
    long double fpu_val_7_6 = arg4 + 2.0f;
    arg4 = fpu_val_7_6;
    int i;

    do
    {
        *(*source_1 + 0x14) = 0;
        *(*source_1 + 0x18) = 0x42f00000;
        *(*source_1 + 0x1c) = 0;
        *(*source_1 + 4) = arg3 + 3.0f;
        *(*source_1 + 8) = fpu_val_7_6;
        Sys_CloseFileStream_18(*source_1);
        long double fpu_val_7_7 = Sys_AllocateMemory_22(*source_1);
        source_1 = &source_1[1];
        i = i_1;
        i_1 -= 1;
        fpu_val_7_6 = fpu_val_7_7 + arg4;
        arg4 = fpu_val_7_6;
    } while (i != 1);
    return FX_ExplosionSpawn_7(context, arg2, item_4, item_8, 0);
}
