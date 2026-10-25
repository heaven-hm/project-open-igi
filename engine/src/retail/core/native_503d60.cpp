// Native: AIGraph_InitStep_503D60 (blessed address-suffixed placeholder; retail symbol not established); retail VA 0x00503D60-0x00503DE6.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 135 bytes (0x87), 1 block, 0 edges,
//   36 instructions, counted by an ndisasm 3.01 linear sweep of the exact extent (byte accounting: 36 instructions cover all 135 bytes). The `ret` is at
//   0x00503DE6 and the byte after it is `0x90` padding. radare2 `pd` on this extent returns only 34 instructions: it silently omits the two `mov cx`/`mov dx`
//   reads at 0x00503DAC and 0x00503DBA, which is why this file's listing is taken from ndisasm rather than from `pd`.
// Tools/evidence: ndisasm 3.01 (`-b32`) over the exact 135 bytes obtained through scripts/native_fidelity_audit.py RetailImage.va_to_off(); the auditor's
//   call_index() for called-vs-address-taken; the strings and the global's initial value read from the image. Complete extent:
//   0x00503D60 push 0; 0x00503D62 push 0; 0x00503D64 call 0x004FB5E0; 0x00503D69 push eax; 0x00503D6A push 0x768; 0x00503D6F push 0x0054E68C;
//   0x00503D74 call 0x00401900; 0x00503D79 push 0x004D9970; 0x00503D7E push 1; 0x00503D80 push eax; 0x00503D81 mov word [0x0054E66C],ax;
//   0x00503D87 call 0x00401530; 0x00503D8C mov ax,word [0x0054E66C]; 0x00503D92 push 0x00503DF0; 0x00503D97 push 0; 0x00503D99 push eax;
//   0x00503D9A call 0x00401400; 0x00503D9F add esp,0x2C; 0x00503DA2 push 0x00503F20; 0x00503DA7 call 0x004D9FB0;
//   0x00503DAC mov cx,word [0x0054E66C]; 0x00503DB3 push eax; 0x00503DB4 push ecx; 0x00503DB5 call 0x00401400;
//   0x00503DBA mov dx,word [0x0054E66C]; 0x00503DC1 push 0x00504250; 0x00503DC6 push 0xC; 0x00503DC8 push edx; 0x00503DC9 call 0x00401400;
//   0x00503DCE mov eax,dword [0x0054E66C]; 0x00503DD3 and eax,0xFFFF; 0x00503DD8 push eax; 0x00503DD9 push 0x0054E670; 0x00503DDE call 0x004B8810;
//   0x00503DE3 add esp,0x20; 0x00503DE6 ret.
//   STACK ACCOUNTING, derived from the callee prologues rather than assumed. The first `add esp,0x2C` releases 11 dwords = two ignored pushes to 0x004FB5E0
//   plus three for 0x00401900 plus three for 0x00401530 plus three for the first 0x00401400. The second `add esp,0x20` releases 8 dwords = the 0x00503F20 push
//   plus three for the second 0x00401400 plus three for the third plus two for 0x004B8810. The push of 0x00503F20 at 0x00503DA2 is therefore NOT dead: it is the
//   handler argument of the 0x00401400 call at 0x00503DB5 (the deferred-push rule). The pushes of 0 at 0x00503D60/0x00503D62 are genuinely ignored, because
//   0x004FB5E0 takes no parameters. The fourth argument of 0x00401900 is 0, supplied by the second of those two ignored pushes, exactly as in 0x0052CA70.
//   THE EVENT ID OF THE THIRD LINK IS NOT A CLEAN BYTE, and this is preserved rather than tidied: 0x004D9FB0 is `mov al,byte [0x0054864A]; ret`, so it writes
//   only AL and leaves the upper 24 bits of EAX as the previous call's return value. The push at 0x00503DB3 therefore passes
//   (return_of_0x00401400 & 0xFFFFFF00) | g_54864A. Modelling it as a plain widened byte would be wrong for every value whose upper bits are not already zero.
//   Callees, all transcribed: 0x004FB5E0 Sys_GetGlobal16_4FB5E0; 0x00401900 TaskType_Register; 0x00401530 QTask_LinkQTaskEventHandlerNoContextFn;
//   0x00401400 QTask_LinkQTaskEventHandler (three call sites); 0x004D9FB0 Sys_GetGlobal8_4D9FB0; 0x004B8810 Script_QHashNodeRegister.
//   IMAGE SCAN (call_index): 0x004D9970, 0x00503DF0, 0x00503F20 and 0x00504250 are never the target of an `E8` - all four are address-taken only, so all four are
//   passed as cast integers and deliberately NOT declared. Strings read from the image: 0x0054E68C `PhysicsMagicObj`, 0x0054E670 `TASKTYPE_PHYSICSMAGICOBJ`.
//   The global 0x0054E66C holds 0x0181 in the image and is already defined by sys/native_5042a0.cpp, so it is declared extern here to keep one definition per symbol.
// Behavior/CFG: no loop, no conditional branch - one straight-line block. It registers the task type `PhysicsMagicObj` with object size 0x768, stores the returned
//   id at 0x0054E66C, then links three events on that id (event 1 with handler 0x004D9970; event 0 with handler 0x00503DF0; the event id above with handler
//   0x00503F20), links a fourth with event 0xC and handler 0x00504250, and finally registers the script name `TASKTYPE_PHYSICSMAGICOBJ` against the id masked to
//   16 bits. The id is RE-READ from 0x0054E66C before the second, third and fourth links and masked from a dword read for the script registration; that re-reading
//   is preserved. The dword read followed by `and eax,0xFFFF` is equivalent to the 16-bit value, because the mask discards exactly the bits the narrower read omits.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the disassembly above; runtime UNOBSERVED. Recorded limits: (a) the name is the blessed address-suffixed
//   placeholder; the registered strings name the task type and script name, not this routine. (b) All four handler addresses are untranscribed and deliberately
//   undeclared - they are address-taken only. (c) 0x0054E66C is declared extern; sys/native_5042a0.cpp owns its definition. (d) The three ignored-argument pushes
//   are omitted from the generated code because those callees take no parameters; that changes only dead stack writes below esp. (e) The strings are passed as
//   byte-identical literals rather than as retail's absolute addresses.
#include <cstdint>
#include <igi/retail/core/task_types.hpp>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

