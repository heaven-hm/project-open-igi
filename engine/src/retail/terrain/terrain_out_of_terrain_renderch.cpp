// Native: Terrain_OutOfTerrainRenderch; retail VA 0x00514680.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP 0.1.0 (single selected decompiler), radare2 6.2.0 CFG and angr 10.0.4.post1 CFGFast on retail IGI.EXE; evidence docs/fidelity/evidence/0x00514680-*.json; no Ghidra analysis was run for this target.
// Behavior/CFG: hash-bucketed render-chunk cache lookup ((arg2*0xB5AF + arg1*0x9D) % 0x1FD) returns a matching node or allocates from the 0x2EF8-stride pool; exhaustion reports "Out of terrain renderchunks" and halts. Binary Ninja and radare2 recover the same retail graph.
// Status/limits: STRUCTURALLY_VERIFIED; not wired into CMakeLists.txt; no runtime capture exists. The retail pool initialisers 0x00516621 and 0x004B0B00 remain runtime preconditions, detailed below.
//
// Retail structure (from instructions, not decompiler naming)
//   0x00514680  push ebx/esi/edi
//   0x00514681  ebx = [esp+0x0C]  (arg2, pass)
//   0x00514687  edi = [esp+0x10]  (arg1, texture owner)
//   0x0051468B  hash = (uint32)(arg2 * 0xB5AF + arg1 * 0x9D) % 0x1FD   (unsigned div)
//   0x005146AA  bucket = 0x00A7E080 + hash * 12
//   0x005146AD  eax = bucket->head; ecx = [eax]; if (ecx == 0) goto allocate
//   0x005146C1  if ([eax+0x2EEC] == arg1 && [eax+0x2EF0] == arg2) return eax
//   0x005146D5  eax = ecx; ecx = [ecx]; if (ecx != 0) goto 0x5146C1
//   0x005146DD  if (g_a7f880 == g_a7f884) { Debug_Error("Out of terrain renderchunks"); jmp $; }
//   0x005146FB  slot = g_a7f87c[g_a7f880]; node = g_a7f878 + slot * g_a7f888
//   0x00514713  [node + g_a7f88c] = g_a7f880; ++g_a7f880
//   0x00514729  [node+0x2EE8]=0; [node+0x2EEC]=arg1; [node+0x2EF0]=arg2;
//               [node+4]=0; [node]=0; Camera_ApplyLineFilter_33(bucket, node); return node
//
// The traversal is exactly "read next; if next is zero stop; test the current node". The empty
// bucket head points at the record's sentinel_next dword (zero); a non-empty chain ends at the
// sentinel, whose next dword is also zero, so the last real node is still tested.
//
// Pool/layout evidence
//   0x004B0B00(&g_a7f878, 0x2EF4, 0x80, 0) called from 0x00516640:
//     g_a7f888 = align(0x2EF4, 4) = 0x2EF8 (stride)
//     g_a7f87c = base + 0x2EF8 * 0x80
//     g_a7f880 = 0 (used), g_a7f884 = 0x80 (capacity), g_a7f88c = 0x2EF4 (requested size)
//   0x00516621 initialises the 509 buckets at 0x00A7E080: [rec]=rec+4, [rec+8]=rec, [rec+4]=0.
//   The slot index is stored at node+0x2EF4, i.e. in the four alignment-padding bytes after the
//   0x2EF4-byte element; this is why the pool stride is 0x2EF8 and not 0x2EF4.
//
// Status/limits: STRUCTURALLY_VERIFIED; not wired into CMakeLists.txt; no runtime capture exists
// for this source. The bucket table (g_a7e080) and pool capacity (g_a7f884) are defined in this
// translation unit; the remaining pool globals are defined once by terrain_draw.cpp and g_a7f88c
// is std::uint32_t there. The retail initialisers 0x00516621 (buckets) and 0x004B0B00 (pool) are
// not executed by this candidate; the definitions below are the zero-filled file-image state and
// those initialisers remain runtime preconditions, exactly as retail requires.
#include "igi/retail/terrain/terrain_out_of_terrain_renderch.hpp"

