// Native: Terrain_InitContourHashAndPools_4D11C0 (behavior-derived placeholder name; retail symbol unknown; it is the only initializer of the 0x00B04340 contour-hash fields +0x10/+0x18 and of the three quadtree pool descriptors 0x00B12B60/0x00B12C40/0x00B16CC0). retail VA 0x004D11C0-0x004D121A (91 bytes, 4 blocks, 1 edge, 24 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: capstone 5.0.7 reachability decode from the verified entry (fallthrough plus every branch target, no fallthrough past a ret) over the exact image; the extent is bounded by the `ret` at 0x004D121A, and five alignment nops `90 90 90 90 90` follow it (next entry 0x004D1220). Exact bytes b8 58 43 b0 00 83 c9 ff 89 48 f8 c7 00 00 00 00 00 83 c0 1c 3d 58 23 b1 00 7c ed 6a 20 68 bc 02 00 00 68 40 06 00 00 68 60 2b b1 00 e8 0f f9 fd ff 6a 20 6a 32 68 80 0c 00 00 68 40 2c b1 00 e8 fc f8 fd ff 6a 20 6a 00 68 40 1f 00 00 68 c0 6c b1 00 e8 e9 f8 fd ff 83 c4 30 c3 = `mov $0x00B04358,%eax` / `or $0xFFFFFFFF,%ecx` / loop `mov %ecx,-0x8(%eax)` / `movl $0x0,(%eax)` / `add $0x1C,%eax` / `cmp $0x00B12358,%eax` / `jl` / three `push imm` groups / `call 0x004B0B00` / `add $0x30,%esp` / `ret`. Every call displacement resolves as v+5+rel32: 0x004D11F8 + 0xFFFFF90F = 0x004B0B00 (three times: 0x004D11F8, 0x004D1205, 0x004D1212). The loop bound 0x00B12358 - 0x00B04358 = 0xE000 = 0x800 records x 0x1C, i.e. every entry of g_b04340. Call census from the image's 0xE8 enumeration: 0x004C6240 (Terrain_Load) is the only caller.
// Behavior/CFG: no arguments, cdecl, no frame. One loop over all 0x800 records of 0x00B04340 writing the dword -1 at record+0x10 and the dword 0 at record+0x18 (retail walks a cursor at 0x00B04358 = &g_b04340[0] + 0x18 with stride 0x1C, so `[eax-8]` is +0x10 and `[eax]` is +0x18 of the same record, and the two writes stay in that order per record). Then three indexed-block allocations whose FIRST argument is the LAST push: Mem_AllocIndexedBlock(&0x00B12B60, 0x640, 0x2BC, 0x20), Mem_AllocIndexedBlock(&0x00B12C40, 0xC80, 0x32, 0x20) and Mem_AllocIndexedBlock(&0x00B16CC0, 0x1F40, 0, 0x20) - argument order per engine/include/igi/retail/mem/task_list.hpp (descriptor, element size, count, alignment), matching the tree's own reading of the same native in core/native_47cf70.cpp. The third call has count 0, so it allocates the descriptor/index table only; that is retail's value and is not corrected. 0x004D1219 `add $0x30,%esp` is the cdecl cleanup for the twelve pushed argument dwords, emitted by the compiler.
// Status/limits: TRANSCRIBED; LINKABLE (its single callee 0x004B0B00 Mem_AllocIndexedBlock and the four globals g_b04340 / g_b12b60 / g_b12c40 / g_b16cc0 are all tree natives, so it adds no new dependency); STRUCTURALLY_VERIFIED against the 24-instruction listing (4 blocks, 1 edge); runtime UNOBSERVED. The retail return value is left in EAX by the last allocation and no caller consumes it, so the port declares void. Domain: terrain/, because the hash table it initializes is the previous-frame contour hash cache owned by terrain_storage.cpp and the three descriptors are the quadtree free-list pools owned by terrain_loaders.cpp; it is reached only from Terrain_Load (0x004C6240).

#include <cstdint>

#include "igi/retail/mem/task_list.hpp"
#include "igi/retail/terrain/terrain_quadtree.hpp"

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

void NATIVE_DECL Terrain_InitContourHashAndPools_4D11C0()
{
    // 0x004D11C0: `mov eax,0x00B04358` (= &g_b04340[0] + 0x18); 0x004D11C5: `or ecx,-1`; 0x004D11C8-0x004D11D5: the loop
    // body `mov [eax-8],ecx` (record+0x10, stamp) then `mov dword [eax],0` (record+0x18, data), `add eax,0x1C`,
    // `cmp eax,0x00B12358`, `jl`. The bound is the end of the 0x800-record table, so this is every entry.
    for (std::int32_t index = 0; index < 0x800; ++index)
    {
        igi::retail::terrain::g_b04340[index].stamp = 0xFFFFFFFFU; // 0x004D11C8: `mov %ecx,-0x8(%eax)`, ecx = -1
        igi::retail::terrain::g_b04340[index].data = 0U;           // 0x004D11CB: `movl $0x0,(%eax)`
    }
    // 0x004D11D7-0x004D11F8: `push 0x20` / `push 0x2BC` / `push 0x640` / `push 0x00B12B60` / `call 0x004B0B00` - pool A.
    (void)Mem_AllocIndexedBlock(&igi::retail::terrain::g_b12b60, 0x640, 0x2BC, 0x20);
    // 0x004D11FD-0x004D1205: `push 0x20` / `push 0x32` / `push 0xC80` / `push 0x00B12C40` / `call 0x004B0B00` - pool B.
    (void)Mem_AllocIndexedBlock(&igi::retail::terrain::g_b12c40, 0xC80, 0x32, 0x20);
    // 0x004D120A-0x004D1212: `push 0x20` / `push 0` / `push 0x1F40` / `push 0x00B16CC0` / `call 0x004B0B00` - pool C, count 0.
    (void)Mem_AllocIndexedBlock(&igi::retail::terrain::g_b16cc0, 0x1F40, 0, 0x20);
    // 0x004D1219: `add esp,0x30` (twelve argument dwords) / 0x004D121A: `ret`. EAX holds the last allocation's result.
}

} // extern "C"
