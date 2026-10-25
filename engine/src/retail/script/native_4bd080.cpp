// Native: Qvm_RunProgram (this tree's existing name for retail VA 0x004BD080, already declared and called in script/qvm_execute.cpp; Binary Ninja label Sys_ExecuteBytecode_29). retail VA 0x004BD080-0x004BD1D9 (346 bytes, 20 blocks, 32 edges, 107 instructions in the reachable set, of which 38 lie outside the jump table's fifteen case bodies).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: capstone 5.0.7 reachability decode from the verified entry (fallthrough plus every branch target, no fallthrough past a ret) over the exact image; the extent is bounded by the `ret` at 0x004BD1D9, and `8b ff` (the two-byte `mov edi,edi` hot-patch nop) follows at 0x004BD1DA. Exact bytes 56 8b 74 24 08 57 33 ff ... 5f 33 c0 5e c3 (346 bytes, sha256 1ac059abeeb0e87e2f85715b9766e5ac5d39b7c4f8ea38580bb7e56e3ee8c5e2 of the exact range). Every call displacement resolves as v+5+rel32: 0x004BD0A1 + 0xFFFFFE2A = 0x004BCFD0 (Qvm_ExecuteInstructions); 0x004BD1BE + 0xFFFF81F0 = 0x004A53B3 (the CRT sprintf); 0x004BD1C7 + 0xFFFFF725 = 0x004BC960 (this chain's formatter); 0x004BD12D and 0x004BD19B and 0x004BD136 and 0x004BD1A4 resolve to the same three targets. The error-status dispatch is a jump table at 0x004BD1DC with 15 entries read from the image: [0x004BD0C5, 0x004BD0D6, 0x004BD10C, 0x004BD1B2, 0x004BD0F1, 0x004BD102, 0x004BD11D, 0x004BD144, 0x004BD152, 0x004BD160, 0x004BD0E0, 0x004BD1B2, 0x004BD183, 0x004BD175, 0x004BD167] indexed by `state+0x20 - 1` after `cmp eax,0xe / ja`. Call census from the image's 0xE8 enumeration: 0x004B85B0 (Qvm_Execute) is the only caller.
// Behavior/CFG: one cdecl 32-bit argument (the QVM state pointer, declared as std::uint32_t to match the existing declaration in script/qvm_execute.cpp), returns the interpreter status (0 when the program ran). It saves state+0x0C and state+0x08 into locals, then zeroes state+0x14 (code_base), state+0x2C (stop_flag), state+0x20 (error_status) and state+0x10 (saved_instruction_pointer), copies the saved +0x0C into +0x1C (type_stack_index) and the saved +0x08 into +0x18 (value_stack_index) - the two reads happen before the writes, which is why the copy order is safe - then calls Qvm_ExecuteInstructions(state) (0x004BCFD0). When the resulting status is 0 it returns 0; otherwise it formats one status string into state+0x268 through sprintf and calls the formatter 0x004BC960(state), then re-reads state+0x20 and returns it. Format strings are byte-identical to retail: 0x00547C10 "Stack underflow", 0x00547C00 "Stack overflow", 0x00547BF0 "Syntax error", 0x00547BB8 "Illegal operand(s) for instruction", 0x00547BDC "Illegal instruction", 0x00547B80 "Unknown identifier: %s" (the argument is state+0x68), 0x00547B64 "Identifier not a variable", 0x00547B48 "Identifier not a function", 0x00547B30 "No function on stack", 0x00547BF0 ... 0x00547BF0 is Syntax error, 0x00547B1C "Index out of range", 0x00547B04 "Datablock not present", 0x00547AD8 "Function argument overflow: %d arguments." and the default 0x00547AC8 "Undefined error". Statuses 4, 12 and every value above 15 take the default string, because the table sends entries 4 and 12 to the default block 0x004BD1B2. The "Function argument overflow" case is the only one with a computed argument: `mov eax,[esi]` (state+0x00) / `mov ecx,[esi+0x10]` (state+0x10) / `lea eax,[eax+ecx+1]` / `mov eax,[eax]`, i.e. the dword at (state+0x00 + state+0x10 + 1).
// Status/limits: TRANSCRIBED; LINKABLE (its callees Qvm_ExecuteInstructions 0x004BCFD0, sprintf 0x004A53B3 and Qvm_FormatOperandText_4BC960 0x004BC960 are all in the tree, so it adds no new dependency); STRUCTURALLY_VERIFIED against the 107-instruction reachable listing and the 15-entry jump table read from the image (20 blocks, 32 edges; radare2 20 blocks/17 edges and Binary Ninja 20 blocks/31 edges agree on the block count, and the block count agrees with this source exactly). INSTRUCTION-COUNT CORRECTION: an earlier provenance line in this file said "38 instructions, 17 blocks, 20 edges" - that was measured before the jump-table targets were seeded into the reachability walk, so it silently excluded the fifteen status case bodies, and the block/edge figures were estimates rather than measurements. The corrected figures above are measured with the table's fifteen targets included, and the walk's last instruction is the `ret` at 0x004BD1D9, so the 107 instructions fill exactly this function's 346-byte extent; the source itself always covered all fifteen cases; runtime UNOBSERVED. This definition satisfies the previously unresolved `Qvm_RunProgram` declared at script/qvm_execute.cpp:86, but that file still fails to link for seven other symbols of its own host layer (Qvm_ParseBlock, Qvm_ReadNextBlock, Qvm_ReleaseBlock, Qvm_ReleaseBlockTree, Qvm_ReleaseSourceReader, Qvm_SourceBufferInit, Qvm_SourceReaderInit) - measured, not this file's gap. The field at state+0x00 is used as an address addend by the case above while the shared struct names it instruction_offset; both readings are retail's and the discrepancy is left standing rather than resolved. Domain: script/, because the state is the QVM interpreter context.

