// Native: Terrain_BuildMipMaps_2; retail VA 0x00514760. Per-triangle terrain clip-project-and-emit routine.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01, 1,384,448 bytes, image base 0x00400000, entry 0x004A50F5, machine i386 (PE32).
// Tools/evidence: Binary Ninja MCP 0.1.0, radare2 6.2.0 and angr 10.0.4.post1 CFGFast at the exact entry; counts, commands and digests below and in docs/fidelity/evidence/0x00514760-*.
// Behavior/CFG: 7839 bytes, 210 blocks / 284 edges / 2257 instructions; near clip in clip space, four screen-space stages (left/top/right/bottom) with one-outside recursion, then a 3-vertex emitter.
// Status/limits: STRUCTURALLY_VERIFIED (source/object-level property comparison; not CFG-isomorphic). Not RUNTIME_TESTED. Unresolved items are listed at the end of this file.
//
// Evidence basis (one decompiler, as required: Binary Ninja):
//   Binary Ninja: 0x00514760, 7839 bytes, 210 basic blocks, 284 edges, 2257 instructions,
//     7 return blocks, 26 call sites (5 self-recursion + 21 `rep movsd` lifted as __builtin_memcpy).
//   radare2 6.x `afij`: size 7839, 210 blocks, 284 edges, 2257 instructions, cdecl, recursive,
//     cyclomatic complexity 88, minaddr 0x00514760, maxaddr 0x005165FF.
//   angr CFGFast at the exact entry: 7839 bytes, 2257 instructions, 259 nodes / 354 edges (the
//     extra nodes are call-site splits; the extra edges are call-block artefacts).
//   Region map (radare2 blocks, all 210 accounted for):
//     0x00514760..0x005147E4   2 blocks / 37 instructions   entry, refcount, all-outside reject
//     0x005147E5..0x00515136  69 blocks / 683 instructions  near plane (clip space, bit 0x10)
//     0x00515136..0x005156D5  52 blocks / 419 instructions  left   x vs g_bcabd0 (bit 0x01)
//     0x005156D5..0x00515BE5  40 blocks / 389 instructions  top    y vs g_bcabd4 (bit 0x02)
//     0x00515BE5..0x005160AE  28 blocks / 344 instructions  right  x vs g_bcabd8 (bit 0x04)
//     0x005160AE..0x005164F3  16 blocks / 303 instructions  bottom y vs g_bcabdc (bit 0x08)
//     0x005164F3..0x005165FE   3 blocks / 82 instructions   vertex/index emitter and return
//   Loops: exactly four back edges, each a 3-iteration do/while, none nested:
//     0x0051487B -> 0x0051481A (near transform/attribute pass), 0x00514C3E -> 0x00514C3E and
//     0x0051501A -> 0x0051501A (near attribute scale passes), 0x0051651B -> 0x0051651B (emitter).
//   No indirect jumps or calls. The only callee is this function itself (0x00515101, 0x005156A0,
//   0x00515BB0, 0x00516079, 0x005164D3).
//
// What the function does, exactly (each statement below was read from the instruction stream):
//   The triangle is clipped against five planes in a fixed order: the near plane in clip space,
//   then the four screen-space bounds left/top/right/bottom. Every stage works on the three vertex
//   records passed by value (they live in the caller's frame and are mutated in place):
//     * no vertex outside          -> stage skipped;
//     * two vertices outside       -> both are replaced by the edge/plane intersection and the
//                                     stage continues with the same triangle;
//     * one vertex outside         -> the outside vertex becomes one intersection, a local copy
//                                     becomes the second, and the caller recurses once with
//                                     (I1, I2, inside2) to clip the second half-triangle. The
//                                     current invocation then continues with (I1, inside1, inside2).
//   After each stage the three outcode bytes are AND-ed; a shared bit means the whole triangle is
//   outside that plane and the invocation returns. The last stage is followed by the emitter.
//   Outcodes: 0x01 x<left, 0x02 y<top, 0x04 x>right, 0x08 y>bottom, 0x10 z<near. Each stage
//   recomputes only the bits of the planes it has not yet tested (near: all four screen bits,
//   left: top/right/bottom, top: right/bottom, right: bottom, bottom: none).
//
// Floating-point model: the retail code is x87. Products and differences are computed in extended
// precision and rounded only when stored to a float field, and it stores/reloads several
// intermediates deliberately. This transcription keeps the same split: `long double` (80-bit on the
// i386 target) for the extended intermediates and a `float` store wherever retail executes FSTP.
// Two intersection flavours exist in retail:
//   * t rounded to float first (FSTP dword [esp+0x10] + FMUL dword [esp+0x10]);
//   * t kept in extended precision on the stack (FSTP-free FDivision result + FMUL st(1)).
// The choice is per intersection, not per stage. Verified at every plane site:
//   * near: both intersections of both paths extended (0x514D2D, 0x514E7D, 0x514973, 0x514ABE);
//   * left: all four sites rounded (0x5151C9, 0x5152BA, 0x515437, 0x515539);
//   * right: all four sites rounded (0x515C81, 0x515D3A, 0x515E7E, 0x515F47);
//   * top count==2: both extended (0x515768, 0x515830); count==1: first extended (0x515983),
//     second rounded (0x515A62);
//   * bottom count==2: both extended (0x516146, 0x5161D7); count==1: first extended (0x5162E9),
//     second rounded (0x51638E).
// `round_t` therefore covers the two-outside path and the first one-outside intersection;
// `round_second` covers the second one-outside intersection (retail saves the outside vertex,
// clips it toward the first inside vertex with extended t, then clips the saved copy toward the
// second inside vertex with a rounded t on the top and bottom planes).
// The FCOMP-based outcode tests are reproduced with the exact flag bits retail reads: CF for
// "below" (which is also set for unordered/NaN) and (CF|ZF) for "below or equal".
//
// Return value: no caller reads it. The exact register value per return path is reproduced and
// documented at each site; it is incidental state, never a status code.
//
// UNRESOLVED (documented, not guessed - none of these is a control-flow branch):
//   1. g_a7e070 (0x00A7E070): incremented on entry and decremented on every return path. That
//      pair of updates is implemented exactly; the quantity it counts is not established and no
//      semantic name is invented for it.
//   2. FPU control word: the four FISTP sites (0x00516592, 0x005165A7, 0x005165B9, 0x005165CD)
//      round with whatever control word the caller installed; no FLDCW occurs in this function.
//      `std::llrint` assumes round-to-nearest-even. A different inherited mode would shift the
//      converted colour/alpha values by one.
//   3. Record bytes +0x21..+0x23: never read here (the retail caller leaves stack garbage there;
//      terrain_draw.cpp zero-initialises them). They are copied to the output vertex verbatim.
//   4. Record +0x24: this function writes a pointer to its own frame into each record copy. The
//      pointer escapes into the caller's by-value argument copies and dangles after return, exactly
//      as in retail; nothing reads it afterwards.
//   5. The catalog name: `Terrain_BuildMipMaps_2` implies mip-chain construction, but the body has
//      no dimension halving, no level loop, no allocation and no texture/surface access. It is a
//      per-triangle clipper. The name is retained as the established symbol for this VA; the
//      premise is contradicted by the evidence and recorded here rather than renamed silently.
//
// Status: ANALYZED + TRANSCRIBED + STRUCTURALLY_VERIFIED (source/object-level property comparison
//   against retail Binary Ninja/radare2/angr evidence; the candidate is not CFG-isomorphic, see
//   docs/fidelity/evidence/0x00514760-structural-comparison-20261023.json). BUILD_VERIFIED is not
//   claimed as a status increment: the unit compiles (build note at the end) but no runtime,
//   behavioral or paired IGI-Probe observation exists, so RUNTIME_TESTED and VERIFIED are excluded.
//   The structural claim covers the stage order, outcode masks, intersection rounding, recursion
//   shape and emitter; the documented traps and unknown globals below are outside it.
//
// Not added to CMakeLists.txt (deliberate: see the ownership note in the header).
#include "igi/retail/terrain/terrain_mipmaps.hpp"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace igi::retail::terrain
{
namespace
{
// 0x005333E0 holds the float constant 1.0f (six `fld dword [0x5333e0]` sites in this function).
constexpr float kOne = 1.0F;

// ---------------------------------------------------------------------------------------------
// Explicit, documented not-implemented marker. It is only reachable for input states that the
// evidence-backed model cannot produce. Aborting is deliberate: a skeleton that silently continued
// from an unestablished state would emit wrong vertex/index data.
// ---------------------------------------------------------------------------------------------
[[noreturn]] void unresolved_state(const char* what) noexcept
{
    std::fprintf(stderr, "Terrain_BuildMipMaps_2 (0x00514760): unresolved state: %s\n", what);
    std::abort();
}

// Retail stores the screen bounds as dwords but compares them as floats (`fcomp dword [...]`).
[[nodiscard]] float bits_to_float(std::uint32_t bits) noexcept
{
    float value;
    std::memcpy(&value, &bits, sizeof value);
    return value;
}

// Output-buffer stores. The output vertex buffer is a byte buffer supplied by the caller, so the
// writes go through memcpy rather than a reinterpret_cast (no alignment/aliasing assumption).
void store_f32(std::uint8_t* address, float value) noexcept
{
    std::memcpy(address, &value, sizeof value);
}

void store_u32(std::uint8_t* address, std::uint32_t value) noexcept
{
    std::memcpy(address, &value, sizeof value);
}

// x87 FCOMP sets CF when st(0) < mem, and sets CF=ZF=PF=1 when the comparison is unordered.
// Retail tests AH bit 0 for "below" and (AH & 0x41) for "below or equal", so unordered counts as
// below and never as above. These two helpers reproduce those exact predicates.
[[nodiscard]] bool x87_below(float a, float b) noexcept
{
    return std::isnan(a) || std::isnan(b) || a < b;
}

[[nodiscard]] bool x87_below_or_equal(float a, float b) noexcept
{
    return std::isnan(a) || std::isnan(b) || a <= b;
}

// Recomputes the outcode bits for the planes at or after `first_bit` and leaves the earlier bits
// clear, exactly like the per-stage shift/or sequences. `first_bit` is one of
// terrain_outcode_left/top/right/bottom in that increasing order.
[[nodiscard]] std::uint32_t recompute_outcode(const TerrainMipMapsVertex& vertex,
                                              std::uint32_t first_bit) noexcept
{
    std::uint32_t code = 0;
    if (first_bit <= terrain_outcode_left && x87_below(vertex.x, bits_to_float(g_bcabd0)))
    {
        code |= terrain_outcode_left;
    }
    if (first_bit <= terrain_outcode_top && x87_below(vertex.y, bits_to_float(g_bcabd4)))
    {
        code |= terrain_outcode_top;
    }
    if (first_bit <= terrain_outcode_right &&
        !x87_below_or_equal(vertex.x, bits_to_float(g_bcabd8)))
    {
        code |= terrain_outcode_right;
    }
    if (first_bit <= terrain_outcode_bottom &&
        !x87_below_or_equal(vertex.y, bits_to_float(g_bcabdc)))
    {
        code |= terrain_outcode_bottom;
    }
    return code;
}

struct FloatSlot
{
    float* out;
    const float* in;
};

// out = (out - in) * t + in, with the difference, product and sum in extended precision and a
// single rounding on the store. This is the FMUL st(1) / FADD / FSTP pattern used by every
// intersection site.
void lerp_slots(const FloatSlot* slots, std::size_t count, long double t) noexcept
{
    for (std::size_t i = 0; i < count; ++i)
    {
        const long double difference =
            static_cast<long double>(*slots[i].out) - static_cast<long double>(*slots[i].in);
        *slots[i].out =
            static_cast<float>(difference * t + static_cast<long double>(*slots[i].in));
    }
}

// The nine floats every screen-space intersection interpolates: x, y, z and the six attributes.
void screen_lerp_slots(TerrainMipMapsVertex& outside, const TerrainMipMapsVertex& inside,
                       FloatSlot (&slots)[9]) noexcept
{
    slots[0] = {&outside.x, &inside.x};
    slots[1] = {&outside.y, &inside.y};
    slots[2] = {&outside.z, &inside.z};
    slots[3] = {&outside.red, &inside.red};
    slots[4] = {&outside.green, &inside.green};
    slots[5] = {&outside.blue, &inside.blue};
    slots[6] = {&outside.tex_u, &inside.tex_u};
    slots[7] = {&outside.tex_v, &inside.tex_v};
    slots[8] = {&outside.alpha_f, &inside.alpha_f};
}

// The nine floats a near-plane intersection interpolates: the clip-space triple and the six
// attributes. The record's x/y/z are NOT interpolated; they are rewritten by the projection.
void clip_lerp_slots(TerrainMipMapsVertex& outside, const TerrainMipMapsVertex& inside,
                     FloatSlot (&slots)[9]) noexcept
{
    slots[0] = {&outside.clip[0], &inside.clip[0]};
    slots[1] = {&outside.clip[1], &inside.clip[1]};
    slots[2] = {&outside.clip[2], &inside.clip[2]};
    slots[3] = {&outside.red, &inside.red};
    slots[4] = {&outside.green, &inside.green};
    slots[5] = {&outside.blue, &inside.blue};
    slots[6] = {&outside.tex_u, &inside.tex_u};
    slots[7] = {&outside.tex_v, &inside.tex_v};
    slots[8] = {&outside.alpha_f, &inside.alpha_f};
}

// 0x00514A0F / 0x00514DB7 / 0x00514F..: project a clip-space intersection back to screen space.
// x uses the extended-precision reciprocal; y reuses the rounded float reciprocal, exactly like
// the FST dword [rec+8] / FMUL [rec+8] sequence.
void project_from_clip(TerrainMipMapsVertex& vertex, const float* clip) noexcept
{
    const long double inverse = kOne / static_cast<long double>(clip[2]);
    vertex.z = static_cast<float>(inverse);
    vertex.x = static_cast<float>(inverse * static_cast<long double>(g_c311a4) *
                                      static_cast<long double>(clip[0]) +
                                  static_cast<long double>(g_c311b0));
    vertex.y = static_cast<float>(static_cast<long double>(g_c311a0) *
                                      static_cast<long double>(clip[1]) *
                                      static_cast<long double>(vertex.z) +
                                  static_cast<long double>(g_c311b4));
}

// 0x00514C3E / 0x0051501A / 0x00515058: multiply the six attributes by the record's own z
// (FLD z / FMUL attr / FSTP attr).
void scale_attributes_by_z(TerrainMipMapsVertex& vertex) noexcept
{
    vertex.red = static_cast<float>(static_cast<long double>(vertex.z) * vertex.red);
    vertex.green = static_cast<float>(static_cast<long double>(vertex.z) * vertex.green);
    vertex.blue = static_cast<float>(static_cast<long double>(vertex.z) * vertex.blue);
    vertex.tex_u = static_cast<float>(static_cast<long double>(vertex.z) * vertex.tex_u);
    vertex.tex_v = static_cast<float>(static_cast<long double>(vertex.z) * vertex.tex_v);
    vertex.alpha_f = static_cast<float>(static_cast<long double>(vertex.z) * vertex.alpha_f);
}

struct StageResult
{
    bool rejected;        // the post-stage AND-of-outcodes test says the triangle is fully outside
    std::uint32_t value;  // the eax value retail returns on that path (incidental; see the notes)
};

// Retail's role assignment is purely cyclic. For two outside vertices the single inside vertex is
// followed by the two outside ones in order (inside, +1, +2); for one outside vertex the outside is
// followed by the two inside ones. Verified at 0x00514907..0x00514951 and 0x00514C8F..0x00514CDD
// (near), 0x0051515E..0x005151AD and 0x005153E0..0x0051541F (left), 0x00515702..0x00515766 and
// 0x00515931..0x00515981 (top), 0x00515C16..0x00515C7A and 0x00515E28..0x00515E7A (right),
// 0x005160DF..0x00516145 and 0x00516297..0x005162E9 (bottom).
[[nodiscard]] int cyclic_next(int index) noexcept
{
    return (index + 1) % 3;
}

[[nodiscard]] int cyclic_prev(int index) noexcept
{
    return (index + 2) % 3;
}

// ---------------------------------------------------------------------------------------------
// Near plane: 0x005147E5..0x00515136. A vertex with the near bit is OUTSIDE (z below the near
// plane); the stage therefore counts flagged vertices and clips against g_bcadc0 in clip space.
// ---------------------------------------------------------------------------------------------
StageResult clip_near_plane(TerrainMipMapsVertex& r1, TerrainMipMapsVertex& r2,
                            TerrainMipMapsVertex& r3, TerrainMipMapsCall* call)
{
    TerrainMipMapsVertex* const records[3] = {&r1, &r2, &r3};
    const std::uint32_t incoming[3] = {static_cast<std::uint32_t>(r1.flag),
                                       static_cast<std::uint32_t>(r2.flag),
                                       static_cast<std::uint32_t>(r3.flag)};
    int near_outside_count = 0;
    for (const std::uint32_t flag : incoming)
    {
        if ((flag & terrain_outcode_near) != 0)
        {
            ++near_outside_count;
        }
    }
    if (near_outside_count == 0)
    {
        return {false, 0}; // the caller only enters this stage when some vertex has the near bit
    }
    if (near_outside_count == 3)
    {
        unresolved_state("near stage entered with all three vertices behind the near plane; the "
                         "entry AND test must have rejected this triangle");
    }

    // 0x00514818..0x005148F1: three-slot scratch array of clip-space triples at [E-0x30..E-0x01],
    // plus the per-vertex attribute initialisation. The loop always runs for all three records.
    float scratch[3][3];
    for (int i = 0; i < 3; ++i)
    {
        TerrainMipMapsVertex& vertex = *records[i];
        vertex.clip = scratch[i]; // 0x0051481C `mov [rec+0x24], edx`
        if ((vertex.flag & terrain_outcode_near) != 0)
        {
            // 0x00514861..0x00514874: already in clip space.
            scratch[i][0] = vertex.x;
            scratch[i][1] = vertex.y;
            scratch[i][2] = vertex.z;
        }
        else
        {
            // 0x00514827..0x0051485C: unproject the screen-space vertex back to clip space.
            const long double reciprocal = kOne / static_cast<long double>(vertex.z);
            scratch[i][2] = static_cast<float>(reciprocal);
            scratch[i][0] = static_cast<float>(
                (static_cast<long double>(vertex.x) - static_cast<long double>(g_c311b0)) /
                (static_cast<long double>(g_c311a4) * static_cast<long double>(vertex.z)));
            scratch[i][1] = static_cast<float>(
                (static_cast<long double>(vertex.y) - static_cast<long double>(g_c311b4)) /
                (static_cast<long double>(g_c311a0) * static_cast<long double>(vertex.z)));
        }
        // 0x0051487B..0x005148EE: attributes from the packed colour, raw u/v, top-byte alpha.
        vertex.red = static_cast<float>((vertex.colour >> 16) & 0xFFU);
        vertex.green = static_cast<float>((vertex.colour >> 8) & 0xFFU);
        vertex.blue = static_cast<float>(vertex.colour & 0xFFU);
        vertex.tex_u = vertex.u;
        vertex.tex_v = vertex.v;
        vertex.alpha_f = static_cast<float>((vertex.alpha >> 24) & 0xFFU);
    }

    if (near_outside_count == 2)
    {
        // 0x00514907..0x00514C8E: two vertices behind the near plane. Both are replaced in place
        // by their intersection with the inside vertex, then projected back to screen space.
        int inside = 0;
        while ((incoming[inside] & terrain_outcode_near) != 0)
        {
            ++inside;
        }
        const int outside_a = cyclic_next(inside);
        const int outside_b = cyclic_prev(inside);

        const int outside_order[2] = {outside_a, outside_b};
        for (const int outside : outside_order)
        {
            TerrainMipMapsVertex& vertex = *records[outside];
            const TerrainMipMapsVertex& reference = *records[inside];
            // t = (near - inside.clip.z) / (outside.clip.z - inside.clip.z), extended throughout.
            const long double t =
                (static_cast<long double>(g_bcadc0) - static_cast<long double>(reference.clip[2])) /
                (static_cast<long double>(vertex.clip[2]) -
                 static_cast<long double>(reference.clip[2]));
            FloatSlot slots[9];
            clip_lerp_slots(vertex, reference, slots);
            lerp_slots(slots, 9, t);
            project_from_clip(vertex, vertex.clip);
            vertex.flag =
                static_cast<std::uint8_t>(recompute_outcode(vertex, terrain_outcode_left));
        }

        const std::uint32_t and_flags = static_cast<std::uint32_t>(r1.flag) & r2.flag & r3.flag;
        for (TerrainMipMapsVertex* const record : records)
        {
            scale_attributes_by_z(*record);
        }
        // 0x00514C33..0x00514C7C then 0x00515121: ebp = AND of the slots, tested after the scale
        // loop. eax still holds it, so the rejecting return hands back the AND-of-outcodes.
        return {and_flags != 0, and_flags};
    }

    // 0x00514C8F..0x00515121: exactly one vertex behind the near plane. It is replaced by the
    // first intersection; a local copy becomes the second; the second half-triangle is clipped by
    // one recursive call with (I1, I2, inside2).
    int outside = 0;
    while ((incoming[outside] & terrain_outcode_near) == 0)
    {
        ++outside;
    }
    const int first_inside = cyclic_next(outside);
    const int second_inside = cyclic_prev(outside);

    TerrainMipMapsVertex& outside_record = *records[outside];
    TerrainMipMapsVertex& first_inside_record = *records[first_inside];
    TerrainMipMapsVertex& second_inside_record = *records[second_inside];

    // 0x00514CE0..0x00514D17: save the outside record and its clip-space triple.
    TerrainMipMapsVertex second = outside_record;
    float second_clip[3] = {outside_record.clip[0], outside_record.clip[1], outside_record.clip[2]};
    second.clip = second_clip; // 0x00514CFA `mov [esp+0x44], ecx`

    {
        // 0x00514D1E..0x00514DF0, first intersection, in clip space, then projected.
        const long double t = (static_cast<long double>(g_bcadc0) -
                               static_cast<long double>(first_inside_record.clip[2])) /
                              (static_cast<long double>(outside_record.clip[2]) -
                               static_cast<long double>(first_inside_record.clip[2]));
        FloatSlot slots[9];
        clip_lerp_slots(outside_record, first_inside_record, slots);
        lerp_slots(slots, 9, t);
        project_from_clip(outside_record, outside_record.clip);
        outside_record.flag =
            static_cast<std::uint8_t>(recompute_outcode(outside_record, terrain_outcode_left));
    }
    {
        // 0x00514E67..0x00514FB3, second intersection from the saved copy.
        const long double t = (static_cast<long double>(g_bcadc0) -
                               static_cast<long double>(second_inside_record.clip[2])) /
                              (static_cast<long double>(second.clip[2]) -
                               static_cast<long double>(second_inside_record.clip[2]));
        FloatSlot slots[9];
        clip_lerp_slots(second, second_inside_record, slots);
        lerp_slots(slots, 9, t);
        project_from_clip(second, second.clip);
        second.flag = static_cast<std::uint8_t>(recompute_outcode(second, terrain_outcode_left));
    }

    // 0x00514FEB..0x0051500D: AND of the three argument slots (the saved copy is not included).
    const std::uint32_t and_flags = static_cast<std::uint32_t>(r1.flag) & r2.flag & r3.flag;

    // 0x00515011..0x00515056: scale the three slot records, then 0x00515058..0x005150EB scales the
    // saved copy separately (it is not in the slot pointer array).
    for (TerrainMipMapsVertex* const record : records)
    {
        scale_attributes_by_z(*record);
    }
    scale_attributes_by_z(second);

    // 0x0051506C..0x00515101: push call, then I1, I2, inside2; `add esp,0xC4` after the return.
    // 0x00515114 reloads the AND into ebp and tests it at 0x00515121; eax is NOT reloaded on this
    // path, so this path's rejecting return hands back the callee's return value.
    const std::uint32_t recursive_value =
        Terrain_BuildMipMaps_2(outside_record, second, second_inside_record, call);
    return {and_flags != 0, recursive_value};
}

// ---------------------------------------------------------------------------------------------
// One screen-space plane stage: left (0x00515136), top (0x005156D5), right (0x00515BE5), bottom
// (0x005160AE). `first_bit` is the first outcode bit the stage recomputes; the bottom stage passes
// 0 because it clears the new vertices' outcodes instead (0x0051625F `mov byte [rec+0x20], 0`).
// ---------------------------------------------------------------------------------------------
StageResult clip_screen_plane(TerrainMipMapsVertex& r1, TerrainMipMapsVertex& r2,
                              TerrainMipMapsVertex& r3, std::uint32_t bit, float plane,
                              bool compare_x, std::uint32_t first_bit, bool round_t,
                              bool round_second, TerrainMipMapsCall* call)
{
    TerrainMipMapsVertex* const records[3] = {&r1, &r2, &r3};
    const std::uint32_t incoming[3] = {static_cast<std::uint32_t>(r1.flag),
                                       static_cast<std::uint32_t>(r2.flag),
                                       static_cast<std::uint32_t>(r3.flag)};
    if (((incoming[0] | incoming[1] | incoming[2]) & bit) == 0)
    {
        return {false, 0}; // nothing to do, the caller continues with the next stage
    }

    int outside_count = 0;
    int outside_indices[3] = {0, 0, 0};
    for (int i = 0; i < 3; ++i)
    {
        if ((incoming[i] & bit) != 0)
        {
            outside_indices[outside_count++] = i;
        }
    }
    if (outside_count == 3)
    {
        unresolved_state("clip stage entered with all three vertices outside; the preceding AND "
                         "test must have rejected this triangle");
    }

    const auto interpolate_to_plane = [&](TerrainMipMapsVertex& outside,
                                          const TerrainMipMapsVertex& inside, bool round) {
        const long double inside_axis = compare_x ? static_cast<long double>(inside.x)
                                                  : static_cast<long double>(inside.y);
        const long double outside_axis = compare_x ? static_cast<long double>(outside.x)
                                                   : static_cast<long double>(outside.y);
        const long double quotient =
            (static_cast<long double>(plane) - inside_axis) / (outside_axis - inside_axis);
        const long double t =
            round ? static_cast<long double>(static_cast<float>(quotient)) : quotient;
        FloatSlot slots[9];
        screen_lerp_slots(outside, inside, slots);
        lerp_slots(slots, 9, t);
        outside.flag = first_bit != 0
                           ? static_cast<std::uint8_t>(recompute_outcode(outside, first_bit))
                           : static_cast<std::uint8_t>(0);
    };

    if (outside_count == 2)
    {
        int inside = 0;
        while ((incoming[inside] & bit) != 0)
        {
            ++inside;
        }
        const int outside_a = cyclic_next(inside);
        const int outside_b = cyclic_prev(inside);
        interpolate_to_plane(*records[outside_a], *records[inside], round_t);
        interpolate_to_plane(*records[outside_b], *records[inside], round_t);
        const std::uint32_t and_flags = static_cast<std::uint32_t>(r1.flag) & r2.flag & r3.flag;
        return {and_flags != 0, and_flags};
    }

    // One vertex outside: save it, build both intersections, recurse with (I1, I2, inside2), then
    // continue with (I1, inside1, inside2).
    const int outside = outside_indices[0];
    const int first_inside = cyclic_next(outside);
    const int second_inside = cyclic_prev(outside);
    TerrainMipMapsVertex& outside_record = *records[outside];
    TerrainMipMapsVertex& first_inside_record = *records[first_inside];
    TerrainMipMapsVertex& second_inside_record = *records[second_inside];

    TerrainMipMapsVertex second = outside_record;
    second.clip = outside_record.clip;

    interpolate_to_plane(outside_record, first_inside_record, round_t);
    interpolate_to_plane(second, second_inside_record, round_second);

    const std::uint32_t and_flags = static_cast<std::uint32_t>(r1.flag) & r2.flag & r3.flag;
    const std::uint32_t recursive_value =
        Terrain_BuildMipMaps_2(outside_record, second, second_inside_record, call);
    // 0x005156B3 / 0x00515BC3 / 0x0051608C reload the saved AND into eax, so the left/top/right
    // rejecting returns hand back the AND-of-outcodes; the bottom stage leaves eax as the callee's
    // return value (0x005164D8 `add esp,0xC4` is followed directly by `test ebp,ebp`).
    return {and_flags != 0, bit == terrain_outcode_bottom ? recursive_value : and_flags};
}

// ---------------------------------------------------------------------------------------------
// Emitter: 0x005164F3..0x005165F4. Emits exactly one triangle: three 0x20-byte vertices and three
// uint16 indices. Reads and writes only the call record's buffers.
// ---------------------------------------------------------------------------------------------
void emit_triangle(TerrainMipMapsVertex& r1, TerrainMipMapsVertex& r2, TerrainMipMapsVertex& r3,
                   TerrainMipMapsCall* call) noexcept
{
    TerrainMipMapsVertex* const records[3] = {&r1, &r2, &r3};

    // 0x005164F3..0x00516517: one triangle, three indices; the index cursor is 3*n uint16 wide.
    const std::int32_t triangle_index = *call->triangle_count;
    *call->triangle_count = triangle_index + 1;
    std::uint16_t* index_cursor =
        call->index_output + static_cast<std::size_t>(triangle_index) * 3;

    for (TerrainMipMapsVertex* const record : records)
    {
        // 0x0051651B..0x0051654A: z and its reciprocal, then fog into +0x08 and depth into +0x0C.
        // UNRESOLVED 2 (FPU control word) applies only to the FISTP conversions further down.
        const long double z = static_cast<long double>(record->z);
        const float inverse = static_cast<float>(kOne / z);
        record->depth = static_cast<float>(z * static_cast<long double>(g_bcad70));
        record->z = static_cast<float>(z * static_cast<long double>(g_a7f894) +
                                       static_cast<long double>(g_a7e074));

        // 0x0051654D..0x0051655B: index = (uint16)vertex_count + (uint16)base_vertex.
        const std::int32_t vertex_index = *call->vertex_count;
        const std::uint16_t index =
            static_cast<std::uint16_t>(static_cast<std::uint16_t>(vertex_index) +
                                       static_cast<std::uint16_t>(call->base_vertex));
        *index_cursor++ = index;

        // 0x0051655F..0x00516579: destination vertex is counted, then the first 0x20 bytes of the
        // record are copied verbatim (x, y, z, w, colour, alpha, u, v - all already finalised).
        std::uint8_t* destination =
            call->vertex_output +
            static_cast<std::size_t>(static_cast<std::uint32_t>(vertex_index)) * 0x20U;
        *call->vertex_count = vertex_index + 1;
        std::memcpy(destination, record, 0x20);

        // 0x0051657B..0x0051658B: perspective-corrected texture coordinates.
        store_f32(destination + 0x18,
                  static_cast<float>(static_cast<long double>(inverse) * record->tex_u));
        store_f32(destination + 0x1C,
                  static_cast<float>(static_cast<long double>(inverse) * record->tex_v));

        // 0x0051658B..0x0051659D: alpha byte, FISTP to int64 then `shl 24` of the low dword.
        const std::int64_t alpha = std::llrint(static_cast<long double>(inverse) * record->alpha_f);
        store_u32(destination + 0x14,
                  static_cast<std::uint32_t>(static_cast<std::uint64_t>(alpha)) << 24);

        // 0x005165A0..0x005165DB: 0xRRGGBB packed from three FISTP int64 conversions.
        const std::int64_t red = std::llrint(static_cast<long double>(inverse) * record->red);
        const std::int64_t green = std::llrint(static_cast<long double>(inverse) * record->green);
        const std::int64_t blue = std::llrint(static_cast<long double>(inverse) * record->blue);
        const std::uint32_t packed =
            ((static_cast<std::uint32_t>(static_cast<std::uint64_t>(red)) << 8) |
             static_cast<std::uint32_t>(static_cast<std::uint64_t>(green)))
                << 8 |
            static_cast<std::uint32_t>(static_cast<std::uint64_t>(blue));
        store_u32(destination + 0x10, packed);
    }
}
} // namespace

// ---------------------------------------------------------------------------------------------
// 0x00514760 entry point.
// ---------------------------------------------------------------------------------------------
extern "C" std::uint32_t Terrain_BuildMipMaps_2(TerrainMipMapsVertex r1, TerrainMipMapsVertex r2,
                                                TerrainMipMapsVertex r3,
                                                TerrainMipMapsCall* call)
{
    // 0x00514778..0x00514780: ++g_a7e070. UNRESOLVED 1: the counter's purpose is not established;
    // the update pair is reproduced exactly.
    ++g_a7e070;

    // 0x00514786..0x005147CC: outcode bytes, AND and OR.
    const std::uint32_t flag1 = static_cast<std::uint32_t>(r1.flag);
    const std::uint32_t flag2 = static_cast<std::uint32_t>(r2.flag);
    const std::uint32_t flag3 = static_cast<std::uint32_t>(r3.flag);
    const std::uint32_t and_flags = flag1 & flag2 & flag3;
    std::uint32_t or_flags = flag1 | flag2 | flag3;

    // 0x005147D2..0x005147E4: all three outside the same plane -> nothing is emitted. eax still
    // holds the first record's outcode byte (masked at 0x005147AF), so that is the return value.
    if (and_flags != 0)
    {
        --g_a7e070;
        return flag1;
    }

    // Near plane first, in clip space (0x005147E5 `test bl,0x10`).
    if ((or_flags & terrain_outcode_near) != 0)
    {
        const StageResult result = clip_near_plane(r1, r2, r3, call);
        if (result.rejected)
        {
            --g_a7e070; // 0x00515125
            return result.value;
        }
        or_flags = static_cast<std::uint32_t>(r1.flag) | r2.flag | r3.flag;
    }

    // Then the four screen-space bounds. Each stage dispatches on the OR recomputed by the
    // previous stage (bl at 0x00515136 / 0x005156D5 / 0x00515BE5 / 0x005160AE).
    struct ScreenStage
    {
        std::uint32_t bit;
        float plane;
        bool compare_x;
        std::uint32_t first_bit;
        bool round_t;
        bool round_second;
    };
    const ScreenStage stages[4] = {
        {terrain_outcode_left, bits_to_float(g_bcabd0), true, terrain_outcode_top, true, true},
        {terrain_outcode_top, bits_to_float(g_bcabd4), false, terrain_outcode_right, false,
         true},
        {terrain_outcode_right, bits_to_float(g_bcabd8), true, terrain_outcode_bottom, true, true},
        {terrain_outcode_bottom, bits_to_float(g_bcabdc), false, 0U, false, true},
    };
    for (const ScreenStage& stage : stages)
    {
        if ((or_flags & stage.bit) == 0)
        {
            continue;
        }
        const StageResult result =
            clip_screen_plane(r1, r2, r3, stage.bit, stage.plane, stage.compare_x,
                              stage.first_bit, stage.round_t, stage.round_second, call);
        if (result.rejected)
        {
            --g_a7e070; // 0x005156D4 / 0x00515BE4 / 0x005160AD / 0x005164F2
            return result.value;
        }
        or_flags = static_cast<std::uint32_t>(r1.flag) | r2.flag | r3.flag;
    }

    // 0x005164F3..0x005165F4: emit one triangle, then return 0 (eax is the emitter's loop counter,
    // which reaches zero).
    emit_triangle(r1, r2, r3, call);
    --g_a7e070; // 0x005165F4 return path
    return 0;
}
} // namespace igi::retail::terrain

