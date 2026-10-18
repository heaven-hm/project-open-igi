// IGI native provenance: Mem_AllocChunk - 0x004AA670 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004AA670-Mem_AllocChunk.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004AA670-Mem_AllocChunk.hpp"
void* Mem_AllocChunk(int* context)

{
    return Mem_AllocWithRetry(context, g_936250);
}
