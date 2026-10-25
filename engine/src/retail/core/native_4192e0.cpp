// Native: BackgroundFX_BuildRotation_4192E0; retail VA 0x004192E0 (0x004192E0-0x00419322, 67 bytes, 22 instructions, 1 block, 0 edges).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: build/retail-dis.txt (GNU objdump i386 byte+instruction listing: d9 44 24 08 d9 ff / the dword stores / d8 4c 24 0c d9 c0 d9 18 d9 44 24 08 d9 fe d9 c0 d8 4c 24 0c d9 58 04 d9 e0 d8 4c 24 0c d9 58 08 d9 58 0c c3); radare2 6.2.0 af/afij (size 67, 1 block, 0 edges, 22 instructions); caller 0x00419860 (three blocks per call).
// Behavior/CFG: five cdecl stack arguments (output pointer, angle, scale, two dwords), straight-line body: stores the two dwords at out+0x10/out+0x14 and the third argument's raw bits at out+0x18, then writes cos(angle)*scale at out+0x00 and out+0x0C, sin(angle)*scale at out+0x04 and -sin(angle)*scale at out+0x08. No branches, no calls.
// Status/limits: BUILD_VERIFIED (static only); runtime UNOBSERVED. The retail computes the trig values with x87 `fcos`/`fsin` on a float loaded by `flds` (extended-precision result stored back as a 32-bit float); the candidate uses the float `sinf`/`cosf` results, which can differ from the x87 sequence in the last bit for some inputs. That precision difference is a recorded limitation, not a claim of bit equality. Retail statement order preserved: `fcos` (0x004192E4), the dword stores and the scale-bits store (0x004192F2-0x004192FC), the cos*scale store at out+0x00 (0x004192FF-0x00419305), then `fsin` (0x00419307-0x0041930B), the sin*scale store at out+0x04 (0x0041930D-0x00419313), the negated store at out+0x08 (0x00419316-0x0041931C) and the cos*scale store at out+0x0C (0x0041931F).
#include "dev_log.hpp"
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

// Stack arguments as the body reads them: [esp+4] out, [esp+8] angle, [esp+0xC] scale, [esp+0x10] and [esp+0x14] the two
// dwords stored at out+0x10 and out+0x14. The 0x00419860 caller builds three of these blocks per call.
void NATIVE_DECL BackgroundFX_BuildRotation_4192E0(std::uint32_t* out, float angle, float scale,
                                                   std::uint32_t field_10, std::uint32_t field_14)
{
    NATIVE_LOG_ENTER("BackgroundFX_BuildRotation_4192E0", "0x004192E0");
    float* const fields = reinterpret_cast<float*>(out);

    // 0x004192E0-0x004192E4: `flds 0x8(%esp); fcos` - cos(angle) is computed before any store.
    const float cos_value = std::cos(angle);

    // 0x004192E6-0x004192FC: `mov 0x4(%esp),%eax`, then `mov 0x10(%esp),%ecx; mov %ecx,0x10(%eax)`,
    // `mov 0x14(%esp),%edx; mov %edx,0x14(%eax)`, `mov 0xC(%esp),%ecx; mov %ecx,0x18(%eax)`.
    out[0x10 / 4] = field_10;
    out[0x14 / 4] = field_14;
    std::memcpy(&out[0x18 / 4], &scale, sizeof(scale));   // the scale dword itself, as the retail copies it

    // 0x004192FF-0x00419305: `fmuls 0xC(%esp); fld %st(0); fstps (%eax)` - cos*scale at out+0x00.
    const float cos_scaled = cos_value * scale;
    fields[0x00 / 4] = cos_scaled;

    // 0x00419307-0x0041930B: `flds 0x8(%esp); fsin` - sin(angle) is computed only after the out+0x00 store.
    const float sin_value = std::sin(angle);

    // 0x0041930D-0x00419313: `fld %st(0); fmuls 0xC(%esp); fstps 0x4(%eax)` - sin*scale at out+0x04.
    const float sin_scaled = sin_value * scale;
    fields[0x04 / 4] = sin_scaled;

    // 0x00419316-0x0041931C: `fchs; fmuls 0xC(%esp); fstps 0x8(%eax)` - -sin*scale at out+0x08.
    fields[0x08 / 4] = -sin_scaled;

    // 0x0041931F: `fstps 0xC(%eax)` - the surviving cos*scale lands at out+0x0C.
    fields[0x0C / 4] = cos_scaled;
}

} // extern "C"

#undef NATIVE_DECL
