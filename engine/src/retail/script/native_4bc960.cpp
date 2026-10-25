// Native: Qvm_FormatOperandText_4BC960 (behavior-derived placeholder name; retail symbol unknown; Binary Ninja label Sys_CheckHeap_29 is wrong - this body formats operand text and calls no allocator). retail VA 0x004BC960-0x004BCA92 (307 bytes, 9 blocks, 12 edges, 41 instructions in the reachable set).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: capstone 5.0.7 reachability decode from the verified entry (fallthrough plus every branch target, no fallthrough past a ret) over the exact image; the extent is bounded by the `ret` at 0x004BCA92, and the first following byte is 0x90 padding. Exact bytes 55 8b ec 83 e4 f8 b8 08 10 00 00 e8 30 8b fe ff 53 56 57 ... 5f 5e 5b 8b e5 5d c3 (307 bytes, sha256 d9a3af4925ff7a458990097906ecc79a81abf8387bd1fa95a11fe32f8e5e68b4 of the exact range). Call sites: 0x004BC96B + 0xFFFF8B30 = 0x004A54A0 (the MSVC 6 __chkstk stack-probe helper, not transcribed and NOT called from this source - see limits) and three `call 0x004A53B3` = the CRT sprintf (0x004BC9BB, 0x004BC9DF, 0x004BC9FF, 0x004BCA25, 0x004BCA4A, 0x004BCA66). The operand-type dispatch is a jump table at 0x004BCA94 with 7 entries read from the image: [0x004BC9AA, 0x004BC9C9, 0x004BCA13, 0x004BC9ED, 0x004BCA5C, 0x004BCA5C, 0x004BCA39] indexed by `type_stack[i] - 1` after `cmp eax,6 / ja`. Call census from the image's 0xE8 enumeration: 0x004BD080 (Qvm_RunProgram) is the only caller.
// Behavior/CFG: one cdecl pointer argument (the QVM state at [ebp+8]), no meaningful return (retail leaves EAX at the last sprintf result and no caller consumes it). A do-while over the instruction index `instruction` (state+0x1C, register ebx) from its entry value to the limit at state+0x0C, with a separate operand cursor `cursor` (state+0x18, register esi): type_stack[instruction] selects the format, the case formats value_stack[cursor] into a 0x1000-byte stack buffer, and the cursor advances by 1 for the scalar cases or by `value_stack[cursor] + 1` for the length-prefixed string/data cases. The loop stops early when the type is unknown (the shared 0x004BCA5C block sets the stop flag at [esp+0xC] to 1 and the tail at 0x004BCA81 exits on it). Format strings are byte-identical to retail: 0x0053FDE4 "%d", 0x00547A60 "%f", 0x0053F730 "%s", 0x00547AAC "\"%s\"", 0x00547A98 "%d words of data.", 0x00547A84 "UNKNOWN DATA TYPE". The `%f` case loads a 32-bit float (`fld dword [ecx+esi*4]`) and passes it as a double (`fstp qword [esp]`). The 0x004BCA53 `mov ebx,[esp+0x1c]` in the "%d words of data." case reloads the clobbered instruction index from its local slot; in this source the loop counter is a named local and no reload is needed. 0x004BCA8F `mov esp,ebp` / `pop ebp` is the frame teardown for the aligned frame (`and esp,-8`).
// Status/limits: TRANSCRIBED; LINKABLE (its only callee is the CRT sprintf, declared through <cstdio>; it adds no tree dependency); STRUCTURALLY_VERIFIED against the 41-instruction reachable listing and the 7-entry jump table read from the image (9 blocks, 12 edges); runtime UNOBSERVED. THE BUFFER IS NEVER CONSUMED: the formatted text is written to the stack frame and no instruction in this body, and no caller (0x004BD080 calls it and then re-reads state+0x20), ever reads it back. That dead store is retail's behavior and is preserved verbatim rather than tidied away. THE 0x004A54A0 CALL IS A COMPILER ARTIFACT, NOT SOURCE BEHAVIOR: 0x004BC966 `mov eax,0x1008` / 0x004BC96B `call 0x004A54A0` is the __chkstk probe for the 0x1008-byte frame, so this source declares the buffer and lets the compiler emit its own probe; no symbol reference to 0x004A54A0 is created, which is why this file is linkable although that helper has no file. Domain: script/, because the state is the QVM interpreter context and the caller is the QVM program runner.

#include <cstdint>
#include <cstdio>

#include "igi/retail/script/qvm_native_state.hpp"

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

