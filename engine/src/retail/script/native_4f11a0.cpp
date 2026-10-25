// Native: QHashNode_Find (placeholder until VERIFIED); retail VA 0x004F11A0-0x004F11E3 (68 bytes, 8 blocks, 10 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (56 57 8b7c240c 57 e8b4d7ffff 8d0440 83c404 8b0485a80ca700 8b30 85f6 7420 8b4808 8b09 85c9 7406 3bcf 7415 eb09 50 e8bcfaffff 83c404 8bc6 8b36 85f6 75e0 33c0 5f 5e c3).
// Behavior/CFG: finds the hash node whose key matches the argument. It hashes with 0x4EE960 (value % 0xFB), multiplies by 3, loads the bucket pointer from 0x00A70CA8, and walks the singly-linked node list from [bucket]. For each node it loads the key through [node+0x08]; a null key releases that node with 0x4F0C90 and continues, a matching key returns the node itself, and an exhausted list returns null.
// Status/limits: STRUCTURALLY_VERIFIED (static only); the node list link is the node's first word; a null key means "stale slot" in retail and is released rather than skipped, which is preserved; the bucket table is zero-initialised storage because its insert counterpart is untranscribed; runtime UNOBSERVED.
#include "igi/retail/script/hash_table.hpp"

#include <cstdint>

using igi::retail::script::g_hash_buckets_a70ca8;

extern "C" {

std::uint32_t Script_HashBucket(std::uint32_t value); // 0x004F11A7: call 0x4ee960
void QHashNode_Release(void* node);                   // 0x004F11CF: call 0x4f0c90

void* QHashNode_Find(void* const value)
{
    std::uint32_t index = Script_HashBucket(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(value))); // 0x004F11A7
    index *= 3;                                                                                  // 0x004F11AC: lea eax,[eax+eax*2]
    void* node = reinterpret_cast<void*>(static_cast<std::uintptr_t>(g_hash_buckets_a70ca8[index])); // 0x004F11B2: mov eax,[eax*4+0xa70ca8]
    void* current = *reinterpret_cast<void**>(node);                                             // 0x004F11B9: mov esi,[eax]
    if (current == nullptr)                                                                      // 0x004F11BB/0x004F11BD: test esi,esi; je
    {
        return nullptr;                                                                          // 0x004F11DF: xor eax,eax
    }
    for (;;)
    {
        void* const key_pointer = *reinterpret_cast<void**>(static_cast<std::uint8_t*>(node) + 0x08); // 0x004F11BF: mov ecx,[eax+8]
        const std::uint32_t key = *reinterpret_cast<std::uint32_t*>(key_pointer);                // 0x004F11C2: mov ecx,[ecx]
        if (key == 0U)                                                                           // 0x004F11C4/0x004F11C6: test ecx,ecx; je
        {
            QHashNode_Release(node);                                                      // 0x004F11CE/0x004F11CF
        }
        else if (key == static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(value)))     // 0x004F11C8/0x004F11CA: cmp ecx,edi; je 0x4F11E1
        {
            return node;                                                                         // 0x004F11E1/0x004F11E2/0x004F11E3: pop edi; pop esi; ret
        }
        node = current;                                                                          // 0x004F11D7: mov eax,esi
        current = *reinterpret_cast<void**>(current);                                            // 0x004F11D9: mov esi,[esi]
        if (current == nullptr)                                                                  // 0x004F11DB/0x004F11DD: test esi,esi; jne
        {
            return nullptr;                                                                      // 0x004F11DF
        }
    }
}

} // extern "C"
