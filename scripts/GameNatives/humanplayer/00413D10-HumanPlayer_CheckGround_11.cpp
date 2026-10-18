// IGI native provenance: HumanPlayer_CheckGround_11 - 0x00413D10 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 00413D10-HumanPlayer_CheckGround_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_CheckGround_11.hpp"
void* HumanPlayer_CheckGround_11(int context, int arg2, int arg3, int arg4, int arg5, int arg6, void* arg7, int arg8, int arg9)

{
    if (*(arg7 + 0x6d4) == 0x181)
        return arg7;

    int item_30 = context;
    int item_28_1 = arg3;
    int item_20_1 = arg5;
    int item_c_1 = arg8;
    int item_2c_1 = arg2;
    int item_4_1 = arg9;
    int entity_6 = 0;
    entity_6 = g_5391ae;
    int item_24_1 = arg4;
    int item_1c_1 = arg6;
    int source_1 = 0;
    source_1 = *(arg7 + 0x6d4);
    void* item_10_1 = arg7;
    int item_8_1 = *(arg7 + 0x6e0);
    int item_14_1 = 0;
    int item_18_1 = 0;
    return (*(((entity_6 * 0x180 + source_1) << 2) + &g_a96ae0))(0, &item_30);
}
