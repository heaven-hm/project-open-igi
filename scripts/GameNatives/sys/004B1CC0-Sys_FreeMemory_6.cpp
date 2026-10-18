// IGI native provenance: Sys_FreeMemory_6 - 0x004B1CC0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B1CC0-Sys_FreeMemory_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B1CC0-Sys_FreeMemory_6.hpp"
int Sys_FreeMemory_6(int context, int arg2)

{
    int result = context * 0x23;
    *((result << 2) + &g_9436cc) = arg2;
    return result;
}
