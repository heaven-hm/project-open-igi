// IGI native provenance: Camera_ApplyWideScreen_2 - 0x004A2910 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A2910-Camera_ApplyWideScreen_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_ApplyWideScreen_2.h"
struct status_msg* Camera_ApplyWideScreen_2(void* context, unsigned int arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5)

{
    int kind = QHash_ValueGet();
    char context_val = kind;

    if (!context_val)
        QHash_ValueSet();

    kind = g_543d58;
    struct status_msg* result = QTask_HandleInput(context, kind, 0);
    result->head[8] = arg2;
    result->head[9] = arg3;
    result->head[0xa] = arg4;
    result->head[0xb] = arg5;
    result->head[0x13] = 0;
    result->head[0x14] = Timer_Now();
    result->head[0xc] = 0;
    result->head[0xc] = Sys_DispatchOpcode_18(g_936024);
    Sys_IlfFileCorruptS(&result->head[0xd], g_936020);

    if (!context_val)
        Sys_DispatchOpcode_2();

    return result;
}
