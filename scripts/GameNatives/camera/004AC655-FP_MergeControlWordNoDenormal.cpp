// Native: FP_MergeControlWordNoDenormal; retail VA 0x004AC655; signature x86 cdecl uint32(uint32,uint32).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja 0x004AC655; RE-Agent Radare2 ev_ec0e8469a340469ce2dc4d62876b8a15eb0ac44fc0809a6b410b663dc84a0fab;
// angr differential replay by scripts/fpu_state_diff_runner.py (8/8); spawn-mode paired capture observes one startup
// call returning a pointer-sized class on both images.
// Behavior/CFG: clears the denormal-mask bit from the mask, then forwards to FP_MergeControlWord; the shared body is
// why a call to this native also produces one observation of the merge helper.
// The legacy catalog name "Camera_ApplyLineFilter_24" is stale; the body performs no camera work.
// Status/limits: canonical reference transcription for the record's engine implementation
// engine/src/retail/core/fpu_state.cpp. Not compiled into the candidate build.
#include <cstdint>

std::uint32_t FP_MergeControlWord(std::uint32_t flags, std::uint32_t mask);

std::uint32_t FP_MergeControlWordNoDenormal(std::uint32_t flags, std::uint32_t mask)
{
    return FP_MergeControlWord(flags, mask & 0xfff7ffffU);
}
