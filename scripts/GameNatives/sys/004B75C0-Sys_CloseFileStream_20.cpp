// IGI native provenance: Sys_CloseFileStream_20 - 0x004B75C0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B75C0-Sys_CloseFileStream_20.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B75C0-Sys_CloseFileStream_20.hpp"
int* Sys_CloseFileStream_20(void* context, void* arg2)

{
    Sys_PopStack_17(arg2, 1);
    int* result = Sys_DispatchOpcode_20(*(context + 0x10), *(context + 0x14), arg2);
    int i = 0;
    int entity_1 = 0;

    if (*(context + 0x14) <= 0)
        return result;

    int context_val;
    int item_10 = context_val;

    do
    {
        int j = 0;

        if (*(context + 0x10) > 0)
        {
            char* temp_4 = (entity_1 << 4) + context + 0x24;

            do
            {
                char context_1 = *temp_4;
                temp_4 = &temp_4[0x10];
                *(entity_1 + result + 0x10) = context_1;
                entity_1 += 1;
                j += 1;
            } while (j < *(context + 0x10));
        }

        i += 1;
    } while (i < *(context + 0x14));

    return result;
}
