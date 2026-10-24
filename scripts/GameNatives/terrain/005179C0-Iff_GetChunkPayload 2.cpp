// IGI native provenance: Iff_GetChunkPayload - 0x005179C0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 005179C0-Iff_GetChunkPayload.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Iff_GetChunkPayload.h"
int Iff_GetChunkPayload(void* context)

{
    return *(context + 0x1c);
}
