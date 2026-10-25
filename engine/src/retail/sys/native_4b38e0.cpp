// Native: Sys_ComposeRotation_4B38E0 (behavior-derived placeholder name: it composes a Z, a Y and an X rotation of three angle arguments into one ten-dword matrix; retail symbol unknown); retail VA 0x004B38E0-0x004B3B5F.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 640 bytes (0x280), 1 block, 0 edges, 204
//   instructions, counted with af + afbj over the exact image; the body ends with the ret at 0x004B3B5F and the next function starts at 0x004B3B60.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf and the objdump listing build/retail-dis.txt over the same extent. Every frame slot and every source
//   operand below was derived mechanically from the listing: the entry esp is tracked through sub $0x50,%esp, the six pushes that stay pending
//   until the delayed add $0x18,%esp at 0x004B3A3E, the three calls and the pops, and each 0xNN(%esp) operand is mapped to the frame base
//   F = entry_esp-0x50. That is why the operands read differently before and after 0x004B3A3E for the same frame byte. Call sites: 0x004B3BE0
//   once (0x004B38F0), 0x004B3BA0 once (0x004B38FF), 0x004B3B60 once (0x004B3A2C); no indirect call and no other callee.
// Behavior/CFG: one basic block, no branch. Arguments: [esp+4] = arg0, the destination (it reaches ebx at 0x004B38E8); [esp+8] = arg1, passed to
//   the X builder at 0x004B3A2C; [esp+0xC] = arg2, passed to the Y builder at 0x004B38FF; [esp+0x10] = arg3, passed to the Z builder at 0x004B38F0.
//   Sequence: Z rotation written straight into arg0; Y rotation written into the 0x50-byte frame; nine x87 groups computing a product into
//   frame[10..18]; the counter slot frame[19] = frame[9] + arg0[9] + 1 by integer add; ten dwords copied from frame[10..19] to arg0; the X
//   rotation written into the frame; the same nine groups again, in place, against the X matrix and against arg0; the counter again; a second
//   ten-dword copy; and arg0[9] = 0. Each x87 group is (A*B + C*D) + E*F with retail's two faddp additions in that order.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 204-instruction listing and afbj (640 B / 1 block / 0 edges); runtime UNOBSERVED.
//   Recorded limits: (a) retail's trig inside the three builders uses x87 fsin/fcos; those builders carry that limitation themselves. (b) The two
//   counter-slot updates are integer adds on float bit patterns (retail uses mov/lea/add on the same dwords), so they are transcribed through a
//   std::uint32_t view of the frame; the first block's counter value is what the first copy carries into arg0[9] and the second block reads it
//   back from arg0[9] at 0x004B3ABC. (c) Each x87 group is one C++ expression, so the two integer blocks that retail interleaves between a
//   group's terms are placed immediately before that group; their operands do not overlap the group, so the order is unobservable. (d) The port
//   uses a 20-float frame because the 0x50-byte frame is twenty dwords and all accesses fall inside it. (e) What the tenth dword of the matrix
//   is used for is NOT established by this body beyond the +1 accumulation and the final clearing.
#include "dev_log.hpp"
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// Defined at their own addresses in this same folder; declared here only to be called, as retail's three call sites do.
extern void NATIVE_DECL Sys_BuildRotationX_4B3B60(float* matrix, float angle);   // 0x004B3B60
extern void NATIVE_DECL Sys_BuildRotationY_4B3BA0(float* matrix, float angle);   // 0x004B3BA0
extern void NATIVE_DECL Sys_BuildRotationZ_4B3BE0(float* matrix, float angle);   // 0x004B3BE0

