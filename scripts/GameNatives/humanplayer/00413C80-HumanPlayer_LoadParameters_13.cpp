// IGI native provenance: HumanPlayer_LoadParameters_13 - 0x00413C80 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00413C80-HumanPlayer_LoadParameters_13.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_LoadParameters_13.hpp"
int HumanPlayer_LoadParameters_13(int context, int arg2, int arg3, void* arg4)

{
    int result = arg3;
    int entity_val = 0;
    entity_val = g_5391b1;
    int source_val = 0;
    source_val = *(arg4 + 0x1c);
    result = 0;
    int entity_4 = *(((entity_val * 0x180 + source_val) << 2) + &g_a96ae0);

    if (!entity_4)
        return 0;

    entity_4(arg4, &result);
    return result;
}
