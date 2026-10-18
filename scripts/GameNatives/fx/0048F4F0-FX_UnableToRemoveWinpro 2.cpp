// IGI native provenance: FX_UnableToRemoveWinpro - 0x0048F4F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048F4F0-FX_UnableToRemoveWinpro.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "FX_UnableToRemoveWinpro.h"
int* FX_UnableToRemoveWinpro(int context)

{
    int* i_1 = g_5c8df0;

    for (int* i = *i_1; i; i = *i)
    {
        if (i_1[2] == context)
        {
            int* result = HeapList_Remove(i_1);
            i_1[2] = 0;
            return result;
        }

        i_1 = i;
    }

    Error_Show("Unable to remove WinProc callback");

    while (true)
        /* nop */
}
