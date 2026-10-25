// Native: AIGraph_Handler_509480; retail VA 0x00509480 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00509480-0x00509514 (149 bytes, 5 blocks, 58 instructions).
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x00509480-0x00509514; radare2 6.2.0 pd/afij (149 bytes, 5 blocks, 58 instructions); linked by the AIGraph registrar 0x00508FA0 at 0x0050922E (0x00401400 event 0x004F9240()).
// Behavior/CFG: five blocks, two counted loops. Drains two lists through 0x004FA850: first it asks the callee for a count (arg1 = the word at object+0x04), then pops that many entries two at a time from the same list, then repeats the same pair of loops once more. The popped values are written to stack words the body never reads - only the calls and the list mutation have an effect.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 58-instruction listing; runtime UNOBSERVED. Link-blocked on 0x004FA850 (not transcribed); its prototype is inferred from these call sites. Retail reuses its OWN arg2 stack slot as the first out word (0x00509483 `lea 0x10(%esp),%eax` = &arg2) and re-reads that slot as the loop count (0x00509497), so the caller's argument word is overwritten; this file reproduces that literally by passing &object and by re-reading it in the loop condition, with the object pointer cached first exactly as retail caches it in ESI (0x00509488) before the callee rewrites the slot.
#include <cstdint>
#include <cstring>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x004FA850: removes one entry from the list at arg1 and stores its value word through arg2 (53 bytes, 1 block, 20 instructions; calls
// 0x004B17C0 and 0x004B16D0). Transcribed in engine/src/retail/core/native_4fa850.cpp.
void NATIVE_DECL GraphList_PopHead_4FA850(void* list, std::int32_t* out);

void NATIVE_DECL AIGraph_Handler_509480(void* task, void* object)
{
    (void)task;                                                               // arg1 unread: retail's first load targets the word at [esp+8]

    // 0x00509483: lea 0x10(%esp),%eax (&arg2); 0x00509488: mov 0x14(%esp),%esi (the object, read before the callee rewrites the slot);
    // 0x0050948D..0x00509492: push %eax; mov 0x4(%esi),%ecx; push %ecx; call 0x4FA850.
    void* const target = object;
    GraphList_PopHead_4FA850(*reinterpret_cast<void**>(static_cast<std::uint8_t*>(target) + 0x04U),
                             reinterpret_cast<std::int32_t*>(&object));

    // 0x00509497: mov 0x20(%esp),%eax - the count is the word the callee just wrote into the argument slot. Retail re-reads the slot on
    // every iteration (0x005094C0); the loop's own callees receive other stack words (0x005094A7/0x005094B5), so the value cannot change
    // and the single read below is equivalent. The read goes through memcpy because retail's punt (a void* parameter slot read as a dword)
    // is not expressible in C++ without breaking strict aliasing.
    std::int32_t count = 0;
    std::memcpy(&count, &object, sizeof count);
    for (std::int32_t index = 0; index < count; ++index)                     // 0x005094A2..0x005094CA
    {
        std::int32_t first = 0;
        std::int32_t second = 0;
        // 0x005094A4..0x005094AD: reload [esi+4]; lea 0xc(%esp),%edx; push %edx; push %eax; call 0x4FA850.
        GraphList_PopHead_4FA850(*reinterpret_cast<void**>(static_cast<std::uint8_t*>(target) + 0x04U), &first);
        // 0x005094B2..0x005094BB: reload [esi+4]; lea 0x14(%esp),%ecx; push %ecx; push %edx; call 0x4FA850.
        GraphList_PopHead_4FA850(*reinterpret_cast<void**>(static_cast<std::uint8_t*>(target) + 0x04U), &second);
        (void)first;                                                          // retail never reads either out word
        (void)second;
    }

    // 0x005094CC..0x005094D5: reload [esi+4]; lea 0x8(%esp),%eax; push %eax; push %ecx; call 0x4FA850 - this count lives in a local, not in
    // the argument slot (the sub $0x8 frame at 0x00509480 supplies it).
    std::int32_t second_count = 0;
    GraphList_PopHead_4FA850(*reinterpret_cast<void**>(static_cast<std::uint8_t*>(target) + 0x04U), &second_count); // 0x005094DA

    for (std::int32_t index = 0; index < second_count; ++index)               // 0x005094E1..0x005094E5, 0x0050950A..0x0050950D
    {
        std::int32_t first = 0;
        std::int32_t second = 0;
        // 0x005094E7..0x005094F0: reload [esi+4]; lea 0xc(%esp),%edx; push %edx; push %eax; call 0x4FA850.
        GraphList_PopHead_4FA850(*reinterpret_cast<void**>(static_cast<std::uint8_t*>(target) + 0x04U), &first);
        // 0x005094F5..0x005094FE: reload [esi+4]; lea 0x14(%esp),%ecx; push %ecx; push %edx; call 0x4FA850.
        GraphList_PopHead_4FA850(*reinterpret_cast<void**>(static_cast<std::uint8_t*>(target) + 0x04U), &second);
        (void)first;
        (void)second;
    }
}                                                                             // 0x00509514: ret
} // extern "C"

#undef NATIVE_DECL
