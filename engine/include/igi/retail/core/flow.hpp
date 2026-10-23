#pragma once

// The retail Flow class and its phase handlers.
//
// igi.exe registers the Flow class after the Game class in the class
// registration sequence at 0x00402930, and links three phase handlers into
// the handler table at 0x00A96AE0:
//
//   phase 0  0x00402070 Flow_Start   the flow start, which stores the flow
//                                      object and registers the input port
//   phase 1  0x00402260 Flow_RunHandler  the per-frame state dispatch
//   phase 2  0x00402740 Flow_Delete  the teardown, which loads config.qsc
//
// The Flow class carries a 0x54 byte object, and its class id is kept at
// 0x00567C7C. The per-frame dispatch reads the phase 1 table, so the Flow
// class's id and its phase links decide which handler the frame reaches.

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define IGI_RETAIL_CDECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define IGI_RETAIL_CDECL __attribute__((cdecl))
#else
#define IGI_RETAIL_CDECL
#endif

namespace igi::retail::core {

// The Flow class's per-frame state dispatch, the level path's arithmetic.
// The retail computes it on the x87 stack at 0x0040229B..0x004022C6:
//   fild  dword [flow + 0x3C]        the object's counter
//   fdivr qword [0x00533230]         1000.0 / counter
//   fimul dword [flow + 0x34]        times the object's factor (an integer)
//   fild  dword [base]               the object's base at +0x50 plus +0x30
//   faddp                            adds the base
//   fistp qword [esp + 0x10]         the total, as an integer
// The port keeps the 80-bit intermediates with long double, which is the x87
// type on the x86 target.
inline std::int64_t Flow_LevelScale(std::int32_t counter, std::int32_t factor, std::int32_t base) noexcept
{
    const long double scaled = (1000.0L / static_cast<long double>(counter)) *
                               static_cast<long double>(factor);
    return static_cast<std::int64_t>(scaled + static_cast<long double>(base));
}

} // namespace igi::retail::core

extern "C" {

// The retail Flow class id at 0x00567C7C.
extern std::uint16_t QTask_FlowClassId;

// The retail flow state byte at 0x005C8E00.
extern std::uint8_t Flow_StateByte;

// The retail frame timer at 0x005C8E34.
extern std::uint32_t Loop_StartTicks;

// The retail flow flags at 0x005C8E71 and 0x00A70C5B.
extern std::uint8_t Flow_PausedFlag;
extern std::uint8_t AppContext_PausedFlag;

// Native: Flow_Init() - VA 0x00401FC0 - complete port.
// Behavior: reads the current task type through 0x004018F0, registers the
//           "Flow" class with a 0x54 byte object through 0x00401900, keeps the
//           class id at 0x00567C7C, and links the phase 0 handler 0x00402070
//           through 0x00401400, the phase 1 handler 0x00402260 and the phase 2
//           handler 0x00402740 through 0x00401530.
void IGI_RETAIL_CDECL Flow_Init();

// Native: Flow_Start() - VA 0x00402070 - complete port.
// Behavior: stores the flow object at 0x00567C8C, the same global the frame
//           dispatch record at 0x00402880 returns, and registers the input port
//           handler through 0x004012A0 with the current task type and zero.
void IGI_RETAIL_CDECL Flow_Start(void* flow);

// Native: Flow_RunHandler() - VA 0x00402260 - complete port.
// Behavior: reads the elapsed ticks through 0x00490370, and dispatches on the
//           state byte at 0x005C8E00 and the flags at 0x00A70C5B and
//           0x005C8E71. The level path computes the frame's scale on the x87
//           stack from the flow object's counter, factor and base. The
//           states the MVP demo does not enter are not transcribed and report
//           themselves as such.
void IGI_RETAIL_CDECL Flow_RunHandler(void* flow);

// Native: Flow_CreateHandler() - VA 0x00402800 - complete port.
// Behavior: reads the Flow class id at 0x00567C7C and registers the input port
//           handler through 0x004012A0 with the object, the class id and zero.
void IGI_RETAIL_CDECL Flow_CreateHandler(void* flow);

// Native: Flow_Delete() - VA 0x00402740 - complete port.
// Behavior: reads the frame timer through 0x0048F180, loads the
//           "LOCAL:config.qsc" script through 0x00405BD0, runs the script
//           setup at 0x004EE3B0 and then reads the flow object's state.
void IGI_RETAIL_CDECL Flow_Delete(void* flow);

} // extern "C"

#undef IGI_RETAIL_CDECL
