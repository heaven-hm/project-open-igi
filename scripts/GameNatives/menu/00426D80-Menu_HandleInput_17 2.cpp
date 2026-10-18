// IGI native provenance: Menu_HandleInput_17 - 0x00426D80 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00426D80-Menu_HandleInput_17.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Menu_HandleInput_17.h"
int Menu_HandleInput_17(void* context, char arg2)

{
    *(context + 0x140) = arg2;
    return arg2;
}
