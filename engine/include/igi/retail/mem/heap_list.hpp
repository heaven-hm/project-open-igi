#pragma once

namespace igi::retail::mem {

// Native: HeapList_Insert() - VA 0x004AF8F0 - complete port.
// Duplicate status: 0x004AF8F0 is also transcribed as igi::retail::sys::Sys_InsertListTail_4AF8F0 in
//   engine/src/retail/sys/native_4af8f0.cpp. Both exist; see docs/fidelity/evidence/duplicate-list-native-conflict.md.
//   The remove half 0x004AF960 was deleted from this pair of files on 2026-10-25 (it now lives only in sys/native_4af960.cpp).
//   The invented aliases Camera_SetFieldOfView_33 / Camera_ApplyHandiCam_33 are not retail names and are being removed.
// Behavior: inserts `arg2` at the tail of an intrusive doubly-linked list whose header is at
//           `context` (`context + 4` = head anchor, `context + 8` = tail pointer): writes
//           `arg2[1] = *(context + 8)`, `*arg2 = context + 4`, `**(context + 8) = arg2`,
//           `*(context + 8) = arg2`, and returns `context`.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE (SHA256: dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01);
//                 VA 0x004AF8F0-0x004AF910, 32 bytes; CLEAN/camera/004AF8F0-Camera_SetFieldOfView_33.c.
// CFG: radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Confidence: high for exact pointer splice sequence.
// Signature: x86 cdecl void*(void* container, void** node); 32-bit target only (the 64-bit port branch was removed as scaffolding).
// Usage/tests: links allocated heap blocks and model/instance nodes; exercised by heap_list_test.cpp and mem_alloc_test.cpp.
void* HeapList_Insert(void* container, void** node);
inline void* Camera_SetFieldOfView_33(void* context, void** arg2) { return HeapList_Insert(context, arg2); }

} // namespace igi::retail::mem

using igi::retail::mem::HeapList_Insert;
using igi::retail::mem::Camera_SetFieldOfView_33;

