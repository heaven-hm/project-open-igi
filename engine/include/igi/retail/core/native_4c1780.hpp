#pragma once

extern "C" {

// Native: AnimSound_LoadParameters_4C1780 (placeholder name until VERIFIED; the
// BN alias AnimSound_LoadParameters is not established for a ret-only body).
// Retail VA 0x004C1780: a single `c3`, called as the task-list registrar at
// position 7 of QTask_RegisterClasses 0x00402930. It registers nothing.
void AnimSound_LoadParameters_4C1780();

} // extern "C"
