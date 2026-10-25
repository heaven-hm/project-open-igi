// Native: Graph_UnapplyParentTransform_4F93B0; retail VA 0x004F93B0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x004F93B0-0x004F945D; radare2 6.2.0 pdfj (174 bytes, 3 blocks, 61 instructions, 2 edges); called only from 0x004F91D9 (0x004F9180) with (object, scratch).
// Behavior/CFG: three blocks. When the object's +0x64 flag is clear it copies the input 24-byte block straight into object+0x20; when the flag is set it subtracts the double translation at [[object+0x14]+0x20,+0x28,+0x30] from the input block, multiplies by the transposed 3x3 float matrix at [[object+0x14]+0x70..0x90] (row 0 = +0x88,+0x7c,+0x70; row 1 = +0x8c,+0x80,+0x74; row 2 = +0x90,+0x84,+0x78) and stores the result through the scratch into object+0x20.
// Status/limits: BUILD_VERIFIED; STRUCTURALLY_VERIFIED against the 61-instruction listing; runtime UNOBSERVED. Leaf: no calls. The three fstp %st(0) at 0x004F943E-0x004F9442 only clear the x87 stack (the differences stay live across the products); a C++ port has no stack to clear. Evaluation order per row is (m2*v2 + m1*v1) + m0*v0, matching the flds/fmul/faddp sequence.
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

void NATIVE_DECL Graph_UnapplyParentTransform_4F93B0(void* object, void* in)
{
    NATIVE_LOG_ENTER("Graph_UnapplyParentTransform_4F93B0", "0x004F93B0");
    std::uint8_t* const fields = static_cast<std::uint8_t*>(object);

    if (*reinterpret_cast<std::uint8_t*>(fields + 0x64U) == 0U)                          // 0x004F93B7/0x004F93BB/0x004F93BE: mov 0x64(%edx),%al; test %al,%al; je 0x4f944a
    {
        std::memcpy(fields + 0x20U, in, 0x18U);                                          // 0x004F944A..0x004F9456: mov 0x28(%esp),%esi; lea 0x20(%edx),%edi; rep movsl $0x6
        return;                                                                          // 0x004F9458..0x004F945D: pop %edi; pop %esi; add $0x18,%esp; ret
    }

    std::uint8_t* const target = *reinterpret_cast<std::uint8_t**>(fields + 0x14U);       // 0x004F93C8: mov 0x14(%edx),%eax
    std::uint8_t* const in_bytes = static_cast<std::uint8_t*>(in);
    double const dx = *reinterpret_cast<double const*>(in_bytes + 0x00U)
                    - *reinterpret_cast<double const*>(target + 0x20U);                  // 0x004F93D2/0x004F93D4: fldl (%ecx); fsubl 0x20(%eax)
    double const dy = *reinterpret_cast<double const*>(in_bytes + 0x08U)
                    - *reinterpret_cast<double const*>(target + 0x28U);                  // 0x004F93D7/0x004F93DA
    double const dz = *reinterpret_cast<double const*>(in_bytes + 0x10U)
                    - *reinterpret_cast<double const*>(target + 0x30U);                  // 0x004F93DD/0x004F93E0
    float const* const matrix = reinterpret_cast<float const*>(target + 0x70U);          // 0x004F93E3..0x004F9436: the nine floats at +0x70..+0x90, read transposed

    struct
    {
        double x;
        double y;
        double z;
    } local;
    local.x = static_cast<double>(matrix[6]) * dz + static_cast<double>(matrix[3]) * dy;    // 0x004F93E3..0x004F93FC
    local.x += static_cast<double>(matrix[0]) * dx;                                         // 0x004F93F7..0x004F93FC
    local.y = static_cast<double>(matrix[7]) * dz + static_cast<double>(matrix[4]) * dy;    // 0x004F9402..0x004F9419
    local.y += static_cast<double>(matrix[1]) * dx;                                         // 0x004F9414..0x004F9419
    local.z = static_cast<double>(matrix[8]) * dz + static_cast<double>(matrix[5]) * dy;    // 0x004F941F..0x004F9436
    local.z += static_cast<double>(matrix[2]) * dx;                                         // 0x004F9431..0x004F9436

    std::memcpy(fields + 0x20U, &local, 0x18U);                                          // 0x004F943C: rep movsl $0x6 into object+0x20
}                                                                                        // 0x004F9444..0x004F9449: pop %edi; pop %esi; add $0x18,%esp; ret
} // extern "C"

#undef NATIVE_DECL