#include <cstddef>
#include <cstring>

#include "igi/retail/error/error_show.hpp"

namespace igi::retail::terrain
{
namespace
{
static_assert(sizeof(void*) == 4, "this transcription models the 32-bit retail address space");
static_assert(sizeof(TerrainRenderChunk) == 0x2EF4, "retail element size");
static_assert(offsetof(TerrainRenderChunk, next) == chunk_offset_next, "node.next");
static_assert(offsetof(TerrainRenderChunk, prev) == chunk_offset_prev, "node.prev");
static_assert(offsetof(TerrainRenderChunk, triangle_count) == chunk_offset_triangle_count,
              "node.triangle_count");
static_assert(offsetof(TerrainRenderChunk, texture_owner) == chunk_offset_texture_owner,
              "node.texture_owner");
static_assert(offsetof(TerrainRenderChunk, pass) == chunk_offset_pass, "node.pass");
static_assert(sizeof(TerrainRenderChunkBucket) == 0x0C, "retail bucket size");
static_assert(offsetof(TerrainRenderChunkBucket, head) == 0x00, "bucket.head");
static_assert(offsetof(TerrainRenderChunkBucket, sentinel_next) == 0x04, "bucket.sentinel_next");
static_assert(offsetof(TerrainRenderChunkBucket, sentinel_prev) == 0x08, "bucket.sentinel_prev");

using Address = std::uint32_t;

[[nodiscard]] Address to_address(const void* pointer) noexcept
{
    return static_cast<Address>(reinterpret_cast<std::uintptr_t>(pointer));
}

[[nodiscard]] TerrainRenderChunk* to_chunk(Address address) noexcept
{
    return reinterpret_cast<TerrainRenderChunk*>(static_cast<std::uintptr_t>(address));
}

// Raw 32-bit reads/writes keep the retail address arithmetic and avoid type-punning the bucket
// sentinel (which is not a complete TerrainRenderChunk object) through a TerrainRenderChunk*.
[[nodiscard]] std::uint32_t load_u32(Address address) noexcept
{
    std::uint32_t value;
    std::memcpy(&value, reinterpret_cast<const void*>(static_cast<std::uintptr_t>(address)),
                sizeof value);
    return value;
}

void store_u32(Address address, std::uint32_t value) noexcept
{
    std::memcpy(reinterpret_cast<void*>(static_cast<std::uintptr_t>(address)), &value,
                sizeof value);
}

// 0x005146F9: retail jumps to itself after the fatal error. Keep the hang observable instead of
// letting the optimizer fall through a side-effect-free loop.
[[noreturn]] void halt_forever() noexcept
{
    for (;;)
    {
    }
}

// 0x004AF910 Camera_ApplyLineFilter_33, inlined because it is a resolved 23-byte leaf:
//   edx = [bucket]; node[1] = bucket; node[0] = edx; [edx+4] = node; [bucket] = node
// The helper's return value (bucket) is ignored by the caller.
void insert_at_bucket_head(TerrainRenderChunkBucket& bucket, Address node) noexcept
{
    const Address old_head = to_address(bucket.head);              // 0x004AF918
    store_u32(node + chunk_offset_prev, to_address(&bucket));      // 0x004AF91A
    store_u32(node + chunk_offset_next, old_head);                 // 0x004AF91D
    store_u32(old_head + chunk_offset_prev, node);                 // 0x004AF921
    bucket.head = to_chunk(node);                                  // 0x004AF924
}
} // namespace

// 0x00A7E080: 509 12-byte buckets. 0x00516621 `mov eax, 0xa7e080` initialises each record
// [rec] = rec + 4, [rec + 8] = rec, [rec + 4] = 0 and advances by 0xC until 0xA7F85C, so the
// extent is 509 * 12 = 0x17DC bytes (0x00A7E080..0x00A7F85B). This is the one engine definition.
// The candidate stores the zero-filled file-image state; the retail initialiser above must run
// before Terrain_OutOfTerrainRenderch traverses a bucket.
TerrainRenderChunkBucket g_a7e080[terrain_render_chunk_bucket_count] = {};

// 0x00A7F884: 4-byte element capacity. 0x004B0B4B `mov dword [esi+0xc], ebx` (ebx = arg3 = 0x80
// from 0x0051669F) sets it during the 0x00516640 pool initialiser; 0x005146E2
// `mov ecx, [0xa7f884]` reads it for the fatal count == capacity test. One engine definition,
// zero in the file image and 0x80 only after that initialiser runs.
std::uint32_t g_a7f884 = 0;

extern "C" TerrainRenderChunk* Terrain_OutOfTerrainRenderch(const void* texture_owner,
                                                           std::int32_t pass)
{
    const Address key = to_address(texture_owner);                 // 0x00514687 edi
    const std::uint32_t pass_value = static_cast<std::uint32_t>(pass); // 0x00514681 ebx

    // 0x0051468B-0x005146A8: 32-bit multiply/add wrapping, then unsigned division by 0x1FD.
    const std::uint32_t hash = (pass_value * 0xB5AFU + key * 0x9DU) % 0x1FDU;

    // 0x005146AA-0x005146B4: edx = hash*3; eax = [0x00A7E080 + hash*12]; edx = same address.
    TerrainRenderChunkBucket& bucket = g_a7e080[hash];

    // 0x005146BB-0x005146DB: sentinel-terminated walk. `next` is read before the key/pass test, so
    // the sentinel (whose next dword is zero) is never compared as a real chunk.
    Address node = to_address(bucket.head);
    for (;;)
    {
        const Address next = load_u32(node + chunk_offset_next);   // 0x005146BB
        if (next == 0)                                             // 0x005146BF
        {
            break;
        }
        if (load_u32(node + chunk_offset_texture_owner) == key)    // 0x005146C1
        {
            if (load_u32(node + chunk_offset_pass) == pass_value)  // 0x005146C9
            {
                return to_chunk(node);                             // 0x005146CF -> 0x00514756
            }
        }
        node = next;                                               // 0x005146D5
    }

    // 0x005146DD-0x005146EA: the fatal condition is equality, not >=. A corrupted count above the
    // capacity follows the allocation path in retail, so the comparison is kept exact. The count is
    // loaded once and reused for the free-slot index and the slot store; retail reloads it only
    // for the increment at 0x0051471E.
    const std::uint32_t count = g_a7f880;                      // 0x005146DD
    if (count == g_a7f884)
    {
        error::Error_Show("Out of terrain renderchunks"); // retail callee 0x004AF7B0 (Debug_Error)
        halt_forever();                                    // 0x005146F9 jmp $
    }

    // 0x005146FB-0x00514724: pop the next free slot, form the 32-bit byte offset, store the
    // element's own slot index in the four bytes at element + g_a7f88c, then advance the used count.
    const std::uint32_t slot = g_a7f87c[count];                // 0x00514701
    const Address element = g_a7f878 + slot * g_a7f888;        // 0x00514704-0x00514711
    // 0x00514713-0x0051471B: retail loads the slot offset from 0x00A7F88C (0x2EF4 after the pool
    // initialiser) and stores at [node + offset]; keep the global read rather than its value.
    store_u32(element + g_a7f88c, count);                      // 0x00514713/0x0051471B
    ++g_a7f880;                                                // 0x0051471E-0x00514724

    // 0x00514729-0x00514746: initialise the chunk before linking it. The two stack arguments for
    // the insert call are pushed at 0x00514719/0x0051471A, before these stores; the values are
    // unchanged, so calling the helper after the stores preserves the retail effect.
    store_u32(element + chunk_offset_triangle_count, 0);       // 0x00514729
    store_u32(element + chunk_offset_texture_owner, key);      // 0x00514733
    store_u32(element + chunk_offset_pass, pass_value);        // 0x00514739
    store_u32(element + chunk_offset_prev, 0);                 // 0x0051473F
    store_u32(element + chunk_offset_next, 0);                 // 0x00514746

    insert_at_bucket_head(bucket, element);                    // 0x0051474C -> 0x004AF910
    return to_chunk(element);                                  // 0x00514754 eax = esi
}
} // namespace igi::retail::terrain
