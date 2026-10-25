// Native: AIGraph_InitStep_51B400 (blessed address-suffixed placeholder; retail symbol not established); retail VA 0x0051B400-0x0051B4A6.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 167 bytes (0xA7), 1 block, 0 edges,
//   45 instructions, counted by an ndisasm 3.01 linear sweep of the exact extent; the evidence list's 45 for this address agrees with the sweep, and the byte
//   accounting closes (45 instructions cover all 167 bytes). The `ret` is at 0x0051B4A6 and the byte after it is `0x90` padding, not part of this routine.
// Tools/evidence: ndisasm 3.01 (`-b32`) over the exact 167 bytes obtained through scripts/native_fidelity_audit.py RetailImage.va_to_off(); the strings and the
//   global's storage class read from the image. Complete extent:
//   0x0051B400 push 0; 0x0051B402 push 0; 0x0051B404 call 0x004018F0; 0x0051B409 push eax; 0x0051B40A push 4; 0x0051B40C push 0x0054EDE8;
//   0x0051B411 call 0x00401900; 0x0051B416 push 0x004C1780; 0x0051B41B push 0; 0x0051B41D push eax; 0x0051B41E mov [0x00A83764],ax;
//   0x0051B424 call 0x00401400; 0x0051B429 mov ax,[0x00A83764]; 0x0051B42F push 0x004C1780; 0x0051B434 push 2; 0x0051B436 push eax;
//   0x0051B437 call 0x00401530; 0x0051B43C add esp,0x2C; 0x0051B43F push 0x0051B4B0; 0x0051B444 call 0x004EC060; 0x0051B449 mov cx,[0x00A83764];
//   0x0051B450 push eax; 0x0051B451 push ecx; 0x0051B452 call 0x00401400; 0x0051B457 add esp,0xC; 0x0051B45A push 0x0051B4E0;
//   0x0051B45F call 0x004EC050; 0x0051B464 mov dx,[0x00A83764]; 0x0051B46B push eax; 0x0051B46C push edx; 0x0051B46D call 0x00401400;
//   0x0051B472 add esp,0xC; 0x0051B475 push 0x0051B4F0; 0x0051B47A call 0x004EC040; 0x0051B47F push eax; 0x0051B480 mov ax,[0x00A83764];
//   0x0051B486 push eax; 0x0051B487 call 0x00401400; 0x0051B48C mov ecx,[0x00A83764]; 0x0051B492 and ecx,0xFFFF; 0x0051B498 push ecx;
//   0x0051B499 push 0x0054EDD4; 0x0051B49E call 0x004B8810; 0x0051B4A3 add esp,0x14; 0x0051B4A6 ret.
//   STACK ACCOUNTING, from the callee prologues rather than from the push count. The body makes 22 pushes and releases exactly 22 dwords: `add esp,0x2C`
//   at 0x0051B43C releases 11 (two ignored pushes to 0x004018F0 plus three for 0x00401900 plus three for each of the first two links); `add esp,0xC` at
//   0x0051B457 and at 0x0051B472 release 3 each; and the final `add esp,0x14` at 0x0051B4A3 releases 5, folding the third link's three pushes together with
//   the two pushes of 0x004B8810 (there is NO separate cleanup after the third 0x00401400 call). 11 + 3 + 3 + 5 = 22. 0x00401900 reads FOUR argument slots,
//   so its fourth is the second of the two pushes made for 0x004018F0, still live and holding 0 - the same shape as 0x0052CA70, 0x00503D60 and 0x00505C90.
//   The pushes at 0x0051B43F (0x0051B4B0), 0x0051B45A (0x0051B4E0) and 0x0051B475 (0x0051B4F0) are NOT dead: each is the handler argument of the
//   0x00401400 call that follows the byte getter, pushed before that getter purely by instruction scheduling (the deferred-push rule). The two pushes to
//   0x004018F0 ARE genuinely ignored - that callee takes no parameters - and so are the three pushes to 0x004EC060/0x004EC050/0x004EC040, which are
//   6-byte `mov al,[global]; ret` bodies that never read the stack (0x004EC060 `mov al,[0x0054DB82]`, 0x004EC050 `mov al,[0x0054DB81]`,
//   0x004EC040 `mov al,[0x0054DB80]`; all three verified by ndisasm over their own 6-byte extents).
//   CALLED vs ADDRESS-TAKEN, decided by an E8-at-instruction-boundary decode. scripts/native_fidelity_audit.py call_index() MISSES the three calls in this
//   body: its single whole-.text linear sweep desyncs, and 0x0051B444, 0x0051B45F and 0x0051B47A are not even instruction boundaries in it, so it reports
//   0x004EC040/0x004EC050/0x004EC060 as address-taken only. They ARE called, and 0x004C1780 is called from 162 sites. A raw `E8`-target scan is conclusive
//   in the other direction: 0x0051B4B0, 0x0051B4E0 and 0x0051B4F0 have NO `E8` byte anywhere in the image targeting them, so they are certainly
//   address-taken only and are passed as cast integers and deliberately NOT declared.
//   0x004C1780 is AnimSound_LoadParameters, a real transcribed function AND a real call edge, so it is declared and referenced as a symbol rather than as a
//   cast integer - the same treatment 0x004D3710 gets in core/native_508d70.cpp. 0x004EC040, 0x004EC050 and 0x004EC060 are called but NOT transcribed, so
//   they are declared here and remain undefined until their own natives land: that is the true state of the frontier, not a defect in this file.
//   PARTIAL REGISTER WRITES, preserved rather than tidied: the three 0x004EC0xx getters write ONLY AL, so each event word pushed is
//   (previous_call_return & 0xFFFFFF00) | byte. The previous call is 0x00401530 for the first, and the preceding 0x00401400 for the second and third.
//   The id is RE-READ from 0x00A83764 into ax, cx, dx, ax and then as a masked dword before its five uses rather than kept in a register.
//   String read from the image: 0x0054EDE8 `AnimSound`; 0x0054EDD4 `TASKTYPE_ANIMSOUND`. The global 0x00A83764 is above the raw end 0x00552000, so it is
//   zero fill, and no other file in the tree owns it, so this file defines it carrying 0.
// Behavior/CFG: no loop, no conditional branch - one straight-line block. It reads the current task type as a parent, registers the task type `AnimSound` with
//   object size 4, that parent and 0, stores the returned 16-bit id at 0x00A83764, then links five handlers on that id: event 0 with 0x004C1780 through the
//   context link, event 2 with 0x004C1780 through the no-context link, then three more through the context link with handlers 0x0051B4B0, 0x0051B4E0 and
//   0x0051B4F0, whose event words are the three 0x004EC0xx byte getters. Finally it registers the script name `TASKTYPE_ANIMSOUND` against the id masked to 16
//   bits. The five links are strictly ordered because each link's return feeds the next event word.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the disassembly above; runtime UNOBSERVED. Recorded limits: (a) the name is the blessed
//   address-suffixed placeholder; `AnimSound` names the registered task type, not this routine. (b) 0x0051B4B0, 0x0051B4E0 and 0x0051B4F0 are address-taken
//   only and deliberately undeclared; all three are untranscribed and remain open frontier. (c) 0x004EC040, 0x004EC050 and 0x004EC060 are called, declared
//   and NOT yet transcribed, so this file reports them undefined. (d) 0x00A83764 is defined here because no other file owns it. (e) The five ignored-argument
//   pushes are omitted from the generated code because those callees take no parameters; that changes only dead stack writes below esp. (f) The strings are
//   passed as byte-identical literals rather than as retail's absolute addresses 0x0054EDE8 and 0x0054EDD4. (g) The return of the 0x00401400 call at
//   0x0051B424 is not carried: retail re-reads the id into ax at 0x0051B429, which leaves only the upper 16 bits of that return in EAX and only the low 16
//   bits of the class id reach 0x00401400, so the value is unobservable. (h) 0x004EC040/0x004EC050/0x004EC060 zero-extend their byte return in the port, so
//   the upper 24 bits are restored by hand from the previous call's return to reproduce retail's partial-register value; only the low byte of an event word
//   reaches the handler table index in 0x00401400 (it masks the word to its low byte), so this restoration changes no handler slot.
#include <cstdint>
#include <igi/retail/core/native_4c1780.hpp>
#include <igi/retail/core/task_types.hpp>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

