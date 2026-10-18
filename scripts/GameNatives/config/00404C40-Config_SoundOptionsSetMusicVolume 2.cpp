// IGI native provenance: Config_SoundOptionsSetMusicVolume - 0x00404C40 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 00404C40-Config_SoundOptionsSetMusicVolume.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Config_SoundOptionsSetMusicVolume.h"
int Config_SoundOptionsSetMusicVolume(long double context , int arg2, float arg3 )

// Sig: void Config_SoundOptionsSetMusicVolume(float volume)
// Note: Sets music volume for the active profile. Param: volume = float 0.0-1.0
// (pops a float arg - pass a float!).
{
    Profile_ValueSet(arg3, 0.0f);
    *(Config_SaveGameOptions() + 4) = context;
    int item_18 = 0;
    int item_10 = 0;
    int item_c = 0;
    char* item_8 = &g_567c74;
    memcpy(arg2, &item_18, 0x18);
    return arg2;
}
