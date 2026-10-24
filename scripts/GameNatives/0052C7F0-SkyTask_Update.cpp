// IGI native provenance: SkyTask_Update - 0x0052C7F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0052C7F0-SkyTask_Update.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0052C7F0-SkyTask_Update.hpp"
unsigned int SkyTask_Update()

{
    DynObj_CreateMagicObject_4(g_a84558);
    unsigned int result = QTask_CloseEvent_2(g_54f6e4);
    g_54f6e4 = 0x181;
    return result;
}
