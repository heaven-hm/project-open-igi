// Native: AIGraph_Handler_509520; retail VA 0x00509520 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00509520-0x00509562 (67 bytes, 3 blocks, 24 instructions).
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x00509520-0x00509562; radare2 6.2.0 afij (67 bytes, 3 blocks, 24 instructions); the literal at 0x0054E840 is "RouteTable"; linked by the AIGraph registrar 0x00508FA0 at 0x0050921E (0x00401400 event 0x004F9230()).
// Behavior/CFG: three blocks, no loop. Releases the object's buffer word (calls 0x005092E0) when the word at object+0xB8 is zero, then registers the object's buffer with the graph-stream table: 0x004F9EB0(*arg2, "RouteTable", 0xA, buffer, 8 * side * side) with side the dword at object+0x58.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 24-instruction listing; runtime UNOBSERVED. Link-blocked on 0x005092E0 and 0x004F9EB0 (neither transcribed); their prototypes are inferred from these call sites.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x005092E0: the AIGraph state release (88 bytes, 5 blocks, 27 instructions; calls 0x004B0D60 and 0x004B0D10). NOT transcribed yet.
void NATIVE_DECL AIGraph_ResetState_5092E0(void* object);

// 0x004F9EB0: the graph-stream node adder (267 bytes, 14 blocks, 117 instructions; calls 0x004F9E70, 0x004FA5E0 and 0x004FA4E0).
// NOT transcribed yet - declared only. Prototype from this call site: five words.
void NATIVE_DECL GraphStream_AddNode_4F9EB0(std::uint32_t value, char const* name, std::int32_t count,
                                            void* buffer, std::uint32_t size);

void NATIVE_DECL AIGraph_Handler_509520(void* object, std::uint32_t const* arg2)
{
    // 0x00509525/0x0050952B: mov 0xb8(%esi),%eax; test %eax,%eax; jne 0x509538 - only a zero buffer triggers the release.
    if (*reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(object) + 0xB8U) == 0)
    {
        AIGraph_ResetState_5092E0(object);                                    // 0x0050952F: push %esi; call 0x5092E0
    }

    // 0x00509538: mov 0x58(%esi),%eax; 0x00509541: mov %eax,%ecx; 0x00509543: imul %eax,%ecx; 0x0050954A: shl $0x3,%ecx.
    std::uint32_t const side = *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(object) + 0x58U);
    std::uint32_t const size = (side * side) << 3U;
    void* const buffer = *reinterpret_cast<void**>(static_cast<std::uint8_t*>(object) + 0xB8U); // 0x0050953B: mov 0xb8(%esi),%edx

    // 0x0050954D..0x00509559: push %ecx (size); mov (%eax),%ecx; push %edx (buffer); push $0xa; push $0x54e840; push %ecx; call 0x4F9EB0.
    GraphStream_AddNode_4F9EB0(*arg2, "RouteTable", 0xA, buffer, size);       // 0x0050955E: add $0x14,%esp
}                                                                             // 0x00509562: ret
} // extern "C"

#undef NATIVE_DECL
