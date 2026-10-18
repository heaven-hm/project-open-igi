// IGI native provenance: StatusMessage_Init_2 - 0x00485220 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00485220-StatusMessage_Init_2.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00485220-StatusMessage_Init_2.hpp"
int StatusMessage_Init_2(void* self)

{
    unsigned int local_10_1;
    unsigned int index;
    unsigned int result;
    unsigned int status;

    if (!g_5c8e00)
    {
        local_10_1 = 0x3f800000;
        index = 0x3f800000;
        result = 0;
        status = 0;
    }
    else
    {
        strncpy(&local_10_1, "333?333?", 8);
        result = 0x3e99999a;
        status = 0x3e99999a;
    }

    int local_2c = 0;
    unsigned int result_val_1 = Level_LoadLevel_6() + 0x60;
    unsigned int result_val_2 = Level_StartMission_4();
    struct status_msg* result_val_5 = SoundSys_AttachLipsync_11(self, status, result, index, local_10_1,
        result_val_2, result_val_1, 0, 0, 0, 0x3f800000);
    *(self + 0x24) = result_val_5;
    FadeOutEnd_OnEvent(result_val_5);
    TerraDrawTask_OnEvent(result_val_5, Level_StartMission_4());
    GlowTask_OnEvent(result_val_5);
    FadeOutEnd_Process(result_val_5);
    data_5c8a20 = FX_FragmentSimulate_8(self);
    return SoundSys_AttachLipsync_10(self, 1);
}
