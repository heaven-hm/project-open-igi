// IGI native provenance: Sys_PushStack_3 - 0x004B1180 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B1180-Sys_PushStack_3.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B1180-Sys_PushStack_3.hpp"
int Sys_PushStack_3()

{
    int result = g_94396c;
    *(result * 0x8c + &g_943704) = result;
    g_94396c = result + 1;
    return result;
}
