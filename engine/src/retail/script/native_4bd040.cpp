// Native: Qvm_ReadIndexedBlock_4BD040; retail VA 0x004BD040 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004BD040-0x004BD065 (38 bytes, 1 block, 0 edges, 16 instructions).
// Tools/evidence: radare2 6.2.0 af/pd over the exact bytes (8b 44 24 08 8b 4c 24 04 8b 54 24 10 56 ...), re-read from the retail image; caller:
//   0x004B8A80 (Sys_ResolveSymbol_23_4B8A80) at 0x004B8A9B; not in the manifest.
// Behavior/CFG: single-block cdecl wrapper, four stack arguments ([esp+4] argument frame, [esp+8] 32-bit index, [esp+0xC] destination buffer,
//   [esp+0x10] count). It reads the count and frame/index before saving ESI, loads the destination into ESI (`mov 0x10(%esp),%esi` after the
//   push, which is the [esp+0xC] argument), pushes the five arguments of Qvm_ReadArgumentValue_4BCAE0 - destination, count, frame, index,
//   selector 3 - stores a zero byte at destination[0] (0x004BD057) and calls it at 0x004BD05A. It then cleans the five arguments, returns the
//   destination pointer in EAX (0x004BD062: mov %esi,%eax) and restores ESI. The callee's own success flag is discarded, so a failed read
//   returns the destination whose first byte was already cleared.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED; does not link yet: 0x004BCAE0 (274 bytes, 15 blocks, 100 instructions) has no engine native
//   and is declared here with NATIVE_DECL. Selector 3 is the string branch of 0x004BCAE0's dispatch: it copies at most `size-1` bytes into the
//   destination through 0x004A5500 and stores a terminator at destination[size-1], which is why this wrapper clears destination[0] first and
//   why the two script callers pass a 0x10/0x20-byte block as the destination. How the count is used beyond that terminator write is inside the
//   untranscribed callee and stays unestablished here.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x004BCAE0 - the shared frame-argument reader; argument read order [esp+4] destination, [esp+8] size, [esp+0xC] frame, [esp+0x10] index,
// [esp+0x14] selector, success flag in EAX.
std::int32_t NATIVE_DECL Qvm_ReadArgumentValue_4BCAE0(void* destination, std::uint32_t size,
                                                      void* argument_frame, std::int32_t index,
                                                      std::uint32_t selector);

char* NATIVE_DECL Qvm_ReadIndexedBlock_4BD040(void* argument_frame, std::int32_t index, void* destination,
                                              std::uint32_t count)
{
    static_cast<std::uint8_t*>(destination)[0] = 0;                                // 0x004BD057: movb $0x0,(%esi)
    Qvm_ReadArgumentValue_4BCAE0(destination, count, argument_frame, index, 3U);    // 0x004BD051-0x004BD05A: push 3; push index; push frame; push count; push destination; call 0x004BCAE0
    return static_cast<char*>(destination);                                        // 0x004BD062: mov %esi,%eax
}                                                                                  // 0x004BD05F/0x004BD064/0x004BD065: add $0x14,%esp; pop %esi; ret

} // extern "C"

#undef NATIVE_DECL
