// IGI native provenance: AnimSpline_ProcessStep_50BE90 - 0x0050BE90 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0050BE90-AnimSpline_ProcessStep_50BE90.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AnimSpline_ProcessStep_50BE90.h"

char* AnimSpline_ProcessStep_50BE90(int* arg1)

{
    int* data_val = arg1;
    void* result_val = *data_val;

    while (g_a7cec8)
        /* nop */

    int base_val;
    int var_4 = base_val;
    g_a7cec8 = 1;
    *(result_val + 4) = 1;
    *(result_val + 8) = &data_bc7a00;
    int esi_1 = __builtin_memcpy(0xbc8a00, data_val[2], 0x400);
    short* ecx_1 = &data_bc7a00;
    char* i;

    for (i = &data_bc8a01; i < 0xbc8e01;)
    {
        data_val = i[1];
        esi_1 = *i;
        data_val &= 0xf8;
        esi_1 &= 0xfffc;
        data_val = data_val;
        unsigned char ebx_1 = i[0xffffffff] >> 3;
        int edx_2 = data_val << 5 | esi_1;
        i = &i[4];
        esi_1 = ebx_1;
        data_val = edx_2 << 3 | esi_1;
        *ecx_1 = data_val;
        ecx_1 = &ecx_1[1];
    }

    return i;
}
