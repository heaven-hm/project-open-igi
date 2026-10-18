// IGI native provenance: HumanPlayer_HandleInput - 0x004083C0 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004083C0-HumanPlayer_HandleInput.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_HandleInput.hpp"
int HumanPlayer_HandleInput(short context, int arg2, int arg3, int arg4, int* arg5)

{
    int item_18 = arg3;
    int data_3 = 0;
    data_3 = g_53851a;
    return (*(((data_3 * 0x180 + context) << 2) + &g_a96ae0))(0, &item_18, item_18, arg2, *arg5,
        arg5[1], arg5[2], arg4);
}
