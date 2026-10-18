// IGI native provenance: Matrix_InvalidateSlot - 0x004B7470 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B7470-Matrix_InvalidateSlot.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Matrix_InvalidateSlot.h"
void* Matrix_InvalidateSlot(void* context)

{
    if (context)
    {
        void* data_1 = *(context + 0xc);

        if (data_1)
            return data_1 + context;
    }

    return 0;
}
