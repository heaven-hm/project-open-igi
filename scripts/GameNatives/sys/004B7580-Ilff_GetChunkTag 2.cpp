// IGI native provenance: Ilff_GetChunkTag - 0x004B7580 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B7580-Sys_OpenFileStream_20.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_OpenFileStream_20.h"
int Ilff_GetChunkTag(int* context)

{
    if (!context)
        return 0;

    return *context;
}
