// Native: AIGraph_Handler_509450; retail VA 0x00509450 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00509450-0x00509476 (39 bytes, 1 block, 0 edges, 14 instructions: 8b4c2404 8b4158 8bd0 0fafd0 8b81b8000000 8b4c2408 c1e203 52 8b11 50 52 e85d15ffff 83c40c c3).
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x00509450-0x00509476; radare2 6.2.0 afij (39 bytes, 1 block, 14 instructions); linked by the AIGraph registrar 0x00508FA0 at 0x00509242 (0x00401400 event 0x004F9220()).
// Behavior/CFG: one block, no branch, no loop. Reads the side word at object+0x58, squares it and shifts left by three (32-bit wrap preserved),
//                reads the buffer word at object+0xB8 and the caller's value through arg2, and calls 0x004FA9D0(*arg2, buffer, size).
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 14-instruction listing; runtime UNOBSERVED. Link-blocked on 0x004FA9D0
//                (not transcribed); its prototype is inferred from this call site only.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x004FA9D0: inserts one value into the graph list (64 bytes, 1 block, 26 instructions; calls 0x004FA7E0, 0x004FA850 and 0x004B17C0).
// NOT transcribed yet - declared only. Prototype from this call site: list word, value word, size word; the word types are inferred
// from the pushes and are not established by this body.
void NATIVE_DECL GraphList_Insert_4FA9D0(void* list, void* value, std::uint32_t size);

void NATIVE_DECL AIGraph_Handler_509450(void* object, std::uint32_t const* arg2)
{
    // 0x00509454: mov 0x58(%ecx),%eax; 0x00509457: mov %eax,%edx; 0x00509459: imul %eax,%edx; 0x00509466: shl $0x3,%edx.
    std::uint32_t const side = *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(object) + 0x58U);
    std::uint32_t const size = (side * side) << 3U;
    void* const buffer = *reinterpret_cast<void**>(static_cast<std::uint8_t*>(object) + 0xB8U); // 0x0050945C: mov 0xb8(%ecx),%eax

    // 0x00509469..0x0050946E: push %edx (size); mov (%ecx),%edx; push %eax (buffer); push %edx (*arg2); call 0x4FA9D0.
    GraphList_Insert_4FA9D0(reinterpret_cast<void*>(static_cast<std::uintptr_t>(*arg2)), buffer, size);                             // 0x00509473: add $0xc,%esp
}                                                                             // 0x00509476: ret
} // extern "C"

#undef NATIVE_DECL
