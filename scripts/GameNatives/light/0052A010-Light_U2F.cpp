// IGI native provenance: Light_U2F - 0x0052A010 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0052A010-Light_U2F.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Light_U2F.hpp"
int Light_U2F(int context, int arg2, int arg3, void* arg4)

{
    int item_4 = arg3;
    item_4 = g_54f494;
    DebugText_SetCursor(0x14, 5);
    void* item_14 = arg4 + 0x1a8;
    SoundSys_Update3DPosition_11("%s");
    DebugText_SetCursor(0x14, 6);
    int item_24 = *(arg4 + 0x1b8);
    SoundSys_Update3DPosition_11("A:%d");
    DebugText_SetCursor(0x14, 7);
    int item_34 = *(arg4 + 0x1bc);
    SoundSys_Update3DPosition_11("B:%d");
    DebugText_SetCursor(0x14, 8);
    SoundSys_Update3DPosition_11("U:%.2f");
    DebugText_SetCursor(0x14, 9);
    SoundSys_Update3DPosition_11("U:%.2f");
    bool data_3 = *(arg4 + 0x215);
    item_4 += *(arg4 + 0x214) * 0xe0;
    bool flag_0 = *(arg4 + 0x216);
    *(&item_4 + 1) += data_3 * 0xe0;
    *(&item_4 + 2) += flag_0 * 0xe0;
    DebugText_SetCursor(0x14, 0xa);
    int* item_20 = &item_4;
    SoundSys_Update3DPosition_11("L:%s");
    int result = DebugText_SetCursor(0x14, 0xb);
    int source_1 = *(arg4 + 0x218);

    if (source_1 > 3)
        return result;

    switch (source_1)
    {
        case 0:
        {
            return SoundSys_Update3DPosition_11("M:World");
            break;
        }
        case 1:
        {
            return SoundSys_Update3DPosition_11("M:Object");
            break;
        }
        case 2:
        {
            return SoundSys_Update3DPosition_11("M:Speed");
            break;
        }
        case 3:
        {
            return SoundSys_Update3DPosition_11("M:Angles");
            break;
        }
    }
}
