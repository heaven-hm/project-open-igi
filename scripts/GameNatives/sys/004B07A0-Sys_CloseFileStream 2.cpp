// IGI native provenance: Sys_CloseFileStream - 0x004B07A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B07A0-Sys_CloseFileStream.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_CloseFileStream.h"
int Sys_CloseFileStream(int context, int arg2, int arg3)

{
    if (arg2 < 0)
        return 0xffffffff;

    if (!arg3)
        return *(context + arg2 * 0x14 + 0x10);

    return *(context + arg2 * 0x14 + 0xc);
}
