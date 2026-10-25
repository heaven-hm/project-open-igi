// Native: Graph_ApplyParentTransform_4F92E0; retail VA 0x004F92E0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x004F92E0-0x004F93A3; radare2 6.2.0 pdfj (196 bytes, 3 blocks, 70 instructions, 2 edges); called only from 0x004F91AF/0x004F91C9 (0x004F9180) with (scratch, object).
// Behavior/CFG: three blocks. Always copies the object's 24 orientation bytes from object+0x20 into the scratch through the object's +0x64 flag test; when that flag is set it also treats the copied block as three doubles, multiplies them by the 3x3 float matrix at [[object+0x14]+0x70..0x90] (row 0 = +0x70,+0x74,+0x78, row 1 = +0x7c,+0x80,+0x84, row 2 = +0x88,+0x8c,+0x90), stores the product back into the scratch, and adds the double translation at [[object+0x14]+0x20,+0x28,+0x30].
// Status/limits: BUILD_VERIFIED; STRUCTURALLY_VERIFIED against the 70-instruction listing; runtime UNOBSERVED. Leaf: no calls. The two rep movsl $0x6 copies are 24-byte memcpys. Products stay in x87 extended precision until the final fstpl, matching the retail evaluation order (row = m2*vz + m1*vy then + m0*vx). The +0x14 pointer is treated as the transform owner, not the object itself.
#include "dev_log.hpp"
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

void NATIVE_DECL Graph_ApplyParentTransform_4F92E0(void* out, void* object)
{
    NATIVE_LOG_ENTER("Graph_ApplyParentTransform_4F92E0", "0x004F92E0");
    std::uint8_t* const fields = static_cast<std::uint8_t*>(object);

    if (*reinterpret_cast<std::uint8_t*>(fields + 0x64U) == 0U)                          // 0x004F92EE/0x004F92F1/0x004F92F3: mov 0x64(%eax),%cl; test %cl,%cl; je 0x4f9391
    {
        std::memcpy(out, fields + 0x20U, 0x18U);                                         // 0x004F9391..0x004F939C: lea 0x20(%eax),%esi; mov $0x6,%ecx; rep movsl
        return;                                                                          // 0x004F939E..0x004F93A3: pop %edi; pop %esi; mov %ebp,%esp; pop %ebp; ret
    }

    std::uint8_t* const target = *reinterpret_cast<std::uint8_t**>(fields + 0x14U);       // 0x004F92F9: mov 0x14(%eax),%edx
    std::memcpy(out, fields + 0x20U, 0x18U);                                             // 0x004F9302..0x004F9309: rep movsl $0x6 - the object block becomes the input vector

    std::uint8_t* const out_bytes = static_cast<std::uint8_t*>(out);
    double const vx = *reinterpret_cast<double const*>(out_bytes + 0x00U);               // 0x004F931C: fmull (%eax)
    double const vy = *reinterpret_cast<double const*>(out_bytes + 0x08U);               // 0x004F9314: fmull 0x8(%eax)
    double const vz = *reinterpret_cast<double const*>(out_bytes + 0x10U);               // 0x004F930E: fmull 0x10(%eax)
    float const* const matrix = reinterpret_cast<float const*>(target + 0x70U);          // 0x004F930B..0x004F9368: the nine floats at +0x70..+0x90

    struct
    {
        double x;
        double y;
        double z;
    } product;
    product.x = static_cast<double>(matrix[1]) * vy + static_cast<double>(matrix[2]) * vz;  // 0x004F930B..0x004F931E
    product.x += static_cast<double>(matrix[0]) * vx;                                       // 0x004F9319..0x004F931E
    product.y = static_cast<double>(matrix[4]) * vy + static_cast<double>(matrix[5]) * vz;  // 0x004F932F..0x004F9348
    product.y += static_cast<double>(matrix[3]) * vx;                                       // 0x004F9343..0x004F9348
    product.z = static_cast<double>(matrix[7]) * vy + static_cast<double>(matrix[8]) * vz;  // 0x004F934E..0x004F936A
    product.z += static_cast<double>(matrix[6]) * vx;                                       // 0x004F9362..0x004F936A

    std::memcpy(out, &product, 0x18U);                                                    // 0x004F9370: rep movsl $0x6 - the product replaces the scratch

    double* const result = reinterpret_cast<double*>(out);                                // 0x004F9372..0x004F9388: the translation accumulates on the scratch
    result[0] = *reinterpret_cast<double const*>(target + 0x20U) + result[0];             // 0x004F9372/0x004F9375/0x004F9377: fldl 0x20(%edx); faddl (%eax); fstpl (%eax)
    result[1] = *reinterpret_cast<double const*>(target + 0x28U) + result[1];             // 0x004F9379..0x004F937F
    result[2] = *reinterpret_cast<double const*>(target + 0x30U) + result[2];             // 0x004F9382..0x004F9388
}                                                                                         // 0x004F938B..0x004F9390: pop %edi; pop %esi; mov %ebp,%esp; pop %ebp; ret
} // extern "C"

#undef NATIVE_DECL
