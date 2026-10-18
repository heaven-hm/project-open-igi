// IGI native provenance: Sys_AllocateMemory_2 - 0x004B0A80 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B0A80-Sys_AllocateMemory_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B0A80-Sys_AllocateMemory_2.hpp"
int Sys_AllocateMemory_2(int context, int arg2, char* arg3, int arg4)

{
    char* entity_val = nullptr;
    int result = arg2 - 1;

    if (result <= 0)
    {
        *context = 0;
        return result;
    }

    char* context_val = arg3;

    do
    {
        result = *context_val;

        if (!result)
            break;

        if (result != 0x2f)
        {
            entity_val[context] = result;
            entity_val = &entity_val[1];
        }
        else
        {
            result = context_val[1];
            context_val = &context_val[1];

            if (result < 0x30 || result > 0x39)
            {
                entity_val[context] = result;
                entity_val = &entity_val[1];
            }
            else
            {
                Sys_CheckHeap(&entity_val[context], arg2 - entity_val, arg4, result - 0x30);
                int dest_1 = context;
                int i = 0xffffffff;

                while (i)
                {
                    bool flag_0_1 = 0 != *dest_1;
                    dest_1 += 1;
                    i -= 1;

                    if (!flag_0_1)
                        break;
                }

                entity_val = ~i - 1;
            }
        }

        result = arg2 - 1;
        context_val = &context_val[1];
    } while (entity_val < result);

    entity_val[context] = 0;
    return result;
}
