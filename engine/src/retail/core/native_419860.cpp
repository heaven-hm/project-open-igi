// Native: BackgroundFX_BuildAxes_419860; retail VA 0x00419860 (0x00419860-0x0041994D, 238 bytes, 64 instructions, 1 block, 0 edges).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: build/retail-dis.txt (GNU objdump i386 byte+instruction listing, complete body with the three x87 blocks and the three 0x00419950 calls); radare2 6.2.0 af/afij (size 238, 1 block, 0 edges, 64 instructions); the float constants are read from the image at the addresses named below and their exact values are reproduced as literals.
// Behavior/CFG: two cdecl arguments (an object pointer the body never reads, and a float angle). Straight-line, no branches: three times it computes one cosine/sine-scaled value and one angle-scaled value into a 0x1C-byte local transform record via 0x004192E0, then calls 0x00419950 with one of the three static records at 0x00539BE8 / 0x00539BF8 / 0x00539C08 and the same local record as context. Block 1 uses cos, block 2 uses cos, block 3 uses sin.
// Status/limits: BUILD_VERIFIED (static only); runtime UNOBSERVED. Two recorded notes: (1) the retail computes cos/sin with x87 `fcos`/`fsin` on a float and rounds to 32-bit on store, while the candidate uses float `cosf`/`sinf`, so last-bit equality is not claimed for any of the three blocks; (2) the three static records at 0x00539BE8/0x00539BF8/0x00539C08 and every object their pointers name are now defined in this file as typed objects with real pointers (data increment: contents read from the image at 0x00539AF0-0x00539C14, the records ending at 0x00539C17), so no data symbol of this body stays undefined; 0x00539AF0 belongs to core/native_419760.cpp. Local record layout written by 0x004192E0: +0x00 cos*scale, +0x04 sin*scale, +0x08 -sin*scale, +0x0C cos*scale, +0x10 arg4, +0x14 arg5, +0x18 arg3 bits (0x1C bytes total, exactly the frame this function allocates). Retail statement order preserved per block: the angle*constant load, the fcos/fsin, the two pushed argument dwords, the second angle*constant load, the two computes, then 0x004192E0(out, angle-scaled, trig-scaled, arg4, arg5) and 0x00419950(static record, out).
#include "dev_log.hpp"
#include <cmath>
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// Static BackgroundFX data block at 0x00539AF0-0x00539C14, transcribed as typed objects with real pointers in place of
// retail's absolute pointer words. Every byte below was read from the retail image: the block is inside .data's file-backed
// raw range (section VA 0x00536000, virtual size 0x6FC558, raw size 0x1A000) so these are real image contents, not zero-fill.
// The objects are contiguous in retail from 0x00539AF0 to 0x00539C17; 0x00539AF0 itself (the task-type word, bytes 81 01) is
// owned by core/native_419760.cpp (lowest retail writer 0x0041977E), and 0x00539C18 starts the class-name string
// "BackgroundFX" already reproduced byte-identically as a literal in that file. One definition per symbol here; the pointer
// fields hold object addresses instead of the retail VAs.

// 0x00539AFC/0x00539B54 entries: one 0x10-byte entry per line segment, four floats (x0, y0, x1, y1) - the pair layout the
// 0x00419540 body transforms with stride 0x10 and 0x00A94E84 consumes as two 0x24-byte vertices.
struct BackgroundFX_Segment
{
    float field_00;
    float field_04;
    float field_08;
    float field_0C;
};

// 0x00539AF8 and 0x00539B50: {count at +0x00, five 0x10-byte entries at +0x04} = 0x54 bytes each.
struct BackgroundFX_DrawList
{
    std::int32_t count;
    BackgroundFX_Segment segments[5];
};

// 0x00539BA8 (two entries) and 0x00539BC0 (one entry): the 0xC-byte circle records the 0x00419630 body consumes, three
// floats {x, y, radius-field}.
struct BackgroundFX_Circle
{
    float field_00;
    float field_04;
    float field_08;
};

