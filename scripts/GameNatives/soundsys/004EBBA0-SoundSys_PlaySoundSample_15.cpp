// IGI native provenance: SoundSys_PlaySoundSample_15 - 0x004EBBA0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004EBBA0-SoundSys_PlaySoundSample_15.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_PlaySoundSample_15.hpp"
struct status_msg* SoundSys_PlaySoundSample_15(void* context, unsigned int arg2)

{
    int kind;
    kind = g_54db68;
    struct status_msg* result = QTask_HandleInput(context, kind, 0);

    if (result)
    {
        result->head[8] = 0x3f800000;
        result->head[9] = 0;
        result->head[0xa] = 0;
        result->head[0xb] = 0;
        result->head[0xc] = 0x3f800000;
        result->head[0xd] = 0;
        result->head[0xe] = 0;
        result->head[0xf] = 0;
        result->head[0x10] = 0x3f800000;
        result->head[0x11] = 0;
        result->head[0x18] = arg2;
    }

    return result;
}
