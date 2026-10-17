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

// Retail IGI.EXE function at VA 0x00401050; C# Game_Frame is only a candidate label.
void IGI_RETAIL_CDECL Game_DispatchCurrentFrameCallback();

// Retail IGI.EXE getter at VA 0x00402880.
igi::retail::core::FrameDispatchRecordView* IGI_RETAIL_CDECL Game_GetFrameDispatchRecord();

// Retail IGI.EXE setter at VA 0x004028A0; writes a 32-bit value at record offset 0x2c.
void IGI_RETAIL_CDECL Game_SetFrameDispatchRecordValue(std::uint32_t value);

// Retail IGI.EXE helper at VA 0x0048F480; stores the byte in DAT_005C8DE8.
void IGI_RETAIL_CDECL Game_SetLoopStatus(std::uint8_t value);

}

#undef IGI_RETAIL_CDECL
