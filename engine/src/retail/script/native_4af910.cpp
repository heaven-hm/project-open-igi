// Native: Script_QHashListInsert (placeholder until VERIFIED); retail VA 0x004AF910-0x004AF926 (23 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (8b442404 8b4c2408 8b10 894104 8911 8b10 894a04 8908 c3); sole caller 0x4C0560 pushes the bucket pointer then the record, matching (head, node). No callees.
// Behavior/CFG: inserts node at the front of the doubly linked list headed by head: node->prev = head, node->next = head->next, the old first node's prev = node, head->next = node, in exactly the retail store order; returns nothing.
// Status/limits: BUILD_VERIFIED (static only); the second [head] reload before the old-first back-link is kept as retail performs it; runtime UNOBSERVED.
#include <cstdint>

extern "C" {

void Script_QHashListInsert(void* const head, void* const node)
{
    std::uint8_t* const head_bytes = static_cast<std::uint8_t*>(head);   // 0x004AF910: mov eax,[esp+4]
    std::uint8_t* const node_bytes = static_cast<std::uint8_t*>(node);   // 0x004AF914: mov ecx,[esp+8]
    std::uint8_t* const first = *reinterpret_cast<std::uint8_t**>(head_bytes);          // 0x004AF918: mov edx,[eax]
    *reinterpret_cast<std::uint8_t**>(node_bytes + 4) = head_bytes;                     // 0x004AF91A: mov [ecx+4],eax
    *reinterpret_cast<std::uint8_t**>(node_bytes) = first;                              // 0x004AF91D: mov [ecx],edx
    std::uint8_t* const first_again = *reinterpret_cast<std::uint8_t**>(head_bytes);    // 0x004AF91F: mov edx,[eax]
    *reinterpret_cast<std::uint8_t**>(first_again + 4) = node_bytes;                    // 0x004AF921: mov [edx+4],ecx
    *reinterpret_cast<std::uint8_t**>(head_bytes) = node_bytes;                         // 0x004AF924: mov [eax],ecx
}

} // extern "C"
