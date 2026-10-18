// IGI native provenance: Config_SoundOptionsGetSoundsEffects - 0x00404A90 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 00404A90-Config_SoundOptionsGetSoundsEffects.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00404A90-Config_SoundOptionsGetSoundsEffects.hpp"
int Config_SoundOptionsGetSoundsEffects(int context)

// Sig: int Config_SoundOptionsGetSoundsEffects()
// Note: Returns 1 if sound effects are enabled for the active profile, else 0.
{
    int item_18 = *(Config_SaveGameOptions() + 0xc);
    char* item_8 = &g_567c74;
    double item_10 = *(Config_SaveGameOptions() + 0xc);
    memcpy(context, &item_18, 0x18);
    return context;
}
