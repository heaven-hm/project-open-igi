// IGI native provenance: Menu_FormatScreen_11 - 0x004223A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004223A0-Menu_FormatScreen_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Menu_FormatScreen_11.h"
unsigned int Menu_FormatScreen_11()

{
    DynObj_CreateMagicObject_4(g_57bc04);
    unsigned int result = QTask_CloseEvent_2(g_53b800);
    g_53b800 = 0x181;
    return result;
}
