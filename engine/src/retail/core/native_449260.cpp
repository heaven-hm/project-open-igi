// Native: Task_ClearFlag_449260 (behavior-derived placeholder: it zeroes the dword at argument+0x13C; retail symbol unknown); retail VA 0x00449260-0x0044926E.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 15 bytes (0xF), 1 block, 0 edges,
//   3 instructions, counted over the exact image. The body ends at the `ret` at 0x0044926E. 0x0044926F is a 0x90 NOP:
//   alignment padding before the next function at 0x00449270, not part of this routine. An earlier revision of this block
//   claimed 16 bytes through 0x0044926F, which included that padding byte.
// Tools/evidence: build/retail-dis.txt, the complete extent: `mov 0x4(%esp),%eax` (0x00449260 - the single stack argument),
//   `movl $0x0,0x13c(%eax)` (0x00449264, ten bytes) and `ret` (0x0044926E). No branch, no call, no other data reference. Callers:
//   `call 0x449260` appears in the listing, so this is a directly called routine and not a handler body.
// Behavior/CFG: no loop, no call, no return value. It stores a 32-bit zero at argument+0x13C and returns. The field's meaning is not
//   established by these three instructions; only its width (a 32-bit store) and offset are.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 3-instruction listing; runtime UNOBSERVED. Recorded limits: (a) the name is a
//   behavior-derived placeholder with the address kept as its suffix; the retail symbol is not established. (b) Nothing in this body
//   establishes what argument+0x13C is, so it is handled as a byte offset that receives a zero, never as an invented field of an invented type.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

void NATIVE_DECL Task_ClearFlag_449260(void* const target)
{
    // 0x00449260: mov 0x4(%esp),%eax; 0x00449264: movl $0x0,0x13c(%eax); 0x0044926E: ret.
    *reinterpret_cast<std::uint32_t*>(static_cast<char*>(target) + 0x13C) = 0U;
}

} // extern "C"

#undef NATIVE_DECL