// 0x00539BD0: the 0x18-byte two-run record the 0x00419330 body consumes {count_x, count_y, field_08, field_0C, centre_x,
// centre_y}.
struct BackgroundFX_AxisPair
{
    std::int32_t count_00;
    std::int32_t count_04;
    std::uint32_t field_08;
    std::uint32_t field_0C;
    float centre_10;
    float centre_14;
};

// 0x00539BE8/0x00539BF8/0x00539C08: the three 0x10-byte linked records 0x00419950 walks {+0x00 segment-list pointer,
// +0x04 circle count, +0x08 circle array pointer, +0x0C tail pointer consumed by 0x00419330}.
struct BackgroundFX_DrawRecord
{
    void* field_00;
    std::int32_t count;
    void* field_08;
    void* field_0C;
};

// 0x00539AF8: count 5, entries {0,70,0,300} {5,70,5,300} {0,-80,0,-300} {90,0,300,0} {-65,65,-190,190}.
BackgroundFX_DrawList g_539af8 = {
    5,
    {{0.0F, 70.0F, 0.0F, 300.0F},
     {5.0F, 70.0F, 5.0F, 300.0F},
     {0.0F, -80.0F, 0.0F, -300.0F},
     {90.0F, 0.0F, 300.0F, 0.0F},
     {-65.0F, 65.0F, -190.0F, 190.0F}}};

// 0x00539B50: count 5, entries {-300,-100,300,-100} {-300,100,300,100} {-200,100,-200,-100} {200,100,200,-100}
// {0,-400,0,150}.
BackgroundFX_DrawList g_539b50 = {
    5,
    {{-300.0F, -100.0F, 300.0F, -100.0F},
     {-300.0F, 100.0F, 300.0F, 100.0F},
     {-200.0F, 100.0F, -200.0F, -100.0F},
     {200.0F, 100.0F, 200.0F, -100.0F},
     {0.0F, -400.0F, 0.0F, 150.0F}}};

// 0x00539BA8: two circle records {0,0,100} {28,-28,60}.
BackgroundFX_Circle g_539ba8[2] = {{0.0F, 0.0F, 100.0F}, {28.0F, -28.0F, 60.0F}};

// 0x00539BC0: one circle record {0,-100,200}.
BackgroundFX_Circle g_539bc0[1] = {{0.0F, -100.0F, 200.0F}};

// 0x00539BD0: two-run record {20, 20, 0, 0, 300.0f, 300.0f}.
BackgroundFX_AxisPair g_539bd0 = {20, 20, 0U, 0U, 300.0F, 300.0F};

// 0x00539BE8: {no segment list, 0 circles, no circle array, tail = 0x00539BD0}. Retail words: 0, 0, 0, 0x00539BD0.
BackgroundFX_DrawRecord g_539be8 = {nullptr, 0, nullptr, &g_539bd0};

// 0x00539BF8: {segments 0x00539AF8, 2 circles, circles 0x00539BA8, no tail}.
BackgroundFX_DrawRecord g_539bf8 = {&g_539af8, 2, &g_539ba8[0], nullptr};

// 0x00539C08: {segments 0x00539B50, 1 circle, circles 0x00539BC0, no tail}.
BackgroundFX_DrawRecord g_539c08 = {&g_539b50, 1, &g_539bc0[0], nullptr};

void NATIVE_DECL BackgroundFX_BuildRotation_4192E0(std::uint32_t* out, float angle, float scale,
                                                  std::uint32_t field_10, std::uint32_t field_14);
void NATIVE_DECL BackgroundFX_EmitRecord_419950(void* record, void* context);

