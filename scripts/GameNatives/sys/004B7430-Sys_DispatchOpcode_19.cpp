// IGI native provenance: Sys_DispatchOpcode_19 - 0x004B7430 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B7430-Sys_DispatchOpcode_19.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B7430-Sys_DispatchOpcode_19.hpp"
int Sys_DispatchOpcode_19(void* context, int arg2)

{
    int result = *(context + 8);
    *(context + 8) = result + 1;
    *(context + (result << 2) + 0x14) = arg2;
    return result;
}
