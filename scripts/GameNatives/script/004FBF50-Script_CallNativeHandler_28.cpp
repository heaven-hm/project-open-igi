// IGI native provenance: Script_CallNativeHandler_28 - 0x004FBF50 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004FBF50-Script_CallNativeHandler_28.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004FBF50-Script_CallNativeHandler_28.hpp"
int Script_CallNativeHandler_28(void* context, float arg2 , float arg3, int arg4, float arg5 )

{
    return Script_SpawnNewTask_30(context + 0xf0, arg2, arg3, *(context + arg4 * 0x38 + 0x1dc), arg5);
}
