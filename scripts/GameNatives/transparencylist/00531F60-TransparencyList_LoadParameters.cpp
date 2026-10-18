// IGI native provenance: TransparencyList_LoadParameters - 0x00531F60 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00531F60-TransparencyList_LoadParameters.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "TransparencyList_LoadParameters.hpp"
int TransparencyList_LoadParameters(void* context)

{
    DynObj_RemoveSymbol_7(*(context + 0x68), context, 0xffffffff);
    Sys_ExecuteBytecode_2(*(context + 0x74));
    void* temp_val = *(context + 0x78);
    *(context + 0x74) = 0;
    int result = Sys_ExecuteBytecode_2(temp_val);
    *(context + 0x78) = 0;
    return result;
}
