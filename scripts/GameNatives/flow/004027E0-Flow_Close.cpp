// IGI native provenance: Flow_Close - 0x004027E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004027E0-Flow_InitEvents.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004027E0-Flow_Close.hpp"
unsigned int Flow_Close()

{
    int data_val;
    data_val = g_567c7c;
    return QTask_CloseEvent_2(data_val);
}
