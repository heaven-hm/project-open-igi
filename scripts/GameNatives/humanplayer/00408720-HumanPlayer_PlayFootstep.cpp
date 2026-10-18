// IGI native provenance: HumanPlayer_PlayFootstep - 0x00408720 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00408720-HumanPlayer_PlayFootstep.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_PlayFootstep.hpp"
int HumanPlayer_PlayFootstep(void* context)

{
    int item_8;
    int data_val = Script_ParseEnumReal32_13(context, &item_8);
    int item_4;
    int temp_1 = item_8 + item_4;

    if (temp_1)
        return 1;

    return *(HumanPlayer_LoadParameters_13(data_val, temp_1, item_4, context) + 0x6dc);
}
