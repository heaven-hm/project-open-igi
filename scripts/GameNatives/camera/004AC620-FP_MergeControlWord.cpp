// Native: FP_MergeControlWord; retail VA 0x004AC620; signature x86 cdecl uint32(uint32,uint32), implicit x87 control word.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja 0x004AC620; RE-Agent Radare2 ev_029b0df50610264b006f30e9c6001f7703d1230c1675ecf2df2191af861d1620;
// angr differential replay by scripts/fpu_state_diff_runner.py (8/8); spawn-mode paired capture observes one startup
// call returning a pointer-sized class on both images.
// Behavior/CFG: reads the live x87 control word, converts it to CRT flags, replaces the masked bits with the request,
// converts the merged flags back to a control word, loads it and returns the merged flags.
// The legacy catalog name "Camera_SetFieldOfView_24" is stale; the body performs no camera work.
// Status/limits: canonical reference transcription for the record's engine implementation
// engine/src/retail/core/fpu_state.cpp. Not compiled into the candidate build.
#include <cstdint>

std::uint32_t FP_ControlWordToFlags(std::uint16_t word);
std::uint32_t FP_FlagsToControlWord(std::uint32_t flags);

std::uint32_t FP_MergeControlWord(std::uint32_t flags, std::uint32_t mask)
{
    std::uint16_t word;
    __asm__ volatile("fwait\n\tfnstcw %0" : "=m"(word) : : "memory");
    const std::uint32_t result = (FP_ControlWordToFlags(word) & ~mask) | (flags & mask);
    word = static_cast<std::uint16_t>(FP_FlagsToControlWord(result));
    __asm__ volatile("fldcw %0" : : "m"(word) : "memory");
    return result;
}
