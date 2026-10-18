// IGI native provenance: Sys_FreeMemory_29 - 0x004BD3F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004BD3F0-Sys_FreeMemory_29.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_FreeMemory_29.h"
int Sys_FreeMemory_29(int context, int arg2, int arg3, int arg4)

{
    if (context >= 0 && context < 0x31 && arg2 >= 0 && arg2 < 4)
    {
        int entity_3 = *(((arg2 + context * 6) << 2) + &g_a96108);

        if (entity_3)
            return entity_3(arg3, arg4, context, arg2);
    }

    return context;
}
