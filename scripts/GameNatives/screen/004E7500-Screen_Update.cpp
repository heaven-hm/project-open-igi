// IGI native provenance: Screen_Update - 0x004E7500 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E7500-Screen_Update.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Screen_Update.hpp"
unsigned int Screen_Update()

{
    QTask_CreateTask_2(g_5488c8);
    short entity_val = g_a5e618;
    g_5488c8 = 0xff;
    unsigned int result = QTask_CloseEvent_2(entity_val);
    g_a5ea66 = 0;
    return result;
}
