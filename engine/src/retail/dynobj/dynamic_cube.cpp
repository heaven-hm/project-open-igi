#include "igi/retail/dynobj/dynamic_cube.hpp"

namespace igi::retail::dynobj {

#if defined(__GNUC__) && !defined(__clang__) && defined(__i386__)
__attribute__((noinline, optimize("no-omit-frame-pointer")))
#endif

// Native: DynObj_LinkDynamicCube_7; retail VA 0x004C67E0.
// Transcribed from the retail x86 instruction sequence: XOR each paired coordinate, OR the
// differences, BSR the highest set bit, shift 1 by that index, and apply the retail clamps.
std::uint32_t DynObj_LinkDynamicCube_7(std::uint32_t min_x,
                                      std::uint32_t min_y,
                                      std::uint32_t min_z,
                                      std::uint32_t max_x,
                                      std::uint32_t max_y,
                                      std::uint32_t max_z) noexcept
{
    const std::uint32_t difference = (min_x ^ max_x) | (min_y ^ max_y) | (min_z ^ max_z);
    if (difference == 0U)
    {
        return 0x2000U;
    }

#if defined(__GNUC__) && defined(__i386__)
    // Retail uses [ebp+8] as scratch for both the OR result and the BSR index.
    // Preserve the observed caller-stack write in the Windows x86 build.
    __asm__ volatile("movl %0, 8(%%ebp)" : : "r"(difference) : "memory");
#endif

    std::uint32_t highest_bit = 0U;
    for (std::uint32_t bits = difference; bits >>= 1U;)
    {
        ++highest_bit;
    }

#if defined(__GNUC__) && defined(__i386__)
    __asm__ volatile("movl %0, 8(%%ebp)" : : "r"(highest_bit) : "memory");
#endif

    const std::uint32_t span = 1U << highest_bit;
    if (span < 0x2000U)
    {
        return 0x2000U;
    }
    if (span <= 0x40000000U)
    {
        return span;
    }
    return 0x40000000U;
}

} // namespace igi::retail::dynobj
