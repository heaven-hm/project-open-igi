#pragma once

#include <cstddef>
#include <cstdint>

namespace igi::retail::resource {

// Retail resource hash node (`Sys_PopStack_15` 0x004B59F0 / `Is_ResourceLoaded` 0x004B5B90).
// On 32-bit retail ABI:
//   +0x00: list node prev (`result[0]`)
//   +0x04: list node next (`result[1]`)
//   +0x08..+0x1F: child list / self pointer (`result[2..7]`)
//   +0x20: cached hash (`result[8]`)
//   +0x24: resource kind (`result[9]`)
//   +0x28: byte size (`result[10]`)
//   +0x2C: loaded flag (`result[11]`)
//   +0x30: active flag (`result[12]`)
//   +0x34: payload pointer (`result[13]`)
//   +0x38: NUL-terminated resource name (`result[14..]`)
struct ResourceHashNode {
    ResourceHashNode* prev;      // +0x00 (result[0])
    ResourceHashNode* next;      // +0x04 (result[1])
    void* link_08[6];            // +0x08..+0x1F (result[2..7]: child node links + child list header)
    std::int32_t hash;           // +0x20 (result[8])
    std::int32_t kind;           // +0x24 (result[9])
    std::int32_t size;           // +0x28 (result[10])
    std::int32_t loaded_flag;    // +0x2C (result[11])
    std::int32_t flag_30;        // +0x30 (result[12])
    void* data;                  // +0x34 (result[13])
    char name_storage[0x200];    // +0x38..+0x237 (result[14..0x8D])
    void* debug_path_8e;         // +0x238 (result[0x8E])
    std::int32_t meta_8f;        // +0x23C (result[0x8F])
    std::int32_t meta_90;        // +0x240 (result[0x90])
    std::int32_t pool_active_idx;// *(g_943e34 + result)

    [[nodiscard]] char* name() noexcept
    {
        return name_storage;
    }
    [[nodiscard]] const char* name() const noexcept
    {
        return name_storage;
    }
};

#if defined(_M_IX86) || defined(__i386__)
static_assert(offsetof(ResourceHashNode, next) == 0x04);
static_assert(offsetof(ResourceHashNode, hash) == 0x20);
static_assert(offsetof(ResourceHashNode, kind) == 0x24);
static_assert(offsetof(ResourceHashNode, size) == 0x28);
static_assert(offsetof(ResourceHashNode, loaded_flag) == 0x2C);
static_assert(offsetof(ResourceHashNode, flag_30) == 0x30);
static_assert(offsetof(ResourceHashNode, data) == 0x34);
static_assert(offsetof(ResourceHashNode, name_storage) == 0x38);
#endif

// Root of the retail resource table (global 0x00943E40). The retail walk treats the root as a
// node-shaped header: it tests +0x20 and +0x38 on the root too, and follows +0x04 to the first
// entry, so the root uses the same layout.

// 0x004B5B90 - Is_ResourceLoaded.
// Resolves the alias (iterating QFile_AliasResolve depths until it stops advancing), upper-cases
// the result, converts '\' to '/', hashes it and probes the resource chain. Returns 1 and writes
// the matching node to `out_entry` when found; otherwise returns 0.
std::int32_t Is_ResourceLoaded(const char* resource_path, ResourceHashNode** out_entry);

// Port-owned storage for the retail table root 0x00943E40 and the two string buffers the native
// uses (0x100 bytes each).
[[nodiscard]] ResourceHashNode& Resource_GetHashRoot();

} // namespace igi::retail::resource
