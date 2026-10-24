// IGI native provenance: Texture_GetActiveBackendId - 0x004B0F80 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B0F80-Sys_ReadFileBlock_3.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_ReadFileBlock_3.h"
int Texture_GetActiveBackendId()

{
    return g_a94a64;
}
