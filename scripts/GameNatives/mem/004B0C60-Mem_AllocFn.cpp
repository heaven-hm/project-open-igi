// IGI native provenance: Mem_AllocFn - 0x004B0C60 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B0C60-Mem_AllocFn.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B0C60-Mem_AllocFn.hpp"
void *Mem_AllocFn(int context, int arg2)
{
    return Mem_AllocateRawBlock((unsigned int)(context + arg2 + 0x1c));
}
