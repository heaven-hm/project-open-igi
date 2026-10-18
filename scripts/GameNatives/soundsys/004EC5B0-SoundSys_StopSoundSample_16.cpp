// IGI native provenance: SoundSys_StopSoundSample_16 - 0x004EC5B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004EC5B0-SoundSys_StopSoundSample_16.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_StopSoundSample_16.hpp"
void* SoundSys_StopSoundSample_16(void* context, int arg2)

{
    *(context + 0x58) = arg2;

    if (arg2 <= 0)
    {
        *(context + 0x54) = 0;
        return arg2;
    }

    void* data_2 = Mem_Alloc(arg2 * 0x18, 4);
    *(context + 0x54) = data_2;
    return data_2;
}
