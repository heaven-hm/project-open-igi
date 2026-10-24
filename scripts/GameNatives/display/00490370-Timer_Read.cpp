// IGI native provenance: Timer_Read - 0x00490370 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00490370-Display_RestoreDevice.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00490370-Timer_Read.hpp"
int Timer_Read()

{
    return GetTickCount() - g_tick_base;
}
