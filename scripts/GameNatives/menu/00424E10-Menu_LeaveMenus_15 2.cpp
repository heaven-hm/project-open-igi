// IGI native provenance: Menu_LeaveMenus_15 - 0x00424E10 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00424E10-Menu_LeaveMenus_15.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Menu_LeaveMenus_15.h"
unsigned int Menu_LeaveMenus_15()

{
    int data_val;
    data_val = g_57bc60;
    return QTask_CloseEvent_2(data_val);
}
