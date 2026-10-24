// IGI native provenance: Medipack_OnEvent - 0x00470600 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00470600-Medipack_OnEvent.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00470600-Medipack_OnEvent.hpp"
unsigned int Medipack_OnEvent()

{
    int data_val;
    data_val = g_5be334;
    return QTask_CloseEvent_2(data_val);
}
