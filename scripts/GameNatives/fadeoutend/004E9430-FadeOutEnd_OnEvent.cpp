// IGI native provenance: FadeOutEnd_OnEvent - 0x004E9430 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E9430-FadeOutEnd_OnEvent.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004E9430-FadeOutEnd_OnEvent.hpp"
struct status_msg* FadeOutEnd_OnEvent(void* context)

{
    int kind;
    kind = g_a5ec70;
    struct status_msg* result = QTask_HandleInput(context, kind, 0);
    SoundSys_StopSoundSample_13(result);
    return result;
}
