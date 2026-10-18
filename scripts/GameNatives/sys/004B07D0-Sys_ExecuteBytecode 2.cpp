// IGI native provenance: Sys_ExecuteBytecode - 0x004B07D0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B07D0-Sys_ExecuteBytecode.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_ExecuteBytecode.h"
int Sys_ExecuteBytecode(int* context, int* arg2, int* arg3, int arg4, int* arg5, int arg6, int arg7)

{
    int dest_val = arg4;
    int i = 0xffffffff;

    while (i)
    {
        bool flag_0_1 = 0 != *dest_val;
        dest_val += 1;
        i -= 1;

        if (!flag_0_1)
            break;
    }

    int i_1 = 0;

    if (~i - 1 > 0)
    {
        int dest_1 = arg7;

        do
        {
            if (Sys_AllocateMemory(&arg7, arg3, i_1 + arg4, arg5, arg6, dest_1))
            {
                if (context)
                    *context = i_1;

                if (arg2)
                    *arg2 = arg7;

                return 1;
            }

            i_1 += 1;
        } while (i_1 < ~i - 1);
    }

    return 0;
}
