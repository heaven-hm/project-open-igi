// IGI native provenance: Human_IsActivePlayer - 0x00463150 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00463150-AI_CheckHearing_19.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_CheckHearing_19.h"
int Human_IsActivePlayer(int context)

{
    int result = Level_InitTextures_5(0);
    result = context == result;
    return result;
}
