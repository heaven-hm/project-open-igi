// Native: Terrain_TraverseContourQuadtree; retail VA 0x004D14E0; signature int32_t __cdecl(void*).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01 (1,384,448 bytes, image base 0x00400000, entry 0x004A50F5, machine i386).
// Tools/evidence: Binary Ninja 5.0, radare2 6.0.5 and angr CFGFast on 0x004D14E0 (exact counts and caveats below).
// Behavior/CFG: 146 blocks / 228 edges; split/merge octree traversal with octant frustum culling, five-level descent callbacks and a 0x800-slot previous-frame cache; always returns 0x800.
// Status/limits: STRUCTURALLY_VERIFIED (source/object level). The retail-derived level arithmetic is
// transcribed at every shift: cl = level+1 for centre_x/y/z, corner_z_level, half_extent and
// world_x/y (0x004D1E6D..0x004D1F5C), and level for the merge undo (0x004D1B76) and the
// post-increment corner advance (0x004D1D39). The worklist is retail's own 128-entry stack region
// (ebp-0x408..ebp-0x8) and its maximum live occupancy is proved to be 1 + 8*15 = 121.
// Linkage: this unit is in the igi target (CMakeLists.txt), terrain_loaders.cpp no longer carries the
// throwing placeholder, and g_bcab08/g_bcaae0 have their single definition in terrain_storage.cpp.
// The linked body in build-clean-win32/igi1.exe was linked before this correction, so a host rebuild
// is outstanding and no executable-level claim is made here.
// Evidence: docs/fidelity/evidence/0x004D14E0-structural-comparison.json (per-property comparison,
// worklist proof, candidate digests) and docs/fidelity/native-verification.json.
// Not claimed: runtime behaviour, memory effects, rendered output or culling equality; only static
// structure and control flow were compared, and only against the pre-link object.
//
// Details:
//   * Binary Ninja 5.0: function 0x004D14E0, 4,181 bytes, 146 basic blocks, 228 edges,
//     1,265 instructions, definition `int32_t(void* arg1)`.
//   * radare2 6.0.5: `afij` size 4,181, 146 blocks, 228 edges (exact agreement with Binary Ninja);
//     `pdf~call` shows only the two direct calls 0x004C5810 and 0x004AF7B0.
//   * angr CFGFast: entry 0x004D14E0 recovered, 159 blocks, 245 edges, no unresolved jumps.
//     The extra blocks are rep-movsd/loop self-blocks (0x4D15B6, 0x4D15EC, 0x4D162F, 0x4D18C4,
//     0x4D2347) and the three intentional `jmp $` sites (0x4D242D, 0x4D243C, 0x4D2533).
//   * Callers: Render_DrawLightmapMesh_4 (0x004D2540) at 0x004D2639, itself called from
//     Render_SkinBones_4 (0x004D2650) at 0x004D26AD.
//   * Direct callees: 0x004C5810 (resets g_b97df4/g_b97d24/g_b97df8/g_ba2058/g_ba5060, returns 0;
//     Binary Ninja's name DynObj_ParseConfigFile_6 is misleading) and 0x004AF7B0 (Debug_Error,
//     sprintf into 0xA84640, optional g_936268 callback, tail dispatch through g_93626C).
// Scope: one function, 0x004D14E0..0x004D2535. The translation unit is self-contained apart from the
//   retail-owned runtime state declared in terrain_quadtree.hpp and the two unresolved callees.
// CFG/branches: every conditional in the retail body has a counterpart below; the split and merge
//   halves are entered from the single test of work-item bit 0 at 0x004D1A5C, the octant loop runs
//   exactly eight times (`var_62c_3` from 0x0C to 0x2C), the level descent runs exactly five times
//   (`var_620_1` = 5), and the eviction loop runs exactly 0x800 times.
// Gaps carried in this candidate:
//     - the two callback tables at 0x00A9CAE0/0x00A9C4E0 are all zero in the file image and are
//       populated at runtime, so the callbacks cannot be resolved statically;
//     - 0x004AF7B0 and 0x004C5810 are declared and called, not reconstructed here;
//     - the 0x20-byte child record stride at 0x004D2052 conflicts with the 0x40-byte node view
//       (the root node is [context+0xC]+0x20 and every node field read goes up to +0x3D), so the
//       relationship between the .ctr record array and the traversal node array is unresolved;
//     - the worklist bound of 121 live entries holds only while the executed algorithm does not
//       fault on a deeper LOD node chain: a level-16 entry would descend P[k] at 0x004D1C74 and
//       read [child+0x30] at 0x004D1C82, so a hierarchy that supplied deeper
//       chains could push past the proved bound. No such input is established; the array therefore
//       keeps retail's own 128 entries and no synthetic guard is added;
//     - node +0x08 (restore pointer) overlaps contour_index[4..5] and node +0x18 (child mask)
//       overlaps child[3]; the implementation keeps raw byte-offset access to preserve the aliasing;
//     - x87 80-bit intermediates are modelled with double; comparisons use IEEE strict < / > and
//       may differ from x87 on NaN or on the last bit; FISTP is modelled with `static_cast`
//       (truncation), but the runtime x87 control word was not observed, so if the game runs with
//       round-to-nearest those integer conversions can differ by one;
//     - the 32-bit node/child pointers are only valid for the i686 target.
#include "igi/retail/terrain/terrain_quadtree.hpp"

#include "igi/retail/terrain/terrain_loaders.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstring>

