// Native: BackgroundFX_EmitSegments_419540 (placeholder name; retail symbol unknown - the body transforms 0x10-byte segment records through a 2x3 matrix and hands each pair of vertices to the render-backend table at 0x00A94E84). retail VA 0x00419540 (0x00419540-0x00419625, 230 bytes, 6 blocks, 8 edges, 77 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: build/retail-dis.txt (GNU objdump i386 complete byte+instruction listing, all 77 instructions quoted below); radare2 6.2.0 af/afij (size 230, 6 blocks, 8 edges); constants 0x005333E0 (1.0f) and 0x005333E8? read from .rdata - the 1.0f/0.075f values are the immediate bit patterns 0x3F800000/0x3D99999A the listing moves with `mov`, not memory loads.
// Behavior/CFG: two pointer arguments (a segment list and a transform matrix). Fills the seven constant tail floats of two 0x24-byte vertex records (0x3F800000, 0, 0, 0x3D99999A, 0, 0x3F800000, 0), builds a five-dword primitive descriptor {1, 0x00080000, 0, 2, &records[0]}, then for i < list->count transforms source pairs at list+4+i*0x10 into records[0] and records[1] and calls render_backend[0x00A84A54] with the descriptor. The count is re-read from [list] after every call.
// Status/limits: BUILD_VERIFIED (static only); runtime UNOBSERVED and the startup chain cannot reach it yet. Indirect dispatch is counted: one `call *0x00A94E84(,%edx,4)` at 0x00419609, index from 0x00A84A54 (defined zero dword in core/ native_419330.cpp). Direct calls: none. The vertex records are a 0x24-byte stride (9 floats) in retail and are modelled as records[2][9]; the descriptor->record pointer is the only pointer handed to the backend, so the record base may be a local. Float cos/sin is not used in this body, so no x87 rounding limitation applies here. Frame mapping (esp after the four prologue pushes is the frame base B): B+0x10..B+0x1F descriptor {1, 0x80000, 0, 2, &B+0x24}; B+0x20 = the record pointer; B+0x24/B+0x48 = the two 0x24-byte records (tail at +8, coordinates at +0/+4). The two constant blocks the prologue writes at 0x0041955C-0x00419574 are the record tails, written before the descriptor and before the loop.
#include "dev_log.hpp"
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

// 0x00A84A54: backend index read by this body; defined as a zero dword in core/native_419330.cpp (lowest retail load).
extern std::uint32_t g_a84a54;

