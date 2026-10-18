// IGI native provenance: Sys_OpenFileStream - 0x004B0690 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B0690-Sys_OpenFileStream.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_OpenFileStream.h"
int Sys_OpenFileStream(int context, int* arg2, int arg3, int arg4, int arg5, int arg6)

{
    int data_val = *arg2;

    if (data_val)
    {
        int entity_1 = data_val * 5;
        void* entity_2 = context + (entity_1 << 2);

        if (*(context + (entity_1 << 2) - 8) == arg3 && *(entity_2 - 0xc) == arg4 - 1
            && *(entity_2 - 0x14) == arg6)
        {
            *(entity_2 - 4) += 1;
            int result = *(entity_2 - 0xc) + 1;
            *(entity_2 - 0xc) = result;
            return result;
        }
    }

    int* entity_4 = context + data_val * 0x14;
    entity_4[4] = 0;
    entity_4[3] = arg3;
    entity_4[2] = arg4;
    entity_4[1] = arg5;
    *entity_4 = arg6;

    if (g_93aad8 <= data_val + 1)
        g_93aad8 = data_val + 1;

    *arg2 = data_val + 1;
    return data_val + 1;
}
