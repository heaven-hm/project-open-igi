// Native: Graph_Get_4F9200 (placeholder until VERIFIED); retail VA 0x004F9200-0x004F.. (4-6 bytes, single load+ret).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP + radare2 6.2.0 bytes a1 10 6d a7 00; xref writer Graph_Init 0x004FAA60; image bytes in .data.
// Behavior/CFG: mov eax,[0xA76D10]; ret - returns the graph-state value in AL/AX/EAX.
// Status/limits: TRANSCRIBED; image init 0U; runtime-written by the untranscribed Graph_Init 0x4FAA60 (port global reads it). ABI note: retail mov al/ax preserves upper EAX bits, C++ narrow returns zero-extend; callers in scope read AL/AX. Runtime UNOBSERVED (reachable from Graph registration at AIGraph id 122).
#include "igi/retail/core/graph_state.hpp"

extern "C" std::uint32_t g_a76d10 = 0U; // retail 0x00A76D10, image value 0U

extern "C" {

std::uint32_t Graph_Get_4F9200()
{
    return g_a76d10; // 0x004F9200: a1 10 6d a7 00 (mov eax,[0xA76D10]); c3
}

} // extern "C"
