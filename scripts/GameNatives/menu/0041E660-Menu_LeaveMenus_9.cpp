// IGI native provenance: Menu_LeaveMenus_9 - 0x0041E660 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0041E660-Menu_LeaveMenus_9.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Menu_LeaveMenus_9.hpp"
unsigned int Menu_LeaveMenus_9()

{
    DynObj_CreateMagicObject_4(g_57bbd0);
    unsigned int result = QTask_CloseEvent_2(g_53b674);
    g_53b674 = 0x181;
    return result;
}
