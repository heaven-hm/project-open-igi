// Native: FP_FlagsToControlWord; retail VA 0x004AC6FD; signature x86 cdecl uint32(uint32).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja 0x004AC6FD; RE-Agent Radare2 ev_5794d39045f969439f47829def84c28cd0fc587a5ba368c17fa8f9599618b5ef;
// angr differential replay by scripts/fpu_state_diff_runner.py (8/8); the spawn-mode paired capture observes the
// startup call returning control word 0x023F on both images.
// Behavior/CFG: inverse of FP_ControlWordToFlags; maps CRT flag bits back to an x87 control word, keeping the
// zero/default precision branch (64-bit control vs 24-bit precision) exact.
// The legacy catalog name "Camera_ApplyWideScreen_24" is stale; the body performs no camera work.
// Status/limits: canonical reference transcription for the record's engine implementation
// engine/src/retail/core/fpu_state.cpp. Not compiled into the candidate build.
#include <cstdint>

std::uint32_t FP_FlagsToControlWord(std::uint32_t flags)
{
    std::uint32_t result = 0;
    if (flags & 0x10U) result = 1U;
    if (flags & 8U) result |= 4U;
    if (flags & 4U) result |= 8U;
    if (flags & 2U) result |= 0x10U;
    if (flags & 1U) result |= 0x20U;
    if (flags & 0x80000U) result |= 2U;
    const std::uint32_t rounding = flags & 0x300U;
    if (rounding == 0x100U) result |= 0x400U;
    else if (rounding == 0x200U) result |= 0x800U;
    else if (rounding == 0x300U) result |= 0xc00U;
    const std::uint32_t precision = flags & 0x30000U;
    if (precision == 0U) result |= 0x300U;
    else if (precision == 0x10000U) result |= 0x200U;
    if (flags & 0x40000U) result |= 0x1000U;
    return result;
}
