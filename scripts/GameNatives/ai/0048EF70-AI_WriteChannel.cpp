// IGI native provenance: AI_WriteChannel - 0x0048EF70 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048EF70-AI_WriteChannel.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0048EF70-AI_WriteChannel.hpp"
void* AI_WriteChannel(void* context, int arg2, float arg3)

{
    int entity_2 = *(*((*(context + 0x3d38) << 2) + &g_54151c) + (arg2 << 2));

    if (entity_2 != 0xffffffff)
    {
        *(context + (entity_2 << 2) + 0x3c74) = arg3;
        return context;
    }

    int item_104 = arg2;
    void* item_108 = context + 0x3d15;
    int item_10c_1 = Script_EvaluateCondition_4(context);
    void item_100;
    return sprintf(&item_100, "AI(%d) type(%s) channel(%d)");
}
