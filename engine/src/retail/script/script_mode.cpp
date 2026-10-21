#include "igi/retail/script/script_mode.hpp"

namespace igi::retail::script
{
std::uint8_t g_a758bd{};
std::uint8_t g_a758be{};

// Native: Script_GetParseMode; retail VA 0x004F1160; uint32_t __cdecl(void).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja disassembly 0x004F1160-0x004F117A; independent Radare2/angr checks pending.
// Behavior/CFG: Read bytes 0x00A758BE and 0x00A758BD; return 0 only for BE==0 and BD!=0. Catalog label retained; enum/real parsing is not established.
// Status/limits: TRANSCRIBED; candidate requires differential and runtime evidence.
std::uint32_t Script_GetParseMode()
{
    if (g_a758be != 0)
    {
        return 1;
    }
    if (g_a758bd != 0)
    {
        return 0;
    }
    return 1;
}
}
