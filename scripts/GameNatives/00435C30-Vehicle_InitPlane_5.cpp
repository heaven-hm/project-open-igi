// IGI native provenance: Vehicle_InitPlane_5 - 0x00435C30 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00435C30-Vehicle_InitPlane_5.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00435C30-Vehicle_InitPlane_5.hpp"
unsigned int Vehicle_InitPlane_5()

{
    int data_val;
    data_val = g_57bd64;
    return QTask_CloseEvent_2(data_val);
}
