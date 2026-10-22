// IGI native provenance: Script_GetParseMode - 0x004F1160 - transcribed from retail igi.exe.
// Evidence: Retail bytes 0x004F1160-0x004F117A; Binary Ninja and Radare2 agree on two
// byte loads with no calls or loops. Behavioural evidence: native_unicorn_runner executes
// the retail bytes under unicorn and compares all four flag combinations plus boundary
// bytes (8/8 cases).
// Behaviour: reads the byte at 0x00A758BE; when set returns 1. Otherwise reads the byte
// at 0x00A758BD; returns 0 when set, else 1. It has no arguments and no side effects.
// Limitation: the catalog label Script_ParseEnumReal32_5 is not supported by this code;
// the function only reads two flags and returns 0 or 1, so enum/real parsing is not
// established. The name here describes the observed behaviour only.
#include <cstdint>

extern std::uint8_t g_a758be;
extern std::uint8_t g_a758bd;

std::uint32_t __cdecl Script_GetParseMode(void)
{
    if (g_a758be != 0) {
        return 1;
    }
    if (g_a758bd != 0) {
        return 0;
    }
    return 1;
}
