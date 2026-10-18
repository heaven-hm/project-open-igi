// IGI native provenance: SoundSys_AttachLipsync_8 - 0x004E7180 - CFG score 93.3% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E7180-SoundSys_AttachLipsync_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_AttachLipsync_8.hpp"
int SoundSys_AttachLipsync_8(char* context, int arg2, int arg3)

{
    char result = g_a5e60a;

    if (result)
        return result;

    unsigned int data_val = Display_CheckCapabilities_12(context);
    int data_1 = Display_RestoreDevice_12(context);
    void item_58;

    if (!arg3)
        Display_SetGammaRamp_12(&item_58);
    else
        memcpy(&item_58, arg3, 0x20);

    int data_2 = g_a5e5f8;
    g_a5e5f8 = data_2 + 1;
    int data_4 = arg2;
    data_4 |= 1;
    float* saved_index_3 = &item_58;
    int item_78 = data_2;
    return SoundSys_SetMasterVolume_8(item_78, data_val, data_1, data_4, saved_index_3);
}
