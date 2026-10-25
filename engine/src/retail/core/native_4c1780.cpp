// Native: AnimSound_LoadParameters; retail VA 0x004C1780. previous note: Native: AnimSound_LoadParameters (placeholder name until VERIFIED; BN alias AnimSound_LoadParameters is not established for a ret-only body).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004C1780-0x004C1780.
// Tools/evidence: radare2 6.2.0 `p8` = c3 and Binary Ninja function boundary (1 block, 0 callees); Tester leaf packet CHAIN-LEAVES.md (consumer list: 175 callers, incl. 0x5183D0 at 0x5184E3-F7 and 0x530270 at 0x530284, and it is also stored as a callback pointer).
// Behavior/CFG: the whole body is `ret`; it registers nothing and changes no state. It is reached from QTask_RegisterClasses 0x00402930 as the task-list registrar (callsite 0x00402A0x sequence position 7).
// Status/limits: STRUCTURALLY_VERIFIED; runtime UNOBSERVED (the QTask-entry boundary stops before 0x00402930 completes); the BN alias is not accepted as the name.
#include "igi/retail/core/native_4c1780.hpp"

extern "C" {

void AnimSound_LoadParameters()
{
    // 0x004C1780: c3 (ret).
}

} // extern "C"
