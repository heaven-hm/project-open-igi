#pragma once

// The retail class registration sequence.
//
// igi.exe registers every task class in one sequence at 0x00402930, and the
// class ids that sequence produces are what the handler table is indexed by:
// the table entry for a class is at class_id + phase * 384, and the per-frame
// dispatch reads the index out of the game object. A class that is registered
// in a different order therefore lands in a different slot, so the port runs
// the same sequence in the same order.
//
// The sequence is a straight line of calls to the class registrars. The
// registrars the MVP demo needs are transcribed; the ones outside the MVP
// scope are listed so the order stays visible and their classes can be added
// without moving the ones that follow.

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

namespace igi::retail::core {

// One entry of the retail's registration sequence, the registrar it calls
// and the address the retail has.
struct ClassRegistrar
{
    std::uint32_t address;
    const char* name;
};

// The retail's sequence, in the retail's order.
inline constexpr ClassRegistrar class_registrars[] = {
    {0x00508F80U, "AI graph"},
    {0x00489560U, "AI detection"},
    {0x00407D60U, "material defines"},
    {0x00488BB0U, "level flow"},
    {0x00402FF0U, "game options"},
    {0x00415530U, "Level and Game classes"},
    {0x004C1780U, "task list"},
    {0x00401FC0U, "Flow class"},
    {0x00488800U, "edit variable"},
    {0x00488620U, "sub level"},
    {0x00487BE0U, "guard generator"},
    {0x004872C0U, "conditional container"},
};

} // namespace igi::retail::core

extern "C" {

// Native: QTask_RegisterClasses() - VA 0x00402930 - complete port.
// Behavior: the retail's class registration sequence. It calls each class
//           registrar in order, so the class ids the handler table is indexed
//           by follow the order. The port runs the same sequence: the
//           registrars the MVP demo needs register their classes, and the
//           ones outside the MVP scope report that they are not transcribed
//           rather than shifting the ids silently.
void NATIVE_DECL QTask_RegisterClasses();

} // extern "C"

#undef NATIVE_DECL