void NATIVE_DECL Sys_ComposeRotation_4B38E0(float* const out, float const angle_x, float const angle_y, float const angle_z)
{
    NATIVE_LOG_ENTER("Sys_ComposeRotation_4B38E0", "");
    // F = entry_esp-0x50: the whole 0x50-byte frame. frame[0..9] is the matrix the builder writes, frame[10..19] the product being assembled.
    float frame[20];
    std::uint32_t* const frame_bits = reinterpret_cast<std::uint32_t*>(frame);
    std::uint32_t* const out_bits = reinterpret_cast<std::uint32_t*>(out);

    // 0x004B38E0: sub $0x50,%esp; 0x004B38E3: mov 0x60(%esp),%eax (arg3, the Z angle); 0x004B38E7/0x004B38EC/0x004B38ED: push %ebx, %esi, %edi;
    // 0x004B38E8: mov 0x58(%esp),%ebx (arg0, the destination); 0x004B38EE/0x004B38EF: push %eax; push %ebx; 0x004B38F0: call 0x004B3BE0.
    Sys_BuildRotationZ_4B3BE0(out, angle_z);

    // 0x004B38F5: mov 0x70(%esp),%ecx (arg2, the Y angle); 0x004B38F9: lea 0x14(%esp),%edx (the frame base); 0x004B38FD/0x004B38FE: push %ecx;
    // push %edx; 0x004B38FF: call 0x004B3BA0.
    Sys_BuildRotationY_4B3BA0(frame, angle_y);

    // Nine x87 groups: frame[10..18] = the Y matrix times arg0, element by element, in retail's order.
    frame[10] = (frame[0] * out[0] + frame[6] * out[2]) + frame[3] * out[1];  // 0x004B3904-0x004B391C
    frame[11] = (frame[1] * out[0] + frame[7] * out[2]) + frame[4] * out[1];  // 0x004B3920-0x004B3938
    frame[12] = (frame[2] * out[0] + frame[8] * out[2]) + frame[5] * out[1];  // 0x004B393C-0x004B3954
    frame[13] = (frame[0] * out[3] + frame[6] * out[5]) + frame[3] * out[4];  // 0x004B3958-0x004B3971
    frame[14] = (frame[1] * out[3] + frame[7] * out[5]) + frame[4] * out[4];  // 0x004B3975-0x004B398E
    frame[15] = (frame[2] * out[3] + frame[8] * out[5]) + frame[5] * out[4];  // 0x004B3992-0x004B39AB
    frame[16] = (frame[0] * out[6] + frame[6] * out[8]) + frame[3] * out[7];  // 0x004B39AF-0x004B39C8
    // 0x004B39DA-0x004B39FD: mov 0x24(%ebx),%eax (out[9] as bits); mov 0x40(%esp),%ecx (frame[9] as bits); lea 0x1(%ecx,%eax,1),%edx;
    //   mov %edx,0x68(%esp) - frame[19] (M2[9]) = frame[9] + out[9] + 1. Retail executes this inside the group that follows, between that group's
    //   terms; a single C++ expression per x87 group cannot be split, and none of these three operands overlaps the group, so the order is
    //   unobservable through memory or the return value.
    frame_bits[19] = frame_bits[9] + out_bits[9] + 1U;
    frame[17] = (frame[1] * out[6] + frame[7] * out[8]) + frame[4] * out[7];  // 0x004B39CC-0x004B3A04
    frame[18] = (frame[2] * out[6] + frame[8] * out[8]) + frame[5] * out[7];  // 0x004B3A08-0x004B3A21

    // 0x004B39E1/0x004B39E5/0x004B39F8/0x004B3A25: lea 0x44(%esp),%esi (frame+0x28); mov %ebx,%edi (out); mov $0xa,%ecx; rep movsl - ten dwords copied.
    for (std::int32_t i = 0; i < 10; ++i) { out[i] = frame[10 + i]; }

    // 0x004B3A27-0x004B3A2C: lea 0x20(%esp),%ecx (frame); push %ecx; call 0x004B3B60. The X angle was pushed at 0x004B3A03 from 0x74(%esp) = arg2.
    Sys_BuildRotationX_4B3B60(frame, angle_x);

    // 0x004B3A3E: add $0x18,%esp - retail releases the six argument dwords still pending from the three calls here; the port has none.
    // Second product: the same nine groups again, now against the X matrix the call above wrote into frame[0..9] and against out[],
    // which holds the first product. The stores are in place, in the same order.

    frame[10] = (frame[0] * out[0] + frame[6] * out[2]) + frame[3] * out[1];  // 0x004B3A31-0x004B3A4C
    frame[11] = (frame[1] * out[0] + frame[7] * out[2]) + frame[4] * out[1];  // 0x004B3A50-0x004B3A68
    frame[12] = (frame[2] * out[0] + frame[8] * out[2]) + frame[5] * out[1];  // 0x004B3A6C-0x004B3A84
    frame[13] = (frame[0] * out[3] + frame[6] * out[5]) + frame[3] * out[4];  // 0x004B3A88-0x004B3AA1
    // 0x004B3ABC-0x004B3ACF: mov 0x24(%ebx),%edx (out[9] as bits, which the copy above put there); mov 0x30(%esp),%eax (frame[9] as bits);
    //   lea 0x1(%eax,%edx,1),%ecx; mov %ecx,0x58(%esp) - frame[19] (M2[9]) = frame[9] + out[9] + 1, as in the first block, and again placed
    //   before the group it runs inside.
    frame_bits[19] = frame_bits[9] + out_bits[9] + 1U;
    frame[14] = (frame[1] * out[3] + frame[7] * out[5]) + frame[4] * out[4];  // 0x004B3AA5-0x004B3AD8
    frame[15] = (frame[2] * out[3] + frame[8] * out[5]) + frame[5] * out[4];  // 0x004B3ADC-0x004B3AF5
    frame[16] = (frame[0] * out[6] + frame[6] * out[8]) + frame[3] * out[7];  // 0x004B3AF9-0x004B3B12
    frame[17] = (frame[1] * out[6] + frame[7] * out[8]) + frame[4] * out[7];  // 0x004B3B16-0x004B3B2F
    frame[18] = (frame[2] * out[6] + frame[8] * out[8]) + frame[5] * out[7];  // 0x004B3B33-0x004B3B4C

    // 0x004B3B50: rep movsl - the second product is copied into out[] again (source lea 0x34(%esp),%esi set at 0x004B3AC3, count 0xa at 0x004B3AD3).
    for (std::int32_t i = 0; i < 10; ++i) { out[i] = frame[10 + i]; }

    // 0x004B3B53: movl $0x0,0x24(%ebx) - the composition counter slot is cleared before returning.
    out[9] = 0.0F;
    // 0x004B3B52/0x004B3B5A/0x004B3B5B: pop %edi, %esi, %ebx; 0x004B3B5C: add $0x50,%esp; 0x004B3B5F: ret.
}

} // extern "C"

#undef NATIVE_DECL