namespace igi::retail::terrain
{
namespace
{
// -------------------------------------------------------------------------------------------------
// Constants read from the retail image (checked bytes / immediates in the disassembly).
// -------------------------------------------------------------------------------------------------

// 0x00534B10 float 1.73210001; 0x00534B0C float 1073741824.0; 0x00533 504 float 0.5.
// 0x00534AE0 double 64.0; 0x00534B08 float 9600.0.
constexpr float kSqrtThreeLiteral = 1.73210001f;
constexpr float kTwoPowThirty = 1073741824.0f;
constexpr float kHalf = 0.5f;
constexpr double kCubeExtent = 64.0;
constexpr float kVisibilityRange = 9600.0f;

// 0x004D1A33 initialises the parent level to -1; 0x004D1F2A forms 0x40000000 >> (level+1).
constexpr std::int32_t kCornerCoordinate = 0x40000000; // 1 << 30
constexpr std::int32_t kInitialLevel = -1;

// 0x004D150F stores 0xE0000000 when the height-source pointer is null.
constexpr std::int32_t kNoHeightLimit = static_cast<std::int32_t>(0xE0000000u);

// 0x004D2104/0x004D2133: the cache has 0x800 entries and the probe wraps with & 0x7FF.
constexpr std::uint32_t kHashEntryCount = 0x800;
constexpr std::uint32_t kHashProbeMask = 0x7FF;

// 0x004D21E3: records above this count are dropped and counted in g_b12b80.
constexpr std::int32_t kRecordLimit = 0xFFF;
// 0x004D208A: subdivision stops at level 0x10.
constexpr std::int32_t kMaximumLevel = 0x10;
// 0x004D20A9: cache insertion starts above level 6.
constexpr std::int32_t kHashLevelThreshold = 0x6;
// 0x004D1E16..0x004D2312: the octant loop increments var_62c_3 from 0x0C to 0x2C.
constexpr int kOctantOffsets = 8;
// 0x004D1A99: the level descent count is 5.
constexpr int kLevelCount = 5;
// Worklist (0x004D1A20 `lea esi,[esp+0x240]`): retail grows it upward with no bounds check. The
// post-push esp is ebp-0x648, so the base is ebp-0x408; the bytes up to the topmost local (var_8 at
// ebp-0x8) are 0x400 = 128 eight-byte entries, and Binary Ninja's stack layout places no other
// local inside that interval. 128 is retail's own capacity, so it is kept unchanged.
// Maximum live occupancy, derived from the algorithm: a visit pushes at most one entry per octant
// (the subdivide path jumps to the same push at 0x004D209F, and the mark path only pushes when
// var_628_6 is non-zero at 0x004D22A2/0x004D22A8) and the octant loop runs eight times, so at most
// eight entries are live per level; the root occupies level 0, an entry is processed only at
// level+1 (0x004D1D37), and the subdivision site refuses level+1 >= 0x10
// (0x004D2085..0x004D208D). The mark path is not level-guarded, but a push from level 15 would
// create a level-16 entry whose split half descends P[k] by one more node (0x004D1C74..0x004D1C7E)
// and reads [child+0x30] at 0x004D1C82; at the last level that child is null, so the executed
// algorithm cannot take the push and the level never exceeds 15. The shared push block
// (0x004D22B1..0x004D22D0) is therefore capped at level 15: 1 + 8*15 = 121 <= 128.
constexpr int kWorklistCapacity = 128;

// 0x00548444: byte table `00 01 02 04 05 00 00 00 ...`; the traversal uses the first five bytes.
// 0x00548060: 8 cases x 8 octants, stored as 64 dwords; 0x004D15EC reads the low byte of each dword
// (stride 4) into var_488, which is exactly this 64-byte sequence.
constexpr std::uint8_t kChildIndexTable[0x40] = {
    0, 1, 2, 3, 4, 5, 6, 7,
    2, 0, 3, 1, 6, 4, 7, 5,
    3, 2, 1, 0, 7, 6, 5, 4,
    1, 3, 0, 2, 5, 7, 4, 6,
    1, 0, 3, 2, 5, 4, 7, 6,
    3, 1, 2, 0, 7, 5, 6, 4,
    2, 3, 0, 1, 6, 7, 4, 5,
    0, 2, 1, 3, 4, 6, 5, 7,
};

// 0x00548444: the five LOD node slot indices 0, 1, 2, 4, 5 (slot 3 is skipped). The code reaches
// it as 0x0054843A + 0x0A, i.e. base 0x00548444 indexed by k-0x0A.
constexpr std::uint8_t kLodSlotTable[5] = {0, 1, 2, 4, 5};

// 0x004D0EF0 builds the 0x40-byte table at 0x00B12BE0 as rows 0..3 `rot | (j & 4)` and rows 4..7
// `((r-j)&3) | (~j & 4)`; 0x004D15B6 copies it to var_448. The only write to 0x00B12BE0 is that
// builder, so the verified content is reproduced here and the copy is kept for shape.
constexpr std::uint8_t kOctantPermutation[0x40] = {
    0, 1, 2, 3, 4, 5, 6, 7,
    1, 2, 3, 0, 5, 6, 7, 4,
    2, 3, 0, 1, 6, 7, 4, 5,
    3, 0, 1, 2, 7, 4, 5, 6,
    4, 7, 6, 5, 0, 3, 2, 1,
    5, 4, 7, 6, 1, 0, 3, 2,
    6, 5, 4, 7, 2, 1, 0, 3,
    7, 6, 5, 4, 3, 2, 1, 0,
};

// -------------------------------------------------------------------------------------------------
// Raw byte-offset access. The retail node is 0x40 bytes but its fields overlap (see the file
// header); byte offsets keep every load/store faithful to the instruction that produced it.
// -------------------------------------------------------------------------------------------------

[[nodiscard]] inline std::uint8_t load_u8(const void* base, std::size_t offset) noexcept
{
    std::uint8_t value = 0;
    std::memcpy(&value, static_cast<const std::uint8_t*>(base) + offset, sizeof value);
    return value;
}

[[nodiscard]] inline std::uint16_t load_u16(const void* base, std::size_t offset) noexcept
{
    std::uint16_t value = 0;
    std::memcpy(&value, static_cast<const std::uint8_t*>(base) + offset, sizeof value);
    return value;
}

[[nodiscard]] inline std::uint32_t load_u32(const void* base, std::size_t offset) noexcept
{
    std::uint32_t value = 0;
    std::memcpy(&value, static_cast<const std::uint8_t*>(base) + offset, sizeof value);
    return value;
}

[[nodiscard]] inline double load_f64(const void* base, std::size_t offset) noexcept
{
    double value = 0.0;
    std::memcpy(&value, static_cast<const std::uint8_t*>(base) + offset, sizeof value);
    return value;
}

inline void store_u8(void* base, std::size_t offset, std::uint8_t value) noexcept
{
    std::memcpy(static_cast<std::uint8_t*>(base) + offset, &value, sizeof value);
}

inline void store_u32(void* base, std::size_t offset, std::uint32_t value) noexcept
{
    std::memcpy(static_cast<std::uint8_t*>(base) + offset, &value, sizeof value);
}

inline void store_f64(void* base, std::size_t offset, double value) noexcept
{
    std::memcpy(static_cast<std::uint8_t*>(base) + offset, &value, sizeof value);
}

// 32-bit retail pointers are stored as dwords in the node and .ctr records.
[[nodiscard]] inline std::uint8_t* as_node(std::uint32_t value) noexcept
{
    return reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(value));
}

[[nodiscard]] inline void* as_object(std::uint32_t value) noexcept
{
    return reinterpret_cast<void*>(static_cast<std::uintptr_t>(value));
}

[[nodiscard]] inline std::uint32_t as_u32(const void* pointer) noexcept
{
    return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pointer));
}

