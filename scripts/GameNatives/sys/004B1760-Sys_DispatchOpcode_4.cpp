// IGI native provenance: Sys_DispatchOpcode_4 - 0x004B1760 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004B1760-Sys_DispatchOpcode_4.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B1760-Sys_DispatchOpcode_4.hpp"
int Sys_DispatchOpcode_4(int* context)

{
    int entity_val = *context;
    return (*(entity_val * 0x8c + &g_9436d4))(entity_val, context);
}
