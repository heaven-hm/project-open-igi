// IGI native provenance: ShadowTask_Update - 0x00519820 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00519820-ShadowTask_Update.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00519820-ShadowTask_Update.hpp"
unsigned int ShadowTask_Update()

{
    DynObj_CreateMagicObject_4(g_a83744);
    unsigned int result = QTask_CloseEvent_2(g_54ed3c);
    g_54ed3c = 0x181;
    return result;
}
