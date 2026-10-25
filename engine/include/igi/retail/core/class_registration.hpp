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

extern "C" {

// Native: QTask_RegisterClasses() - VA 0x00402930.
// Behavior: the retail's class registration sequence - 155 call sites in retail
//           order, beginning with the six that open the function (0x00508F80,
//           0x00489560, 0x00407D60, 0x00488BB0, 0x00402FF0, 0x00415530) and
//           ending in the tail jump to 0x004CEF30. The body is transcribed in
//           engine/src/retail/core/native_402930.cpp.
// Status/limits: TRANSCRIBED. The registration order is the retail order and no
//           registrar is substituted or skipped; targets with no transcription
//           are fail-closed placeholders, so the sequence cannot silently
//           shorten and shift the class ids the handler table is indexed by.
void NATIVE_DECL QTask_RegisterClasses();

} // extern "C"

#undef NATIVE_DECL
