// Native: Sub_4C7560 (address-suffixed placeholder: this VA has no manifest record; the body clears handler-state counters, which establishes no retail name); retail VA 0x004C7560-0x004C75E4.
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000; .text VA 0x00401000 raw 0x1000.
// Tools/evidence: capstone 5.0.7 decode of the exact extent: 46 instructions, 133 bytes, `ret` at 0x004C75E4, 0 `90` nops after. Exact bytes: 53 8B 5C 24 08 55 8B 6C 24 10 56 57 8B 75 50 33 FF 55 56 8B 46 34 89 BC 83 E8 00 00 00 E8 5E F7 FF FF 56 53 E8 67 00 00 00 83 C4 10 33 C9 C7 44 24 18 FE 00 00 00 EB 04 8B 5C 24 14 8B 46 34 8B D0 C1 E2 07 2B D0 8D 04 11 33 D2 66 8B 55 1C 8D 84 43 18 01 00 00 03 C7 33 DB 39 1C 95 E0 6A A9 00 0F 95 C3 66 29 18 8B 44 24 18 41 81 C7 80 01 00 00 48 89 44 24 18 75 C1 5F C7 45 50 00 00 00 00 5E 5D 5B C3. ENTRY BOUNDARY: an image-wide arithmetic scan of every `E8` displacement finds 60 call sites, and the bytes before the call at 0x00426869 are `8B 44 24 04 50 8B 40 68 50` = `mov eax,[esp+4]; push eax; mov eax,[eax+0x68]; push eax`, so retail's callers push exactly two arguments in the order (object, object->0x68) - which is why the body reads arg1 as the field value and arg2 as the object. THE FORWARD JUMP AT 0x004C7596 IS NOT A TAIL CALL: its target 0x004C759C is 6 bytes ahead, the bytes it skips (0x004C7598) are an instruction, and 0x004C7598 is the conditional-branch target of 0x004C75D7 - it is the loop head. A forward unconditional jump is a tail call only when its target is neither a conditional-branch target of the same function nor inside the function's fall-through region. THE PROJECT AUDITOR FLAGS THIS BODY AND THAT IS EXPECTED: `python3 scripts/native_fidelity_audit.py --va 0x004C7560` reports `1 other` with `tail-jump bodies detected: 1`, because the extent ends in the jump and the auditor cannot decide locally. The corroboration is the two bytes above - `jmp 0x004C759C` at 0x004C7596 and `jne 0x004C7598` at 0x004C75D7 are 6 and 4 bytes apart - so the jump target is the loop head and the true extent is 46 instructions, not the 20 the tail-call default would give.
// Behavior/CFG: 46 instructions, one loop. Prologue saves ebx/ebp/esi/edi and loads ebx = arg1 (field) and ebp = arg2 (object); esi = *(void**)(object+0x50). It clears *(std::uint32_t*)(field + *(std::uint32_t*)(esi+0x34)*4 + 0xE8) to zero, calls 0x004C6CE0(esi, object) and 0x004C75F0(field, esi), then cleans 4 dwords (`add esp,0x10`). The loop then stores 0xFE into the INCOMING ARGUMENT SLOT (`mov [esp+0x18],0xFE` at 0x004C758E) and reuses it as the counter - which is why arg2 was saved into ebp first and why the loop body re-reads arg1 from `[esp+0x14]` (ebx is clobbered by the `setne`/`xor` below). Each iteration: index = *(std::uint16_t*)(object+0x1C) + edi, where edi advances by the phase stride 0x180 per pass; the counter slot at field + (ecx + *(std::uint32_t*)(esi+0x34)*254)*2 + 0x118 is decremented by one when QTask_HandlerTable[index] is non-null (0/1 through `setne bl`), where QTask_HandlerTable is 0x00A96AE0, the table task_types.hpp owns. The `jmp 0x004C759C` at 0x004C7596 exists only so the first pass skips the arg1 reload at 0x004C7598 - ebx still holds arg1 from the prologue. After the loop, *(void**)(object+0x50) is set to null and the four saved registers are restored; the body ends `ret` with no `add esp`, so the caller cleans the two arguments.
// Status/limits: TRANSCRIBED; NOT LINKABLE if its two callees are absent from the tree (measured undefined list is reported to the Lead rather than hidden). STRUCTURALLY_VERIFIED against the 46-instruction listing (extent bounded by the `ret`, the loop head and the counter's stack slot both read from the bytes); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile); runtime UNOBSERVED. What the field value is, what 0x004C6CE0 and 0x004C75F0 do, and what the counters mean are not established by this body, so both callees keep address-suffixed placeholders. Domain: core/, because its caller is core/native_449950.cpp and it maintains the QTask handler-state tables that task_types.hpp describes.

