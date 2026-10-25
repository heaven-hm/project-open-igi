// Native: Sys_RandomNext_4B4770 (behavior-derived placeholder name: it advances a three-dword generator state and returns the next scaled value;
//   retail symbol unknown); retail VA 0x004B4770-0x004B47B6.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 71 bytes (0x47), 1 block, 0 edges,
//   23 instructions, counted with af + afbj over the exact image; the 0x90 at 0x004B47B7 is alignment padding before the next function.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf and the objdump listing build/retail-dis.txt over the same extent; both agree instruction for
//   instruction, and the raw bytes 83ec08 8b44240c c744240400000000 8b08 8b5008 c1e907 33d1 895008 8bca 8b5004 33d1 895004 8bca 8b10 03d1 8910
//   8bc2 25ffff0000 89442400 df6c2400 dc0da84a5300 83c408 c3 were checked with px. The single data reference is the double at 0x00534AA8,
//   whose image bytes are 00 00 00 00 00 00 F0 3E = 0x3EF0000000000000 = 2^-16 = 1.52587890625e-05, read with `px 8`. There are 178 call
//   sites in `.text`, so this is a shared generator rather than a local helper. No callee at all: the body is self-contained.
// Behavior/CFG: one basic block, no branch. It takes one pointer argument at [esp+0xC] (the 8-byte frame is the int64 scratch below it) and
//   advances three dwords at +0x00, +0x04 and +0x08 in this exact order: third ^= first >> 7 (logical), second ^= third, first += second. It
//   then masks the new first dword with 0xFFFF, stores it in the low half of an 8-byte scratch whose high half retail explicitly zeroes
//   (0x004B4777), loads that scratch as a signed 64-bit integer with fild qword, multiplies by the 2^-16 double at 0x00534AA8 and returns the
//   product in st(0). No frame beyond the 8-byte scratch, no stack probe, no side effect other than the three state dwords.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 23-instruction listing and afbj (71 B / 1 block / 0 edges); runtime UNOBSERVED.
//   The scratch is transcribed as a std::int64_t because fild qword reads eight bytes as a signed integer, and its high half is zero by
//   construction, so the sign is never negative; the arithmetic is kept on the same 32-bit unsigned values retail's instructions operate on.
//   The argument is a bare pointer here because nothing in the body establishes a record type; the three fields are dwords at +0x00/+0x04/+0x08.
//   The name records the behaviour the instructions show (a generator step returning a scaled value) and keeps its address suffix; it is not a
//   retail symbol name and no distribution or seeding property is claimed.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00534AA8: the double scale, image bytes 00 00 00 00 00 00 F0 3E (0x3EF0000000000000 = 2^-16 = 1.52587890625e-05). Nothing else in the tree
// owns it yet.
double const g_534aa8 = 1.52587890625e-05;

double NATIVE_DECL Sys_RandomNext_4B4770(void* const arg0)
{
    std::uint32_t* const state = static_cast<std::uint32_t*>(arg0);

    // 0x004B4777: mov dword [esp+4],0 - the high half of the 8-byte scratch that fild qword reads at 0x004B47A9. Represented by the integer
    // type below rather than by a separate store.
    std::uint32_t const first = state[0];                                        // 0x004B477F: mov (%eax),%ecx
    std::uint32_t third = state[2];                                             // 0x004B4781: mov 0x8(%eax),%edx
    third ^= first >> 7U;                                                       // 0x004B4784: shr $0x7,%ecx; 0x004B4787: xor %ecx,%edx
    state[2] = third;                                                           // 0x004B4789: mov %edx,0x8(%eax)
    std::uint32_t const second = state[1] ^ third;                              // 0x004B478E: mov 0x4(%eax),%edx; 0x004B4791: xor %ecx,%edx
    state[1] = second;                                                          // 0x004B4793: mov %edx,0x4(%eax)
    std::uint32_t const next = first + second;                                  // 0x004B4798: mov (%eax),%edx; 0x004B479A: add %ecx,%edx
    state[0] = next;                                                            // 0x004B479C: mov %edx,(%eax)

    // 0x004B47A0: and $0xffff,%eax; 0x004B47A5: mov %eax,0x0(%esp); 0x004B47A9: fild qword [esp] (signed 64-bit load of the masked value with
    // its zeroed high half); 0x004B47AD: fmul qword [0x00534AA8].
    std::int64_t const scaled = static_cast<std::int64_t>(next & 0xFFFFU);
    return static_cast<double>(scaled) * ::g_534aa8;                            // 0x004B47B3/0x004B47B6: add esp,8; ret (result in st(0))
}

} // extern "C"

#undef NATIVE_DECL
