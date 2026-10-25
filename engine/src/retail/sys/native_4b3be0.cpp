// Native: Sys_BuildRotationZ_4B3BE0 (behavior-derived placeholder name: it builds the Z-axis 3x3 rotation for one angle into a ten-dword destination; retail symbol unknown); retail VA 0x004B3BE0-0x004B3C1F.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 64 bytes (0x40), 1 block, 0 edges, 21 instructions, counted with af + afbj over the exact
//   image.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf and the objdump listing build/retail-dis.txt over the same extent; the raw bytes d9442408 d9fe 8b442404 33c9 894808 894814 894818 89481c c740200000803f 894824 d9442408 d9ff d9542408 d918 8b542408 d9c0 d9e0 d95804 895010 d9580c c3 were checked
//   with px. The body takes its destination pointer at [esp+4] and its angle at [esp+8] as a 32-bit float (fld dword [esp+8]), computes the
//   trig with the x87 fsin/fcos pair, and keeps every store 32 bits wide. 0x004B38E0 calls it exactly once.
// Behavior/CFG: one basic block, no branch, no callee, no frame, no stack probe. Statement order is retail's: the sine is computed first
//   (fld/fSin before any store), then the zero and 1.0f stores, then the cosine, then the four rotation stores. The destination is written at
//   ten dword offsets, of which 0x00-0x20 are the 3x3 rotation in row-major order and 0x24 is zeroed as well.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 21-instruction listing and afbj (64 B / 1 block / 0 edges); runtime UNOBSERVED.
//   Retail computes the trig with x87 fsin/fcos on a value loaded as a 32-bit float, which keeps the result in 80-bit precision until the 32-bit
//   store; the port therefore computes in long double and rounds at each store, the same modelling core/native_419630.cpp records. std::sin and
//   std::cos on long double are not guaranteed bit-identical to fsin/fcos in the last bit, so last-bit equality is not claimed. The immediate
//   0x3f800000 is exactly 1.0f and retail's zero stores write the four bytes 00 00 00 00, so both are transcribed as float values with identical
//   bits. The destination's type is a float[10] because ten dwords are written; what the tenth dword belongs to is NOT established here.
#include <cmath>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

void NATIVE_DECL Sys_BuildRotationZ_4B3BE0(float* const arg0, float const arg1)
{
    float* const out = arg0;

    // 0x004B3BE0/0x004B3BE4: fld dword [esp+8]; fsin - the sine is computed before any store.
    long double const sin_value = std::sin(static_cast<long double>(arg1));

    out[2] = 0.0F;                                                               // 0x004B3BEC: mov dword [eax+8],ecx (ecx = 0)
    out[5] = 0.0F;                                                               // 0x004B3BEF: mov dword [eax+0x14],ecx
    out[6] = 0.0F;                                                               // 0x004B3BF2: mov dword [eax+0x18],ecx
    out[7] = 0.0F;                                                               // 0x004B3BF5: mov dword [eax+0x1c],ecx
    out[8] = 1.0F;                                                               // 0x004B3BF8: mov dword [eax+0x20],0x3f800000
    out[9] = 0.0F;                                                               // 0x004B3BFF: mov dword [eax+0x24],ecx

    // 0x004B3C02/0x004B3C06: fld dword [esp+8]; fcos.
    long double const cos_value = std::cos(static_cast<long double>(arg1));
    float const cos_float = static_cast<float>(cos_value);                       // 0x004B3C08: fst dword [esp+8] (the 32-bit rounding)
    out[0] = cos_float;                                                          // 0x004B3C0C: fstp dword [eax]
    out[1] = static_cast<float>(-sin_value);                                     // 0x004B3C12/0x004B3C14/0x004B3C16: fld %st(0); fchs; fstp dword [eax+4]
    out[4] = cos_float;                                                          // 0x004B3C19: mov dword [eax+0x10],edx (edx = the bits stored at 0x004B3C08)
    out[3] = static_cast<float>(sin_value);                                      // 0x004B3C1C: fstp dword [eax+0xc]
}

} // extern "C"

#undef NATIVE_DECL