#include "igi/retail/core/task_types.hpp"

#include <cstdint>

// task_types.hpp defines NATIVE_DECL for its own declarations and #undefs it again, so it is re-established here.
#ifndef NATIVE_DECL
#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif
#endif

extern "C" {

// NATIVE GAP: 0x004C6CE0 has no manifest record and no file. Measured: 1 call site (0x004C757D), two arguments,
// result unused. The body was not read, so it keeps an address-suffixed placeholder.
void Sub_4C6CE0(void* record, void* object);

// NATIVE GAP: 0x004C75F0 has no manifest record and no file. Measured: 1 call site (0x004C7584), two arguments,
// result unused. The body was not read, so it keeps an address-suffixed placeholder.
void Sub_4C75F0(std::uint32_t field, void* record);

void NATIVE_DECL Sub_4C7560(std::uint32_t field, void* object)
{
    std::uint8_t* const field_base = reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(field));
    std::uint8_t* const object_base = static_cast<std::uint8_t*>(object);

    void* const record = *reinterpret_cast<void**>(object_base + 0x50); // 0x004C756C: mov esi,[ebp+0x50]
    void* record_base = record;

    // 0x004C7573: mov eax,[esi+0x34]; 0x004C7576: mov [ebx+eax*4+0xe8],edi (edi = 0 from 0x004C756F).
    *reinterpret_cast<std::uint32_t*>(field_base + *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(record_base) + 0x34) * 4 + 0xE8) = 0U;

    Sub_4C6CE0(record, object);        // 0x004C7571: push ebp; 0x004C7572: push esi; 0x004C757D: call 0x4c6ce0
    Sub_4C75F0(field, record);         // 0x004C7582: push esi; 0x004C7583: push ebx; 0x004C7584: call 0x4c75f0
    // 0x004C7589: add esp,0x10 cleans the four pushed dwords above.

    // 0x004C7592: the counter lives in the incoming argument slot [esp+0x18] (arg2's slot); arg2 is already in ebp.
    // The loop head at 0x004C7598 reloads arg1 because ebx is clobbered below, and the first pass enters at 0x004C759C.
    std::uint32_t phase_offset = 0; // 0x004C756F/0x004C75CC: edi, advanced by the phase stride each pass
    std::uint32_t phase_index = 0;  // 0x004C758C/0x004C75CB: ecx
    for (std::uint32_t remaining = igi::retail::core::task_phase_count; remaining != 0; --remaining) { // 0x004C758E: 0xFE; 0x004C75D2: dec; 0x004C75D7: jne
        const std::uint32_t record_field = *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(record_base) + 0x34); // 0x004C759C
        // 0x004C759F-0x004C75A6: edx = record_field*128 - record_field = record_field*127; eax = phase_index + edx*2.
        const std::uint32_t counter_index = phase_index + record_field * 127U * 2U;
        // 0x004C75A9-0x004C75AB: edx = *(std::uint16_t*)(object+0x1c); 0x004C75B6: edx += phase_offset.
        const std::uint32_t table_index = static_cast<std::uint32_t>(*reinterpret_cast<std::uint16_t*>(object_base + 0x1C)) + phase_offset;
        std::uint16_t* const counter = reinterpret_cast<std::uint16_t*>(field_base + static_cast<std::uintptr_t>(counter_index) * 2U + 0x118); // 0x004C75AF
        // 0x004C75BA: cmp [edx*4+0xa96ae0],ebx(0); 0x004C75C1: setne bl; 0x004C75C4: sub word [eax],bx.
        // 0x00A96AE0 is declared at global scope by task_types.hpp (line 121) as ::QTask_HandlerTable.
        const std::uint16_t linked = (::QTask_HandlerTable[table_index] != nullptr) ? 1U : 0U;
        *counter = static_cast<std::uint16_t>(*counter - linked);

        ++phase_index;                                   // 0x004C75CB: inc ecx
        phase_offset += igi::retail::core::task_phase_stride; // 0x004C75CC: add edi,0x180
    }

    // 0x004C75DA: mov dword [ebp+0x50],0.
    *reinterpret_cast<void**>(object_base + 0x50) = nullptr;
    // 0x004C75D9/0x004C75E1-0x004C75E3: pop edi, pop esi, pop ebp, pop ebx; 0x004C75E4: ret (cdecl).
}

} // extern "C"

#undef NATIVE_DECL
