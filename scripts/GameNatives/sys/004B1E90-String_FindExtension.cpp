// IGI native provenance: String_FindExtension - 0x004B1E90 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B1E90-String_FindExtension.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B1E90-String_FindExtension.hpp"
char* String_FindExtension(char* context, int arg2, int arg3)

{
    int dest_val = arg3;
    int i = 0xffffffff;

    while (i)
    {
        bool flag_0_1 = 0 != *dest_val;
        dest_val += 1;
        i -= 1;

        if (!flag_0_1)
            break;
    }

    int entity_val = ~i;
    char* entity_1 = entity_val - 1;

    if (entity_val != 1)
    {
        while (entity_1[arg3] != 0x2e)
        {
            char* f1_1 = entity_1;
            entity_1 -= 1;

            if (f1_1 == 1)
                break;
        }
    }

    char* result = &entity_1[arg3];

    if (!context)
        return result;

    Camera_ApplyLineFilter_5(context, result, arg2 - 1);
    context[arg2 - 1] = 0;
    return result;
}
