// Native: Sys_GetBlock5BDDB0_46B4D0; retail VA 0x0046B4D0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x0046B4D0-0x0046B4D6 (6 bytes, 1 block, 0 edges, 2 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm over the raw bytes (b8 b0 dd 5b 00 c3). The block 0x005BDDB0 is the only reference to that address in the image; its extent comes from the writer in the next routine, 0x0046B4E9 (mov %ecx,0x38(%eax)), which stores a four-byte value at offset 0x38, so the block is at least 0x3C bytes.
// Behavior/CFG: returns the address of the 0x005BDDB0 block in eax; no argument, no branch, no memory access, no call.
// Status/limits: STRUCTURALLY_VERIFIED; runtime UNOBSERVED. This routine only publishes the block address; the block's meaning is not established, and its extent is the proven lower bound 0x3C bytes rather than a claim about the full object.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x005BDDB0: block this routine publishes. Extent lower bound 0x3C from the 4-byte store at +0x38 in 0x0046B4E9.
std::uint8_t g_5bddb0[0x3C];

std::uint8_t* NATIVE_DECL Sys_GetBlock5BDDB0_46B4D0(void)
{
    return ::g_5bddb0;                                                  // 0x46B4D0: mov $0x005BDDB0,%eax; 0x46B4D5: ret
}

} // extern "C"

#undef NATIVE_DECL
