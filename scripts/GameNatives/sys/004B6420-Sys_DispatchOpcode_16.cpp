// IGI native provenance: Sys_DispatchOpcode_16 - 0x004B6420 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B6420-Sys_DispatchOpcode_16.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B6420-Sys_DispatchOpcode_16.hpp"
int* Sys_DispatchOpcode_16(char* context, int arg2, int* arg3)

{
    return Resource_CreateNode(context, 0, arg3, arg2, 3, nullptr);
}
