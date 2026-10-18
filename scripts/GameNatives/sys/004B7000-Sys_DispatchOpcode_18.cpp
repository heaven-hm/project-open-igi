// IGI native provenance: Sys_DispatchOpcode_18 - 0x004B7000 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B7000-Sys_DispatchOpcode_18.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B7000-Sys_DispatchOpcode_18.hpp"
int* Sys_DispatchOpcode_18(int* context)

{
    int* result = Mem_Alloc(0x18, 4);

    if (Sys_IlfFileCorruptS(result, context))
        return result;

    Sys_ExecuteBytecode_2(result);
    return 0;
}
