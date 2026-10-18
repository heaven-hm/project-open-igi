// IGI native provenance: HumanPlayer_LoadParameters_8 - 0x0040E620 - CFG score 94.4% (PASS; structural only); runtime behavior not verified.
// Evidence: 0040E620-HumanPlayer_LoadParameters_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_LoadParameters_8.h"
char HumanPlayer_LoadParameters_8(void* context, float arg2)

{
    FallLanding(arg2);
    char result = *(context + 8);

    if (result & 4)
    {
        memset(arg2 + 0x664, 0, 0x18);

        if (result & 1)
        {
            void* data_val = Table_GetAt(arg2 + 0x4c4, 0);

            if (data_val)
            {
                Script_ParseEnumInt32_13(data_val);
                Script_ExecuteLevelScript_12(arg2 + 0x4c4, nullptr, nullptr);
                void* entity_1 = *(arg2 + 0xcfc);
                *(arg2 + 0xd04) = *(arg2 + 0xd08);
                *(arg2 + 0xd08) = 1;
                int data_2 = *(entity_1 + 4);
                *(arg2 + 0xd0c) = data_2;
                data_2(arg2 + 0xcf8, arg2);
            }

            result = Script_EvaluateCondition_12(arg2);
            *(context + 0x14) = sub_45c740;
        }
    }

    return result;
}
