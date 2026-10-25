// Native: Sys_BuildRotationX_4B3B60 (behavior-derived placeholder name: it builds the X-axis 3x3 rotation for one angle into a ten-dword destination; retail symbol unknown); retail VA 0x004B3B60-0x004B3B9F.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 64 bytes (0x40), 1 block, 0 edges, 21 instructions, counted with af + afbj over the exact
//   image.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf and the objdump listing build/retail-dis.txt over the same extent; the raw bytes d9442408 d9fe 8b442404 33c9 c7000000803f 894804 894808 89480c 894818 894824 d9442408 d9ff d9542408 d95810 8b542408 d9c0 d9e0 d95814 895020 d9581c c3 were checked
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

void NATIVE_DECL Sys_BuildRotationX_4B3B60(float* const arg0, float const arg1)
{
    float* const out = arg0;

    // 0x004B3B60/0x004B3B64: fld dword [esp+8]; fsin - the sine is computed before any store.
    long double const sin_value = std::sin(static_cast<long double>(arg1));

    out[0] = 1.0F;                                                               // 0x004B3B6C: mov dword [eax],0x3f800000
    out[1] = 0.0F;                                                               // 0x004B3B72: mov dword [eax+4],ecx (ecx = 0)
    out[2] = 0.0F;                                                               // 0x004B3B75: mov dword [eax+8],ecx
    out[3] = 0.0F;                                                               // 0x004B3B78: mov dword [eax+0xc],ecx
    out[6] = 0.0F;                                                               // 0x004B3B7B: mov dword [eax+0x18],ecx
    out[9] = 0.0F;                                                               // 0x004B3B7E: mov dword [eax+0x24],ecx

    // 0x004B3B81/0x004B3B85: fld dword [esp+8]; fcos.
    long double const cos_value = std::cos(static_cast<long double>(arg1));
    float const cos_float = static_cast<float>(cos_value);                       // 0x004B3B87: fst dword [esp+8] (the 32-bit rounding)
    out[4] = cos_float;                                                          // 0x004B3B8B: fstp dword [eax+0x10]
    out[5] = static_cast<float>(-sin_value);                                     // 0x004B3B92/0x004B3B94/0x004B3B96: fld %st(0); fchs; fstp dword [eax+0x14]
    out[8] = cos_float;                                                          // 0x004B3B99: mov dword [eax+0x20],edx (edx = the bits stored at 0x004B3B87)
    out[7] = static_cast<float>(sin_value);                                      // 0x004B3B9C: fstp dword [eax+0x1c]
}

} // extern "C"

#undef NATIVE_DECL
