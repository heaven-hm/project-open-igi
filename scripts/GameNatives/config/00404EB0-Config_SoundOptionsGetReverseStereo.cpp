// IGI native provenance: Config_SoundOptionsGetReverseStereo - 0x00404EB0 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 00404EB0-Config_SoundOptionsGetReverseStereo.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00404EB0-Config_SoundOptionsGetReverseStereo.hpp"
int Config_SoundOptionsGetReverseStereo(int context)

// Sig: int Config_SoundOptionsGetReverseStereo()
// Note: Returns 1 if stereo is reversed for the active profile, else 0.
{
    int item_18 = *(Config_SaveGameOptions() + 0xf);
    char* item_8 = &g_567c74;
    double item_10 = *(Config_SaveGameOptions() + 0xf);
    memcpy(context, &item_18, 0x18);
    return context;
}