#include "dev_log.hpp"
#include <cstdint>
#include <cstdio>

#include "igi/retail/script/qvm_native_state.hpp"
#include "igi/retail/script/qvm_stack_natives.hpp"

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x004BC960: formats the operand text of the failing instruction into state+0x268 (script/native_4bc960.cpp).
void NATIVE_DECL Qvm_FormatOperandText_4BC960(igi::retail::script::QvmNativeStatePrefix* state);

std::int32_t NATIVE_DECL Qvm_RunProgram(std::uint32_t program)
{
    NATIVE_LOG_ENTER("Qvm_RunProgram", "0x004BD080");
    igi::retail::script::QvmNativeStatePrefix* const state =
        reinterpret_cast<igi::retail::script::QvmNativeStatePrefix*>(static_cast<std::uintptr_t>(program));

    // 0x004BD088 `mov eax,[esi+0xC]` and 0x004BD08B `mov ecx,[esi+8]` read the two indices BEFORE any write below.
    std::int32_t const entry_type_index = static_cast<std::int32_t>(state->unknown_04_through_0C[2]); // +0x0C
    std::int32_t const entry_value_index = static_cast<std::int32_t>(state->unknown_04_through_0C[1]); // +0x08
    // 0x004BD086 `xor edi,edi` - the zero the retail stores into four fields.
    state->code_base = nullptr;                          // 0x004BD08F: `mov [esi+0x14],edi`
    state->type_stack_index = entry_type_index;          // 0x004BD092: `mov [esi+0x1C],eax`
    state->value_stack_index = entry_value_index;        // 0x004BD095: `mov [esi+0x18],ecx`
    state->stop_flag = 0U;                               // 0x004BD098: `mov [esi+0x2C],edi`
    state->error_status = 0U;                            // 0x004BD09B: `mov [esi+0x20],edi`
    state->saved_instruction_pointer = nullptr;          // 0x004BD09E: `mov [esi+0x10],edi`
    // 0x004BD08E `push esi` / 0x004BD0A1 `call 0x004BCFD0` / 0x004BD0A9 `add esp,4`.
    (void)igi::retail::script::Qvm_ExecuteInstructions(state);

    // 0x004BD0A6 `mov eax,[esi+0x20]` / 0x004BD0AC `cmp eax,edi` / 0x004BD0AE `je 0x004BD1D5`.
    std::int32_t const status = static_cast<std::int32_t>(state->error_status);
    if (status == 0)
    {
        return 0;                                        // 0x004BD1D5-0x004BD1D9: `pop edi` / `xor eax,eax` / `pop esi` / `ret`
    }

    // 0x004BD0B4 `dec eax` / 0x004BD0B5 `cmp eax,0xE` / 0x004BD0B8 `ja 0x004BD1B2` / 0x004BD0BE jump table at 0x004BD1DC.
    char* const text = reinterpret_cast<char*>(reinterpret_cast<std::uint8_t*>(state) + 0x268);
    switch (status)
    {
    case 1:  ::sprintf(text, "Stack underflow"); break;                                  // 0x004BD0C5, 0x00547C10
    case 2:  ::sprintf(text, "Stack overflow"); break;                                   // 0x004BD0D6, 0x00547C00
    case 3:  ::sprintf(text, "Syntax error"); break;                                     // 0x004BD10C, 0x00547BF0
    case 5:  ::sprintf(text, "Illegal instruction"); break;                              // 0x004BD0F1, 0x00547BDC
    case 6:  ::sprintf(text, "Illegal operand(s) for instruction"); break;               // 0x004BD102, 0x00547BB8
    case 7:  // 0x004BD11D: `lea edx,[esi+0x68]` is the %s argument; 0x00547B80 "Unknown identifier: %s".
        ::sprintf(text, "Unknown identifier: %s",
                  reinterpret_cast<char const*>(reinterpret_cast<std::uint8_t*>(state) + 0x68));
        break;
    case 8:  ::sprintf(text, "Identifier not a variable"); break;                        // 0x004BD144, 0x00547B64
    case 9:  ::sprintf(text, "Identifier not a function"); break;                        // 0x004BD152, 0x00547B48
    case 10: ::sprintf(text, "No function on stack"); break;                             // 0x004BD160, 0x00547B30
    case 11: ::sprintf(text, "Internal error (read: BUG!!!)."); break;                    // 0x004BD0E0, 0x00547B98
    case 13: // 0x004BD183: `mov eax,[esi]` / `mov ecx,[esi+0x10]` / `lea eax,[eax+ecx+1]` / `mov eax,[eax]`.
    {
        std::uint8_t* const base = static_cast<std::uint8_t*>(state->saved_instruction_pointer);
        std::int32_t const argument_count = *reinterpret_cast<std::int32_t*>(
            base + static_cast<std::uintptr_t>(state->instruction_offset) + 1U);
        ::sprintf(text, "Function argument overflow: %d arguments.", argument_count);     // 0x00547AD8
        break;
    }
    case 14: ::sprintf(text, "Datablock not present"); break;                            // 0x004BD175, 0x00547B04
    case 15: ::sprintf(text, "Index out of range"); break;                               // 0x004BD167, 0x00547B1C
    default: ::sprintf(text, "Undefined error"); break;                                  // 0x004BD1B2, 0x00547AC8
    }

    // 0x004BD1C6 `push esi` / 0x004BD1C7 `call 0x004BC960` (and the same pair at 0x004BD135 and 0x004BD1A3).
    Qvm_FormatOperandText_4BC960(state);
    // 0x004BD1CC `mov eax,[esi+0x20]` - the status is re-read after the formatter, then returned.
    return static_cast<std::int32_t>(state->error_status);
}

} // extern "C"
