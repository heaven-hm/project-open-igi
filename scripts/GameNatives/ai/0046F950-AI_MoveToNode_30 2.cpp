// IGI native provenance: AI_MoveToNode_30 - 0x0046F950 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0046F950-AI_MoveToNode_30.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_MoveToNode_30.h"
void* AI_MoveToNode_30(int context, char arg2)

{
    void* result = Table_GetAt(context + 0x1c4, 0);

    if (!result)
        return result;

    char item_2f_1 = 0;
    char item_30 = arg2;
    int item_2c_1 = 0;
    int item_28_1 = 0;
    int item_24_1 = 0;
    int item_20_1 = 0;
    int item_10_1 = 0;
    int item_c_1 = 0;
    int item_8_1 = 0x3f800000;
    int item_4_1 = 0;
    unsigned int data_2 = AI_AlertAllies_17();
    int data_3 = 0;
    data_3 = *(result + 0x1c);
    return (*(((data_2 * 0x180 + data_3) << 2) + &g_a96ae0))(result, &item_30);
}
