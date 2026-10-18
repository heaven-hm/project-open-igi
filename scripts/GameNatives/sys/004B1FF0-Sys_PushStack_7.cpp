// IGI native provenance: Sys_PushStack_7 - 0x004B1FF0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B1FF0-Sys_PushStack_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B1FF0-Sys_PushStack_7.hpp"
int Sys_PushStack_7(void* context)

{
    int result = 0;

    for (int* i = Sys_ResolveSymbol_19(context); i; i = Matrix_InvalidateSlot(i))
    {
        if (Sys_OpenFileStream_20(i) == 0x48584554)
        {
            i = Matrix_InvalidateSlot(i);

            if (!i)
                return 0;

            if (Sys_OpenFileStream_20(i) != 0x59444f42)
                return 0;

            if (!Sys_CheckHeap_19(i, context))
                return 0;

            result += 1;
        }
    }

    return result;
}
