// IGI native provenance: GameFunctions_IsDebugKeyPressed - 0x00414FD0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00414FD0-GameFunctions_IsDebugKeyPressed.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "GameFunctions_IsDebugKeyPressed.h"
int GameFunctions_IsDebugKeyPressed(void* context)

{
    if (*(context + 0xc28c62) & 0x80 && g_c28c7f & 0x80 && g_c28c8c & 0x80
            && Config_GetActivePlayerProfileRecord()
            && *(Config_GetActivePlayerProfileRecord() + 0x560) >= 0xe && g_57b194)
        return 1;

    return 0;
}
