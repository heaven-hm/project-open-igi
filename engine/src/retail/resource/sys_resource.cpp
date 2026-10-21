#include "igi/retail/resource/sys_resource.hpp"

#include "igi/retail/error/error_show.hpp"
#include "igi/retail/mem/heap_list.hpp"
#include "igi/retail/resource/is_resource_loaded.hpp"
#include "igi/retail/resource/resource_lifecycle.hpp"
#include "igi/retail/sys/ilff_navigation.hpp"
#include "igi/retail/sys/resource_lookup.hpp"

#include <cstring>
#include <vector>

namespace igi::retail::resource {
namespace {

std::vector<ResourceHashNode> g_pool_nodes;
std::vector<std::int32_t> g_pool_free_indices;
std::int32_t g_943e28 = 0;

// Port-owned buffer for `0x00943C20` (used by Sys_Resource when unpacking `"RESOURCE"` archives).
char g_943c20[0x220]{};

SysAllocateMemory5Fn g_sys_allocate_memory_5 = nullptr;
SysOpenFileStream5Fn g_sys_open_file_stream_5 = nullptr;

void EnsurePoolInitialized()
{
    if (g_pool_nodes.empty())
    {
        Resource_InitPoolForTest(64U);
    }
}

void SpliceResourceNodeIntoGlobalList(ResourceHashNode* node)
{
    auto& root = Resource_GetHashRoot();
    // On 32-bit retail ABI, `Camera_SetFieldOfView_33(&g_943e38, result)` splices `result` into
    // `g_943e38` where `g_943e38 + 4` is `0x00943E3C` (head anchor) and `g_943e38 + 8` is
    // `0x00943E40` (tail pointer / `hash_root`). We preserve the exact doubly-linked list semantics
    // on `ResourceHashNode::prev` (`result[0]`) and `ResourceHashNode::next` (`result[1]`) across
    // both 32-bit and 64-bit hosts so `Is_ResourceLoaded` walks every node before the sentinel.
    if (root.next == nullptr)
    {
        static ResourceHashNode s_tail_sentinel{};
        s_tail_sentinel = ResourceHashNode{};
        root.next = &s_tail_sentinel;
        s_tail_sentinel.prev = &root;
    }

    ResourceHashNode* const old_head = root.next;
    node->prev = &root;
    node->next = old_head;
    if (old_head != nullptr)
    {
        old_head->prev = node;
    }
    root.next = node;
}

void UnlinkResourceNodeFromGlobalList(ResourceHashNode* node)
{
    auto& root = Resource_GetHashRoot();
    if (&root == node)
    {
        return;
    }
    if (node->prev != nullptr)
    {
        node->prev->next = node->next;
    }
    if (node->next != nullptr)
    {
        node->next->prev = node->prev;
    }
    node->prev = nullptr;
    node->next = nullptr;
}

std::int32_t FreeMemory16Thunk(ResourceHashNode* node)
{
    return Sys_FreeMemory_16(node);
}

} // namespace

// Native: Resource_MatchPrefix; retail VA 0x004AAD10; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE (SHA256: dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01); VA 0x004AAD10-0x004AAD48, 56 bytes, digest 6f465400533e36a49cafc375dc4058fb995240dab60cd989d242464616bf2034; CLEAN/camera/004AAD10-Camera_ApplyLineFilter_21.c.
// Behavior/CFG: compares up to `max_len` bytes of `str1` (`context`) and `str2` (`arg2`), bounded by the length of `str1` (plus its NUL byte when shorter than `max_len`); returns `-1` if the first differing byte of `str2 > str1`, `1` if `str2 < str1`, or `0` if equal. radare2 6 blocks/8 edges; Binary Ninja 6 blocks; 100% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact bounded prefix comparison (`strncmp`-equivalent).
// Previous catalog alias: Camera_ApplyLineFilter_21.
std::int32_t Resource_MatchPrefix(const void* context, const void* arg2, std::int32_t arg3)
{
    std::int32_t i = arg3;

    if (i != 0)
    {
        const std::int32_t i_2 = i;
        const auto* dest_1 = static_cast<const std::uint8_t*>(context);
        const auto* const source_1 = dest_1;

        while (i != 0)
        {
            const bool flag_0_1 = (*dest_1 != 0U);
            ++dest_1;
            --i;
            if (!flag_0_1)
            {
                break;
            }
        }

        std::int32_t i_1 = -i + i_2;
        const auto* dest_2 = source_1;
        const auto* source_2 = static_cast<const std::uint8_t*>(arg2);

        while (i_1 != 0)
        {
            const bool flag_1_1 = (*source_2 == *dest_2);
            ++source_2;
            ++dest_2;
            --i_1;
            if (!flag_1_1)
            {
                break;
            }
        }

        const auto data_1 = static_cast<std::uint8_t>(*(source_2 - 1));
        i = 0;
        const auto f0_1 = static_cast<std::uint8_t>(*(dest_2 - 1));

        if (data_1 > f0_1)
        {
            return ~i;
        }

        if (data_1 != f0_1)
        {
            return 1;
        }
    }

    return i;
}

// Native: Resource_CreateNode; retail VA 0x004B59F0; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B59F0-0x004B5AE9, 249 bytes, digest d1a0fe269c4734cc2da57044b5771afd3f5ea268e254542d7e62292455455c97; CLEAN/sys/004B59F0-Sys_PopStack_15.c.
// Behavior/CFG: allocates a resource table slot from pool `g_943e20` using free-list index table `g_943e24[g_943e28++]` and stride `g_943e30`, records its active index in `*(g_943e34 + result)`, upper-cases `context` into `result + 0x38` (`result[0x0E]`) via Resource_CanonicalizePath (0x004B59C0), normalizes `'\\'` (`0x5C`) to `'/'` (`0x2F`), sets `result[0x0D]` (`+0x34` data pointer) to `arg3 ? arg3 : result`, `result[0x0A]` (`+0x28` size) = `arg4`, `result[0x09]` (`+0x24` kind) = `arg5`, `result[0x0C]` (`+0x30`) = `1`, `result[0x0B]` (`+0x2C` loaded_flag) = `0`, `result[0x8E]` (`+0x238`) = `arg2`, `result[0x08]` (`+0x20` hash) = Resource_CalculatePathHash(`&result[0x0E]`), initializes links `result[0..7]`, links `result` into `g_943e38` (`0x00943E40` hash list) via HeapList_Insert (0x004AF8F0), and copies `arg6[0..1]` into `result[0x8F..0x90]`. radare2 10 blocks/13 edges; Binary Ninja 10 blocks; 100% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact slot allocation, path normalization, hash computation, and list insertion.
// Previous catalog alias: Sys_PopStack_15.
void* Resource_CreateNode(const char* context, void* arg2, void* arg3, std::int32_t arg4,
                          std::int32_t arg5, const std::int32_t* arg6)
{
    EnsurePoolInitialized();
    const std::int32_t data_10 = g_943e28;
    const std::int32_t slot_index = g_pool_free_indices[static_cast<std::size_t>(data_10)];
    ResourceHashNode* const result = &g_pool_nodes[static_cast<std::size_t>(slot_index)];
    result->pool_active_idx = data_10;
    g_943e28 += 1;

    sys::Sys_ResolveSymbol_15(result->name(), const_cast<char*>(context));
    char i = result->name()[0];

    if (i != '\0')
    {
        char* data_3 = result->name();
        do
        {
            char entity_3 = '/';
            if (i != '\\')
            {
                entity_3 = *data_3;
            }
            *data_3 = entity_3;
            i = data_3[1];
            ++data_3;
        } while (i != '\0');
    }

    void* result_1 = arg3;
    if (result_1 == nullptr)
    {
        result_1 = result;
    }

    result->data = result_1;
    result->size = arg4;
    result->kind = arg5;
    result->flag_30 = 1;
    result->loaded_flag = 0;
    result->debug_path_8e = arg2;
    result->hash = static_cast<std::int32_t>(sys::Sys_CheckHeap_15(result->name()));
    result->next = nullptr;
    result->prev = nullptr;
    result->link_08[1] = nullptr; // result[3] = 0
    result->link_08[0] = nullptr; // result[2] = 0
    result->link_08[3] = &result->link_08[4]; // result[5] = &result[6]
    result->link_08[5] = &result->link_08[3]; // result[7] = &result[5]
    result->link_08[4] = nullptr; // result[6] = 0
    SpliceResourceNodeIntoGlobalList(result);
    result->link_08[2] = result;  // result[4] = result

    if (arg6 == nullptr)
    {
        result->meta_8f = 0;
        result->meta_90 = 0;
        return result;
    }

    result->meta_8f = arg6[0];
    result->meta_90 = arg6[1];
    return result;
}

// Native: Resource_FreeNode; retail VA 0x004B5C90; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B5C90-0x004B5D6A, 218 bytes, digest ff4a8050127977de2b75901a65663b90290f997e9f45732720f5b2c1994cb5b2; CLEAN/sys/004B5C90-Sys_FreeMemory_16.c.
// Behavior/CFG: switches on `context[9]` (`+0x24` resource kind): - case 0 (container): walks child list at `context[5]`, recursively calling Resource_FreeNode on each child node (`i_1[2]`), unlinks `context` via Camera_ApplyHandiCam_33 (0x004AF910), frees `context[0x0D]` via Sys_OpenFileStream_5 (0x004B1A30), and returns the slot to free-list `g_943e24` / `g_943e28`; - case 1, 4 (sub-resource): unlinks child link `&context[2]` and `context`, then returns slot; - case 2 (standalone raw resource): frees `context[0x0D]`, unlinks `context`, returns slot; - case 3: unlinks `context` and returns slot. radare2 13 blocks/18 edges; Binary Ninja 13 blocks; 100% structural agreement.
// Return value: when the freed node's `pool_active_idx` equals the decremented `g_943e28` (freeing the
// last active slot) retail returns the new count in EAX; otherwise, after the swap-pop relocates the
// last active node into the freed position, retail returns that relocated node's address in EAX
// (verified by executing retail 0x004B5C90 under angr: freeing a middle node of three returned
// 0x0600004AC = &pool[table[new_count]], and mutated that node's +0x244 to the freed index).
// Status/limits: STRUCTURALLY_VERIFIED; high for exact switch cases, child recursion, list removal, and pool swap-pop.
// Previous catalog alias: Sys_FreeMemory_16.
std::int32_t Resource_FreeNode(void* context)
{
    auto* const node = static_cast<ResourceHashNode*>(context);
    std::int32_t result = node->kind;
    std::int32_t source_1 = 0;

    switch (result)
    {
        case 0:
        {
            auto** i_1 = static_cast<void**>(node->link_08[3]);
            for (auto** i = static_cast<void**>(*i_1); i != nullptr; i = static_cast<void**>(*i))
            {
                Resource_FreeNode(i_1[2]);
                i_1 = i;
            }

            UnlinkResourceNodeFromGlobalList(node);
            if (g_sys_open_file_stream_5 != nullptr)
            {
                g_sys_open_file_stream_5(node->data);
            }
            goto label_4b5cf2;
        }
        case 1:
        case 4:
        {
            Camera_ApplyHandiCam_33(&node->link_08[0]);
            goto label_4b5ced;
        }
        case 2:
        {
            if (g_sys_open_file_stream_5 != nullptr)
            {
                g_sys_open_file_stream_5(node->data);
            }
        label_4b5ced:
            UnlinkResourceNodeFromGlobalList(node);
        label_4b5cf2:
            result = g_943e28 - 1;
            source_1 = node->pool_active_idx;
            g_943e28 = result;

            if (source_1 != result)
            {
                goto label_4b5d30;
            }
            break;
        }
        case 3:
        {
            UnlinkResourceNodeFromGlobalList(node);
            result = g_943e28 - 1;
            source_1 = node->pool_active_idx;
            g_943e28 = result;

            if (source_1 != result)
            {
            label_4b5d30:
                const std::int32_t last_slot = g_pool_free_indices[static_cast<std::size_t>(result)];
                ResourceHashNode* const last_node = &g_pool_nodes[static_cast<std::size_t>(last_slot)];
                const std::int32_t dest_2 = g_pool_free_indices[static_cast<std::size_t>(source_1)];
                const std::int32_t temp_2 = last_node->pool_active_idx;
                g_pool_free_indices[static_cast<std::size_t>(source_1)] =
                    g_pool_free_indices[static_cast<std::size_t>(temp_2)];
                g_pool_free_indices[static_cast<std::size_t>(temp_2)] = dest_2;
                last_node->pool_active_idx = source_1;
                // Retail leaves EAX holding the relocated last node's address on this path
                // (the decremented count only when no swap was needed; see provenance above).
                result = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(last_node)
                    & 0xFFFFFFFFULL);
            }
            break;
        }
        default:
            break;
    }

