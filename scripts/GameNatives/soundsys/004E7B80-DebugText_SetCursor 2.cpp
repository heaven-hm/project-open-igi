// IGI native provenance: DebugText_SetCursor - 0x004E7B80 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E7B80-DebugText_SetCursor.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "DebugText_SetCursor.h"
int DebugText_SetCursor(int context, int arg2)

{
    char result = g_a5ea75;

    if (!result)
        return result;

    return SoundSys_LoadSoundDef_9(g_a5ebd0, context, arg2);
}
