// IGI native provenance: Menu_PlayMusic_17 - 0x00426D60 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00426D60-Menu_PlayMusic_17.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Menu_PlayMusic_17.hpp"
unsigned int Menu_PlayMusic_17()

{
    int data_val;
    data_val = g_53babc;
    return QTask_CloseEvent_2(data_val);
}
