// IGI native provenance: FlashTask_OnEvent - 0x00530C60 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00530C60-FlashTask_OnEvent.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "FlashTask_OnEvent.hpp"
unsigned int FlashTask_OnEvent()

{
    unsigned int result = QTask_CloseEvent_2(g_54f7a0);
    g_a845ec = 0;
    return result;
}
