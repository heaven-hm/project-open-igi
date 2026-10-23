// Native: FP_ControlWordToFlags; retail VA 0x004AC66B; signature x86 cdecl uint32(uint16).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja 0x004AC66B; RE-Agent Radare2 ev_d074b7c6ef6553ff018c78e84399e70947c9c13e5cce319e5fd630ac95fa2d17;
// angr differential replay by scripts/fpu_state_diff_runner.py (8/8), spawn-mode paired capture observes one startup
// call whose return class (pointer-sized, bit 0x80000 set) matches the candidate's.
// Behavior/CFG: maps x87 exception masks, rounding control, precision control and the infinity bit into CRT flag
// bits; the AH byte of the result carries the mask bits, which is why the observed return is a pointer-sized class.
// The legacy catalog name "Camera_ApplyBandFilter_24" is stale: the body performs no camera work, and the same
// address is reached from the retail entry-point chain as FPU initialisation.
// Status/limits: canonical reference transcription for the record's engine implementation
// engine/src/retail/core/fpu_state.cpp. Not compiled into the candidate build.
#include <cstdint>

std::uint32_t FP_ControlWordToFlags(std::uint16_t word)
{
    std::uint32_t result = 0;
    if (word & 1U) result = 0x10U;
    if (word & 4U) result |= 8U;
    if (word & 8U) result |= 4U;
    if (word & 0x10U) result |= 2U;
    if (word & 0x20U) result |= 1U;
    if (word & 2U) result |= 0x80000U;
    const std::uint16_t rounding = word & 0xc00U;
    if (rounding == 0x400U) result |= 0x100U;
    else if (rounding == 0x800U) result |= 0x200U;
    else if (rounding == 0xc00U) result |= 0x300U;
    const std::uint16_t precision = word & 0x300U;
    if (precision == 0U) result |= 0x20000U;
    else if (precision == 0x200U) result |= 0x10000U;
    if (!(word & 0x1000U)) return result;
    return result | 0x40000U;
}
