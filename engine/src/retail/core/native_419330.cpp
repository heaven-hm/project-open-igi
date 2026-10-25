// Native: BackgroundFX_EmitAxisRuns_419330 (placeholder name; retail symbol unknown - the body emits two runs of 0x24-byte
// vertex records, one spread along the transform's x axis and one along its y axis, around a centre taken from the 0x18-byte
// record it receives, and hands each pair of vertices to the render-backend table at 0x00A94E84).
// retail VA 0x00419330 (0x00419330-0x00419537, 520 bytes, 7 blocks, 11 edges, 144 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: build/retail-dis.txt (GNU objdump i386 complete byte+instruction listing, all 144 instructions quoted
// below); radare2 6.2.0 af/afij (size 520, 7 blocks, 11 edges); float constants read from the image: 0x00533504 = 0.5f
// (00 00 00 3f) and 0x0053357C = 0.05000000074505806f (cd cc 4c 3d).
// Behavior/CFG: arguments [esp+4] = 0x18-byte record {count_x, count_y, field_08, field_0C, centre_x, centre_y, [esp+8] =
// transform context}. Fills the constant tails of two 0x24-byte vertex records, builds the descriptor {1, 0x00080000, 0, 2,
// &records[0]}, computes the two run offsets from the record/context product, then runs the first run count_x+1 times and the
// second count_y+1 times (both counts re-read from the record after every dispatch); each dispatch hands the descriptor to
// render_backend[0x00A84A54]. The run 1 end point adds (centre_x*m[0]) and (centre_x*m[1]); run 2 adds (centre_y*m[2]) and
// (centre_y*m[3])*0.05.
// Status/limits: TRANSCRIBED (static only); runtime UNOBSERVED and the startup chain cannot reach it yet. Indirect dispatch
// is counted: two `call *0x00A94E84(,%eax,4)` sites at 0x0041948E and 0x0041951D, both indexed by 0x00A84A54 (this file owns
// that zero dword, the lowest retail load being 0x00419458). Direct calls: none. No cos/sin is used, so no x87 library
// rounding limitation applies; the x87 intermediates are float loads/multiplies/adds whose results are rounded only at the
// `fstps` stores, which the plain float expressions below reproduce. The 0x18-byte record is read through raw int/float
// offsets because retail reads it that way; the owning data objects live in core/native_419860.cpp.
//
// Frame mapping (esp after the two prologue pushes is the frame base B): B+0x08 = val2, B+0x0C = val1, B+0x10 = run-2 y
// offset, B+0x14/B+0x18 = run-1/run-2 increments, B+0x1C/B+0x20 = run-2 x increment / centre_y*m[2], B+0x24 = centre_x*m[1],
// B+0x28 = centre_x*m[0], B+0x2C = val1 save, B+0x30..B+0x43 descriptor, B+0x44/B+0x68 = the two 0x24-byte records.
#include "igi/retail/dynobj/local_models_mef.hpp"

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00A84A54: backend index read by this body. Retail never writes it (twenty load sites in the image, no store), and the
// dword sits in .data's zero-fill tail (virtual size 0x6FC558 at 0x00536000, raw size 0x1A000), so the value is 0 at
// runtime; the load is still transcribed rather than folded so the dependency stays visible. This file owns the definition
// because 0x00419330 is the lowest retail address that loads it.
std::uint32_t g_a84a54 = 0U;

