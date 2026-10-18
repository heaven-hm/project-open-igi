// IGI native provenance: Sys_ExecuteBytecode_26 - 0x004BB110 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004BB110-Sys_ExecuteBytecode_26.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_ExecuteBytecode_26.h"
char* Sys_ExecuteBytecode_26(int context, void* arg2)

{
    char* result = nullptr;
    char* source_val = arg2 + 1;

    if (*(arg2 + 1) == 0x22)
    {
        *context = 0;
        return 0;
    }

    int context_val;
    int item_8 = context_val;
    char i;

    do
    {
        i = *source_val;

        if (i != 0x5c)
            result[context] = i;
        else
        {
            char context_1 = source_val[1];
            source_val = &source_val[1];
            int entity_val = context_1;

            if (entity_val == 0x6e)
                result[context] = 0xa;
            else if (entity_val == 0x72)
                result[context] = 0xd;
            else if (entity_val == 0x74)
                result[context] = 9;
            else
                result[context] = context_1;
        }

        i = source_val[1];
        result = &result[1];
        source_val = &source_val[1];
    } while (i != 0x22);
    result[context] = 0;
    return result;
}
