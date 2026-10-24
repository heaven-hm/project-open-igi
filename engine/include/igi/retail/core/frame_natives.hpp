#pragma once

#include <cstddef>
#include <csetjmp>
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

namespace igi::retail::core {

using FrameDispatchCallback = void (NATIVE_DECL *)(void*);
// The optional handler the retail calls from AppContext_SetIsActive (0x0048F140).
using FrameFlagHandler = std::int32_t (NATIVE_DECL *)(std::int32_t, std::int32_t);

// The retail Flow object at 0x00567C8C. The class registers a 0x54 byte object
// through 0x00401900, and Flow_RunHandler (0x00402260) reads the fields from
// +0x08 through +0x50, so the port carries the retail's whole object.
struct FrameDispatchRecordView {
    std::byte unknown_0x00[0x08];
    // +0x08: the flow object's task list. 0x004012A0 links records here through
    // 0x004AF8F0: +0x08 holds the first node, +0x0C the list's first slot and
    // +0x10 the last node. Flow_RunHandler starts its walk at [object + 0x08].
    std::uint32_t task_first;
    std::uint32_t task_list_first;
    std::uint32_t task_list_last;
    std::byte unknown_0x14[0x08];
    std::uint16_t dispatch_table_index; // +0x1C, the class id
    std::byte unknown_0x1E[0x0A];       // +0x1E..+0x27
    std::uint32_t state;                // +0x28, the running flow state
    std::uint32_t unknown_0x2C;         // +0x2C, the pending flow state
    std::uint32_t base_low;             // +0x30
    std::uint32_t counter;              // +0x34
    std::uint32_t phase_counter;        // +0x38
    std::uint32_t divisor;              // +0x3C
    std::uint32_t frame_counter;        // +0x40
    std::uint8_t unknown_0x44;          // +0x44
    std::byte unknown_0x45[0x03];
    std::uint32_t port_record;          // +0x48, the input port record
    std::uint32_t step;                 // +0x4C
    std::uint32_t base_high;            // +0x50
};

static_assert(offsetof(FrameDispatchRecordView, dispatch_table_index) == 0x1C);
static_assert(offsetof(FrameDispatchRecordView, unknown_0x2C) == 0x2C);
static_assert(offsetof(FrameDispatchRecordView, base_high) == 0x50);
static_assert(sizeof(FrameDispatchRecordView) == 0x54);

} // namespace igi::retail::core

extern "C" {

extern std::jmp_buf QTask_SetjmpFrame;
// The retail's loop status byte at 0x005C8DE8, shared with the window
// procedure at 0x0048F970.
extern std::uint8_t Loop_StatusByte;
extern std::int32_t AppContext_IsActive;
extern igi::retail::core::FrameFlagHandler AppContext_OptionalHandler;
extern std::int32_t AppContext_OptionalHandlerContext;
extern igi::retail::core::FrameDispatchRecordView* Game_FrameDispatchRecord;
// The phase 1 base of the retail handler table, 0x00A970E0. The table lives in
// task_types.cpp, because the retail stores the class handlers in it.
igi::retail::core::FrameDispatchCallback* QTask_Phase1HandlerTable();

// Native: QTask_OpenEvent() - VA 0x00401050 - complete port.
// Behavior: establishes the retail __setjmp3 frame boundary on 0x0054F8E0; when 0, fetches
//           Game_GetContext() (0x00402880), reads the 16-bit dispatch index at +0x1C, and
//           invokes the callback at 0x00A970E0[index](Game_GetContext()); when non-zero,
//           calls the loop status setter (0x0048F480).
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE (SHA256: dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01);
//                 VA 0x00401050-0x00401094, 68 bytes, digest c1c4e05efbb946828539e30c465e82356be70d32d878a62a0512201816b1ff93.
// CFG: radare2 3 blocks/2 edges; Binary Ninja 3 blocks; 100% structural agreement.
// Confidence: high for exact control flow, offsets, and call order.
// Signature: x86 cdecl void(); ported as void QTask_OpenEvent().
// Usage/tests: invoked by retail main message loop (0x0048F530); exercised by frame_natives_test.cpp.
void NATIVE_DECL QTask_OpenEvent();

// Native: Game_GetContext() - VA 0x00402880 - complete port.
// Behavior: returns the global game-context pointer stored at 0x00567C8C.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x00402880-0x00402886, 6 bytes,
//                 digest 64bb29e43e21aa80d264c1d4f7588317866c098a20c52f27e2d415debf912d7c.
// CFG: radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Confidence: high for exact instruction behavior.
// Signature: x86 cdecl FrameDispatchRecordView*(); ported as FrameDispatchRecordView* Game_GetContext().
// Usage/tests: called by QTask_OpenEvent (0x00401050); exercised by frame_natives_test.cpp.
igi::retail::core::FrameDispatchRecordView* NATIVE_DECL Game_GetContext();

// Native: Flow_SetState() - VA 0x004028A0 - complete port.
// Behavior: writes `value` into the 32-bit field at offset +0x2C of the global context at 0x00567C8C.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004028A0-0x004028AE, 14 bytes,
//                 digest fe8cd535469ff604fe50034d4a1f8f6cd5cb3ae5b15c571c28c4c375d43e860d.
// CFG: radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Confidence: high for exact instruction behavior.
// Interface: retail writes the argument and leaves its bits in EAX; uint32_t preserves that observed result. The original source type is unknown.
// Usage/tests: stores pending flow state in game context; exercised by frame_natives_test.cpp.
std::uint32_t NATIVE_DECL Flow_SetState(std::uint32_t value);

// Native: Loop_SetStatusByte() - VA 0x0048F480 - complete port.
// Behavior: stores `value` into the global loop status byte at 0x005C8DE8.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x0048F480-0x0048F48A, 10 bytes,
//                 digest ee3c22c2c4363e72b7199c5ea3ec80e0c5661a1354a9c93b5b3ff1b4fd0e1464.
// CFG: radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Confidence: high for exact instruction behavior.
// Signature: x86 cdecl, one uint8_t argument. Retail 0x0048F480 is `mov al,byte [esp+4]` / `mov byte [0x5C8DE8],al`
// / `ret`: the loaded byte is in AL when the function returns, so the value is also the result. Ported as
// std::uint32_t Loop_SetStatusByte(std::uint8_t value) returning the stored byte. Note retail writes only AL, so
// the upper bits of EAX are not defined by those three instructions; the candidate returns the zero-extended byte.
// Usage/tests: called by QTask_OpenEvent (0x00401050) on nonlocal abort; exercised by frame_natives_test.cpp.
std::uint32_t NATIVE_DECL Loop_SetStatusByte(std::uint8_t value);

// Native: AppContext_SetIsActive() - VA 0x0048F140 - complete port.
// Behavior: sign-extends the low byte of the argument, stores it in the global
//           frame flag at 0x005C8BFC, and when the optional handler at
//           0x005C8C18 is set, calls it with the argument at 0x005C8C1C and
//           returns its result.
std::int32_t NATIVE_DECL AppContext_SetIsActive(std::int32_t value);

// Native: AppContext_GetIsActive() - VA 0x0048F170 - complete port.
// Behavior: returns the global frame flag at 0x005C8BFC. The retail main loop
//           (0x0048F544) calls it between the frame clear (0x00498460) and
//           the per-frame dispatch (0x00401050) and branches on the result:
//           non-zero dispatches the frame, zero sleeps 500 ms.
std::int32_t NATIVE_DECL AppContext_GetIsActive();

}

#undef NATIVE_DECL
