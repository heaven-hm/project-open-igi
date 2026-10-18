// IGI native provenance: Sys_AllocateMemory_17 - 0x004B64F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B64F0-Sys_AllocateMemory_17.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_AllocateMemory_17.h"
int Sys_AllocateMemory_17(int* context, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9)

{
    return Texture_UploadMipmapPacket(context[1] + arg2 * 0x24, *context, arg4, arg5, arg6, arg7, arg8, arg9, arg3);
}
