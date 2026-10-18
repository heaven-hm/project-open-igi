// IGI native provenance: SoundSys_LoadSoundDef_9 - 0x004E75C0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E75C0-SoundSys_LoadSoundDef_9.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_LoadSoundDef_9.hpp"
void* SoundSys_LoadSoundDef_9(void* context, int arg2, int arg3)

{
    *(context + 0x18) = arg2;
    *(context + 0x1c) = arg3;
    return context;
}
