// IGI native provenance: Script_ExecuteLevelScript_28 - 0x004FBF00 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004FBF00-Script_ExecuteLevelScript_28.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Script_ExecuteLevelScript_28.h"
float Script_ExecuteLevelScript_28(void* context, float arg2, float* arg3, float* arg4, int arg5, int arg6) {
    int entity_1 = arg6 * 7;
    int item_4 = entity_1;
    int item_8 = arg5;
    return Script_FormatString_29(context + 0xf0, context + 0x70, arg2, arg3, *arg4, *(context + (entity_1 << 3) + 0x1c4));
}