    return result;
}

// Helper that splices a child node's `&child->link_08[0]` (`&child[2]`) into the parent container's
// child list at `&parent->link_08[3]` (`parent + 0x14`), matching `Camera_SetFieldOfView_33(arg2 + 0x14, &child[2])`.
namespace {

void LinkChildIntoContainer(ResourceHashNode* parent, ResourceHashNode* child)
{
    void** const item = &child->link_08[0];
    // parent->link_08[3] = head_anchor (`result[5]`), parent->link_08[4] = 0 (`result[6]`),
    // parent->link_08[5] = tail (`result[7]`).
    void** const tail = &parent->link_08[5];
    item[1] = *tail;
    item[0] = &parent->link_08[4];
    *static_cast<void***>(*tail) = item;
    *tail = item;
}

} // namespace

// Native: Resource_RegisterIlffContainer; retail VA 0x004B60F0; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B60F0-0x004B637F, 655 bytes, digest 115d8cda2c658da73d9407d22baf6b6ee0562634c1e8d259034645ca75867cc6; CLEAN/sys/004B60F0-Sys_ErrorInResourceFileS.c.
// Behavior/CFG: walks ILFF chunks inside an `ILFF`/`IRES` container (`arg3`) starting at Ilff_GetFirstChunk(arg3) (0x004B7490): requires `NAME` (`0x454D414E`) followed by a payload chunk (`BODY` `0x59444F42`, nested `ILFF`/`IRES` `0x46464C49`/`0x53455249`, `CSTR` `0x52545343`, or `PATH` `0x48544150`, with optional `DBUG` `0x47554244` sibling), allocates a child resource node via Resource_CreateNode (0x004B59F0), and splices `&child[2]` into the parent container's child list (`arg2 + 0x14`) via HeapList_Insert (0x004AF8F0). Calls Error_Show("Error in resource file: '%s'", context) on malformed chunk tags. radare2 28 blocks/45 edges; Binary Ninja 28 blocks; 100% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact FourCC constants (`NAME`, `BODY`, `IRES`, `CSTR`, `PATH`, `DBUG`) and child-list linking.
// Previous catalog alias: Sys_ErrorInResourceFileS.
std::int32_t Resource_RegisterIlffContainer(const char* context, void* arg2, void* arg3,
                                            const std::int32_t* arg4)
{
    auto* const parent = static_cast<ResourceHashNode*>(arg2);
    auto* source_val = static_cast<std::int32_t*>(sys::Sys_ResolveSymbol_19(arg3));

    if (source_val != nullptr)
    {
        while (true)
        {
            const char* data_2 = nullptr;
            void* context_1 = nullptr;
            std::int32_t* data_4 = nullptr;
            std::int32_t item_c = 0;
            std::int32_t item_8 = 0;
            ResourceHashNode* body_child = nullptr;

            if (sys::Sys_OpenFileStream_20(source_val) != 0x454D414E) // 'NAME'
            {
                if (sys::Sys_OpenFileStream_20(source_val) == 0x46464C49) // 'ILFF'
                {
                    if (sys::Sys_FreeMemory_20(source_val) != 0x47554244) // 'DBUG'
                    {
                        goto label_4b636f;
                    }
                    goto label_4b6347;
                }

            label_4b636f:
                error::Error_Show("Error in resource file: '%s'", context);
                // Retail 0x004B637D: EB FE (jmp $) after Error_Show.
                // Malformed containers halt here; they do not return a failure code.
                for (;;)
                {
                }
            }
            else
            {
                data_2 = static_cast<const char*>(sys::Sys_CheckHeap_19(source_val, arg3));
                source_val = static_cast<std::int32_t*>(sys::Sys_PopStack_19(source_val));

                if (source_val == nullptr)
                {
                    goto label_4b636f;
                }

                context_1 = nullptr;
                data_4 = static_cast<std::int32_t*>(sys::Sys_PopStack_19(source_val));

                if (data_4 != nullptr
                    && sys::Sys_OpenFileStream_20(data_4) == 0x46464C49
                    && sys::Sys_FreeMemory_20(data_4) == 0x47554244)
                {
                    for (auto* i = static_cast<std::int32_t*>(sys::Sys_ResolveSymbol_19(data_4));
                         i != nullptr;
                         i = static_cast<std::int32_t*>(sys::Sys_PopStack_19(i)))
                    {
                        if (sys::Sys_OpenFileStream_20(i) == 0x52545343) // 'CSTR'
                        {
                            context_1 = sys::Sys_CheckHeap_19(i, data_4);
                        }
                    }
                }

                if (sys::Sys_OpenFileStream_20(source_val) != 0x59444F42) // 'BODY'
                {
                    const std::int32_t data_16 = sys::Sys_OpenFileStream_20(source_val);
                    std::int32_t data_17 = 0;

                    if (data_16 == 0x46464C49)
                    {
                        data_17 = sys::Sys_FreeMemory_20(source_val);
                    }

                    if (data_16 == 0x46464C49 && data_17 == 0x53455249) // 'ILFF' / 'IRES'
                    {
                        sys::Sys_AllocateMemory_20(&item_c, &item_8, source_val);
                        auto* const child = static_cast<ResourceHashNode*>(
                            Sys_PopStack_15(data_2, context_1, source_val, 0, 1, arg4));
                        LinkChildIntoContainer(parent, child);

                        if (Sys_ErrorInResourceFileS(context, arg2, source_val, arg4) == 0)
                        {
                            return 0;
                        }

                        goto label_4b6347;
                    }

                    if (sys::Sys_OpenFileStream_20(source_val) == 0x52545343) // 'CSTR'
                    {
                        sys::Sys_AllocateMemory_20(&item_c, &item_8, source_val);
                        auto* const child = static_cast<ResourceHashNode*>(
                            Sys_PopStack_15(data_2, nullptr, sys::Sys_CheckHeap_19(source_val, arg3),
                                            item_c, 1, arg4));
                        LinkChildIntoContainer(parent, child);
                        goto label_4b6347;
                    }

                    if (sys::Sys_OpenFileStream_20(source_val) != 0x48544150) // 'PATH'
                    {
                        goto label_4b636f;
                    }

                    sys::Sys_AllocateMemory_20(&item_c, &item_8, source_val);
                    auto* const child = static_cast<ResourceHashNode*>(
                        Sys_PopStack_15(data_2, context_1, sys::Sys_CheckHeap_19(source_val, arg3),
                                        item_c, 4, arg4));
                    LinkChildIntoContainer(parent, child);
                    goto label_4b6347;
                }

                sys::Sys_AllocateMemory_20(&item_c, &item_8, source_val);
                body_child = static_cast<ResourceHashNode*>(
                    Sys_PopStack_15(data_2, context_1, sys::Sys_CheckHeap_19(source_val, arg3),
                                    item_c, 1, arg4));
                LinkChildIntoContainer(parent, body_child);

            label_4b6347:
                source_val = static_cast<std::int32_t*>(sys::Sys_PopStack_19(source_val));
                if (source_val == nullptr)
                {
                    break;
                }
            }
        }
    }

    return 1;
}

