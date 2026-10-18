// IGI native provenance: Menu_RequestScreen_11 - 0x00421700 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00421700-Menu_RequestScreen_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Menu_RequestScreen_11.hpp"
unsigned int Menu_RequestScreen_11()

{
    DynObj_CreateMagicObject_4(g_57bbfc);
    unsigned int result = QTask_CloseEvent_2(g_53b7f0);
    g_53b7f0 = 0x181;
    return result;
}
