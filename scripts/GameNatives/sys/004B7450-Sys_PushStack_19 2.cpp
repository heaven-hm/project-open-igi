// IGI native provenance: Matrix_InvalidateSlot - 0x004B7450 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B7450-Matrix_InvalidateSlot.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Matrix_InvalidateSlot.h"
int Matrix_InvalidateSlot(int context, int arg2)

{
    *(context + (arg2 << 2) + 0x14) = 0;
    return arg2;
}
