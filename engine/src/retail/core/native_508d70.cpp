// Native: AIGraph_InitStep_508D70 (blessed address-suffixed placeholder; retail symbol not established); retail VA 0x00508D70-0x00508DFF.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 144 bytes (0x90), 1 block, 0 edges,
//   38 instructions, counted by an ndisasm 3.01 linear sweep of the exact extent; the evidence list's 38 for this address agrees with the sweep, and the byte
//   accounting closes (38 instructions over all 144 bytes). The `ret` is at 0x00508DFF and the byte after it is padding.
// Tools/evidence: ndisasm 3.01 (`-b32`) over the exact 144 bytes obtained through scripts/native_fidelity_audit.py RetailImage.va_to_off(); the auditor's
//   call_index() for called-vs-address-taken; the string and the globals' initial values read from the image. Complete extent:
//   0x00508D70 push 0; 0x00508D72 push 0; 0x00508D74 call 0x004018F0; 0x00508D79 push eax; 0x00508D7A push 0x994; 0x00508D7F push 0x0054E834;
//   0x00508D84 call 0x00401900; 0x00508D89 push 0; 0x00508D8B mov [0x00A7B298],ax; 0x00508D91 call 0x004017C0; 0x00508D96 push 0;
//   0x00508D98 mov [0x0054E830],al; 0x00508D9D call 0x004017C0; 0x00508DA2 mov [0x0054E831],al; 0x00508DA7 mov ax,[0x00A7B298];
//   0x00508DAD push 0x00508E00; 0x00508DB2 push 0; 0x00508DB4 push eax; 0x00508DB5 call 0x00401400; 0x00508DBA mov cx,[0x00A7B298];
//   0x00508DC1 push 0x004D3710; 0x00508DC6 push 1; 0x00508DC8 push ecx; 0x00508DC9 call 0x00401530; 0x00508DCE mov dx,[0x00A7B298];
//   0x00508DD5 push 0x00508E30; 0x00508DDA push 2; 0x00508DDC push edx; 0x00508DDD call 0x00401530; 0x00508DE2 add esp,0x40;
//   0x00508DE5 push 0x00508E40; 0x00508DEA call 0x004E7530; 0x00508DEF push eax; 0x00508DF0 mov ax,[0x00A7B298]; 0x00508DF6 push eax;
//   0x00508DF7 call 0x00401530; 0x00508DFC add esp,0xC; 0x00508DFF ret.
//   STACK ACCOUNTING, from the callee prologues. The first `add esp,0x40` releases 16 dwords: two ignored pushes to 0x004018F0, three for 0x00401900, one for the
//   first 0x004017C0, one for the second 0x004017C0, three for the first 0x00401400, three for the first 0x00401530, three for the second 0x00401530. The second
//   `add esp,0xC` releases 3 dwords: the 0x00508E40 push, the 0x004E7530 return push, and the third 0x00401530's argument. The push of 0x00508E40 at 0x00508DE5
//   is therefore NOT dead: it is the handler of the 0x00401530 call at 0x00508DF7 (the deferred-push rule, the same pattern as 0x0052CA70 and 0x00503D60). The
//   pushes of 0 at 0x00508D70/0x00508D72 are genuinely ignored because 0x004018F0 takes no parameters, and the fourth argument of 0x00401900 is 0, supplied by the
//   second of those ignored pushes.
//   Callees: 0x004018F0 QTask_GetCurrentType; 0x00401900 TaskType_Register; 0x004017C0 QTask_AllocateEvent (two sites); 0x00401400 QTask_LinkQTaskEventHandler;
//   0x00401530 QTask_LinkQTaskEventHandlerNoContextFn (three sites); 0x004E7530 Core_GetEventWord_4E7530. All are transcribed.
//   HANDLERS - the four differ in kind, so they are not treated alike. 0x00508E00, 0x00508E30 and 0x00508E40 are never the target of an `E8` anywhere in the image
//   (call_index), so all three are passed as cast integers and deliberately NOT declared. 0x004D3710 IS called - it is QTask_NoOpHandler, a real transcribed
//   function - so it is declared and referenced as a symbol, exactly as core/native_427d90.cpp does, and it is a real link edge rather than a cast integer.
//   String read from the image: 0x0054E834 `Sync`. Globals: 0x00A7B298 is above the raw end 0x00552000, so it is zero fill and its definition carries 0; 0x0054E830
//   and 0x0054E831 are file-backed bytes holding 0xFF each in the image, and their definitions carry that value. None of the three is declared anywhere else in
//   the tree, so this file defines all three.
// Behavior/CFG: no loop, no conditional branch - one straight-line block. It reads the current task type as a parent, registers the task type `Sync` with object
//   size 0x994 and that parent, stores the returned id at 0x00A7B298, allocates two events keeping only the low byte of each, then links four handlers on the id:
//   event 0 with 0x00508E00, event 1 with QTask_NoOpHandler, event 2 with 0x00508E30, and the event word from 0x004E7530 with 0x00508E40. The id is RE-READ from
//   0x00A7B298 into ax, cx, dx and ax again before each use rather than kept in a register, and that re-reading is preserved.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the disassembly above; runtime UNOBSERVED. Recorded limits: (a) the name is the blessed address-suffixed
//   placeholder; `Sync` names the registered task type, not this routine. (b) 0x00508E00, 0x00508E30 and 0x00508E40 are untranscribed and deliberately undeclared -
//   they are address-taken only and remain open frontier. (c) The three ignored-argument pushes are omitted from the generated code because those callees take no
//   parameters; that changes only dead stack writes below esp. (d) The string is passed as a byte-identical literal rather than as retail's address 0x0054E834.
#include <cstdint>
#include <igi/retail/core/task_types.hpp>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