// 0x004D1818/0x004D182E/...: FISTP qword then use the low dword. Values here fit in 64 bits; the
// cast is round-toward-zero, the default x87 rounding mode.
[[nodiscard]] inline std::int32_t truncate_to_i32(double value) noexcept
{
    return static_cast<std::int32_t>(static_cast<std::int64_t>(value));
}

// 0x004D1B7A etc. use SAR, i.e. arithmetic right shift.
[[nodiscard]] inline std::int32_t shift_right(std::int32_t value, unsigned amount) noexcept
{
    return value >> amount;
}

// 32-bit adds/subs wrap in the retail code; make the wrap explicit instead of relying on signed
// overflow.
[[nodiscard]] inline std::int32_t add_i32(std::int32_t left, std::int32_t right) noexcept
{
    return static_cast<std::int32_t>(static_cast<std::uint32_t>(left) + static_cast<std::uint32_t>(right));
}

[[nodiscard]] inline std::int32_t sub_i32(std::int32_t left, std::int32_t right) noexcept
{
    return static_cast<std::int32_t>(static_cast<std::uint32_t>(left) - static_cast<std::uint32_t>(right));
}

[[nodiscard]] inline std::int32_t abs_i32(std::int32_t value) noexcept
{
    if (value >= 0)
    {
        return value;
    }
    return static_cast<std::int32_t>(0u - static_cast<std::uint32_t>(value));
}

// 0x004D1915..0x004D1976: eight iterations shift the accumulator left and add the next low bit, so
// bit 0 becomes bit 7. This is a full 8-bit reversal.
[[nodiscard]] inline std::uint8_t reverse_bits8(std::uint8_t value) noexcept
{
    std::uint8_t result = 0;
    for (int bit = 0; bit < 8; ++bit)
    {
        result = static_cast<std::uint8_t>((result << 1) | ((value >> bit) & 1u));
    }
    return result;
}

// 0x004D1CA9 / 0x004D1AE3: `push [item+0x48]; push item`. The callback receives the item first and
// the +0x48 argument second; the return value is ignored.
inline void invoke_callback(TerrainContourCallback callback, void* item) noexcept
{
    callback(item, as_object(load_u32(item, 0x48)));
}

// 0x004D1AE3/0x004D1CAB: zero-extended 16-bit category index selects the callback.
[[nodiscard]] inline std::uint16_t item_category(const void* item) noexcept
{
    return load_u16(item, 0x1C);
}

// 0x004D1B18..0x004D1B3F (type 5) and 0x004D1CDA..0x004D1D02 (type 4): append an 8-byte allocation
// record and bump the global count.
inline void emit_allocation_record(std::uint8_t type, void* item) noexcept
{
    std::uint8_t* cursor = g_b12c58;
    store_u32(cursor, 4, as_u32(item));
    cursor = g_b12c58;
    store_u8(cursor, 0, type);
    cursor = g_b12c58;
    g_b12c58 = cursor + 8;
    ++g_b12c20;
}

// 0x004D23CC..0x004D241B (pool A), 0x004D2443..0x004D2492 (pool B),
// 0x004D249D..0x004D24E2 (pool C): return a handle to the pool and patch the map/handle arrays.
void release_pool_slot(
    std::uint32_t data,
    std::int32_t*& handles,
    std::int32_t& count,
    std::int32_t* map,
    std::int32_t base,
    std::int32_t stride) noexcept
{
    const std::int32_t handle = handles[data];
    const std::int32_t top = --count;
    if (handle != top)
    {
        const std::int32_t slot = map[top] * stride + base;
        const std::int32_t old_mapping = map[handle];
        const std::int32_t moved_handle = handles[slot];
        map[handle] = map[moved_handle];
        map[moved_handle] = old_mapping;
        handles[slot] = handle;
    }
}

// 0x004D242D / 0x004D243C / 0x004D2533: retail spins forever after Debug_Error.
[[noreturn]] inline void halt_forever() noexcept
{
    for (;;)
    {
    }
}

// The five LOD node pointers are read from context+0x10 + table[k]*4 for k = 0..4, i.e. slots
// 0, 1, 2, 4, 5.
constexpr int kLodSlotCount = 5;
constexpr int kLodTableBase = 0x0A;
constexpr std::uint32_t kRootNodeOffset = 0x20;
constexpr std::uint32_t kContextRootOffset = 0x0C;
constexpr std::uint32_t kContextLodOffset = 0x10;
constexpr std::uint32_t kNodeCopyBytes = 0x40;

struct WorkItem
{
    const std::uint8_t* node;
    std::uint32_t flags;
};
} // namespace

