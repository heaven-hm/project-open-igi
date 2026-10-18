#pragma once

#include <cstddef>
#include <csetjmp>
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define IGI_RETAIL_CDECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define IGI_RETAIL_CDECL __attribute__((cdecl))
#else
#define IGI_RETAIL_CDECL
#endif

namespace igi::retail::core {

using FrameDispatchCallback = void (IGI_RETAIL_CDECL *)(void*);

struct FrameDispatchRecordView {
    std::byte unknown_0x00[0x1C];
    std::uint16_t dispatch_table_index;
    std::byte unknown_0x1E[0x0E];
    std::uint32_t unknown_0x2C;
};

static_assert(offsetof(FrameDispatchRecordView, dispatch_table_index) == 0x1C);
static_assert(offsetof(FrameDispatchRecordView, unknown_0x2C) == 0x2C);
static_assert(sizeof(FrameDispatchRecordView) == 0x30);

} // namespace igi::retail::core

extern "C" {

extern std::jmp_buf DAT_0054F8E0;
extern std::uint8_t DAT_005C8DE8;
extern igi::retail::core::FrameDispatchRecordView* DAT_00567C8C;
extern igi::retail::core::FrameDispatchCallback DAT_00A970E0[];

// Native: QTask_OpenEvent() - VA 0x00401050 - complete port.
// Behavior: establishes the retail __setjmp3 frame boundary on 0x0054F8E0; when 0, fetches
//           Game_GetContext() (0x00402880), reads the 16-bit dispatch index at +0x1C, and
//           invokes the callback at 0x00A970E0[index](Game_GetContext()); when non-zero,
//           calls FX_SmokeEmit_27(0) (0x0048F480).
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE (SHA256: dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01);
//                 VA 0x00401050-0x00401094, 68 bytes, digest c1c4e05efbb946828539e30c465e82356be70d32d878a62a0512201816b1ff93.
// CFG: radare2 3 blocks/2 edges; Binary Ninja 3 blocks; 100% structural agreement.
// Confidence: high for exact control flow, offsets, and call order.
// Signature: x86 cdecl void(); ported as void QTask_OpenEvent().
// Usage/tests: invoked by retail main message loop (0x0048F530); exercised by frame_natives_test.cpp.
void IGI_RETAIL_CDECL QTask_OpenEvent();

// Native: Game_GetContext() - VA 0x00402880 - complete port.
// Behavior: returns the global game-context pointer stored at 0x00567C8C.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x00402880-0x00402886, 6 bytes,
//                 digest 64bb29e43e21aa80d264c1d4f7588317866c098a20c52f27e2d415debf912d7c.
// CFG: radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Confidence: high for exact instruction behavior.
// Signature: x86 cdecl FrameDispatchRecordView*(); ported as FrameDispatchRecordView* Game_GetContext().
// Usage/tests: called by QTask_OpenEvent (0x00401050); exercised by frame_natives_test.cpp.
igi::retail::core::FrameDispatchRecordView* IGI_RETAIL_CDECL Game_GetContext();

// Native: Flow_SetState() - VA 0x004028A0 - complete port.
// Behavior: writes `value` into the 32-bit field at offset +0x2C of the global context at 0x00567C8C.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004028A0-0x004028AE, 14 bytes,
//                 digest fe8cd535469ff604fe50034d4a1f8f6cd5cb3ae5b15c571c28c4c375d43e860d.
// CFG: radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Confidence: high for exact instruction behavior.
// Interface: retail writes the argument and leaves its bits in EAX; uint32_t preserves that observed result. The original source type is unknown.
// Usage/tests: stores pending flow state in game context; exercised by frame_natives_test.cpp.
std::uint32_t IGI_RETAIL_CDECL Flow_SetState(std::uint32_t value);

// Native: Loop_SetStatusByte() - VA 0x0048F480 - complete port.
// Previous catalog alias: FX_SmokeEmit_27.
// Behavior: stores `value` into the global loop status byte at 0x005C8DE8.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x0048F480-0x0048F48A, 10 bytes,
//                 digest ee3c22c2c4363e72b7199c5ea3ec80e0c5661a1354a9c93b5b3ff1b4fd0e1464.
// CFG: radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Confidence: high for exact instruction behavior.
// Signature: x86 cdecl void(uint8_t value); ported as void Loop_SetStatusByte(std::uint8_t value).
// Usage/tests: called by QTask_OpenEvent (0x00401050) on nonlocal abort; exercised by frame_natives_test.cpp.
void IGI_RETAIL_CDECL Loop_SetStatusByte(std::uint8_t value);
inline void IGI_RETAIL_CDECL FX_SmokeEmit_27(std::uint8_t value) { Loop_SetStatusByte(value); }

}

#undef IGI_RETAIL_CDECL