// 0x00A7B298 is zero fill (above the raw end of .data); 0x0054E830 and 0x0054E831 are file-backed bytes holding 0xFF. Defined here: no other file declares them.
std::uint16_t g_a7b298 = 0U;
std::uint8_t g_54e830 = 0xFFU;
std::uint8_t g_54e831 = 0xFFU;

extern "C" {

// 0x004E7530: argument-less dword getter, defined inside `extern "C"` in its own translation unit; it has no header.
std::uint32_t NATIVE_DECL Core_GetEventWord_4E7530(void); // 0x004E7530: mov eax,dword [0x005488C8]; ret

// 0x004D3710: a real transcribed function used as a handler (single `ret`), declared as core/native_427d90.cpp declares it.
void NATIVE_DECL QTask_NoOpHandler(void);

void NATIVE_DECL AIGraph_InitStep_508D70()
{
    // 0x00508D70-0x00508D84: read the current task type as the parent; the two pushes before the call are ignored by 0x004018F0.
    const std::uint16_t parent_type = QTask_GetCurrentType();

    // 0x00508D79-0x00508D8B: register "Sync", size 0x994, that parent, and 0 as the fourth argument - supplied by the second ignored push.
    const std::uint16_t type_id = TaskType_Register("Sync", 0x994U, parent_type, 0U);
    ::g_a7b298 = type_id; // 0x00508D8B: mov [0x00A7B298],ax

    // 0x00508D91-0x00508D98: allocate an event with argument 0; the low byte of the result goes to 0x0054E830.
    ::g_54e830 = static_cast<std::uint8_t>(QTask_AllocateEvent(0U));

    // 0x00508D9D-0x00508DA2: allocate an event with argument 0; the low byte of the result goes to 0x0054E831.
    ::g_54e831 = static_cast<std::uint8_t>(QTask_AllocateEvent(0U));

    // 0x00508DA7-0x00508DB5: the id is re-read from memory, then event 0 is linked with handler 0x00508E00.
    QTask_LinkQTaskEventHandler(
        ::g_a7b298, // 0x00508DA7: mov ax,[0x00A7B298]
        0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00508E00));

    // 0x00508DBA-0x00508DC9: the id is re-read into cx, then event 1 is linked with the real handler QTask_NoOpHandler.
    QTask_LinkQTaskEventHandlerNoContextFn(
        ::g_a7b298, // 0x00508DBA: mov cx,[0x00A7B298]
        1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(&QTask_NoOpHandler));

    // 0x00508DCE-0x00508DDD: the id is re-read into dx, then event 2 is linked with handler 0x00508E30.
    QTask_LinkQTaskEventHandlerNoContextFn(
        ::g_a7b298, // 0x00508DCE: mov dx,[0x00A7B298]
        2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00508E30));

    // 0x00508DE5-0x00508DF7: 0x00508E40 is pushed here but belongs to the call at 0x00508DF7 as its handler (deferred push), and 0x004E7530 ignores its argument.
    const std::uint32_t event_word = Core_GetEventWord_4E7530();
    QTask_LinkQTaskEventHandlerNoContextFn(
        ::g_a7b298, // 0x00508DF0: mov ax,[0x00A7B298]
        event_word,
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00508E40));
}

} // extern "C"

#undef NATIVE_DECL
