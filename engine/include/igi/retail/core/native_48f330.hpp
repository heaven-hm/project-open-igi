#pragma once

// Native at retail VA 0x0048F330.
//
// `mov eax,[esp+4]; mov [0x005C8BF0],eax; ret` -- one block, 9 bytes. Stores the
// argument in the force-precompiled-qvm flag and leaves the same bits in EAX.
//
// The storage at 0x005C8BF0 belongs to this writer, so it is defined here rather
// than beside the reader. engine/src/retail/script/qvm_native_loader.cpp binds to
// this single owner instead of keeping a second copy of the same retail address,
// which is the same arrangement that file already uses for 0x005C8BEC.

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// Retail global at VA 0x005C8BF0.
extern std::int32_t g_5c8bf0;

// Native at VA 0x0048F330. Returns the argument bits, as the retail leaves them
// in EAX.
std::int32_t NATIVE_DECL SetForcePrecompiled(std::int32_t enabled);

} // extern "C"

#undef NATIVE_DECL
