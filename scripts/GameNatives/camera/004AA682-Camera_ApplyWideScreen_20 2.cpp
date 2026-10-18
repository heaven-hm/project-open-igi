// IGI native provenance: Mem_AllocWithRetry - 0x004AA682 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004AA682-Mem_AllocWithRetry.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Mem_AllocWithRetry.h"
void* Mem_AllocWithRetry(int* context, int arg2)

{
    if (context <= 0xffffffe0)
    {
        bool flag_1_1;

        do
        {
            void* result = Mem_AllocateRawBlock(context);

            if (result || arg2 == result)
                return result;

            flag_1_1 = Mem_InvokeNewHandler(context);
        } while (flag_1_1);
    }

    return nullptr;
}
