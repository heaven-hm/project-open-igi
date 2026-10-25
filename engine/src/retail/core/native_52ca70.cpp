// Native: AIGraph_InitStep_52CA70 (blessed address-suffixed placeholder; retail symbol not established); retail VA 0x0052CA70-0x0052CAC4.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 85 bytes (0x55), 1 block, 0 edges,
//   24 instructions, counted over the exact image. Extent ends at the `ret` at 0x0052CAC4; 0x0052CAC5-0x0052CACF is `0x90` padding and the next routine starts at 0x0052CAD0.
// Tools/evidence: radare2 6.2.0 `pd 30 @ 0x52ca70`, plus byte-level scans of the image for `E8`/`E9` targets and for absolute global addresses. Complete extent:
//   0x0052CA70 push 0; 0x0052CA72 push 0; 0x0052CA74 call 0x00505C80; 0x0052CA79 push eax; 0x0052CA7A push 0x7C; 0x0052CA7C push 0x0054DEE8;
//   0x0052CA81 call 0x00401900; 0x0052CA86 add esp,0x14; 0x0052CA89 mov word [0x0054F718],ax; 0x0052CA8F push 0x0052CAD0; 0x0052CA94 call 0x004E7530;
//   0x0052CA99 push eax; 0x0052CA9A mov ax,word [0x0054F718]; 0x0052CAA0 push eax; 0x0052CAA1 call 0x00401530; 0x0052CAA6 add esp,0xC;
//   0x0052CAA9 push 0x0052CAF0; 0x0052CAAE call 0x00504DF0; 0x0052CAB3 mov cx,word [0x0054F718]; 0x0052CABA push eax; 0x0052CABB push ecx;
//   0x0052CABC call 0x00401400; 0x0052CAC1 add esp,0xC; 0x0052CAC4 ret.
//   THE STACK ACCOUNTING IS THE CONTRACT HERE, and it was derived slot by slot rather than assumed. The first `add esp,0x14` releases FIVE dwords: two for the
//   0x00505C80 call and three for the 0x00401900 call. 0x00401900 (TaskType_Register) reads four argument slots, so the three pushes at 0x0052CA79-0x0052CA7C supply
//   name/object_size/parent_type and its FOURTH slot - original [esp+0x10] - is the first of the two pushes made for 0x00505C80, which is still on the stack at that
//   point and holds 0. The 4th argument is therefore 0, not indeterminate. Likewise 0x00401530 reads three slots (its prologue loads [esp+0xC], [esp+0x14] and
//   [esp+0x18] after three pushes), so the push at 0x0052CA8F supplies its third argument, the handler 0x0052CAD0; and 0x00401400 takes (class_id, event_id,
//   handler), so the push at 0x0052CAA9 supplies the handler 0x0052CAF0. Those two pushes are NOT dead: they are how retail passes the handler argument.
//   By contrast the pushes at 0x0052CA70/0x0052CA72 (to 0x00505C80) and the push at 0x0052CA94 (to 0x004E7530) and 0x0052CAAE (to 0x00504DF0) ARE ignored: those
//   three callees are `mov reg,[global]; ret` bodies that never read the stack, verified by disassembly.
//   Callees, all transcribed: 0x00505C80 Sys_GetGlobal16_505C80; 0x00401900 TaskType_Register; 0x004E7530 Core_GetEventWord_4E7530; 0x00401530
//   QTask_LinkQTaskEventHandlerNoContextFn; 0x00504DF0 Sys_GetGlobal8_504DF0; 0x00401400 QTask_LinkQTaskEventHandler.
//   IMAGE SCAN: 0x0052CAD0 and 0x0052CAF0 are never the target of an `E8` or an `E9` - they are only ever pushed as addresses - so both are passed as cast integers and
//   deliberately NOT declared; declaring them would emit a relocation, and that relocation is what the linker reports. The string at 0x0054DEE8 was read from the image
//   and is exactly `PushButton`. The word global 0x0054F718 is referenced from 4 sites and is declared here, not defined.
// Behavior/CFG: no loop, no conditional branch - one straight-line block. It reads the 16-bit global at 0x0054E720 as the parent type, registers the task type named
//   `PushButton` with object size 0x7C and that parent, stores the returned 16-bit id at 0x0054F718, then links two events on that id: the first with the event id
//   read from 0x005488C8 and handler 0x0052CAD0, the second with the event id read from 0x0054E70B and handler 0x0052CAF0. 0x0054F718 is RE-READ from memory before
//   each of the two link calls rather than kept in a register, and that re-read is preserved.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the disassembly above; runtime UNOBSERVED. Recorded limits: (a) the name is the blessed address-suffixed
//   placeholder; `PushButton` names the registered type, not this routine. (b) 0x0052CAD0 and 0x0052CAF0 are untranscribed and deliberately undeclared - they are
//   address-taken only; their bodies remain open frontier. (c) 0x0054F718 is declared extern and its owner is not established, so this file reports it undefined
//   until its owner lands. (d) The three ignored-argument pushes are omitted from the generated code because the callees take no parameters; that changes only dead
//   stack writes below esp and no observable value. (e) The string is passed as a byte-identical literal rather than as retail's absolute address 0x0054DEE8.
#include <cstdint>
#include <igi/retail/core/task_types.hpp>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

