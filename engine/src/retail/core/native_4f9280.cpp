// Native: Graph_Get_4F9280 (placeholder until VERIFIED); retail VA 0x004F9280-0x004F.. (4-6 bytes, single load+ret).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP + radare2 6.2.0 bytes a0 69 e3 54 00; xref writer Graph_Init 0x004FAA60; image bytes in .data.
// Behavior/CFG: mov al,[0x54E369]; ret - returns the graph-state value in AL/AX/EAX.
// Status/limits: TRANSCRIBED; image init 0xFFU; runtime-written by the untranscribed Graph_Init 0x4FAA60 (port global reads it). ABI note: retail mov al/ax preserves upper EAX bits, C++ narrow returns zero-extend; callers in scope read AL/AX. Runtime UNOBSERVED (reachable from Graph registration at AIGraph id 122).
#include "igi/retail/core/graph_state.hpp"

extern "C" std::uint8_t g_54e369 = 0xFFU; // retail 0x0054E369, image value 0xFFU

extern "C" {

std::uint8_t Graph_Get_4F9280()
{
    return g_54e369; // 0x004F9280: a0 69 e3 54 00 (mov al,[0x54E369]); c3
}

} // extern "C"
