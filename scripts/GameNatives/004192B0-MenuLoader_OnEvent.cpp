// IGI native provenance: MenuLoader_OnEvent - 0x004192B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004192B0-MenuLoader_OnEvent.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004192B0-MenuLoader_OnEvent.hpp"
unsigned int MenuLoader_OnEvent()

{
    DynObj_CreateMagicObject_4(g_57baf4);
    unsigned int result = QTask_CloseEvent_2(g_539adc);
    g_539adc = 0x181;
    return result;
}
