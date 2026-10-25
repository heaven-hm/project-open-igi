// Native: AIGraph_Handler_5092A0; retail VA 0x005092A0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x005092A0-0x005092D4 (53 bytes, 3 blocks, 2 edges, 22 instructions: 56 57 8b7c240c 8b7704 56 e831000000 8b86b800 0000 83c404 85c0 7416 8b7658 8bce 0fafce c1e103 51 50 57 e8010affff 83c40c 5f 5e c3).
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x005092A0-0x005092D4; radare2 6.2.0 afij (53 bytes, 3 blocks, 22 instructions); linked by the AIGraph registrar 0x00508FA0 at 0x00509282 as the fifth word of the 0x004F9DF0 "RouteTable" registration.
// Behavior/CFG: three blocks, no loop. Reads the object pointer from context+0x04, calls the resource release 0x005092E0 on it, then, when the buffer word at object+0xB8 is non-zero, calls 0x004F9CD0(context, buffer, 8 * side * side) where side is the signed dword at object+0x58 (retail computes ecx = esi * esi with imul, then shl 3; the 32-bit wrap is part of the transcription).
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 22-instruction listing; runtime UNOBSERVED. Link-blocked on 0x005092E0 and 0x004F9CD0, neither of which is transcribed yet; their prototypes here are inferred from this call site only.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x005092E0: the AIGraph state release this handler calls (88 bytes, 5 blocks, 27 instructions; calls 0x004B0D60 and 0x004B0D10).
// NOT transcribed yet - declared only. Prototype from this call site: one argument, the object.
void NATIVE_DECL AIGraph_ResetState_5092E0(void* object);

// 0x004F9CD0: the graph-node buffer consumer (243 bytes, 14 blocks, 102 instructions; jumps through its own 11-entry table at
// 0x004F9DC4). NOT transcribed yet - declared only. Prototype from this call site: three arguments, the context and the buffer word.
void NATIVE_DECL GraphNode_Process_4F9CD0(void* context, void* buffer, std::uint32_t size);

void NATIVE_DECL AIGraph_Handler_5092A0(void* context)
{
    // 0x005092A6: mov 0x4(%edi),%esi - the object lives one word into the context the dispatcher hands over.
    std::uint8_t* const object = *reinterpret_cast<std::uint8_t**>(static_cast<std::uint8_t*>(context) + 0x04U);

    AIGraph_ResetState_5092E0(object);                                        // 0x005092A9: push %esi; 0x005092AA: call 0x5092E0

    void* const buffer = *reinterpret_cast<void**>(object + 0xB8U);           // 0x005092AF: mov 0xb8(%esi),%eax
    if (buffer == nullptr)                                                    // 0x005092B8/0x005092BA: test %eax,%eax; je 0x5092D2
        return;

    // 0x005092BC: mov 0x58(%esi),%esi - retail overwrites its object register with the signed side word.
    std::uint32_t const side = *reinterpret_cast<std::uint32_t*>(object + 0x58U);
    // 0x005092BF..0x005092C4: mov %esi,%ecx; imul %esi,%ecx; shl $0x3,%ecx - the low 32 bits of side*side, then times eight.
    std::uint32_t const size = (side * side) << 3U;
    // 0x005092C7..0x005092CA: push %ecx; push %eax; push %edi; call 0x4F9CD0 - (context, buffer, size).
    GraphNode_Process_4F9CD0(context, buffer, size);                          // 0x005092CF: add $0xc,%esp
}                                                                             // 0x005092D4: ret
} // extern "C"

#undef NATIVE_DECL
