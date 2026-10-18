// IGI native provenance: SoundSys_Update3DPosition_15 - 0x004EBBF0 - CFG score 75.0% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004EBBF0-SoundSys_Update3DPosition_15.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_Update3DPosition_15.hpp"
int SoundSys_Update3DPosition_15(int context, int arg2, int arg3)

{
    int item_18;
    memset(&item_18, 0, 0x18);
    memcpy(context + 0x20, arg2, 0x28);
    memcpy(context + 0x48, arg3, 0x18);
    Display_CloseDevice_12(arg2, arg3, &item_18);
    return arg3;
}
