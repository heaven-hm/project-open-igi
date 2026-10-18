// IGI native provenance: Sys_AllocateMemory_7 - 0x004B1E00 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B1E00-Sys_AllocateMemory_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B1E00-Sys_AllocateMemory_7.hpp"
char* Sys_AllocateMemory_7(char* context, int arg2, char* arg3)

{
    int source_val = 0;
    char i = *arg3;

    while (i)
    {
        if (i == 0x3a)
        {
            if (context)
            {
                Camera_ApplyLineFilter_5(context, &arg3[source_val + 1], arg2);
                context[arg2 - 1] = 0;
            }

            return &arg3[source_val + 1];
        }

        i = arg3[source_val + 1];
        source_val += 1;
    }

    if (context)
    {
        Camera_ApplyLineFilter_5(context, arg3, arg2);
        context[arg2 - 1] = 0;
    }

    return arg3;
}
