// Native: BackgroundFX_EmitFan_419630 (placeholder name; retail symbol unknown - the body builds `count` rotated 0x24-byte vertex records around one 0xC-byte circle record and hands the closed fan to the render-backend table at 0x00A94E84). retail VA 0x00419630 (0x00419630-0x00419751, 290 bytes, 7 blocks, 9 edges, 90 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: build/retail-dis.txt (GNU objdump i386 complete byte+instruction listing, all 90 instructions quoted below); radare2 6.2.0 af/afij (size 290, 7 blocks, 9 edges); float constants read from the image: 0x005333EC = 0.0f (bytes 00 00 00 00), 0x005334A8 = 6.2831854820251465f (db 0f c9 40), 0x00533580 = 0.3141592741012573f (7c d9 a0 3e).
// Behavior/CFG: arguments [esp+4] = 0xC-byte circle record, [esp+8] = transform context. Computes origin from record[0..1] and context[0..5], radius = record[2]*context[6], count = round(radius*pi/10) clamped to at most 0x28 (int64 x87 store, low dword read), then for count > 0 builds one 0x24-byte vertex record per step: record[0] = radius*cos(angle)+origin_x, record[1] = radius*sin(angle)+origin_y, record[2..8] = 1.0f,0,0,0.075f,0,1.0f,0, with angle starting at 0.0f and advancing 2*pi/count per record. It then copies record[0] into record[count] (closing the fan), fills the descriptor {1, 0x00080000, 0, count+1, &record[0]} and calls render_backend[0x00A84A54] once with it. The count is not re-read.
// Status/limits: BUILD_VERIFIED (static only); runtime UNOBSERVED and the startup chain cannot reach it yet. Indirect dispatch is counted: one `call *0x00A94E84(,%eax,4)` at 0x0041973D, index from 0x00A84A54. Direct calls: none. Rounding limits: (a) retail keeps the angle and the 2*pi/count step in 80-bit x87 registers, so this body uses `long double` for both and rounds only at the float stores, exactly as `fcos`/`fsin` + `fstps` do - `std::cos`/`std::sin` on long double are not bit-identical to `fcos`/`fsin` in the last bit, so last-bit equality is not claimed; (b) the count uses `llrintl` to model `fistpll` (x87 round-to-nearest) and then keeps the low 32 bits, as the listing reads only 0x14(%esp); (c) when count > 0 retail leaves one value on the x87 register stack at return (0x00419719 pops only ST(0) of the two live values), which a C++ transcription cannot reproduce; recorded, and unobservable through memory or the return value. Frame mapping (esp after the four prologue pushes is the frame base F): F+0x10 = radius (rounded copy), F+0x14 = count, F+0x1C = origin_x, F+0x20 = origin_y, F+0x24..F+0x37 descriptor, F+0x38..F+0x5FB = 41 vertex records of 0x24 bytes.
#include "igi/retail/dynobj/local_models_mef.hpp"

#include <cmath>
#include <cstdint>
#include <cstring>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00A84A54: backend index read by this body; defined as a zero dword in core/native_419330.cpp.
extern std::uint32_t g_a84a54;