// Native: Resource_RegisterBuffer; retail VA 0x004B5F80; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: copying each entry name (`*(entry + 8)` bytes at `entry + 0x0C`), aligning payload start by Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B5F80-0x004B60E1, 353 bytes, digest 7008df670ec82806c208a728e050b93acf7ce0d8b4a333b9ef0d2ef74bae3442; CLEAN/sys/004B5F80-Sys_Resource.c.
// Behavior/CFG: registers a loaded buffer `arg2` of byte length `arg3` under `context`: 1. Calls Sys_AllocateMemory_5(&item_8, context) (0x004B1820). 2. If `arg2` is an `ILFF`/`IRES` container (`0x46464C49` / `0x53455249`), creates parent node via Resource_CreateNode(context, nullptr, arg2, 0, 0, &item_8), populates children via Resource_RegisterIlffContainer(context, data_2, arg2, &item_8), sets `data_2->loaded_flag = 1`, and returns `arg2`. 3. Otherwise, if `arg3 < 8` or `Resource_MatchPrefix(arg2, "RESOURCE", 8) != 0`, creates a standalone node (`kind = 2`, `size = arg3`) with `loaded_flag = 1`. 4. Otherwise ("RESOURCE" packed archive header), creates a parent node (`kind = 0`), sets `loaded_flag = 1`, and iterates `arg2[2]` (`count`) entries starting at `&arg2[8]` (`+0x20`), advancing by the retail `and al, 0xE0` name stride — the low byte of `name_len + 0x2B` with bits 0-4 cleared, upper bits preserved — and by `(data_size + 0x1F) & 0xFFFFFFE0`, and splicing each child (`kind = 1`) into `parent + 0x14`. radare2 11 blocks/16 edges; Binary Ninja 11 blocks; 94.4% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for all three container branches (`ILFF`/`IRES`, `"RESOURCE"`, and raw file).
// Previous catalog alias: Sys_Resource.
void* Resource_RegisterBuffer(const char* context, void* arg2, std::int32_t arg3)
{
    std::int32_t item_8[2]{};
    if (g_sys_allocate_memory_5 != nullptr)
    {
        g_sys_allocate_memory_5(item_8, context);
    }

    auto* const dwords = static_cast<std::int32_t*>(arg2);
    if (sys::Sys_OpenFileStream_20(dwords) == 0x46464C49
        && sys::Sys_FreeMemory_20(dwords) == 0x53455249)
    {
        auto* const data_2 = static_cast<ResourceHashNode*>(
            Sys_PopStack_15(context, nullptr, arg2, 0, 0, item_8));
        Sys_ErrorInResourceFileS(context, data_2, arg2, item_8);
        data_2->loaded_flag = 1;
        return arg2;
    }

    if (arg3 < 8)
    {
        auto* const node = static_cast<ResourceHashNode*>(
            Sys_PopStack_15(context, nullptr, arg2, arg3, 2, item_8));
        node->loaded_flag = 1;
    }
    else if (Camera_ApplyLineFilter_21(arg2, "RESOURCE", 8) != 0)
    {
        auto* const node = static_cast<ResourceHashNode*>(
            Sys_PopStack_15(context, nullptr, arg2, arg3, 2, item_8));
        node->loaded_flag = 1;
    }
    else
    {
        auto* const data_5 = static_cast<ResourceHashNode*>(
            Sys_PopStack_15(context, nullptr, arg2, 0, 0, item_8));
        data_5->loaded_flag = 1;
        const std::int32_t entity_1 = dwords[2];
        auto* context_2 = reinterpret_cast<std::uint8_t*>(&dwords[8]);

        if (entity_1 > 0)
        {
            std::int32_t remaining = entity_1;
            bool flag_0_1 = false;

            do
            {
                const std::uint32_t data_7 = *reinterpret_cast<const std::uint32_t*>(context_2 + 8);
                const std::int32_t temp_1 = *reinterpret_cast<const std::int32_t*>(context_2 + 4);
                std::memcpy(g_943c20, context_2 + 0x0C, static_cast<std::size_t>(data_7));
                g_943c20[data_7] = '\0';

                // Retail computes the name stride with `add eax, 0x2B; and al, 0xE0`: only the low
                // byte participates in the bit clear, so bits 8+ of `name_len + 0x2B` survive.
                const std::uint32_t stride_input = data_7 + 0x2BU;
                const std::uint32_t data_8 = (stride_input & 0xFFFFFF00U) | ((stride_input & 0xFFU) & 0xE0U);
                auto* const context_3 = context_2 + data_8;
                context_2 = context_3 + ((static_cast<std::uint32_t>(temp_1) + 0x1FU) & 0xFFFFFFE0U);

                auto* const child = static_cast<ResourceHashNode*>(
                    Sys_PopStack_15(g_943c20, nullptr, context_3, temp_1, 1, item_8));
                LinkChildIntoContainer(data_5, child);

                flag_0_1 = (remaining != 1);
                remaining -= 1;
            } while (flag_0_1);
        }
    }

    return arg2;
}

void Resource_InitPoolForTest(std::size_t capacity)
{
    g_pool_nodes.assign(capacity, ResourceHashNode{});
    g_pool_free_indices.resize(capacity);
    for (std::size_t i = 0; i < capacity; ++i)
    {
        g_pool_free_indices[i] = static_cast<std::int32_t>(i);
    }
    g_943e28 = 0;

    auto& root = Resource_GetHashRoot();
    root = ResourceHashNode{};
    root.hash = -1;

    Sys_SetFreeMemory16(&FreeMemory16Thunk);
    Sys_SetResource(&Sys_Resource);
}

std::int32_t Resource_GetActiveCount() noexcept
{
    return g_943e28;
}

void Sys_SetAllocateMemory5(SysAllocateMemory5Fn fn) noexcept
{
    g_sys_allocate_memory_5 = fn;
}

void Sys_SetOpenFileStream5(SysOpenFileStream5Fn fn) noexcept
{
    g_sys_open_file_stream_5 = fn;
}

} // namespace igi::retail::resource
