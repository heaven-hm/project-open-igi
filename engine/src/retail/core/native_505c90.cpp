// Native: AIGraph_InitStep_505C90 (blessed address-suffixed placeholder; retail symbol not established); retail VA 0x00505C90-0x00505D26.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 151 bytes (0x97), 1 block, 0 edges,
//   42 instructions, counted by an ndisasm 3.01 linear sweep of the exact extent; the evidence list's 42 for this address agrees with the sweep, and the byte
//   accounting closes (42 instructions cover all 151 bytes). The `ret` is at 0x00505D26 and the byte after it is `0x90` padding, not part of this routine.
// Tools/evidence: ndisasm 3.01 (`-b32`) over the exact 151 bytes obtained through scripts/native_fidelity_audit.py RetailImage.va_to_off(); the auditor's
//   call_index() for called-vs-address-taken; the string and the globals' storage class read from the image. Complete extent:
//   0x00505C90 push 0; 0x00505C92 push 0; 0x00505C94 call 0x004018F0; 0x00505C99 push eax; 0x00505C9A push 0x20; 0x00505C9C push 0x0054E76C;
//   0x00505CA1 call 0x00401900; 0x00505CA6 push 0x00488700; 0x00505CAB push 1; 0x00505CAD push eax; 0x00505CAE mov [0x00A7A7BC],ax;
//   0x00505CB4 call 0x00401530; 0x00505CB9 add esp,0x20; 0x00505CBC push 0x004E0890; 0x00505CC1 call 0x004C56F0; 0x00505CC6 push eax;
//   0x00505CC7 mov ax,[0x00A7A7BC]; 0x00505CCD push eax; 0x00505CCE call 0x00401400; 0x00505CD3 add esp,0xC; 0x00505CD6 push 0x004879E0;
//   0x00505CDB call 0x004C5800; 0x00505CE0 mov cx,[0x00A7A7BC]; 0x00505CE7 push eax; 0x00505CE8 push ecx; 0x00505CE9 call 0x00401400;
//   0x00505CEE add esp,0xC; 0x00505CF1 push 0x00487E20; 0x00505CF6 call 0x004CEA20; 0x00505CFB mov dx,[0x00A7A7BC]; 0x00505D02 push eax;
//   0x00505D03 push edx; 0x00505D04 call 0x00401400; 0x00505D09 add esp,0xC; 0x00505D0C push 0x00487D90; 0x00505D11 call 0x004CEA10;
//   0x00505D16 push eax; 0x00505D17 mov ax,[0x00A7A7BC]; 0x00505D1D push eax; 0x00505D1E call 0x00401400; 0x00505D23 add esp,0xC; 0x00505D26 ret.
//   STACK ACCOUNTING, from the callee prologues rather than from the push count. The body makes 20 pushes and releases exactly 20 dwords: `add esp,0x20`
//   at 0x00505CB9 releases 8 (two ignored pushes to 0x004018F0 plus three for 0x00401900 plus three for 0x00401530), and each of the four `add esp,0xC`
//   releases 3, which are exactly the three pushes standing above it. 0x00401900 reads FOUR argument slots, so its fourth - original [esp+0x10] - is the
//   second of the two pushes made for 0x004018F0, still live at that point and holding 0; the fourth argument is therefore 0, not indeterminate, exactly
//   as at 0x0052CA70 and 0x00503D60. The push at 0x00505CBC (0x004E0890), at 0x00505CD6 (0x004879E0), at 0x00505CF1 (0x00487E20) and at 0x00505D0C
//   (0x00487D90) are NOT dead: each is the handler argument of the 0x00401400 call that follows the byte getter, pushed before that getter purely by
//   instruction scheduling (the deferred-push rule, three earlier instances). The two pushes to 0x004018F0 ARE genuinely ignored - that callee takes no
//   parameters - as are the four pushes to 0x004C56F0, 0x004C5800, 0x004CEA20 and 0x004CEA10, which are `mov al,[global]; ret` bodies that never read the stack.
//   PARTIAL REGISTER WRITES, preserved rather than tidied. All four byte getters are `mov al,[global]; ret`, so each writes ONLY AL and leaves the upper 24
//   bits of EAX as the previous call's return; the event word pushed at 0x00505CC6/0x00505CE7/0x00505D02/0x00505D16 is therefore
//   (previous_call_return & 0xFFFFFF00) | byte. The previous call is 0x00401530 for the first and the preceding 0x00401400 for the other three. The id is
//   likewise RE-READ from 0x00A7A7BC into ax, cx, dx and ax before its four uses rather than kept in a register, and that re-reading is preserved.
//   Callees, all transcribed: 0x004018F0 QTask_GetCurrentType; 0x00401900 TaskType_Register; 0x00401530 QTask_LinkQTaskEventHandlerNoContextFn;
//   0x004C56F0 DynObj_GetByte_4C56F0; 0x004C5800 DynObj_GetByte_4C5800; 0x00401400 QTask_LinkQTaskEventHandler (four sites); 0x004CEA20
//   QTask_PhaseIdGet548177_4CEA20; 0x004CEA10 QTask_PhaseIdGet548176_4CEA10.
//   HANDLERS - image scan by call_index(): 0x00488700, 0x004E0890, 0x004879E0, 0x00487E20 and 0x00487D90 are never the target of an `E8` in this image,
//   so all five are address-taken only and are passed as cast integers and deliberately NOT declared. 0x00488700 is already transcribed as
//   core/native_488700.cpp `Task_WalkAndDispatch_488700`, but retail pushes it as a plain code immediate and core/native_464260.cpp passes it the same way,
//   so it is a cast integer here too and not a symbol reference.
//   String read from the image: 0x0054E76C `Container`. The global 0x00A7A7BC is above the raw end 0x00552000, so it is zero fill; it is DEFINED by
//   sys/native_505d40.cpp (the argument-less getter that reads the same slot), so this file declares it extern to keep one definition per symbol.
// Behavior/CFG: no loop, no conditional branch - one straight-line block. It reads the current task type as a parent, registers the task type `Container` with
//   object size 0x20, that parent and 0, stores the returned 16-bit id at 0x00A7A7BC, then links five handlers on that id: event 1 with 0x00488700 through
//   the no-context link, and then four events through the context link with handlers 0x004E0890, 0x004879E0, 0x00487E20 and 0x00487D90, whose event words are
//   the four byte getters above. The id is RE-READ from 0x00A7A7BC before every link and the return of each link feeds the next event word, so the five links
//   are strictly ordered and that order is the entire contract of this routine.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the disassembly above; runtime UNOBSERVED. Recorded limits: (a) the name is the blessed
//   address-suffixed placeholder; `Container` names the registered task type, not this routine. (b) 0x00488700, 0x004E0890, 0x004879E0, 0x00487E20 and
//   0x00487D90 are address-taken only and deliberately undeclared; 0x004E0890, 0x004879E0, 0x00487E20 and 0x00487D90 are untranscribed and remain open frontier.
//   (c) 0x00A7A7BC is declared extern; sys/native_505d40.cpp owns its definition. (d) The six ignored-argument pushes are omitted from the generated code because
//   those callees take no parameters; that changes only dead stack writes below esp. (e) The string is passed as a byte-identical literal rather than as retail's
//   absolute address 0x0054E76C. (f) 0x004C56F0, 0x004C5800, 0x004CEA20 and 0x004CEA10 zero-extend their byte return in the port, so the upper 24 bits are
//   restored by hand from the previous call's return to reproduce retail's partial-register value; only the low byte of an event word reaches the handler
//   table index in 0x00401400 (it masks the word to its low byte), so this restoration changes no handler slot.
#include <cstdint>
#include <igi/retail/core/task_types.hpp>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00A7A7BC: the 16-bit task type id this routine writes at 0x00505CAE and re-reads four times. Defined by sys/native_505d40.cpp; declared here so there is one definition.
extern std::uint16_t g_a7a7bc;

