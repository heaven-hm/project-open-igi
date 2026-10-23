#pragma once

namespace igi::retail::mem {

// Native: HeapList_Insert() - VA 0x004AF8F0 - complete port.
// Behavior: inserts `arg2` at the tail of an intrusive doubly-linked list whose header is at
//           `context` (`context + 4` = head anchor, `context + 8` = tail pointer): writes
//           `arg2[1] = *(context + 8)`, `*arg2 = context + 4`, `**(context + 8) = arg2`,
//           `*(context + 8) = arg2`, and returns `context`.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE (SHA256: dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01);
//                 VA 0x004AF8F0-0x004AF910, 32 bytes; CLEAN/camera/004AF8F0-Camera_SetFieldOfView_33.c.
// CFG: radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Confidence: high for exact pointer splice sequence.
// Signature: x86 cdecl void*(void* context, void** arg2); ported as void* HeapList_Insert(void* context, void** arg2).
// Usage/tests: links allocated heap blocks and model/instance nodes; exercised by heap_list_test.cpp and mem_alloc_test.cpp.
void* HeapList_Insert(void* context, void** arg2);
inline void* Camera_SetFieldOfView_33(void* context, void** arg2) { return HeapList_Insert(context, arg2); }

// Native: HeapList_Remove() - VA 0x004AF960 - complete port.
// Behavior: unlinks `context` from an intrusive doubly-linked list by writing
//           `*context[1] = *context`, `*(static_cast<void**>(*context) + 1) = context[1]`,
//           clearing `context[1] = nullptr` and `context[0] = nullptr`, and returning `context`.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004AF960-0x004AF981, 33 bytes,
//                 digest 4ada7252239e45e0b5ed56005655c50a7ca29e3c5e3be153590e183ca807eb46;
//                 CLEAN/camera/004AF960-Camera_ApplyHandiCam_33.c.
// CFG: radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Confidence: high for exact pointer unlink steps.
// Signature: x86 cdecl void**(void** context); ported as void** HeapList_Remove(void** context).
// Usage/tests: unlinks heap and resource nodes; exercised by heap_list_test.cpp.
void** HeapList_Remove(void** context);
inline void** Camera_ApplyHandiCam_33(void** context) { return HeapList_Remove(context); }

} // namespace igi::retail::mem

using igi::retail::mem::HeapList_Insert;
using igi::retail::mem::HeapList_Remove;
using igi::retail::mem::Camera_ApplyHandiCam_33;
using igi::retail::mem::Camera_SetFieldOfView_33;

