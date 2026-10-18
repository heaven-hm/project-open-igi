// IGI native provenance: Sys_PushStack_29 - 0x004BDA00 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004BDA00-Sys_PushStack_29.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_PushStack_29.h"
int Sys_PushStack_29(void* context, int arg2)

{
    int entity_1 = *(context + 0x1c) - 1;
    *(context + 0x18) -= 1;
    int temp_2 = *(context + 0x28);
    *(context + 0x1c) = entity_1;
    *(temp_2 + (entity_1 << 2)) = 1;
    *(*(context + 0x24) + (*(context + 0x18) << 2)) = arg2;
    return arg2;
}
