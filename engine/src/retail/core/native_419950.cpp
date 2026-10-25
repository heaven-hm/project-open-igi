// Native: BackgroundFX_EmitRecord_419950; retail VA 0x00419950 (0x00419950-0x004199A8, 89 bytes, 42 instructions, 3 blocks, 4 edges).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: build/retail-dis.txt (GNU objdump i386 byte+instruction listing, full body 0x00419950-0x004199A8); radare2 6.2.0 af/afij (size 89, 3 blocks, 4 edges, 42 instructions); called twice by 0x00419860 with the two global records 0x00539BF8 and 0x00539C08 and the same context argument.
// Behavior/CFG: two pointer arguments (a record and a context). In retail order: if record+0x00 is non-null, call 0x00419540(record+0x00, context); then loop over record+0x04 entries, calling 0x00419630(record+0x08 + i*0xC, context) and re-reading the count from record+0x04 after every call; then if record+0x0C is non-null, call 0x00419330(record+0x0C, context). No data references; the three callees are the BackgroundFX renderer entries.
// Status/limits: TRANSCRIBED (static only); runtime UNOBSERVED. The three callees are transcribed in this wave: 0x00419540 in core/native_419540.cpp, 0x00419630 in core/native_419630.cpp, 0x00419330 in core/native_419330.cpp (I updated the names used here to match those definitions, one implementation per VA). This body itself therefore leaves no undefined symbol; the undefined set of the wave is the renderer backend table 0x00A94E84, the engine-resident QTask/type registrars, and libm float trig used by the callees. The record layout (+0x00 pointer, +0x04 count, +0x08 array of 0xC-byte entries, +0x0C pointer) is read directly from these instructions.
//
// Retail statement order preserved: the first `test/je` (0x0041995A-0x00419960) guards the 0x00419540 call; the second count
// test (0x00419970-0x00419972) guards the loop; the loop reloads the count at 0x00419985 after each 0x00419630 call; the final
// pointer test (0x00419998) guards the 0x00419330 call.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// Renderer entries of the same class registered by 0x00419760; declared here because they are called from this body and
// defined in core/native_419540.cpp, core/native_419630.cpp and core/native_419330.cpp.
void NATIVE_DECL BackgroundFX_EmitSegments_419540(void* record, void* context);
void NATIVE_DECL BackgroundFX_EmitFan_419630(void* entry, void* context);
void NATIVE_DECL BackgroundFX_EmitAxisRuns_419330(void* quad, void* context);

// Stack arguments: [esp+4] record, [esp+8] context (after `push %ebp; push %esi` the body reads them at 0xC(%esp) each).
void NATIVE_DECL BackgroundFX_EmitRecord_419950(void* record, void* context)
{
    std::uint8_t* const record_bytes = reinterpret_cast<std::uint8_t*>(record);

    // 0x0041995A-0x00419967: `mov (%esi),%eax; test %eax,%eax; je 0x0041996A; push %ebp; push %eax; call 0x00419540; add $0x8,%esp`.
    if (*reinterpret_cast<std::uint32_t*>(record_bytes + 0x00) != 0U)
    {
        BackgroundFX_EmitSegments_419540(
            reinterpret_cast<void*>(static_cast<std::uintptr_t>(*reinterpret_cast<std::uint32_t*>(record_bytes + 0x00))),
            context);
    }

    // 0x0041996A-0x00419972: `mov 0x4(%esi),%eax; push %edi; xor %edi,%edi; test %eax,%eax; jle 0x00419994`.
    std::int32_t count = *reinterpret_cast<std::int32_t*>(record_bytes + 0x04);
    if (count > 0)
    {
        std::int32_t index = 0;
        std::uint32_t offset = 0U;

        // 0x00419977-0x00419991: `mov 0x8(%esi),%edx; mov %ebx,%eax; add %edx,%eax; push %ebp; push %eax; call 0x00419630;
        // mov 0x4(%esi),%eax; add $0x8,%esp; inc %edi; add $0xC,%ebx; cmp %eax,%edi; jl 0x00419977`.
        do
        {
            const std::uint32_t base = *reinterpret_cast<std::uint32_t*>(record_bytes + 0x08);
            BackgroundFX_EmitFan_419630(
                reinterpret_cast<void*>(static_cast<std::uintptr_t>(base + offset)),
                context);
            count = *reinterpret_cast<std::int32_t*>(record_bytes + 0x04);   // 0x00419985: count is re-read after the call
            ++index;
            offset += 0x0CU;
        } while (index < count);
    }

    // 0x00419994-0x004199A3: `mov 0xC(%esi),%esi; pop %edi; test %esi,%esi; je 0x004199A6; push %ebp; push %esi; call 0x00419330; add $0x8,%esp`.
    const std::uint32_t tail = *reinterpret_cast<std::uint32_t*>(record_bytes + 0x0C);
    if (tail != 0U)
    {
        BackgroundFX_EmitAxisRuns_419330(
            reinterpret_cast<void*>(static_cast<std::uintptr_t>(tail)),
            context);
    }
}

} // extern "C"

#undef NATIVE_DECL