// Build evidence (recorded 2026-10-23, this worktree):
//   $ i686-w64-mingw32-g++ -std=gnu++20 -O3 -DNDEBUG -Iengine/include -c
//         engine/src/retail/terrain/terrain_mipmaps.cpp -o build/task-c-evidence/terrain_mipmaps.o
//   exit status 0; also clean with -Wall -Wextra (same object digest).
//   $ file build/task-c-evidence/terrain_mipmaps.o
//   Intel 80386 COFF object file, no line number info, not stripped, 6 sections.
//   $ i686-w64-mingw32-objdump -f build/task-c-evidence/terrain_mipmaps.o
//   file format pe-i386 / architecture: i386 / HAS_RELOC, HAS_DEBUG, HAS_SYMS, HAS_LOCALS
//   $ i686-w64-mingw32-nm -C build/task-c-evidence/terrain_mipmaps.o
//   00000150 T Terrain_BuildMipMaps_2   (unmangled: C linkage preserved)
//   object sha256 caac572588f2c4932a5f98a5806a57f59062a8b79085864e23ec6798a8fea69e;
//   .text 0x2480 bytes; the thirteen retail globals remain undefined references, expected because
//   their definitions live in terrain_draw.cpp / terrain_storage.cpp (g_a7e070 is defined in
//   terrain_draw.cpp). The object also references abort, __mingw_fprintf, llrintl and the SJLJ
//   unwind/personality pair used only by the noreturn trap. Full receipt:
//   docs/fidelity/evidence/0x00514760-build-20261023.json.
