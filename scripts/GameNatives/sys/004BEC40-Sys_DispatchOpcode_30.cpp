// IGI native provenance: Sys_DispatchOpcode_30 - 0x004BEC40 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004BEC40-Sys_DispatchOpcode_30.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004BEC40-Sys_DispatchOpcode_30.hpp"
int Sys_DispatchOpcode_30(void* context, int arg2)

{
    if (arg2 < *(context + 8))
    {
        int entity_2 = *(*(context + 0x10) + (arg2 << 2));

        if (entity_2 < *(context + 0xc))
            return *(context + 0x14) + entity_2;
    }

    return 0;
}