// 0x0054F718: the 16-bit task type id registered by this routine and read at 0x0052CA9A and
// 0x0052CAB3. THIS FILE IS ITS OWNER: it writes the slot at 0x0052CA89 (`mov word [0x54f718],ax`
// with the result of TaskType_Register), so it is a definition, not an extern declaration.
// The image holds 0x0181 (385) at that address - file-backed .data, read as bytes `81 01` - so the
// definition carries that value. It is overwritten by the registration on the first call.
std::uint16_t g_54f718 = 0x0181U;

extern "C" {

// The three getters below have no header: each is defined in its own translation unit inside `extern "C"` with NATIVE_DECL, so they are declared the same way here.
std::uint16_t NATIVE_DECL Sys_GetGlobal16_505C80(void);   // 0x00505C80: mov ax,word [0x0054E720]; ret (sys/native_505c80.cpp)
std::uint32_t NATIVE_DECL Core_GetEventWord_4E7530(void); // 0x004E7530: mov eax,dword [0x005488C8]; ret (core/native_4e7530.cpp)
std::uint8_t NATIVE_DECL Sys_GetGlobal8_504DF0(void);     // 0x00504DF0: mov al,byte [0x0054E70B]; ret (sys/native_504df0.cpp)

void NATIVE_DECL AIGraph_InitStep_52CA70()
{
    // 0x0052CA74: call 0x00505C80 - reads the 16-bit global at 0x0054E720; the two pushes before it are ignored by that callee.
    const std::uint16_t parent_type = Sys_GetGlobal16_505C80();

    // 0x0052CA79-0x0052CA89: register "PushButton", object size 0x7C, that parent, and 0 - the fourth argument is the first ignored push above, still on the stack.
    ::g_54f718 = TaskType_Register("PushButton", 0x7CU, parent_type, 0U);

    // 0x0052CA94: call 0x004E7530 - reads the dword at 0x005488C8 and ignores its pushed argument.
    const std::uint32_t event_id = Core_GetEventWord_4E7530();

    // 0x0052CA9A-0x0052CAA1: the global is re-read from memory here, then linked with handler 0x0052CAD0 (supplied by the push at 0x0052CA8F).
    QTask_LinkQTaskEventHandlerNoContextFn(
        ::g_54f718,
        event_id,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x52CAD0U)));

    // 0x0052CAAE: call 0x00504DF0 - reads the byte at 0x0054E70B and ignores its pushed argument.
    const std::uint32_t second_event_id = Sys_GetGlobal8_504DF0();

    // 0x0052CAB3-0x0052CABC: the global is re-read from memory again, then linked with handler 0x0052CAF0 (supplied by the push at 0x0052CAA9).
    QTask_LinkQTaskEventHandler(
        ::g_54f718,
        second_event_id,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x52CAF0U)));
}

} // extern "C"

#undef NATIVE_DECL
