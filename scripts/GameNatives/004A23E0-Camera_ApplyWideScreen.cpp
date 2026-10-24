// IGI native provenance: Camera_ApplyWideScreen - 0x004A23E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A23E0-Camera_ApplyWideScreen.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A23E0-Camera_ApplyWideScreen.hpp"
struct status_msg* Camera_ApplyWideScreen(void* context, unsigned int arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5, unsigned int arg6)

{
    int kind;
    kind = g_543d4c;
    struct status_msg* result = QTask_HandleInput(context, kind, 0);
    result->head[8] = arg2;
    result->head[9] = arg3;
    result->head[0xa] = arg4;
    result->head[0xb] = arg5;
    result->head[0xc] = 0;
    result->head[0xd] = arg6;
    return result;
}
