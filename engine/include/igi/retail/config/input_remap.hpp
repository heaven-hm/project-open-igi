#pragma once

// The retail input remap table and the GOInRemap native.
//
// igi.exe binds every action to a device control through GOInRemap, which the
// configuration script calls with three arguments. The native stores the three
// argument values in the remap table and the port reads that table when it
// applies the bindings, so the port follows the retail's flow only when the
// table holds the same entries the retail fills.

#include <cstddef>
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define IGI_RETAIL_CDECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define IGI_RETAIL_CDECL __attribute__((cdecl))
#else
#define IGI_RETAIL_CDECL
#endif

namespace igi::retail::config {

// The retail action table stride is 345 dwords, not 0x345. The retail computes
// it from the index with `lea ecx,[eax+eax*2]` (3a), `shl ecx,3` (24a),
// `sub ecx,eax` (23a), `lea ecx,[ecx+ecx*2]` (69a) and
// `lea ecx,[ecx+ecx*4]` (345a), indexing with `[ecx*4]`. Writing 0x345 here made
// the stride 837 and rejected the thirtieth valid remap entry.
inline constexpr std::uint32_t action_table_stride = 345U;

// The retail frame the script arguments live in. The native reads them
// through the frame reader at 0x004B8A20, which takes the frame and the
// argument index.
struct QvmFrameView
{
    std::uint32_t unknown_0x00;
    std::uint32_t* arguments;
    std::uint32_t* argument_values;
};

// The retail remap table stride: 0x00BC25BC + slot * 3 dwords.
inline constexpr std::uint32_t remap_table_stride = 3U;

// The six-dword script value block the native copies to the caller's result
// buffer (retail 0x0040382E..0x00403858). Retail builds it on the stack and
// stamps the script type descriptor at dword 4 (byte +0x10), the same
// 0x00567C74 handle the config value-block natives use for a real. Retail never
// writes dwords 1 and 5, so they are stack residue there.
inline constexpr std::uint32_t remap_result_dword_count = 6U;
inline constexpr std::uint32_t remap_result_type_descriptor = 0x00567C74U;

// The retail action table at 0x00BC2358. Each action owns 345 dwords and the
// first of them is the remap slot the native writes. The retail region runs
// from 0x00BC2358 to about 0x00BD8000 for the actions the retail registers.
inline constexpr std::uint32_t action_table_size = 0x6000U;

// Retail 0x004037C0 performs no capacity check, so the host side needs its own
// limit. The shipped config.qvm declares 72 remap entries; 128 leaves headroom
// while still bounding the index against action_table_size.
inline constexpr std::uint32_t action_table_capacity = 128U;

// The retail remap table at 0x00BC25BC. Each slot holds the three arguments
// the native reads from the script frame.
inline constexpr std::uint32_t remap_table_size = 0x2000U;

} // namespace igi::retail::config

extern "C" {

// The retail action count at 0x00BC2380. The native reads it to find the
// action it binds.
extern std::uint32_t InputRemap_ActionCount;

// The retail action table at 0x00BC2358.
extern std::uint32_t InputRemap_ActionTable[igi::retail::config::action_table_size];

// The retail remap table at 0x00BC25BC.
extern std::uint32_t InputRemap_Table[igi::retail::config::remap_table_size];

// GOInRemap, retail cdecl shape: the QVM hands the native the six-dword result
// buffer (arg1), an unused middle word (arg2) and the script frame (arg3). The
// native writes the three binding words, increments the action slot and copies
// the six-dword value block to arg1, returning arg1 in EAX.
//
// The engine's configuration decoder calls the single-argument adapter below;
// the decoder discards the result block, so the adapter supplies a host-side
// destination. That substitution is an engine-boundary adaptation, not a retail
// fact: retail's destination is the caller's QVM buffer.
std::uint32_t* IGI_RETAIL_CDECL GOInRemap(std::uint32_t* result, std::int32_t unused,
                                          void* qvm_context);

} // extern "C"

// Engine-facing adapter for the retail three-argument native. The engine's
// config decoder has no QVM result buffer, so this overload provides one and
// forwards to the retail-shaped entry above.
inline std::uint32_t* GOInRemap(void* qvm_context)
{
    static std::uint32_t result[igi::retail::config::remap_result_dword_count]{};
    return GOInRemap(result, 0, qvm_context);
}

#undef IGI_RETAIL_CDECL
