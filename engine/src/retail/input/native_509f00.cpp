// Native: Joypad_InitActionFlags (placeholder until VERIFIED); retail VA 0x00509F00-0x00509F43 (68 bytes, 1 block, no callees).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 disassembly/bytes; Binary Ninja MCP decompile; DINPUT-LAYER packet (exact byte listing in dinput-layer.json).
// Behavior/CFG: for slot i: byte[0xBCAA00+i*0x28] = (g_A7B4E0[i] != 0); bytes +1/+3/+4 = 1; byte[0xBCAA00+7+i*0x28] = byte[0xA7B440+i*0x28].
// Status/limits: TRANSCRIBED; no callees, no native dependencies (runtime data block). Runtime UNOBSERVED.
#include "igi/retail/input/dinput_globals.hpp"

#include <cstdint>

extern "C" {

void Joypad_InitActionFlags(std::int32_t index)
{
    std::uint8_t* const slot =
        reinterpret_cast<std::uint8_t*>(g_bcaa00) + static_cast<std::size_t>(index) * 0x28U;
    // 0x00509F00-0x00509F1A: present flag and the three constant bytes.
    slot[0] = (g_a7b4e0[index] != nullptr) ? 1U : 0U;
    slot[1] = 1U;
    slot[3] = 1U;
    slot[4] = 1U;
    // 0x00509F2E-0x00509F43: copy the source byte at 0xA7B440 + i*0x28.
    slot[7] = reinterpret_cast<const std::uint8_t*>(g_a7b440)[static_cast<std::size_t>(index) * 0x28U];
}

} // extern "C"
