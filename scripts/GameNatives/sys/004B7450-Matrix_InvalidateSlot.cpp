// Native: Matrix_InvalidateSlot (catalog alias Matrix_InvalidateSlot); retail VA 0x004B7450; x86 signature/calling convention unconfirmed.
// Artifact: retail IGI.EXE, SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: REA/Ghidra static record in 004B7450-Matrix_InvalidateSlot.md; catalog at assets/BinaryNinja/IGINatives_VERIFIED.json.
// Behavior/CFG: clears dword at context + arg2 * 4 + 0x14; GameNatives candidate returns arg2; catalog reports CFG 100% structural.
// Status/limits: ANALYZED; Ghidra pseudocode confirms the store but not the return; runtime parity not verified.
#include "004B7450-Matrix_InvalidateSlot.hpp"
int Matrix_InvalidateSlot(int context, int arg2)

{
    *(context + (arg2 << 2) + 0x14) = 0;
    return arg2;
}