// 0x00A83764: the 16-bit task type id registered by this routine. Above the raw end 0x00552000, so zero fill; no other file in the tree owns it.
std::uint16_t g_a83764 = 0U;

extern "C" {

// 0x004EC040/0x004EC050/0x004EC060: three 6-byte byte getters that ARE called from this body and are not transcribed yet. Declared so the call edges are real;
// they stay undefined in the tree until their own natives land. Each body is `mov al,[global]; ret`:
//   0x004EC060: mov al,byte [0x0054DB82]   0x004EC050: mov al,byte [0x0054DB81]   0x004EC040: mov al,byte [0x0054DB80]
std::uint8_t NATIVE_DECL Sub_4EC060(void);
std::uint8_t NATIVE_DECL Sub_4EC050(void);
std::uint8_t NATIVE_DECL Sub_4EC040(void);

// 0x004B8810: two-argument script hash-node registrar (script/native_4b8810.cpp, inside `extern "C"`).
void Script_QHashNodeRegister(void* arg1, void* value);

void NATIVE_DECL AIGraph_InitStep_51B400()
{
    // 0x0051B400-0x0051B411: read the current task type as the parent; the two pushes before the call are ignored by 0x004018F0.
    const std::uint16_t parent_type = QTask_GetCurrentType();

    // 0x0051B409-0x0051B41E: register "AnimSound", size 4, that parent, and 0 as the fourth argument - supplied by the second ignored push.
    const std::uint16_t type_id = TaskType_Register("AnimSound", 0x4U, parent_type, 0U);
    ::g_a83764 = type_id; // 0x0051B41E: mov [0x00A83764],ax (the store lands between the third push and the call; only dead stack writes move)

    // 0x0051B416-0x0051B424: link event 0 with handler 0x004C1780. That address IS called from 162 sites, so it is a real link edge and is referenced as a symbol.
    QTask_LinkQTaskEventHandler(
        type_id,
        0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(&AnimSound_LoadParameters));

    // 0x0051B429-0x0051B437: the id is re-read from memory, then event 2 is linked with the same handler through the no-context variant.
    const std::uint16_t id_for_second = ::g_a83764; // 0x0051B429: mov ax,[0x00A83764]
    const std::uint32_t link_2_return = QTask_LinkQTaskEventHandlerNoContextFn(
        id_for_second,
        2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(&AnimSound_LoadParameters));

    // 0x0051B43F-0x0051B452: 0x0051B4B0 is pushed here but belongs to the call at 0x0051B452 as its handler (deferred push), and 0x004EC060 ignores its
    // argument. 0x004EC060 writes only AL, so the event word keeps the upper 24 bits of the previous call's return.
    const std::uint32_t event_word_1 =
        (link_2_return & 0xFFFFFF00U) | static_cast<std::uint32_t>(Sub_4EC060());
    const std::uint16_t id_for_third = ::g_a83764; // 0x0051B449: mov cx,[0x00A83764]
    const std::uint32_t link_3_return = QTask_LinkQTaskEventHandler(
        id_for_third,
        event_word_1,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0051B4B0U)));

    // 0x0051B45A-0x0051B46D: 0x0051B4E0 is the deferred handler of this link, and 0x004EC050 ignores its argument and writes only AL.
    const std::uint32_t event_word_2 =
        (link_3_return & 0xFFFFFF00U) | static_cast<std::uint32_t>(Sub_4EC050());
    const std::uint16_t id_for_fourth = ::g_a83764; // 0x0051B464: mov dx,[0x00A83764]
    const std::uint32_t link_4_return = QTask_LinkQTaskEventHandler(
        id_for_fourth,
        event_word_2,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0051B4E0U)));

    // 0x0051B475-0x0051B487: 0x0051B4F0 is the deferred handler of this link, and 0x004EC040 ignores its argument and writes only AL.
    const std::uint32_t event_word_3 =
        (link_4_return & 0xFFFFFF00U) | static_cast<std::uint32_t>(Sub_4EC040());
    const std::uint16_t id_for_fifth = ::g_a83764; // 0x0051B480: mov ax,[0x00A83764]
    QTask_LinkQTaskEventHandler(
        id_for_fifth,
        event_word_3,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0051B4F0U)));

    // 0x0051B48C-0x0051B49E: the id is read as a dword and masked to 16 bits, then registered under the script name. The final add esp,0x14 at 0x0051B4A3
    // releases this block's five pushes: 0x0051B4F0, the two above, the masked id and the string.
    const std::uint32_t masked_id = static_cast<std::uint32_t>(::g_a83764) & 0xFFFFU;
    Script_QHashNodeRegister(
        const_cast<char*>("TASKTYPE_ANIMSOUND"),
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(masked_id)));
}

} // extern "C"

#undef NATIVE_DECL
