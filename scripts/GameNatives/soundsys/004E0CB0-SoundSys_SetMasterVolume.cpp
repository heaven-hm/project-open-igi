// IGI native provenance: SoundSys_SetMasterVolume - 0x004E0CB0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E0CB0-SoundSys_SetMasterVolume.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_SetMasterVolume.hpp"
unsigned int SoundSys_SetMasterVolume()

{
    int data_val;
    data_val = g_a5e36c;
    return QTask_CloseEvent_2(data_val);
}
