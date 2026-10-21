#pragma once

#include <cstdint>

namespace igi::retail::dynobj {

// A hash-list record. The retail code reads the successor pointer from the record's first
// 32-bit field (`int* result = *context; int* i = *result;`), so the first member is the link.
struct HashItem {
    HashItem* next = nullptr;
};

// Native: DynObj_LookupHashItem() - VA 0x004CF300 - complete port.
// Behavior: walks the singly-linked list rooted at `*context` and returns the node whose
//           zero-based position equals `index` while `node->next != nullptr`.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE (SHA256: dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01);
//                 VA 0x004CF300-0x004CF327, 39 bytes, digest 41eba1a0d28a0e00e3ed94efa873e13b83cbe5d02c2bd78d2c417ccd37895039;
//                 CLEAN/dynobj/004CF300-DynObj_LookupHashItem.c.
// CFG: radare2 5 blocks/6 edges; Binary Ninja 5 blocks; 100% structural agreement.
// Confidence: high for exact loop condition and return value.
// Signature: x86 cdecl HashItem*(HashItem** context, int32_t index).
// Usage/tests: resolves indexed model/magic entries in MTP packages; exercised by hash_lookup_test.cpp.
[[nodiscard]] HashItem* DynObj_LookupHashItem(HashItem** context, std::int32_t index);

// Native: Ilff_FindFormChunk() - VA 0x004CEAA0 - complete port.
// Behavior: walks the big-endian IFF/FORM sibling chunks inside `context` (starting at
//           `context + 0x0C`, bounded by `bswap32(*(uint32_t*)(context + 0x04)) + context + 0x08`)
//           until a chunk whose big-endian tag (`bswap32(chunk[0])`) equals `tag` (`arg2`) is
//           found; writes `bswap32(chunk[1])` to `*out_size` when `out_size != nullptr` and
//           returns `&chunk[2]` (`chunk + 0x08`), or `nullptr` if the form end is reached.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004CEAA0-0x004CEB34, 148 bytes,
//                 digest 372b63ac4186b7efd312dc57ebcaa25371455306a57d7245a6f70b9be1e93f04;
//                 CLEAN/dynobj/004CEAA0-Ilff_FindFormChunk.c.
// CFG: radare2 6 blocks/8 edges; Binary Ninja 6 blocks; 100% structural agreement.
// Confidence: high for exact byte-swap arithmetic, 4-byte alignment, and payload return.
// Signature: x86 cdecl void*(void* context, int32_t arg2, int32_t* arg3).
// Usage/tests: locates TEXF/MODS/VNAM/INST/GTT/LOVS/PALF/BANM chunks in MTP forms; exercised by hash_lookup_test.cpp and texture_loaders_test.cpp.
[[nodiscard]] void* Ilff_FindFormChunk(void* context, std::int32_t arg2, std::int32_t* arg3);

} // namespace igi::retail::dynobj