// Stack arguments: [esp+4] the 0x18-byte run record, [esp+8] transform context.
void NATIVE_DECL BackgroundFX_EmitAxisRuns_419330(void* runs, void* matrix)
{
    const std::int32_t* const run_counts = reinterpret_cast<const std::int32_t*>(runs);
    const float* const run_floats = reinterpret_cast<const float*>(runs);
    const float* const context_floats = reinterpret_cast<const float*>(matrix);

    // The two 0x24-byte vertex records, tails initialised by the prologue loop (0x00419336-0x00419365).
    float records[2][9];

    // 0x00419336: `lea 0x58(%esp),%eax` (esp 0x08 above B); 0x0041933A `mov $0x2,%ecx`; 0x0041933F `mov $0x3D99999A,%edx`;
    // 0x00419348 `mov $0x3F800000,%esi`; 0x00419346 `xor %edi,%edi`; loop body 0x0041934D-0x00419365.
    for (std::int32_t record_index = 0; record_index < 2; ++record_index)
    {
        float* const record = records[record_index];
        record[6] = 0.0F;        // 0x0041934D: mov %edi,-0x4(%eax)
        record[7] = 1.0F;        // 0x00419350: mov %esi,(%eax)      (0x3F800000)
        record[8] = 0.0F;        // 0x00419352: mov %edi,0x4(%eax)
        record[5] = 0.075F;      // 0x00419355: mov %edx,-0x8(%eax)  (0x3D99999A)
        record[3] = 0.0F;        // 0x00419358: mov %edi,-0x10(%eax)
        record[4] = 0.0F;        // 0x0041935B: mov %edi,-0xc(%eax)
        record[2] = 1.0F;        // 0x0041935E: mov %esi,-0x14(%eax)
    }

    // 0x00419367: `mov 0x90(%esp),%esi` - the run record; 0x0041936E-0x00419372: `lea 0x44(%esp),%eax; mov %eax,0x40(%esp)`;
    // 0x00419376: `mov 0x94(%esp),%eax` - the transform context.
    struct
    {
        std::uint32_t field_00;
        std::uint32_t field_04;
        std::uint32_t field_08;
        std::uint32_t field_0C;
        const float* field_10;
    } primitive;

    primitive.field_10 = &records[0][0];

    // 0x0041937D-0x004193A9: run_base_x = run[2]*m[0] + run[3]*m[2] + m[4].
    const float run_base_x = run_floats[2] * context_floats[0] + run_floats[3] * context_floats[2] + context_floats[4];
    // 0x004193AC-0x004193BD: run_base_y = run[3]*m[3] + run[2]*m[1] + m[5] (stored at B+0x10).
    const float run_base_y = run_floats[3] * context_floats[3] + run_floats[2] * context_floats[1] + context_floats[5];
    // 0x004193C1-0x004193C6: offset_c = run[4]*m[0] (centre_x*m[0], stored at B+0x28).
    const float offset_c = run_floats[4] * context_floats[0];
    // 0x004193CA-0x004193CD: offset_d = run[4]*m[1] (centre_x*m[1], stored at B+0x24).
    const float offset_d = run_floats[4] * context_floats[1];
    // 0x004193D4-0x004193DA: offset_e = run[5]*m[2] (centre_y*m[2], stored at B+0x20).
    const float offset_e = run_floats[5] * context_floats[2];
    // 0x004193DE-0x004193E6: offset_g = run[5]*m[3] (centre_y*m[3], stored at B+0x14).
    const float offset_g = run_floats[5] * context_floats[3];

    // 0x004193EA-0x004193FE: val1 = run_base_x - (offset_e + offset_c)*0.5 (0x00533504 = 0.5f), saved at B+0x2C then copied
    // to B+0x0C.
    float value_1 = run_base_x - (offset_e + offset_c) * 0.5F;
    // 0x00419404-0x0041941A: val2 = run_base_y - (offset_g + offset_d)*0.5, stored at B+0x10 then copied to B+0x08.
    float value_2 = run_base_y - (offset_g + offset_d) * 0.5F;

    // 0x0041941E-0x00419430: step_1 = offset_e*0.05f (0x0053357C), stored at B+0x1C.
    const float step_1 = offset_e * 0.05F;
    // 0x00419434-0x0041943E: step_2 = offset_g*0.05f, stored at B+0x18.
    const float step_2 = offset_g * 0.05F;

    // 0x00419387-0x004193A1: descriptor store order {ptr already stored}, 0x80000 at B+0x34, 1 at B+0x30, 0 at B+0x38, 2 at B+0x3C.
    primitive.field_04 = 0x00080000U;
    primitive.field_00 = 1U;
    primitive.field_08 = 0U;
    primitive.field_0C = 2U;

    // 0x004193E4: `cmp %edi,(%esi)`; 0x00419442: `jl 0x0041949F` - run 1 is entered only when run[0] is positive, then loops
    // while the incremented index is not greater than the count re-read from run[0] (0x00419495-0x0041949D).
    std::int32_t count_x = run_counts[0];
    if (count_x > 0)
    {
        std::int32_t index = 0;
        do
        {
            // 0x00419444-0x00419448: `flds 0x10(%esp); fadds 0x2c(%esp)` - the far end of this segment is value_1 + offset_c.
            const float far_x = value_1 + offset_c;
            // 0x0041944C-0x00419461: the near end (value_1, value_2) goes into record 0 at B+0x44/B+0x48.
            records[0][0] = value_1;
            records[0][1] = value_2;
            // 0x00419465: `fstps 0x6c(%esp)` - far_x into record 1 at B+0x68.
            records[1][0] = far_x;
            // 0x00419469-0x00419472: `flds 0xc(%esp); fadds 0x28(%esp); fstps 0x74(%esp)` - far_y = value_2 + offset_d at B+0x6C.
            records[1][1] = value_2 + offset_d;

            // 0x00419476-0x0041948A: value_1 += step_1; value_2 += step_2.
            value_1 = value_1 + step_1;
            value_2 = value_2 + step_2;

            // 0x00419458: `mov 0x00A84A54,%eax`; 0x0041945D: `lea 0x30(%esp),%edx`; 0x00419471: `push %edx`;
            // 0x0041948E: `call *0x00A94E84(,%eax,4)`.
            const std::uint32_t backend = ::g_a84a54;
            reinterpret_cast<igi::retail::dynobj::ModelBackendInitFn>(igi::retail::dynobj::g_a94e84[backend])(
                reinterpret_cast<void**>(&primitive));

            // 0x00419495-0x0041949D: reload run[0], `add $0x4,%esp`, `inc %ebx`, compare, `jle 0x00419444`.
            count_x = run_counts[0];
            ++index;
        } while (index <= count_x);
    }

    // 0x0041949F-0x004194C2: step_1' = offset_c*0.05f (B+0x1C), value_1/value_2 restored from B+0x2C/B+0x10, then
    // 0x004194C2-0x004194CD: step_2' = offset_d*0.05f (B+0x14).
    const float step_1_second = offset_c * 0.05F;
    value_1 = run_base_x - (offset_e + offset_c) * 0.5F;   // 0x004194B6: the saved B+0x2C value copied back to B+0x0C
    value_2 = run_base_y - (offset_g + offset_d) * 0.5F;   // 0x004194BA: the saved B+0x10 value copied back to B+0x08
    const float step_2_second = offset_d * 0.05F;

    // 0x004194B4: `cmp %edi,%eax`; 0x004194D1: `jl 0x0041952F` - run 2 is entered only when run[1] is positive, then loops
    // while the incremented index is not greater than the count re-read from run[1] (0x00419524-0x0041952A).
    std::int32_t count_y = run_counts[1];
    if (count_y > 0)
    {
        std::int32_t index = 0;
        do
        {
            // 0x004194D3-0x004194D7: the far end adds offset_e; 0x004194DB-0x004194F0: near end into record 0 at B+0x44/B+0x48.
            const float far_x = value_1 + offset_e;
            records[0][0] = value_1;
            records[0][1] = value_2;
            records[1][0] = far_x;
            // 0x004194F8-0x00419501: far_y = value_2 + step_2' at B+0x6C.
            records[1][1] = value_2 + step_2_second;

            // 0x00419505-0x00419519: value_1 += step_1'; value_2 += step_2.
            value_1 = value_1 + step_1_second;
            value_2 = value_2 + step_2;

            // 0x004194E7: `mov 0x00A84A54,%eax`; 0x004194EC: `lea 0x30(%esp),%edx`; 0x00419500: `push %edx`;
            // 0x0041951D: `call *0x00A94E84(,%eax,4)`.
            const std::uint32_t backend = ::g_a84a54;
            reinterpret_cast<igi::retail::dynobj::ModelBackendInitFn>(igi::retail::dynobj::g_a94e84[backend])(
                reinterpret_cast<void**>(&primitive));

            // 0x00419524-0x0041952A: reload run[1], `add $0x4,%esp`, `inc %edi`, compare, `jle 0x004194D3`.
            count_y = run_counts[1];
            ++index;
        } while (index <= count_y);
    }
    // 0x0041952F-0x00419537: pop edi/esi; `add $0x84,%esp`; ret.
}

} // extern "C"

#undef NATIVE_DECL
