// IGI native provenance: SoundSys_PlaySoundSample_5 - 0x004E6550 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E6550-SoundSys_PlaySoundSample_5.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_PlaySoundSample_5.hpp"
struct status_msg* SoundSys_PlaySoundSample_5(void* context, int arg2)

{
    int kind;
    kind = g_54880e;
    struct status_msg* result = QTask_HandleInput(context, kind, arg2);
    int dest_val = arg2;
    int i = 0xffffffff;

    while (i)
    {
        bool flag_0_1 = 0 != *dest_val;
        dest_val += 1;
        i -= 1;

        if (!flag_0_1)
            break;
    }

    char entity_3 = *(~i - 1 + arg2 - 1);
    char* item_418_1;
    char* item_414_1;
    void item_400;

    if (entity_3 == 0x2f || entity_3 == 0x5c)
    {
        int item_410_2 = arg2;
        item_414_1 = "%ssounds.qsc";
        item_418_1 = &item_400;
    }
    else
    {
        int item_410_1 = arg2;
        item_414_1 = "%s/sounds.qsc";
        item_418_1 = &item_400;
    }

    Game_DataSymbolLoad(item_418_1, item_414_1);
    SoundSys_Update3DPosition_5(result, &item_400);
    return result;
}
