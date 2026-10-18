// IGI native provenance: SoundSys_LoadSoundDef_11 - 0x004E80D0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E80D0-SoundSys_LoadSoundDef_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_LoadSoundDef_11.h"
unsigned int SoundSys_LoadSoundDef_11()

{
    Display_ClearSurfaces_3(ViewportQTask_Process);
    unsigned int result = QTask_CloseEvent_2(g_54d98c);
    g_a5ec64 = 0;
    return result;
}
