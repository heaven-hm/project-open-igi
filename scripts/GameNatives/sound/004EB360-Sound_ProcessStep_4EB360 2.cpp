// IGI native provenance: Sound_ProcessStep_4EB360 - 0x004EB360 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004EB360-Sound_ProcessStep_4EB360.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sound_ProcessStep_4EB360.h"

void Sound_ProcessStep_4EB360(void* arg1, int* arg2)

{
    __builtin_memset(arg1 + 0x48, 0, 0x18);

    if (!arg2)
    {
        *(arg1 + 0x22) = 0;
        *(arg1 + 0x21) = 0;
        *(arg1 + 0x20) = 1;
        return;
    }

    int dest_ptr = *arg2;
    *(arg1 + 0x20) = arg2[1];
    int edi_1 = dest_ptr + 0x22;
    int i = 0xffffffff;

    while (i)
    {
        bool cond:1_1 = 0 != *edi_1;
        edi_1 += 1;
        i -= 1;

        if (!cond:1_1)
            break;
    }

    int context_val = ~i;
    int esi_2;
    int edi_4;
    edi_4 = __builtin_memcpy(arg1 + 0x22, edi_1 - context_val, context_val & 0xfffffffc);
    __builtin_memcpy(edi_4, esi_2, context_val & 3);
    *(arg1 + 0x21) = 0;
}
