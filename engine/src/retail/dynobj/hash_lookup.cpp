#include "igi/retail/dynobj/hash_lookup.hpp"

namespace igi::retail::dynobj {
namespace {

[[nodiscard]] constexpr std::uint32_t bswap32(std::uint32_t value) noexcept
{
    return (((value & 0x00FF0000U) | (value >> 16U)) >> 8U)
         | (((value & 0x0000FF00U) | (value << 16U)) << 8U);
}

} // namespace

// Native: DynObj_LookupHashItem; retail VA 0x004CF300; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE (SHA256: dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01); VA 0x004CF300-0x004CF327, 39 bytes, digest 41eba1a0d28a0e00e3ed94efa873e13b83cbe5d02c2bd78d2c417ccd37895039; CLEAN/dynobj/004CF300-DynObj_LookupHashItem_14.c.
// Behavior/CFG: walks the singly-linked list rooted at `*context` and returns the node whose zero-based position equals `index` while `node->next != nullptr`. radare2 5 blocks/6 edges; Binary Ninja 5 blocks; 100% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact loop condition and return value.
// Previous catalog alias: DynObj_LookupHashItem_14.
HashItem* DynObj_LookupHashItem(HashItem** context, std::int32_t index)
{
    std::int32_t current_index = 0;
    HashItem* node = *context;
    HashItem* next = node->next;

    while (next != nullptr)
    {
        if (current_index == index)
        {
            return node;
        }

        node = next;
        next = next->next;
        current_index += 1;
    }

    return nullptr;
}

// Native: Ilff_FindFormChunk; retail VA 0x004CEAA0; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004CEAA0-0x004CEB34, 148 bytes, digest 372b63ac4186b7efd312dc57ebcaa25371455306a57d7245a6f70b9be1e93f04; CLEAN/dynobj/004CEAA0-DynObj_CreateMagicObject_13.c.
// Behavior/CFG: walks the big-endian IFF/FORM sibling chunks inside `context` (starting at `context + 0x0C`, bounded by `bswap32(*(uint32_t*)(context + 0x04)) + context + 0x08`) until a chunk whose big-endian tag (`bswap32(chunk[0])`) equals `tag` (`arg2`) is found; writes `bswap32(chunk[1])` to `*out_size` when `out_size != nullptr` and returns `&chunk[2]` (`chunk + 0x08`), or `nullptr` if the form end is reached. radare2 6 blocks/8 edges; Binary Ninja 6 blocks; 100% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact byte-swap arithmetic, 4-byte alignment, and payload return.
// Previous catalog alias: DynObj_CreateMagicObject_13.
void* Ilff_FindFormChunk(void* context, std::int32_t arg2, std::int32_t* arg3)
{
    auto* const base = static_cast<std::uint8_t*>(context);
    const std::uint32_t data_val = *reinterpret_cast<const std::uint32_t*>(base + 4);
    std::uint8_t* entity_val = base + 0x0C;
    const std::uint32_t data_2 = *reinterpret_cast<const std::uint32_t*>(entity_val);
    const auto target_tag = static_cast<std::uint32_t>(arg2);

    if (target_tag != bswap32(data_2))
    {
        std::uint8_t* const limit = base + 8 + bswap32(data_val);
        std::uint32_t data_6 = 0U;

        do
        {
            const std::uint32_t data_4 =
                *reinterpret_cast<const std::uint32_t*>(entity_val + 4);
            entity_val = entity_val + ((bswap32(data_4) + 3U) & 0xFFFFFFFCU) + 8U;

            if (entity_val >= limit)
            {
                return nullptr;
            }

            data_6 = *reinterpret_cast<const std::uint32_t*>(entity_val);
        } while (target_tag != bswap32(data_6));
    }

    if (arg3 != nullptr)
    {
        const std::uint32_t data_8 = *reinterpret_cast<const std::uint32_t*>(entity_val + 4);
        *arg3 = static_cast<std::int32_t>(bswap32(data_8));
    }

    return entity_val + 8U;
}

} // namespace igi::retail::dynobj