std::int32_t Terrain_TraverseContourQuadtree(void* context)
{
    // ---- 0x004D14EC..0x004D151C: height cut-off ------------------------------------------------
    std::int32_t height_limit = kNoHeightLimit;
    {
        const std::uint32_t height_source = load_u32(g_a44344, 0xDFC);
        if (height_source != 0)
        {
            height_limit = truncate_to_i32(load_f64(as_object(height_source), 0));
        }
    }

    // ---- 0x004D151C..0x004D1579: frame stamps, output cursors, cut-off --------------------------
    const std::uint32_t frame = g_c10380;
    const std::uint32_t frame_stamp = frame & 0xFFFFu;        // var_5E4
    const std::uint32_t previous_stamp = (frame - 1u) & 0xFFFFu; // var_5B0
    g_b12b80 = 0;

    // 0x004D1526 loads the address constant 0x00B16D10, not a variable.  The records are written
    // backwards from that boundary, so the candidate's cursor is the byte after the first slot
    // (append stores at cursor - 0x30 and then advances by 0x30).
    std::uint8_t* record_cursor = g_b16ce0.data() + 0x30;    // var_5A8
    std::uint32_t* descriptor_cursor = g_b12c60;             // var_5AC
    std::int32_t record_count = 0;                           // var_61C
    std::int32_t descriptor_count = 0;                       // var_5D4
    const std::uint32_t record_base = load_u32(context, kContextRootOffset); // var_578
    const std::int32_t detail_threshold = g_b16c6c;          // var_580

    // ---- 0x004D1579: reset the allocation-token state -----------------------------------------
    DynObj_ParseConfigFile_6();

    // ---- 0x004D157E..0x004D15E8: extent slopes -------------------------------------------------
    // The frustum tests use the copies at var_5C4/var_5B8, not the globals; a callback could change
    // the globals during the descent, so capture them here.
    const float saved_angle_x = g_bcab20;
    const float saved_angle_y = g_bcab24;
    float extent_x = kSqrtThreeLiteral / std::cos(std::atan(static_cast<double>(saved_angle_x))) * kTwoPowThirty; // var_5DC
    float extent_y = kSqrtThreeLiteral / std::cos(std::atan(static_cast<double>(saved_angle_y))) * kTwoPowThirty; // var_5F8

    // 0x004D15A6..0x004D15FC: copy the octant table (0x10 dwords) and build the per-case child
    // index row from the low bytes of 0x00548060.
    std::uint8_t octant_table[0x40];
    std::memcpy(octant_table, g_b12be0, sizeof octant_table);
    std::uint8_t child_index[0x40];
    for (int i = 0; i < 0x40; ++i)
    {
        child_index[i] = kChildIndexTable[i];
    }

    // ---- 0x004D15FE..0x004D173E: camera position and the eight cube corners --------------------
    const double camera_x = g_bcab08[0];
    const double camera_y = g_bcab08[1];
    const double camera_z = g_bcab08[2];
    const std::int32_t camera_x_i = truncate_to_i32(camera_x);                       // var_58C
    const std::int32_t camera_y_i = truncate_to_i32(static_cast<double>(static_cast<float>(camera_y))); // var_588
    const std::int32_t camera_z_i = truncate_to_i32(static_cast<double>(static_cast<float>(camera_z))); // var_584

    float matrix[10];
    std::memcpy(matrix, g_bcaae0, sizeof matrix);

    const std::int32_t view_x = sub_i32(0, add_i32(camera_x_i, g_b12bc0)); // var_610
    const std::int32_t view_y = sub_i32(0, add_i32(g_b12bc4, camera_y_i)); // var_614
    const std::int32_t view_z = sub_i32(0, add_i32(camera_z_i, g_b12bc8)); // var_60C

    // 0x004D173E..0x004D184C: the ten-float transform produces two 8x3 integer tables. corners[]
    // (var_548) is the cube-corner table; corner_view[] (var_4E8) is the transformed table used by
    // the centre tests below.
    std::int32_t corners[8][3] = {};
    std::int32_t corner_view[8][3] = {};
    for (int corner = 0; corner < 8; ++corner)
    {
        const std::int32_t x = (corner & 1) != 0 ? kCornerCoordinate : -kCornerCoordinate;
        const std::int32_t y = (corner & 2) != 0 ? kCornerCoordinate : -kCornerCoordinate;
        const std::int32_t z = (corner & 4) != 0 ? kCornerCoordinate : -kCornerCoordinate;
        corners[corner][0] = x;
        corners[corner][1] = y;
        corners[corner][2] = z;

        corner_view[corner][0] = truncate_to_i32(
            static_cast<double>(matrix[0]) * static_cast<double>(x) +
            static_cast<double>(matrix[1]) * static_cast<double>(y) +
            static_cast<double>(matrix[2]) * static_cast<double>(z));
        corner_view[corner][1] = truncate_to_i32(
            static_cast<double>(matrix[3]) * static_cast<double>(x) +
            static_cast<double>(matrix[4]) * static_cast<double>(y) +
            static_cast<double>(matrix[5]) * static_cast<double>(z));
        corner_view[corner][2] = truncate_to_i32(
            static_cast<double>(matrix[6]) * static_cast<double>(x) +
            static_cast<double>(matrix[7]) * static_cast<double>(y) +
            static_cast<double>(matrix[8]) * static_cast<double>(z));
    }

    // 0x004D1811..0x004D1845: camera-space corner subtraction.
    std::int32_t view_corner_x = sub_i32(truncate_to_i32(
        static_cast<double>(matrix[0]) * static_cast<double>(view_x) +
        static_cast<double>(matrix[1]) * static_cast<double>(view_y) +
        static_cast<double>(matrix[2]) * static_cast<double>(view_z)), corner_view[0][0]); // var_604
    std::int32_t view_corner_y = sub_i32(truncate_to_i32(
        static_cast<double>(matrix[3]) * static_cast<double>(view_x) +
        static_cast<double>(matrix[4]) * static_cast<double>(view_y) +
        static_cast<double>(matrix[5]) * static_cast<double>(view_z)), corner_view[0][1]); // var_600
    std::int32_t view_corner_z = sub_i32(truncate_to_i32(
        static_cast<double>(matrix[6]) * static_cast<double>(view_x) +
        static_cast<double>(matrix[7]) * static_cast<double>(view_y) +
        static_cast<double>(matrix[8]) * static_cast<double>(view_z)), corner_view[0][2]); // var_608

    std::int32_t corner_x = sub_i32(view_x, corners[0][0]); // var_610 - corners[0]
    std::int32_t corner_y = sub_i32(view_y, corners[0][1]); // var_614 - corners[0]
    std::int32_t corner_z = sub_i32(view_z, corners[0][2]); // var_60C - corners[0]

    // ---- 0x004D1852..0x004D1882: root work item ------------------------------------------------
    WorkItem worklist[kWorklistCapacity];
    int top = 0;
    std::uint32_t root_flags = 0;
    // P[0..4] point at the five 0x40-byte roots; a null input node leaves its slot unused and its
    // level bit clear, so the descent never reads the uninitialised slot (retail leaves it
    // uninitialised; zeroing keeps the same behaviour without indeterminate values).
    const std::uint8_t* P[kLevelCount] = {};
    {
        const std::uint32_t root = load_u32(context, kContextRootOffset);
        if (root != 0)
        {
            const std::uint8_t mask = static_cast<std::uint8_t>(load_u8(as_node(root), 0x38) & 0xF0u);
            const std::uint32_t node = root + kRootNodeOffset;
            store_u8(as_node(node), 0x18, mask);
            worklist[0].node = as_node(node);
            worklist[0].flags = 2;
            root_flags = 2;
        }
        else
        {
            // Retail leaves var_408 uninitialised on this path. It is unreachable with a live
            // terrain context; zero keeps the same bit-0-clear/bit-1-clear behaviour without
            // reading indeterminate stack bytes.
            worklist[0].node = nullptr;
            worklist[0].flags = 0;
        }
    }

    // ---- 0x004D1882..0x004D19C0: five LOD nodes -------------------------------------------------
    {
        std::uint8_t* const saved_nodes = g_a4ef38;
        for (int slot = 0; slot < kLodSlotCount; ++slot)
        {
            const int table_index = kLodTableBase + slot;
            const std::uint32_t slot_index = kLodSlotTable[slot];
            const std::uint32_t node_address = load_u32(context, kContextLodOffset + slot_index * 4);
            if (node_address == 0)
            {
                continue;
            }

            std::uint8_t* const node = const_cast<std::uint8_t*>(as_node(node_address));
            std::uint8_t original[0x40];
            std::memcpy(original, node, sizeof original);
            std::memcpy(saved_nodes + slot * 0x40, original, sizeof original);

            // 0x004D18C6..0x004D1912: reverse the eight dwords at +0x0C..+0x2C, i.e. copy from the
            // saved original in reverse order into the node.
            for (int child = 0; child < 8; ++child)
            {
                store_u32(node, 0x0C + child * 4, load_u32(original, 0x28 - child * 4));
            }
            // 0x004D1915..0x004D1976: reverse the visibility mask.
            store_u8(node, 0x30, reverse_bits8(load_u8(node, 0x30)));
            // 0x004D197D..0x004D199D: publish the traversal root and raise the level bit.
            store_u32(g_a53740, slot * 0x40 + 0x0C, node_address);
            P[slot] = g_a53740 + slot * 0x40;
            root_flags |= 1u << table_index;
        }
        worklist[0].flags = root_flags;
    }

    // ---- 0x004D1A33..0x004D1A42: main loop -------------------------------------------------------
    std::int32_t level = kInitialLevel;   // ebx
    std::int32_t level_shift = 0;         // var_5E0 (== -(level+1) after the first split)
    top = 0;
    for (;;)
    {
        WorkItem& current = worklist[top];
        const std::uint32_t flags = current.flags;
        const int octave = static_cast<int>((flags >> 7) & 7u);
        const std::uint32_t level_mask = flags >> 10;
        const std::int32_t* const v_corner = corner_view[octave];
        const std::int32_t* const c_corner = corners[octave];

        if ((flags & 1u) != 0)
        {
            // ---- merge half, 0x004D1A7E..0x004D1B74 -----------------------------------------------
            if (level_mask != 0)
            {
                std::uint32_t level_bit = 1;
                for (int k = 0; k < kLevelCount; ++k, level_bit <<= 1)
                {
                    if ((level_mask & level_bit) == 0)
                    {
                        continue;
                    }

                    const std::uint8_t* node = P[k];
                    // 0x004D1AC4: the restore pointer at +0x08 replaces P[k].
                    P[k] = as_node(load_u32(node, 0x08));

                    // 0x004D1AC7: 16-bit flags at +0x38, bit 8.
                    if ((load_u16(node, 0x38) & 0x100u) != 0)
                    {
                        for (void* item = as_object(load_u32(node, 0x2C)); item != nullptr;
                             item = as_object(load_u32(item, 0x58)))
                        {
                            // 0x004D1AD2: 16-bit item flags, bit 8.
                            if ((load_u16(item, 0x5C) & 0x100u) != 0)
                            {
                                invoke_callback(g_a9cae0[item_category(item)], item);
                            }
                        }
                    }

                    // 0x004D1AFD: byte flags at +0x38, bit 1.
                    if ((load_u8(node, 0x38) & 0x02u) != 0)
                    {
                        for (void* item = as_object(load_u32(node, 0x2C)); item != nullptr;
                             item = as_object(load_u32(item, 0x58)))
                        {
                            // 0x004D1B0B..0x004D1B16: alloc flag and merge suppression.
                            if ((load_u8(item, 0x5C) & 0x02u) != 0 && load_u32(item, 0x60) == 0)
                            {
                                emit_allocation_record(5, item);
                            }
                        }
                    }
                }
            }

            // 0x004D1B74..0x004D1BEE: undo the split's corner and extent changes.
            // var_610/614/60C are the C-corner accumulators; var_604/600/608 are the V-corner
            // accumulators (0x4D1B74..0x4D1BE0).
            corner_x = sub_i32(corner_x, shift_right(c_corner[0], static_cast<unsigned>(level)));
            corner_y = sub_i32(corner_y, shift_right(c_corner[1], static_cast<unsigned>(level)));
            corner_z = sub_i32(corner_z, shift_right(c_corner[2], static_cast<unsigned>(level)));
            view_corner_x = sub_i32(view_corner_x, shift_right(v_corner[0], static_cast<unsigned>(level)));
            view_corner_y = sub_i32(view_corner_y, shift_right(v_corner[1], static_cast<unsigned>(level)));
            view_corner_z = sub_i32(view_corner_z, shift_right(v_corner[2], static_cast<unsigned>(level)));

            // 0x004D1B81..0x004D1BB2: `fadd st0, st0` doubles the extent slopes.
            extent_x = static_cast<float>(extent_x * 2.0f);
            extent_y = static_cast<float>(extent_y * 2.0f);

            --level;
            ++level_shift;
            --top;
            if (top < 0)
            {
                break;
            }
            continue;
        }

        // ---- split half, 0x004D1C02..0x004D2312 -----------------------------------------------
        const std::uint32_t node_flags = flags | 1u;
        current.flags = node_flags;

        std::int32_t permutation_base = 0;  // var_638_1
        std::uint32_t child_mask = 0;       // var_5D8_1
        const std::uint8_t* node = current.node;
        if ((node_flags & 2u) != 0)
        {
            permutation_base = static_cast<std::int32_t>((node_flags >> 4) & 7u);
            child_mask = load_u8(node, 0x18);
        }

        std::uint8_t visible_accumulator = 0; // var_5CC_1
        g_b12bb8 = 0;
        if (level_mask != 0)
        {
            std::uint32_t level_bit = 1;
            for (int k = 0; k < kLevelCount; ++k, level_bit <<= 1)
            {
                if ((level_mask & level_bit) == 0)
                {
                    continue;
                }

                // 0x004D1C74..0x004D1C7E: descend P[k] through the entry's octave.
                const std::uint8_t* child = as_node(load_u32(P[k], 0x0C + octave * 4));
                P[k] = child;
                visible_accumulator = static_cast<std::uint8_t>(visible_accumulator | load_u8(child, 0x30));

                // 0x004D1C8B: split callback, byte flags bit 7.
                if ((load_u8(child, 0x38) & 0x80u) != 0)
                {
                    for (void* item = as_object(load_u32(child, 0x2C)); item != nullptr;
                         item = as_object(load_u32(item, 0x58)))
                    {
                        if ((load_u8(item, 0x5C) & 0x80u) != 0)
                        {
                            invoke_callback(g_a9c4e0[item_category(item)], item);
                        }
                    }
                }

                // 0x004D1CC4: split allocation record, byte flags bit 0.
                if ((load_u8(child, 0x38) & 0x01u) != 0)
                {
                    for (void* item = as_object(load_u32(child, 0x2C)); item != nullptr;
                         item = as_object(load_u32(item, 0x58)))
                    {
                        if ((load_u8(item, 0x5C) & 0x01u) != 0 && load_u32(item, 0x60) == 0)
                        {
                            emit_allocation_record(4, item);
                        }
                    }
                }
            }
        }

        // 0x004D1D33..0x004D1DC7: advance the level, add the entry octave's corner, halve the
        // extent slopes.
        ++level;
        --level_shift;
        // var_610/614/60C are the C-corner accumulators; var_604/600/608 are the V-corner
        // accumulators (0x4D1D33..0x4D1DC7).
        corner_x = add_i32(corner_x, shift_right(c_corner[0], static_cast<unsigned>(level)));
        corner_y = add_i32(corner_y, shift_right(c_corner[1], static_cast<unsigned>(level)));
        corner_z = add_i32(corner_z, shift_right(c_corner[2], static_cast<unsigned>(level)));
        view_corner_x = add_i32(view_corner_x, shift_right(v_corner[0], static_cast<unsigned>(level)));
        view_corner_y = add_i32(view_corner_y, shift_right(v_corner[1], static_cast<unsigned>(level)));
        view_corner_z = add_i32(view_corner_z, shift_right(v_corner[2], static_cast<unsigned>(level)));

        // 0x004D1D53/0x004D1D91 store the halved values back to var_5DC/var_5F8. They are the
        // values every test in the octant loop uses.
        float extent_x_level = static_cast<float>(extent_x * kHalf);
        float extent_y_level = static_cast<float>(extent_y * kHalf);
        extent_x = extent_x_level;
        extent_y = extent_y_level;

        // 0x004D1DF4..0x004D1E02: a callback may clear the child mask.
        if (g_b12bb8 != 0)
        {
            child_mask = 0;
        }

        const std::uint8_t* const index_row = child_index + permutation_base * 8;      // var_5C0_1
        const std::uint8_t* const permutation_row = octant_table + permutation_base * 8; // var_574_1
        std::uint32_t flag_bit = 1;      // var_63C_3
        std::uint32_t child_record = 0;  // var_5B4; retail leaves it uninitialised across octants

        for (int octant = 0; octant < kOctantOffsets; ++octant, flag_bit <<= 1)
        {
            // 0x004D1E2A/0x004D22EE: the octant loop advances the V and C corner pointers by
            // 0xC each iteration, so every culling test below uses the *loop* octant's corners,
            // not the entry octave's pair used by the level descent and the corner update.
            const std::int32_t* const octant_view = corner_view[octant];
            const std::int32_t* const octant_corner = corners[octant];
            const int permutation = static_cast<std::int8_t>(index_row[octant]);
            const std::uint32_t mask_bit = 1u << permutation;
            const std::uint32_t visible = flag_bit & visible_accumulator;

            // 0x004D1E4A..0x004D1E60: skip unless the octant is visible or present in the mask.
            if (visible == 0 && (child_mask & mask_bit) == 0)
            {
                continue;
            }

            // 0x004D1E6A..0x004D1E95: z extent test.
            const std::int32_t centre_z = add_i32(
                shift_right(octant_view[2], static_cast<unsigned>(level + 1)), view_corner_z);
            const float centre_z_float = static_cast<float>(centre_z);
            if (!(kCubeExtent - static_cast<double>(extent_y_level) < static_cast<double>(centre_z)))
            {
                continue;
            }

            // 0x004D1E9B..0x004D1ED6: x extent test against the z centre.
            const std::int32_t centre_x = add_i32(
                shift_right(octant_view[0], static_cast<unsigned>(level + 1)), view_corner_x);
            const double x_slope = static_cast<double>(centre_z_float) *
                                       static_cast<double>(saved_angle_x) +
                                   static_cast<double>(extent_x_level);
            if (!(static_cast<double>(centre_x) < x_slope))
            {
                continue;
            }
            if (!(static_cast<double>(centre_x) > -x_slope))
            {
                continue;
            }

            // 0x004D1EDC..0x004D1F18: y extent test.
            const std::int32_t centre_y = add_i32(
                shift_right(octant_view[1], static_cast<unsigned>(level + 1)), view_corner_y);
            const double y_slope = static_cast<double>(centre_z_float) *
                                       static_cast<double>(saved_angle_y) +
                                   static_cast<double>(extent_y_level);
            if (!(static_cast<double>(centre_y) < y_slope))
            {
                continue;
            }
            if (!(static_cast<double>(centre_y) > -y_slope))
            {
                continue;
            }

            // 0x004D1F1E..0x004D1F45: height cut-off test. The child corner uses cl = level+1,
            // like the centre tests: cl is formed once per octant iteration at 0x004D1E6D
            // (`lea ecx,[ebx+1]`, ebx = level from var_5F0) and no instruction rewrites ecx
            // before 0x004D1F5C. It is the shift that turns a level into the child cell extent.
            const std::int32_t corner_z_level = add_i32(
                shift_right(octant_corner[2], static_cast<unsigned>(level + 1)), corner_z);
            const std::int32_t half_extent = shift_right(kCornerCoordinate, static_cast<unsigned>(level + 1));
            const std::int32_t top_z = static_cast<std::int32_t>(
                static_cast<std::uint32_t>(half_extent) +
                static_cast<std::uint32_t>(corner_z_level) +
                static_cast<std::uint32_t>(camera_z_i));
            if (top_z < height_limit)
            {
                continue;
            }

            // 0x004D1F8E/0x004D1F9B: clear the per-octant level bits and mark flag.
            std::uint32_t child_flags = 0; // var_638_3
            std::uint32_t mark_child = 0;  // var_628_6

            // 0x004D1F4B..0x004D1F9F: world-space corner and the max-abs metric. These two shifts
            // reuse the same cl = level+1 formed at 0x004D1E6D: 0x004D1F5A `sar edx,cl` (C y) and
            // 0x004D1F5C `sar esi,cl` (C x). The child corner is therefore the level+1 cell corner,
            // the same quantity as corner_z_level above, not the level-sized parent offset.
            const std::int32_t world_x = add_i32(
                shift_right(octant_corner[0], static_cast<unsigned>(level + 1)), corner_x);
            const std::int32_t world_y = add_i32(
                shift_right(octant_corner[1], static_cast<unsigned>(level + 1)), corner_y);
            const std::int32_t max_component =
                static_cast<std::int32_t>(std::max({abs_i32(world_x), abs_i32(world_y), abs_i32(corner_z_level)}));
            const unsigned detail_shift = static_cast<unsigned>((level_shift + 0x19) & 0x1F);
            const std::int32_t detail = shift_right(max_component - half_extent, detail_shift);

            // 0x004D1FA5..0x004D1FC2: the visibility test uses the *global* angle, not the saved one.
            if (visible != 0)
            {
                if (static_cast<double>(detail) * static_cast<double>(g_bcab20) <
                    static_cast<double>(kVisibilityRange))
                {
                    // 0x004D1FC4..0x004D2025: mark the active levels whose child carries
                    // flag bits 0x183 at +0x38/+0x3A.
                    std::uint32_t level_bit = 1;
                    for (int k = 0; k < kLevelCount; ++k, level_bit <<= 1)
                    {
                        if ((level_mask & level_bit) == 0)
                        {
                            continue;
                        }

                        const std::uint8_t* level_node = P[k];
                        if ((load_u8(level_node, 0x30) & flag_bit) == 0)
                        {
                            continue;
                        }

                        const std::uint8_t* level_child = as_node(load_u32(level_node, 0x0C + octant * 4));
                        const std::uint32_t child_state = static_cast<std::uint32_t>(
                            load_u16(level_child, 0x3A) | load_u16(level_child, 0x38));
                        if ((child_state & 0x183u) != 0)
                        {
                            mark_child = 1;
                            child_flags |= level_bit << 10;
                        }
                    }
                }
            }

            // 0x004D2027..0x004D209F: hierarchy path.
            if ((child_mask & mask_bit) != 0)
            {
                // 0x004D204C: `xor eax,eax; mov ax,...` zero-extends the 16-bit slot index.
                const std::uint32_t child_slot =
                    static_cast<std::uint32_t>(load_u16(node, static_cast<std::size_t>(permutation) * 2));
                child_record = record_base + child_slot * 0x20;
                const std::int32_t child_id =
                    static_cast<std::int8_t>(permutation_row[load_u8(node, 0x10 + permutation)]);

                if (detail < detail_threshold && level + 1 < kMaximumLevel)
                {
                    child_flags |= static_cast<std::uint32_t>(child_id << 4) | 2u;
                    worklist[++top].node = as_node(child_record);
                    worklist[top].flags = (static_cast<std::uint32_t>(octant) << 7) | child_flags;
                    continue;
                }

                if (level + 1 > kHashLevelThreshold)
                {
                    // 0x004D20B2..0x004D2113: 32-bit wrapping hash of the camera-relative corner.
                    const std::int32_t key_x = add_i32(world_x, camera_x_i);
                    const std::int32_t key_y = add_i32(world_y, camera_y_i);
                    const std::int32_t key_z = add_i32(corner_z_level, camera_z_i);

                    std::uint32_t hash = (static_cast<std::uint32_t>(key_z) >> 2) & 0x1FFC000u;
                    hash ^= static_cast<std::uint32_t>(key_x) & 0x1FFC000u;
                    hash >>= 3;
                    hash ^= static_cast<std::uint32_t>(key_y) & 0x3FF800u;
                    hash >>= 5;
                    hash ^= (static_cast<std::uint32_t>(key_z) ^ static_cast<std::uint32_t>(key_y) ^
                             static_cast<std::uint32_t>(key_x)) &
                            0x1FFC0u;
                    hash >>= 6;

                    // 0x004D2113..0x004D2166: probe. The initial index is not masked by retail;
                    // only the probe wrap is. For bounded terrain coordinates the initial index is
                    // below 0x800; an out-of-range index would address past the cache.
                    std::uint32_t hash_index = hash;
                    TerrainContourHashEntry* entry = nullptr;
                    for (;;)
                    {
                        entry = &g_b04340[hash_index];
                        if (entry->stamp == 0xFFFFFFFFu)
                        {
                            break;
                        }
                        if (entry->stamp == previous_stamp && entry->x == key_x && entry->y == key_y &&
                            entry->z == key_z)
                        {
                            break;
                        }
                        hash_index = (hash_index + 1u) & kHashProbeMask;
                    }

                    std::uint32_t reused = 0; // var_628_6 for this record
                    if (entry->stamp == previous_stamp)
                    {
                        // 0x004D2189..0x004D21B9: previous-frame entry; reuse its data pointer.
                        entry->stamp = frame_stamp;
                        const std::uint32_t data = entry->data;
                        if (data == 0)
                        {
                            Debug_Error("Should have allocated data for cube existing previous frame");
                            halt_forever();
                        }
                        *descriptor_cursor = data;
                        ++descriptor_cursor;
                        ++descriptor_count;
                        reused = 1;
                    }
                    else
                    {
                        // 0x004D21BB..0x004D21E0: new cache entry.
                        if (entry->data != 0)
                        {
                            Debug_Error("Should't have allocate hash data! ERROR!");
                            halt_forever();
                        }
                        entry->detail = detail;
                        entry->allocation_type = 3;
                        entry->x = key_x;
                        entry->y = key_y;
                        entry->z = key_z;
                        entry->stamp = frame_stamp;
                    }

                    // 0x004D21E3..0x004D21F3: record limit.
                    if (record_count > kRecordLimit)
                    {
                        ++g_b12b80;
                    }
                    else
                    {
                        // 0x004D21F8..0x004D2297: append the 0x30-byte record ending at the
                        // cursor, then advance the cursor by 0x30.
                        if (reused == 0)
                        {
                            std::uint8_t* const record = record_cursor - 0x30;
                            store_u32(record, 0x2C, as_u32(entry));
                            store_u32(record, 0x28, g_ba5060);
                            // 0x004D2226 reads dword [index*4 + 0x00B97B1C] with index =
                            // g_b97d24.  0x00B97B1C is exactly four bytes below the established
                            // 20-dword stack at 0x00B97B20 (g_data_b97b20), so element index n is
                            // g_data_b97b20[n - 1].  Access is bounded by that array; the aliased
                            // 0x00B97B1C object has no established extent of its own.
                            std::uint32_t token = (g_ba2058 << 16) | 0xFFFFu;
                            if (g_b97d24 != 0 &&
                                static_cast<std::size_t>(g_b97d24) <= g_data_b97b20.size())
                            {
                                token = g_data_b97b20[static_cast<std::size_t>(g_b97d24) - 1] |
                                        (g_ba2058 << 16);
                            }
                            store_u32(record, 0x24, token);
                            store_u32(record, 0x20, load_u32(as_node(child_record), 0x1C));
                            store_u32(record, 0x1C, static_cast<std::uint32_t>(child_id));
                            store_u32(record, 0x18, static_cast<std::uint32_t>(level + 1));
                            store_f64(record, 0x10, static_cast<double>(key_z));
                            store_f64(record, 0x08, static_cast<double>(key_y));
                            store_f64(record, 0x00, static_cast<double>(key_x));
                            record_cursor += 0x30;
                            ++record_count;
                        }
                    }
                }
            }

            // 0x004D22A2..0x004D22D2: a visible-but-not-subdivided octant pushes the child record
            // with the accumulated level bits.
            if (mark_child != 0)
            {
                worklist[++top].node = as_node(child_record);
                worklist[top].flags = (static_cast<std::uint32_t>(octant) << 7) | child_flags;
            }
        }
    }

    // ---- 0x004D2327..0x004D2352: restore the five LOD nodes -------------------------------------
    for (int slot = 0; slot < kLodSlotCount; ++slot)
    {
        const std::uint32_t slot_index = kLodSlotTable[slot];
        std::uint8_t* const node =
            const_cast<std::uint8_t*>(as_node(load_u32(context, kContextLodOffset + slot_index * 4)));
        if (node != nullptr)
        {
            std::memcpy(node, g_a4ef38 + slot * 0x40, kNodeCopyBytes);
        }
    }

    // ---- 0x004D2354..0x004D2519: publish counts and evict stale cache entries --------------------
    g_b16ca4 = record_count;
    g_b16c9c = descriptor_count;

    const std::uint32_t frame_high = frame_stamp & 0xFFFF0000u; // var_634_10 (recomputed)
    for (std::uint32_t index = 0; index < kHashEntryCount; ++index)
    {
        TerrainContourHashEntry& entry = g_b04340[index];
        const std::uint32_t stamp = entry.stamp;
        bool reclaim = false;

        if ((stamp & 0xFFFF0000u) != frame_high)
        {
            reclaim = (stamp & 0xFFFFu) + 1u < frame_stamp;
        }
        else if ((stamp & 0xFFFFu) != frame_stamp)
        {
            reclaim = true;
        }
        else
        {
            // Same frame: (frameLow+1) < frameLow is always false; kept for completeness.
            reclaim = (stamp & 0xFFFFu) + 1u < frame_stamp;
        }

        if (reclaim)
        {
            if (entry.data != 0)
            {
                switch (entry.allocation_type)
                {
                case 0:
                    release_pool_slot(entry.data, g_b12b74, g_b12b68, g_b12b64, g_b12b60, g_b12b70);
                    break;
                case 1:
                    release_pool_slot(entry.data, g_b12c54, g_b12c48, g_b12c44, g_b12c40, g_b12c50);
                    break;
                case 2:
                    release_pool_slot(entry.data, g_b16cd4, g_b16cc8, g_b16cc4, g_b16cc0, g_b16cd0);
                    break;
                default:
                    Debug_Error("Illegal alloc info");
                    halt_forever();
                }
            }
            entry.data = 0;
            entry.stamp = 0xFFFFFFFFu;
        }
    }

    return static_cast<std::int32_t>(kHashEntryCount);
}
} // namespace igi::retail::terrain
