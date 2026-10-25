// Native: AIGraph_Handler_509480; retail VA 0x00509480 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00509480-0x00509514 (149 bytes, 5 blocks, 58 instructions).
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x00509480-0x00509514; radare2 6.2.0 afij (149 bytes, 5 blocks, 58 instructions); linked by the AIGraph registrar 0x00508FA0 at 0x0050922E (0x00401400 event 0x004F9240()).
// Behavior/CFG: five blocks, two counted loops. Drains two lists through 0x004FA850: first it asks the callee for a count (arg1 = the word at
//                object+0x04), then pops that many entries two at a time from the same list, then repeats the same pair of loops once more.
//                The popped values are written to stack words the body never reads - only the calls and the list mutation have an effect.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 58-instruction listing; runtime UNOBSERVED. Link-blocked on 0x004FA850
//                (not transcribed); its prototype is inferred from these call sites. DEVIATION: retail reuses its own arg2 stack slot as the
//                first out word (0x00509483 lea 0x10(%esp) = &arg2) and re-reads it as the count; this file uses a local instead. That is
//                unobservable here because retail never uses arg2 again and the dispatcher does not read the slot, but the caller's argument
//                word is left unmodified instead of overwritten.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x004FA850: removes one entry from the list at arg1 and stores its value word through arg2 (53 bytes, 1 block, 20 instructions; calls
// 0x004B17C0 and 0x004B16D0). NOT transcribed yet - declared only.
void NATIVE_DECL GraphList_PopHead_4FA850(void* list, std::int32_t* out);

void NATIVE_DECL AIGraph_Handler_509480(void* task, void* object)
{
    (void)task;                                                               // arg1 unread: retail's first load targets the word at [esp+8]

    std::int32_t count = 0;
    // 0x00509483..0x00509492: lea 0x10(%esp),%eax (&arg2); push %eax; push [esi+4]; call 0x4FA850.
    GraphList_PopHead_4FA850(*reinterpret_cast<void**>(static_cast<std::uint8_t*>(object) + 0x04U), &count);

    for (std::int32_t index = 0; index < count; ++index)                      // 0x0050949E..0x005094A2, 0x005094C7..0x005094CA
    {
        std::int32_t first = 0;
        std::int32_t second = 0;
        // 0x005094A4..0x005094AD: reload [esi+4]; lea 0xc(%esp),%edx; push %edx; push %eax; call 0x4FA850.
        GraphList_PopHead_4FA850(*reinterpret_cast<void**>(static_cast<std::uint8_t*>(object) + 0x04U), &first);
        // 0x005094B2..0x005094BB: reload [esi+4]; lea 0x14(%esp),%ecx; push %ecx; push %edx; call 0x4FA850.
        GraphList_PopHead_4FA850(*reinterpret_cast<void**>(static_cast<std::uint8_t*>(object) + 0x04U), &second);
        (void)first;                                                          // retail never reads either out word
        (void)second;
    }

    // 0x005094CC..0x005094D5: reload [esi+4]; lea 0x8(%esp),%eax; push %eax; push %ecx; call 0x4FA850 - the second list's count.
    std::int32_t second_count = 0;
    GraphList_PopHead_4FA850(*reinterpret_cast<void**>(static_cast<std::uint8_t*>(object) + 0x04U), &second_count);

    for (std::int32_t index = 0; index < second_count; ++index)               // 0x005094E1..0x005094E5, 0x0050950A..0x0050950D
    {
        std::int32_t first = 0;
        std::int32_t second = 0;
        // 0x005094E7..0x005094F0: reload [esi+4]; lea 0xc(%esp),%edx; push %edx; push %eax; call 0x4FA850.
        GraphList_PopHead_4FA850(*reinterpret_cast<void**>(static_cast<std::uint8_t*>(object) + 0x04U), &first);
        // 0x005094F5..0x005094FE: reload [esi+4]; lea 0x14(%esp),%ecx; push %ecx; push %edx; call 0x4FA850.
        GraphList_PopHead_4FA850(*reinterpret_cast<void**>(static_cast<std::uint8_t*>(object) + 0x04U), &second);
        (void)first;
        (void)second;
    }
}                                                                             // 0x00509514: ret
} // extern "C"

#undef NATIVE_DECL
