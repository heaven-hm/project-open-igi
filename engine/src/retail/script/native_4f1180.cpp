// Native: QHash_FindValue (placeholder until VERIFIED); retail VA 0x004F1180-0x004F119B (27 bytes, 3 blocks, 3 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (8b442404 50 e816000000 83c404 85c0 7404 83c00c c3 b8747c5600 c3).
// Behavior/CFG: looks the argument up with 0x4F11A0 and returns node+0x0C when a node is found; on a miss it returns the static empty value object at 0x00567C74 instead of null.
// Status/limits: BUILD_VERIFIED (static only); the miss result is the address of the modelled static object, not the retail absolute address, because the candidate image is DYNAMIC_BASE; runtime UNOBSERVED.
#include "igi/retail/core/qhash_slot_table.hpp"

#include <cstdint>

using igi::retail::core::g_qhash_empty_567c74;

extern "C" {

void* QHashNode_Find(void* value); // 0x004F1185: call 0x4f11a0

void* QHash_FindValue(void* const value)
{
    void* const node = QHashNode_Find(value);                       // 0x004F1184/0x004F1185
    if (node != nullptr)                                                   // 0x004F118D/0x004F118F: test eax,eax; je
    {
        return static_cast<std::uint8_t*>(node) + 0x0C;                    // 0x004F1191: add eax,0xc
    }
    return static_cast<void*>(g_qhash_empty_567c74);                       // 0x004F1195: mov eax,0x567c74
}

} // extern "C"
