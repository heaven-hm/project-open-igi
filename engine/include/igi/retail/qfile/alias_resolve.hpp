#pragma once

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

// Retail 0x004B1020 and 0x004B1FB0 operate on the alias record table at 0x00942330 (32 records of 0x94 bytes,
// name at +0x00, target at +0x10, handle at +0x90) and the resolved-alias buffer at 0x00943A18. That storage is
// owned by qfile/native_4b13a0.cpp, qfile/native_4b12d0.cpp and qfile/native_4b1020.cpp; this header declares only
// the two natives.

namespace igi::retail::qfile {

// 0x004B1020 - QFile_AliasResolve_4B1020.
// Walks the 32 alias records, matches the record name as a prefix of `alias` with Resource_MatchPrefix (0x004AAD10),
// then substitutes the depth-th ';'-separated segment of the record's target, appends the remainder of `alias` after
// the matched name, copies the text into the 0x00943A18 buffer, and copies the result into `destination`.
// Returns depth+1 when another ';' follows the chosen segment, 0 when it is the last segment or when no record matched.
// Running out of segments while skipping calls Debug_Error with the record's target and then spins, as retail does.
std::int32_t NATIVE_DECL QFile_AliasResolve_4B1020(char* destination, const char* alias, std::int32_t depth);

// 0x004B1FB0 - QFile_GetResolvedAlias_4B1FB0. No arguments, returns the 0x00943A18 buffer address.
char* NATIVE_DECL QFile_GetResolvedAlias_4B1FB0();

} // namespace igi::retail::qfile

#undef NATIVE_DECL
