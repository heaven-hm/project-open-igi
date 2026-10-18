// IGI native provenance: Sys_FreeMemory_19 - 0x004B7330 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B7330-Sys_FreeMemory_19.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B7330-Sys_FreeMemory_19.hpp"
int* Sys_FreeMemory_19(int context)

{
    int* result = Mem_Alloc(context * 0xc + 8, 4);
    result[1] = context;
    *result = 0;
    return result;
}