// Stack arguments: [esp+4] object (never read by this body), [esp+8] angle.
void NATIVE_DECL BackgroundFX_BuildAxes_419860(void* /*unused_object*/, float angle)
{
    NATIVE_LOG_ENTER("BackgroundFX_BuildAxes_419860", "0x00419860");
    // The 0x1C-byte transform record 0x004192E0 fills in place; the same buffer is reused for all three blocks.
    std::uint32_t transform[0x1C / 4];

    // ---- block 1: angle * 0.005f, cos-scaled by 0.25f and offset by 1.25f, emitted to record 0x00539BE8.
    // 0x00419860-0x00419871: `flds 0x8(%esp); fmuls 0x5335A4 (0.005f); fcos`.
    const float cos_1 = std::cos(angle * 0.005F);                        // 0x005335A4 = 0.004999999888241291f
    // 0x0041987E-0x0041988A: `fmuls 0x533518 (0.25f); fadds 0x533520 (1.25f); fstps (%esp)`.
    const float scale_1 = cos_1 * 0.25F + 1.25F;                         // 0x00533518 = 0.25f, 0x00533520 = 1.25f
    // 0x0041988D-0x00419898: `flds 0x30(%esp)` (the angle again) `fmuls 0x5335A0 (0.002f); fstps (%esp)`.
    const float angle_1 = angle * 0.002F;                                // 0x005335A0 = 0.0020000000949949026f
    // 0x0041989B-0x0041989C: `push %eax` (the record) then `call 0x004192E0`; the two pushed dwords are 200.0f and 300.0f.
    BackgroundFX_BuildRotation_4192E0(transform, angle_1, scale_1, 0x43480000U /*200.0f*/, 0x43960000U /*300.0f*/);
    // 0x004198A1-0x004198AB: `lea 0x14(%esp),%ecx; push %ecx; push $0x00539BE8; call 0x00419950`.
    BackgroundFX_EmitRecord_419950(&g_539be8, transform);

    // ---- block 2: angle * 0.0025f, cos-scaled by 0.15f and offset by 1.15f, emitted to record 0x00539BF8.
    // 0x004198B0-0x004198C1: `flds 0x40(%esp); fmuls 0x53359C (0.0025f); fcos`.
    const float cos_2 = std::cos(angle * 0.0025F);                       // 0x0053359C = 0.0024999999441206455f
    // 0x004198CE-0x004198DA: `fmuls 0x533598 (0.15f); fadds 0x533594 (1.15f); fstps (%esp)`.
    const float scale_2 = cos_2 * 0.15F + 1.15F;                         // 0x00533598 = 0.15000000596046448f, 0x00533594 = 1.149999976158142f
    // 0x004198DD-0x004198E8: `flds 0x30(%esp); fmuls 0x533590; fstps (%esp)`.
    const float angle_2 = angle * -0.0014285714132711291F;               // 0x00533590 = -0.0014285714132711291f
    // 0x004198EB-0x004198EC: `push %edx` then `call 0x004192E0`; pushed dwords are 250.0f and 400.0f.
    BackgroundFX_BuildRotation_4192E0(transform, angle_2, scale_2, 0x43C80000U /*400.0f*/, 0x437A0000U /*250.0f*/);
    // 0x004198F1-0x004198FB: `lea 0x14(%esp),%eax; push %eax; push $0x00539BF8; call 0x00419950`.
    BackgroundFX_EmitRecord_419950(&g_539bf8, transform);

    // ---- block 3: angle * 0.0033333334f, SIN-scaled and offset by 2.0f, emitted to record 0x00539C08.
    // 0x00419900-0x0041990D: `flds 0x40(%esp); fmuls 0x53358C (0.0033333334f); fsin` - this block uses fsin, not fcos.
    const float sin_3 = std::sin(angle * 0.0033333334140479565F);        // 0x0053358C = 0.0033333334140479565f
    // 0x0041991A: `fadds 0x533588 (2.0f); fstps (%esp)` - note: no multiply before the add in this block.
    const float scale_3 = sin_3 + 2.0F;                                  // 0x00533588 = 2.0f
    // 0x00419923-0x0041992E: `flds 0x30(%esp); fmuls 0x533584; fstps (%esp)`.
    const float angle_3 = angle * 0.0011111111380159855F;                // 0x00533584 = 0.0011111111380159855f
    // 0x00419936: `call 0x004192E0` with `push %ecx` (the record address computed at 0x0041992E); pushed dwords are 250.0f
    // and 150.0f.
    BackgroundFX_BuildRotation_4192E0(transform, angle_3, scale_3, 0x43160000U /*150.0f*/, 0x437A0000U /*250.0f*/);
    // 0x0041993B-0x00419945: `lea 0x14(%esp),%edx; push %edx; push $0x00539C08; call 0x00419950`.
    BackgroundFX_EmitRecord_419950(&g_539c08, transform);
}

} // extern "C"

#undef NATIVE_DECL