// The four byte getters are `mov al,[global]; ret` bodies defined inside `extern "C"` in their own translation units; they have no headers.
std::uint8_t NATIVE_DECL DynObj_GetByte_4C56F0(void);             // 0x004C56F0: mov al,byte [0x00547F3E]; ret (dynobj/native_4c56f0.cpp)
std::uint8_t NATIVE_DECL DynObj_GetByte_4C5800(void);             // 0x004C5800: mov al,byte [0x00547F40]; ret (dynobj/native_4c5800.cpp)
std::uint8_t NATIVE_DECL QTask_PhaseIdGet548177_4CEA20(void);     // 0x004CEA20: mov al,byte [0x00548177]; ret (script/native_4cea20.cpp)
std::uint8_t NATIVE_DECL QTask_PhaseIdGet548176_4CEA10(void);     // 0x004CEA10: mov al,byte [0x00548176]; ret (script/native_4cea10.cpp)

void NATIVE_DECL AIGraph_InitStep_505C90()
{
    // 0x00505C90-0x00505CA1: read the current task type as the parent; the two pushes before the call are ignored by 0x004018F0.
    const std::uint16_t parent_type = QTask_GetCurrentType();

    // 0x00505C99-0x00505CAE: register "Container", size 0x20, that parent, and 0 as the fourth argument - supplied by the second ignored push.
    const std::uint16_t type_id = TaskType_Register("Container", 0x20U, parent_type, 0U);
    ::g_a7a7bc = type_id; // 0x00505CAE: mov [0x00A7A7BC],ax (the store lands between the third push and the call; only dead stack writes move)

    // 0x00505CA6-0x00505CB4: link event 1 with handler 0x00488700 on the id just returned. 0x00488700 is address-taken only.
    const std::uint32_t link_1_return = QTask_LinkQTaskEventHandlerNoContextFn(
        type_id,
        1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00488700U)));

    // 0x00505CBC-0x00505CCE: 0x004E0890 is pushed here but belongs to the call at 0x00505CCE as its handler (deferred push), and 0x004C56F0 ignores its
    // argument. 0x004C56F0 writes only AL, so the event word keeps the upper 24 bits of the previous call's return.
    const std::uint32_t event_word_1 =
        (link_1_return & 0xFFFFFF00U) | static_cast<std::uint32_t>(DynObj_GetByte_4C56F0());
    const std::uint16_t id_for_link_2 = ::g_a7a7bc; // 0x00505CC7: mov ax,[0x00A7A7BC]
    const std::uint32_t link_2_return = QTask_LinkQTaskEventHandler(
        id_for_link_2,
        event_word_1,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x004E0890U)));

    // 0x00505CD6-0x00505CE9: 0x004879E0 is the deferred handler of this link, and 0x004C5800 ignores its argument and writes only AL.
    const std::uint32_t event_word_2 =
        (link_2_return & 0xFFFFFF00U) | static_cast<std::uint32_t>(DynObj_GetByte_4C5800());
    const std::uint16_t id_for_link_3 = ::g_a7a7bc; // 0x00505CE0: mov cx,[0x00A7A7BC] (CX only: EAX above still holds the event word)
    const std::uint32_t link_3_return = QTask_LinkQTaskEventHandler(
        id_for_link_3,
        event_word_2,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x004879E0U)));

    // 0x00505CF1-0x00505D04: 0x00487E20 is the deferred handler of this link, and 0x004CEA20 ignores its argument and writes only AL.
    const std::uint32_t event_word_3 =
        (link_3_return & 0xFFFFFF00U) | static_cast<std::uint32_t>(QTask_PhaseIdGet548177_4CEA20());
    const std::uint16_t id_for_link_4 = ::g_a7a7bc; // 0x00505CFB: mov dx,[0x00A7A7BC] (DX only: EAX above still holds the event word)
    const std::uint32_t link_4_return = QTask_LinkQTaskEventHandler(
        id_for_link_4,
        event_word_3,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00487E20U)));

    // 0x00505D0C-0x00505D1E: 0x00487D90 is the deferred handler of this link, and 0x004CEA10 ignores its argument and writes only AL.
    const std::uint32_t event_word_4 =
        (link_4_return & 0xFFFFFF00U) | static_cast<std::uint32_t>(QTask_PhaseIdGet548176_4CEA10());
    const std::uint16_t id_for_link_5 = ::g_a7a7bc; // 0x00505D17: mov ax,[0x00A7A7BC]
    QTask_LinkQTaskEventHandler(
        id_for_link_5,
        event_word_4,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00487D90U)));
    // 0x00505D23: add esp,0xC releases the last three pushes; 0x00505D26: ret.
}

} // extern "C"

#undef NATIVE_DECL