// 0x0054E66C: the 16-bit task type id, holding 0x0181 in the image. Defined by sys/native_5042a0.cpp; declared here so there is one definition per symbol.
extern std::uint16_t g_54e66c;

extern "C" {

// The two getters are defined inside `extern "C"` in their own translation units and have no headers.
std::uint16_t NATIVE_DECL Sys_GetGlobal16_4FB5E0(void); // 0x004FB5E0 (sys/native_4fb5e0.cpp); takes no parameters, so the two pushes before it are ignored
std::uint8_t NATIVE_DECL Sys_GetGlobal8_4D9FB0(void);   // 0x004D9FB0: `mov al,byte [0x0054864A]; ret` - writes only AL (sys/native_4d9fb0.cpp)

// 0x004B8810: two-argument script hash-node registrar (script/native_4b8810.cpp, inside `extern "C"`).
void Script_QHashNodeRegister(void* arg1, void* value);

void NATIVE_DECL AIGraph_InitStep_503D60()
{
    // 0x00503D60-0x00503D74: read the parent type; the two pushes before the call are ignored by 0x004FB5E0.
    const std::uint16_t parent_type = Sys_GetGlobal16_4FB5E0();

    // 0x00503D69-0x00503D81: register "PhysicsMagicObj", size 0x768, that parent, and 0 as the fourth argument - supplied by the second ignored push.
    const std::uint16_t type_id = TaskType_Register("PhysicsMagicObj", 0x768U, parent_type, 0U);
    ::g_54e66c = type_id; // 0x00503D81: mov word [0x0054E66C],ax

    // 0x00503D79-0x00503D87: link event 1 with handler 0x004D9970 on the id just returned.
    QTask_LinkQTaskEventHandlerNoContextFn(
        type_id,
        1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4D9970U)));

    // 0x00503D8C-0x00503D9A: the id is re-read from memory, then event 0 is linked with handler 0x00503DF0.
    const std::uint16_t id_for_second = ::g_54e66c; // 0x00503D8C: mov ax,word [0x0054E66C]
    const std::uint32_t second_return = QTask_LinkQTaskEventHandler(
        id_for_second,
        0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x503DF0U)));

    // 0x00503DA2-0x00503DB5: 0x00503F20 is pushed here but belongs to the call at 0x00503DB5 as its handler (deferred push), and 0x004D9FB0 ignores its argument.
    const std::uint8_t physics_byte = Sys_GetGlobal8_4D9FB0();

    // 0x00503DAC: the id is re-read again. 0x004D9FB0 wrote only AL, so the event id keeps the upper 24 bits of the previous call's return value.
    const std::uint16_t id_for_third = ::g_54e66c;
    const std::uint32_t third_event_id = (second_return & 0xFFFFFF00U) | static_cast<std::uint32_t>(physics_byte);
    QTask_LinkQTaskEventHandler(
        id_for_third,
        third_event_id,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x503F20U)));

    // 0x00503DBA-0x00503DC9: the id is re-read into dx, then event 0xC is linked with handler 0x00504250.
    const std::uint16_t id_for_fourth = ::g_54e66c; // 0x00503DBA: mov dx,word [0x0054E66C]
    QTask_LinkQTaskEventHandler(
        id_for_fourth,
        0xCU,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x504250U)));

    // 0x00503DCE-0x00503DDE: the id is read as a dword and masked to 16 bits, then registered under the script name.
    const std::uint32_t masked_id = static_cast<std::uint32_t>(::g_54e66c) & 0xFFFFU;
    Script_QHashNodeRegister(
        const_cast<char*>("TASKTYPE_PHYSICSMAGICOBJ"),
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(masked_id)));
}

} // extern "C"

#undef NATIVE_DECL
