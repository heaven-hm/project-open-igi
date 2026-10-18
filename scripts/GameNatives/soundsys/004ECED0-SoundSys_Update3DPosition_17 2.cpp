// IGI native provenance: SoundSys_Update3DPosition_17 - 0x004ECED0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004ECED0-SoundSys_Update3DPosition_17.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_Update3DPosition_17.h"
void* SoundSys_Update3DPosition_17(int* context, float arg2, float arg3, float arg4, float arg5)

{
    long double fpu7 = arg3 * arg3;
    int entity_val;
    int item_4 = entity_val;
    arg4 = arg4 * arg4;
    arg3 = arg5 * arg5;
    int item_8 = entity_val;
    int item_c = entity_val;
    void* result = SoundSys_LoadSoundDef_16(context, arg2, (arg3 + arg4) * arg2 * 0.0833333358f,
        (arg3 + fpu7) * arg2 * 0.0833333358f, (arg4 + fpu7) * arg2 * 0.0833333358f);
    return result;
}