void NATIVE_DECL Qvm_FormatOperandText_4BC960(igi::retail::script::QvmNativeStatePrefix* state)
{
    // 0x004BC966 `mov eax,0x1008` / 0x004BC96B `call 0x004A54A0` allocate the frame; 0x004BC9AD `lea eax,[esp+0x14]`
    // is the buffer's address. The compiler emits its own stack probe for this array.
    char buffer[0x1000];
    std::int32_t stop = 0;                                                    // [esp+0xC]  (0x004BC976)
    std::int32_t instruction = state->type_stack_index;                       // ebx, state+0x1C (0x004BC97E)
    std::int32_t cursor = state->value_stack_index;                           // esi, state+0x18 (0x004BC984)
    std::int32_t const limit = static_cast<std::int32_t>(state->unknown_04_through_0C[2]); // state+0x0C (0x004BC981)

    // 0x004BC987 `cmp ebx,eax` / 0x004BC98D `jge 0x004BCA8C` - the entry test.
    if (instruction >= limit)
    {
        return;
    }

    for (;;)
    {
        // 0x004BC993 `mov eax,[edi+0x28]` / 0x004BC996 `mov eax,[eax+ebx*4]` / 0x004BC999 `dec eax` / 0x004BC99A
        // `cmp eax,6` / 0x004BC99D `ja 0x004BCA5C` (UNSIGNED, so a zero type also takes the unknown path).
        std::uint32_t const operand_type = state->type_stack[instruction] - 1U;
        if (operand_type > 6U)
        {
            // 0x004BCA5C: `lea eax,[esp+0x14]` / `push 0x00547A84` / `push eax` / `call sprintf` / `add esp,8`.
            ::sprintf(buffer, "UNKNOWN DATA TYPE");
            stop = 1;                                                         // 0x004BCA6E
        }
        else
        {
            // 0x004BC9A3 `jmp dword ptr [eax*4 + 0x004BCA94]` - the 7-entry table read from the image.
            switch (operand_type)
            {
            case 0U: // 0x004BC9AA: `mov ecx,[edi+0x24]` / `mov edx,[ecx+esi*4]` / `push 0x0053FDE4` ("%d")
                ::sprintf(buffer, "%d", static_cast<std::int32_t>(state->value_stack[cursor]));
                cursor += 1;                                                  // 0x004BC9C3 `inc esi`
                break;
            case 1U: // 0x004BC9C9: `sub esp,8` / `fld dword [ecx+esi*4]` / `fstp qword [esp]` / `push 0x00547A60` ("%f")
                ::sprintf(buffer, "%f", static_cast<double>(*reinterpret_cast<float const*>(&state->value_stack[cursor])));
                cursor += 1;                                                  // 0x004BC9E7 `inc esi`
                break;
            case 2U: // 0x004BCA13: `lea eax,[edx+esi*4+4]` / `push 0x00547AAC` ("\"%s\"") / `lea esi,[esi+eax+1]`
                ::sprintf(buffer, "\"%s\"", reinterpret_cast<char const*>(&state->value_stack[cursor + 1]));
                cursor += static_cast<std::int32_t>(state->value_stack[cursor]) + 1; // 0x004BCA30-0x004BCA33
                break;
            case 3U: // 0x004BC9ED: `lea ecx,[eax+esi*4+4]` / `push 0x0053F730` ("%s") / `lea esi,[esi+ecx+1]`
                ::sprintf(buffer, "%s", reinterpret_cast<char const*>(&state->value_stack[cursor + 1]));
                cursor += static_cast<std::int32_t>(state->value_stack[cursor]) + 1; // 0x004BCA0A-0x004BCA0D
                break;
            case 4U: // 0x004BCA5C (table entry 5): the same unknown-type block as the `ja` path.
            case 5U: // 0x004BCA5C (table entry 6): the same unknown-type block as the `ja` path.
                ::sprintf(buffer, "UNKNOWN DATA TYPE");
                stop = 1;
                break;
            case 6U: // 0x004BCA39: `mov ebx,[ecx+esi*4]` / `push 0x00547A98` ("%d words of data.") / `lea esi,[esi+ebx+1]`
            {
                std::int32_t const words = static_cast<std::int32_t>(state->value_stack[cursor]);
                ::sprintf(buffer, "%d words of data.", words);
                cursor += words + 1;                                          // 0x004BCA4F
                break;
            }
            default:
                break;
            }
        }
        // 0x004BCA7A `inc ebx` / 0x004BCA7D `mov [esp+0x10],ebx` / 0x004BCA7B `test eax,eax` / 0x004BCA81 `jne 0x004BCA8C`.
        instruction += 1;
        if (stop != 0)
        {
            break;
        }
        // 0x004BCA83 `cmp ebx,[edi+0xC]` / 0x004BCA86 `jl 0x004BC993`.
        if (instruction >= limit)
        {
            break;
        }
    }
    // 0x004BCA8C-0x004BCA92: `pop edi` / `pop esi` / `pop ebx` / `mov esp,ebp` / `pop ebp` / `ret`. EAX holds the
    // last sprintf result; no caller reads it, so the port returns void.
}

} // extern "C"
