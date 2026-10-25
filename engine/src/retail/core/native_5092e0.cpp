// Native: AIGraph_ResetState_5092E0; retail VA 0x005092E0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x005092E0-0x00509337 (88 bytes, 5 blocks, 27 instructions).
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x005092E0-0x00509337; radare2 6.2.0 pd/afij (88 bytes, 5 blocks, 27 instructions); called from 0x005092AA, 0x005093B5 and 0x00509530.
// Behavior/CFG: five blocks, no loop. Pops the heap flag stack with 1, releases the buffer word at object+0xB8 through 0x004B0D10 when it is set, and when the signed side word at object+0x58 is greater than one allocates 8 * side * side bytes (alignment 4) through 0x004B0C60, stores the block back at object+0xB8 and fills it with 0x00509340. Pushes the heap flag stack again on the way out.
// Status/limits: BUILD_VERIFIED; STRUCTURALLY_VERIFIED against the 27-instruction listing; runtime UNOBSERVED. Link-blocked on 0x00509340 only
//                (69 bytes, 7 blocks, 33 instructions; transcribed in native_509340.cpp together with its leaf callee 0x00509390). 0x004B0D60,
//                0x004B0D10, 0x004B0D90 and 0x004B0C60 (mem::Mem_Alloc) are all already transcribed in the tree.
#include "dev_log.hpp"
#include "igi/retail/mem/mem_alloc.hpp"

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x004B0D60 / 0x004B0D90 / 0x004B0D10: the heap flag stack and block release, transcribed in engine/src/retail/mem/
// (Mem_FlagStackPop, Mem_FlagStackPush, Mem_DeAllocFn).
void Mem_FlagStackPop(std::uint8_t flag);
void Mem_FlagStackPush();
void Mem_DeAllocFn(void* pointer);

// 0x004B0C60: the engine allocator, already transcribed as igi::retail::mem::Mem_Alloc (engine/src/retail/mem/mem_alloc.cpp,
// declared in igi/retail/mem/mem_alloc.hpp). Retail pushes the size first (0x0050931C) and the alignment second (0x0050931A), which
// is that function's (context, arg2) parameter order, so the call needs no adapter.

// 0x00509340: the 8 * side * side buffer filler that runs right after the allocation above (69 bytes, 7 blocks, 33 instructions; it walks
// side rows by side columns through 0x00509390). NOT transcribed yet - declared only.
void NATIVE_DECL AIGraph_PopulateBuffer_509340(void* object);

void NATIVE_DECL AIGraph_ResetState_5092E0(void* object)
{
    NATIVE_LOG_ENTER("AIGraph_ResetState_5092E0", "0x005092E0");
    std::uint8_t* const fields = static_cast<std::uint8_t*>(object);

    Mem_FlagStackPop(1);                                                      // 0x005092E1: push $0x1; 0x005092E3: call 0x4b0d60

    void* const buffer = *reinterpret_cast<void**>(fields + 0xB8U);            // 0x005092EF: mov 0xb8(%esi),%eax
    if (buffer != nullptr)                                                    // 0x005092F5/0x005092F7: test %eax,%eax; je 0x50930c
    {
        Mem_DeAllocFn(buffer);                                                // 0x005092F9/0x005092FA: push %eax; call 0x4b0d10
        *reinterpret_cast<std::uint32_t*>(fields + 0xB8U) = 0U;               // 0x00509302: movl $0x0,0xb8(%esi)
    }

    // 0x0050930C: mov 0x58(%esi),%eax; 0x0050930F: cmp %eax,0x1; 0x00509312: jle 0x509331.
    std::int32_t const side = *reinterpret_cast<std::int32_t*>(fields + 0x58U);
    if (side > 1)
    {
        // 0x00509314: imul %eax,%eax; 0x00509317: shl $0x3,%eax; 0x0050931A: push $0x4; 0x0050931C: push %eax; 0x0050931D: call 0x4b0c60.
        std::uint32_t const size =
            (static_cast<std::uint32_t>(side) * static_cast<std::uint32_t>(side)) << 3U;
        void* const allocated =
            igi::retail::mem::Mem_Alloc(static_cast<std::int32_t>(size), 4); // 0x0050931A/0x0050931C/0x0050931D: push $0x4; push %eax; call 0x4b0c60
        *reinterpret_cast<void**>(fields + 0xB8U) = allocated;                // 0x00509323: mov %eax,0xb8(%esi)
        AIGraph_PopulateBuffer_509340(object);                                // 0x00509322/0x00509329: push %esi; call 0x509340
    }

    Mem_FlagStackPush();                                                      // 0x00509331: call 0x4b0d90
}                                                                             // 0x00509336/0x00509337: pop %esi; ret
} // extern "C"

#undef NATIVE_DECL
