// IGI native provenance: PhysicsTerrain_Invoke5 - 0x004EF7F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004EF7F0-PhysicsTerrain_Invoke5.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "PhysicsTerrain_Invoke5.h"

int* PhysicsTerrain_Invoke5(int arg1, int* arg2)

{
    void* dest_ptr = Script_ParseEnumString_5(arg1);
    int i = 0xffffffff;

    while (i)
    {
        bool cond:0_1 = 0 != *dest_ptr;
        dest_ptr += 1;
        i -= 1;

        if (!cond:0_1)
            break;
    }

    int context_val = ~i;
    int esi_1;
    int edi_3;
    edi_3 = __builtin_memcpy(*arg2, dest_ptr - context_val, context_val & 0xfffffffc);
    __builtin_memcpy(edi_3, esi_1, context_val & 3);
    return arg2;
}
