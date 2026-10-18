// IGI native provenance: AI_UpdateOrders_26 - 0x00469CC0 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 00469CC0-AI_UpdateOrders_26.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_UpdateOrders_26.hpp"
double* AI_UpdateOrders_26(void* context, void* arg2)

{
    int entity_val = 0;
    int item_8 = 0;
    entity_val = *(arg2 + 0x1c);
    double item_58[0xa];
    (*((entity_val << 2) + &g_a994e0))(arg2, &item_58);
    return AI_AlertAllies_26(context, &item_58);
}
