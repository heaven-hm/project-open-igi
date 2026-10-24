// IGI native provenance: DynObj_CreateMagicObject - 0x004C0240 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004C0240-DynObj_CreateMagicObject.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004C0240-DynObj_CreateMagicObject.hpp"
int DynObj_CreateMagicObject(int* context)

{
    return *(*context + 4);
}
