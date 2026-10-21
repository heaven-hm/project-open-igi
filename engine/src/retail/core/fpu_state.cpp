#include "igi/retail/core/fpu_state.hpp"
#include <stdexcept>

namespace igi::retail::core
{
std::uint16_t g_6e5bc4{};

// Native: Camera_ApplyBandFilter_24; retail VA 0x004AC66B; uint32 cdecl(uint16).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja 0x004AC66B; RE-Agent Radare2 ev_d074b7c6ef6553ff018c78e84399e70947c9c13e5cce319e5fd630ac95fa2d17.
// Behavior/CFG: Maps x87 exception masks, rounding, precision and infinity control to CRT flags; AH byte writes are preserved as masks.
// Status/limits: TRANSCRIBED; independent CFG and concrete replay required; legacy name does not describe camera behavior.
std::uint32_t Camera_ApplyBandFilter_24(std::uint16_t word)
{
    std::uint32_t result = 0;
    if (word & 1U) result = 0x10U;
    if (word & 4U) result |= 8U;
    if (word & 8U) result |= 4U;
    if (word & 0x10U) result |= 2U;
    if (word & 0x20U) result |= 1U;
    if (word & 2U) result |= 0x80000U;
    const auto rounding = word & 0xc00U;
    if (rounding)
    {
        if (rounding == 0x400U) result |= 0x100U;
        else if (rounding == 0x800U) result |= 0x200U;
        else if (rounding == 0xc00U) result |= 0x300U;
    }
    const auto precision = word & 0x300U;
    if (!precision) result |= 0x20000U;
    else if (precision == 0x200U) result |= 0x10000U;
    if (!(word & 0x1000U)) return result;
    return result | 0x40000U;
}

// Native: Camera_ApplyWideScreen_24; retail VA 0x004AC6FD; uint32 cdecl(uint32).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja 0x004AC6FD; RE-Agent Radare2 ev_5794d39045f969439f47829def84c28cd0fc587a5ba368c17fa8f9599618b5ef.
// Behavior/CFG: Maps CRT flags back to the x87 control word; preserves the original zero/default precision branches.
// Status/limits: TRANSCRIBED; independent CFG and concrete replay required; legacy name retained for source identity.
std::uint32_t Camera_ApplyWideScreen_24(std::uint32_t flags)
{
    std::uint32_t result = 0;
    if (flags & 0x10U) result = 1U;
    if (flags & 8U) result |= 4U;
    if (flags & 4U) result |= 8U;
    if (flags & 2U) result |= 0x10U;
    if (flags & 1U) result |= 0x20U;
    if (flags & 0x80000U) result |= 2U;
    const auto rounding = flags & 0x300U;
    if (rounding)
    {
        if (rounding == 0x100U) result |= 0x400U;
        else if (rounding == 0x200U) result |= 0x800U;
        else if (rounding == 0x300U) result |= 0xc00U;
    }
    const auto precision = flags & 0x30000U;
    if (!precision) result |= 0x300U;
    else if (precision == 0x10000U) result |= 0x200U;
    if (flags & 0x40000U) result |= 0x1000U;
    return result;
}

// Native: Camera_SetFieldOfView_24; retail VA 0x004AC620; uint32 cdecl(uint32,uint32), implicit x87 environment.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja 0x004AC620; RE-Agent Radare2 ev_029b0df50610264b006f30e9c6001f7703d1230c1675ecf2df2191af861d1620.
// Behavior/CFG: Reads CW, merges converted flags under mask, loads converted CW and returns the merged flags.
// Status/limits: TRANSCRIBED; x86 only; pending exceptions and unmasked traps require separate evidence.
std::uint32_t Camera_SetFieldOfView_24(std::uint32_t flags, std::uint32_t mask)
{
#if defined(__i386__) && defined(__GNUC__)
    std::uint16_t word;
    __asm__ volatile("fwait\n\tfnstcw %0" : "=m"(word) : : "memory");
    const auto result = (Camera_ApplyBandFilter_24(word) & ~mask) | (flags & mask);
    word = static_cast<std::uint16_t>(Camera_ApplyWideScreen_24(result));
    __asm__ volatile("fldcw %0" : : "m"(word) : "memory");
    return result;
#else
    throw std::logic_error("Retail x87 control update requires the Windows x86 target.");
#endif
}

// Native: Camera_ApplyLineFilter_24; retail VA 0x004AC655; uint32 cdecl(uint32,uint32).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja 0x004AC655; RE-Agent Radare2 ev_ec0e8469a340469ce2dc4d62876b8a15eb0ac44fc0809a6b410b663dc84a0fab.
// Behavior/CFG: Excludes the denormal mask bit before forwarding to the x87 control update.
// Status/limits: TRANSCRIBED; concrete replay and paired runtime evidence pending.
std::uint32_t Camera_ApplyLineFilter_24(std::uint32_t flags, std::uint32_t mask)
{
    return Camera_SetFieldOfView_24(flags, mask & 0xfff7ffffU);
}

// Native: Camera_ApplyBandFilter_10; retail VA 0x004A7157; uint32 cdecl(void).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja 0x004A7157; RE-Agent Radare2 ev_aafca3332923a2bee890a444e154b09ee31957d53d2041fdae7baf0cc9c69168.
// Behavior/CFG: CRT startup selects 53-bit precision with flags 0x10000 and mask 0x30000.
// Status/limits: TRANSCRIBED; recovered via PE entry 0x004A50F5 -> 0x004A5AA8 -> [0x00543E8C] -> 0x004A5363.
std::uint32_t Camera_ApplyBandFilter_10()
{
    return Camera_ApplyLineFilter_24(0x10000U, 0x30000U);
}

// Native: FP_InitExtendedPrecision; retail VA 0x00498460; no stack arguments, no defined return value.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja 0x00498460; RE-Agent Radare2 ev_7aa88abff21ab34bcb7f814cec70114d5295ab5fa21f690abedfe94bdd8b5da6.
// Behavior/CFG: FWAIT; FNSTCW to global; OR word with 0x0C00; FLDCW; RET. Preserves other control bits.
// Status/limits: TRANSCRIBED; x86 GCC candidate only; controlled replay and paired runtime acceptance pending.
void FP_InitExtendedPrecision()
{
#if defined(__i386__) && defined(__GNUC__)
    __asm__ volatile("fwait\n\tfnstcw %0\n\torw $0x0c00, %0\n\tfldcw %0"
                     : "+m"(g_6e5bc4) : : "cc", "memory");
#else
    throw std::logic_error("Retail x87 rounding initialization requires the Windows x86 target.");
#endif
}
}
