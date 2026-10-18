// IGI native provenance: Sys_FreeMemory_17 - 0x004B6540 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B6540-Sys_FreeMemory_17.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B6540-Sys_FreeMemory_17.hpp"
int Sys_FreeMemory_17(void* context, int arg2, int arg3, short arg4, short arg5, short arg6, short arg7, short arg8, int arg9, float arg10, float arg11, float arg12, float arg13)

{
    int result = arg2 * 0x24;
    *(*(context + 8) + result + 8) = arg6;
    *(*(context + 8) + result + 0xa) = arg7;
    *(*(context + 8) + result + 0xc) = arg4;
    *(*(context + 8) + result + 0xe) = arg5;
    *(*(context + 8) + result + 0x10) = arg8;
    *(*(context + 8) + result + 4) = arg9;
    (*(context + 8))[arg2 * 9] = *(context + 4) + arg3 * 0x24;
    *(*(context + 8) + result + 0x14) = arg10;
    *(*(context + 8) + result + 0x18) = arg11;
    *(*(context + 8) + result + 0x1c) = arg12;
    *(*(context + 8) + result + 0x20) = arg13;
    return result;
}
