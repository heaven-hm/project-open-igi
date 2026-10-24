// IGI native provenance: DynObj_RemoveSymbol_11 - 0x004CE400 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004CE400-DynObj_RemoveSymbol_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "DynObj_RemoveSymbol_11.h"
int DynObj_RemoveSymbol_11(int context, int arg2)

{
    return *(context + (arg2 << 2) + 0xd3c);
}
