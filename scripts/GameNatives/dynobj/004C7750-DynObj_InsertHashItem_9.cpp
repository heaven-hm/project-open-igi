// IGI native provenance: DynObj_InsertHashItem_9 - 0x004C7750 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004C7750-DynObj_InsertHashItem_9.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004C7750-DynObj_InsertHashItem_9.hpp"
int DynObj_InsertHashItem_9(int context, int arg2)

{
    return *(context + (arg2 << 2) + 0x10);
}