// Stack arguments: [esp+4] segment list, [esp+8] transform matrix.
void NATIVE_DECL BackgroundFX_EmitSegments_419540(void* list, void* matrix)
{
    NATIVE_LOG_ENTER("BackgroundFX_EmitSegments_419540", "0x00419540");
    // The two 0x24-byte vertex records. Retail writes their constant tails in a two-iteration loop over a pointer that
    // advances 0x24 per iteration (0x00419543-0x00419574); each record is [x, y, 1.0f, 0, 0, 0.075f, 0, 1.0f, 0].
    float records[2][9];

    // 0x00419543: `lea 0x30(%esp),%eax` with esp 0x10 above B; 0x00419547 `mov $0x2,%edx`; 0x00419552 `mov $0x3F800000,%edi`;
    // 0x00419557 `mov $0x3D99999A,%esi`; loop body 0x0041955C-0x00419574 stores the seven tail words in this instruction order.
    for (std::int32_t record_index = 0; record_index < 2; ++record_index)
    {
        float* const record = records[record_index];
        record[6] = 0.0F;        // 0x0041955C: mov %ecx,-0x4(%eax)   (ecx = 0)
        record[7] = 1.0F;        // 0x0041955F: mov %edi,(%eax)       (0x3F800000)
        record[8] = 0.0F;        // 0x00419561: mov %ecx,0x4(%eax)
        record[5] = 0.075F;      // 0x00419564: mov %esi,-0x8(%eax)   (0x3D99999A)
        record[3] = 0.0F;        // 0x00419567: mov %ecx,-0x10(%eax)
        record[4] = 0.0F;        // 0x0041956A: mov %ecx,-0xc(%eax)
        record[2] = 1.0F;        // 0x0041956D: mov %edi,-0x14(%eax)
    }

    // 0x0041957A-0x0041957E: `lea 0x24(%esp),%eax; mov %eax,0x20(%esp)` - the record pointer stored in the descriptor.
    // 0x00419587: `movl $0x80000,0x14(%esp)`; 0x00419591: `movl $0x1,0x10(%esp)`; 0x00419599: `mov %ecx,0x18(%esp)`;
    // 0x0041959D: `movl $0x2,0x1c(%esp)`. Store order below follows the listing, not the field order.
    struct
    {
        std::uint32_t field_00;
        std::uint32_t field_04;
        std::uint32_t field_08;
        std::uint32_t field_0C;
        const float* field_10;
    } primitive;

    primitive.field_10 = &records[0][0];

    // 0x00419584: `mov 0x0(%ebp),%eax` - the segment count read before the descriptor is finished.
    const std::int32_t count = *reinterpret_cast<const std::int32_t*>(list);

    primitive.field_04 = 0x00080000U;
    primitive.field_00 = 1U;
    primitive.field_08 = 0U;
    primitive.field_0C = 2U;

    // 0x004195A5: `lea 0x4(%ebp),%edi`; 0x004195A8: `jle 0x0041961E` - skip the loop when the count is not positive.
    if (count > 0)
    {
        const float* const matrix_floats = reinterpret_cast<const float*>(matrix);
        const std::uint8_t* source = static_cast<const std::uint8_t*>(list) + 0x04U;
        std::int32_t index = 0;
        std::int32_t limit = count;

        while (index < limit)
        {
            const float* const segment = reinterpret_cast<const float*>(source);

            // 0x004195AE-0x004195C8: x = segment[1]*matrix[2] + segment[0]*matrix[0] + matrix[4], stored at record+0x00.
            records[0][0] = segment[1] * matrix_floats[2] + segment[0] * matrix_floats[0] + matrix_floats[4];
            // 0x004195CC-0x004195DC: y = segment[1]*matrix[3] + segment[0]*matrix[1] + matrix[5], stored at record+0x04.
            records[0][1] = segment[1] * matrix_floats[3] + segment[0] * matrix_floats[1] + matrix_floats[5];
            // 0x004195E0-0x004195F0: second vertex x from segment[3]/segment[2], stored at records[1]+0x00.
            records[1][0] = segment[3] * matrix_floats[2] + segment[2] * matrix_floats[0] + matrix_floats[4];
            // 0x004195F4-0x00419605: second vertex y, stored at records[1]+0x04.
            records[1][1] = segment[3] * matrix_floats[3] + segment[2] * matrix_floats[1] + matrix_floats[5];

            // 0x004195B8: `mov 0x00A84A54,%edx`; 0x004195BE: `lea 0x10(%esp),%ecx`; 0x004195C2: `push %ecx`;
            // 0x00419609: `call *0x00A94E84(,%edx,4)` - the render-backend call, argument = the descriptor.
            // Namespace note: retail's flat global 0x00A94E84 is declared inside `namespace igi::retail::dynobj` as an
            // extern "C" object (include/igi/retail/dynobj/local_models_mef.hpp), so the C++ spelling is
            // `igi::retail::dynobj::g_a94e84`; `::g_a94e84` does not name that object and does not compile - do not
            // "fix" the qualification back to the flat name.
            const std::uint32_t backend = ::g_a84a54;
            reinterpret_cast<igi::retail::dynobj::ModelBackendInitFn>(igi::retail::dynobj::g_a94e84[backend])(
                reinterpret_cast<void**>(&primitive));

            // 0x00419610-0x0041961C: `mov 0x0(%ebp),%eax; add $0x4,%esp; inc %ebx; add $0x10,%edi; cmp %eax,%ebx; jl 0x004195AE`
            // - the count is re-read from the list after the call and the loop compares the incremented index against it.
            limit = *reinterpret_cast<const std::int32_t*>(list);
            ++index;
            source += 0x10U;
        }
    }
    // 0x0041961E-0x00419625: pop edi/esi/ebp/ebx; `add $0x5c,%esp`; ret.
}

} // extern "C"

#undef NATIVE_DECL