// Stack arguments: [esp+4] circle record (0xC bytes), [esp+8] transform context.
void NATIVE_DECL BackgroundFX_EmitFan_419630(void* circle, void* context)
{
    const float* const circle_floats = reinterpret_cast<const float*>(circle);
    const float* const context_floats = reinterpret_cast<const float*>(context);

    // 0x00419630: `sub $0x5ec,%esp` - 41 vertex records of 0x24 bytes plus the descriptor and the four locals.
    float records[41][9];

    // 0x00419644: `flds 0x005333EC` - the starting angle 0.0f, kept on the x87 stack until the loop.
    // 0x0041964A-0x0041965D: origin_x = circle[0]*context[0] + circle[1]*context[2] + context[4].
    const float origin_x = circle_floats[0] * context_floats[0] + circle_floats[1] * context_floats[2] + context_floats[4];
    // 0x00419661-0x00419671: origin_y = circle[1]*context[3] + circle[0]*context[1] + context[5].
    const float origin_y = circle_floats[1] * context_floats[3] + circle_floats[0] * context_floats[1] + context_floats[5];
    // 0x00419675-0x0041967F: v = circle[2]*context[6] in extended precision, `fsts 0x10(%esp)` keeps its rounded float copy,
    // `fmuls 0x00533580` then scales the extended value by pi/10.
    const long double v_extended = static_cast<long double>(circle_floats[2]) * static_cast<long double>(context_floats[6]);
    const float radius = static_cast<float>(v_extended);
    // 0x00419685: `fistpll 0x14(%esp)` (round to nearest) then 0x00419689 `mov 0x14(%esp),%ebx` (low dword only).
    std::int32_t count = static_cast<std::int32_t>(static_cast<std::uint32_t>(
        static_cast<std::uint64_t>(std::llrintl(v_extended * 0x1.41b2f8p-2L))));
    // 0x0041968D-0x00419692: `cmp $0x28,%ebx; jl ...; mov $0x28,%ebx` - only the upper bound is clamped.
    if (count >= 0x28)
    {
        count = 0x28;
    }

    // 0x00419697-0x004196A1: `xor %edx,%edx; xor %ebp,%ebp; cmp %edx,%ebx; mov %ebx,0x14(%esp); jle 0x004196FD`.
    if (count > 0)
    {
        // 0x004196A3: `fildl 0x14(%esp)` (count); 0x004196B4: `fdivrs 0x005334A8` - step = 2*pi/count, kept in extended precision.
        // The numerator is retail's 32-bit float at 0x005334A8 (bytes db 0f c9 40 = 0x40C90FDB) widened
        // exactly by the x87 divide. Written as a hex long-double literal on purpose: a decimal literal
        // such as 6.2831855F lets GCC fold the source decimal at higher precision than the float carries,
        // emitting mantissa 0xC90FDB09A671EF31 instead of the exact 0xC90FDB0000000000. Confirmed by
        // disassembling the object: the hex form emits flds 0x40C90FDB, an exact widening.
        const long double step = 0x1.921fb6p+2L / static_cast<long double>(count);
        long double angle = 0.0L;      // 0x00419644 already pushed the 0.0f starting angle

        // 0x004196BF is the loop head; 0x004196F9 `jne 0x004196BF` is the back edge (count iterations).
        for (std::int32_t index = 0; index < count; ++index)
        {
            float* const record = records[index];

            // 0x004196BF-0x004196C1: `fld %st(1); fcos` - cosine of the accumulated angle.
            const long double cosine = std::cos(angle);

            // 0x004196C3-0x004196D4: the seven constant tail floats, in the listing's store order (same pattern and order as
            // the two-record initialiser at 0x0041955C-0x00419574).
            record[6] = 0.0F;        // 0x004196C3: mov %edx,-0x4(%eax)
            record[7] = 1.0F;        // 0x004196C6: mov %edi,(%eax)      (0x3F800000)
            record[8] = 0.0F;        // 0x004196C8: mov %edx,0x4(%eax)
            record[5] = 0.075F;      // 0x004196CB: mov %esi,-0x8(%eax)  (0x3D99999A)
            record[3] = 0.0F;        // 0x004196CE: mov %edx,-0x10(%eax)
            record[4] = 0.0F;        // 0x004196D1: mov %edx,-0xc(%eax)
            record[2] = 1.0F;        // 0x004196D4: mov %edi,-0x14(%eax)

            // 0x004196DB-0x004196E3: `fmuls 0x10(%esp); fadds 0x1c(%esp); fstps -0x40(%eax)` - record[0] = radius*cos + origin_x.
            record[0] = static_cast<float>(radius * cosine + static_cast<long double>(origin_x));
            // 0x004196E6-0x004196F2: `fld %st(1); fsin; fmuls 0x10(%esp); fadds 0x20(%esp); fstps -0x3c(%eax)` - record[1].
            record[1] = static_cast<float>(radius * std::sin(angle) + static_cast<long double>(origin_y));

            // 0x004196F5-0x004196F7: `fld %st(0); faddp %st,%st(2)` - the accumulated angle advances by the step.
            angle += step;
        }
    }

    // 0x004196FD-0x0041970E: `lea 0x0(%ebp,%ebp,8),%eax` - ebp is the count on the taken path and 0 on the skipped path;
    // `mov $0x9,%ecx; lea 0x38(%esp),%esi; lea 0x38(%esp,%eax,4),%edi`.
    const std::int32_t destination = count > 0 ? count : 0;

    // 0x00419717: `rep movsl` - nine dwords from record[0] into record[destination], closing the fan.
    std::memcpy(&records[destination][0], &records[0][0], 0x24U);

    // 0x0041970A: `mov %edx,0x2c(%esp)` (edx = 0); 0x00419725: `movl $0x80000,0x2c(%esp)` (after `push %edx` the same slot is
    // 0x28); 0x0041972D: `movl $0x1,0x28(%esp)`; 0x00419735: `mov %ebx,0x34(%esp)`; 0x00419739: `mov %ecx,0x38(%esp)`.
    struct
    {
        std::uint32_t field_00;
        std::uint32_t field_04;
        std::uint32_t field_08;
        std::uint32_t field_0C;
        const float* field_10;
    } primitive;

    primitive.field_08 = 0U;
    primitive.field_04 = 0x00080000U;
    primitive.field_00 = 1U;
    // 0x0041971F: `inc %ebx` - the descriptor vertex count is count+1, also on the skipped path.
    primitive.field_0C = static_cast<std::uint32_t>(count + 1);
    primitive.field_10 = &records[0][0];

    // 0x00419712: `mov 0x00A84A54,%eax`; 0x0041971B/0x00419724: the descriptor address is pushed; 0x0041973D: the dispatch.
    // Namespace note: retail's flat global 0x00A94E84 is declared inside `namespace igi::retail::dynobj` as an extern "C"
    // object (include/igi/retail/dynobj/local_models_mef.hpp), so the C++ spelling is `igi::retail::dynobj::g_a94e84`;
    // `::g_a94e84` does not name that object and does not compile - do not "fix" the qualification back to the flat name.
    const std::uint32_t backend = ::g_a84a54;
    reinterpret_cast<igi::retail::dynobj::ModelBackendInitFn>(igi::retail::dynobj::g_a94e84[backend])(reinterpret_cast<void**>(&primitive));

    // 0x00419744-0x00419751: `add $0x4,%esp`; pop edi/esi/ebp/ebx; `add $0x5ec,%esp`; ret.
}

} // extern "C"

#undef NATIVE_DECL
