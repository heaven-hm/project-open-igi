// Native: Task_DefineBinocular (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00470610-0x00470759.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 330 bytes, 1 block, 87 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 87 instructions / 330 bytes / 0 edges) plus
//   the objdump listing build/retail-dis.txt for the same extent (0x00470610-0x00470759; the ret is at 0x00470759, six nops follow, and
//   0x00470760 is the next function, which this body uses as a handler); `ps` at 0x0053FF6C and 0x0053FF58 for the two literals (byte-identical
//   to "Binocular" and "TASKTYPE_BINOCULAR"); `pxw` at 0x005BE33C for the image value; a full stack-pointer simulation over all 87
//   instructions, which returns esp to its entry value at the ret and accounts for every cleanup (0x2c = 11 pushes for the two inert arguments,
//   the registration and the first two links; 0xc seven times; 0x14 once = the last link's three pushes plus the tail's two). Every callee was
//   re-read at its own VA. Argument numbers were derived by counting the pushes above each access, not by reading an [esp+N] offset as an
//   argument number.
// Behavior/CFG: one straight-line body that registers the "Binocular" task type of size 0x1C8 with parent Sys_GetGlobal16_477BF0() -> id at
//   0x005BE33C, then wires eleven handler links: e0->0x00470760 (0x00401400), e2->0x004707D0 (0x00401530), then the event ids from
//   0x004F1FE0->0x004754C0, 0x00460BF0->0x00470890, 0x00460C10->0x004D3710, 0x00487190->0x00470BC0, 0x004F1A70->0x00470C80,
//   0x00413B40->0x00470CE0, 0x00413B30->0x0051BAA0, 0x004F2030->0x00470840 (0x00401530) and 0x004071D0->0x00470D70, and finishes with the
//   script-name tail ("TASKTYPE_BINOCULAR", Binocular id) through 0x004B8810. One basic block, no branch, no loop, no frame, no parameter
//   container, 23 call sites over 14 distinct callees.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The parent getter 0x00477BF0 is `mov ax, word [0x005BE3C6]` - the Gun type id that
//   0x00477E30 stores - so this type is parented onto Gun even though its handlers are not gun handlers; the parent is a transcription fact, not
//   an inference about what the type does. The id slot 0x005BE33C holds image value 0, is written with a 16-bit store and is read back at the
//   tail with a 32-bit load masked by 0xFFFF, so it is declared as a plain std::uint16_t; it is one of the image-zero slots in the 0x005BE3xx
//   cluster. This body registers NO event 1: its two 0x00401530/0x00401400 links use events 0 and 2 only, and that gap is transcribed rather
//   than filled. Three of its handler immediates are shared, confirmed by searching the whole listing for each immediate rather than by reading
//   this body alone: 0x004754C0 is referenced three times in the image (0x0047043F, 0x00470652 here, and 0x004753C2 inside 0x00475380, which
//   this lane already transcribed with the same 0x004F1FE0-driven event), 0x004D3710 is referenced 163 times across the image and is a generic
//   filler rather than a task-specific handler, and 0x0051BAA0 is referenced six times (0x004266EE, 0x0042735E, 0x00447EA5, 0x004706F2 here,
//   0x004EED75 and 0x0051B9BD). All are transcribed as retail writes them; none implies ownership. Documented shape differences, all
//   value-preserving: (1) every getter callee is an argument-less load in retail that ignores the arguments the caller pushes, and the tree's
//   transcriptions take no arguments, so those inert pushes are not reproduced; each is simultaneously the next 0x00401400/0x00401530 link
//   call's pre-pushed handler, which the port keeps as the handler value in the same order; (2) byte-valued event ids are zero-extended in the
//   port, which cannot change behaviour because every link fed by a byte getter here goes through 0x00401400 or 0x00401530, which mask the
//   event word for the phase index. Handlers 0x00470760, 0x004707D0, 0x004754C0, 0x00470890, 0x00470BC0, 0x00470C80, 0x00470CE0 and
//   0x00470D70 are raw code immediates in retail and are not transcribed yet (native gaps in 0x0046xxxx-0x0048xxxx); handlers 0x0051BAA0 and
//   0x004D3710 are above 0x0048FFFF and are implemented elsewhere. Domain: core/, because it is one of the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_477BF0(void);            // 0x00477BF0: argument-less word getter (the Gun type id)
extern "C" std::uint8_t Sys_GetGlobal8_4F1FE0(void);              // 0x004F1FE0: argument-less byte getter (sys/native_4f1fe0.cpp)
extern "C" std::uint8_t Task_GetCachedEventId_460BF0(void);       // 0x00460BF0: lazily allocated event id (core/native_00460bf0.cpp)
extern "C" std::uint8_t Task_GetCachedEventId_460C10(void);       // 0x00460C10: lazily allocated event id (core/native_460c10.cpp)
extern "C" std::uint8_t Sys_GetGlobal8_487190(void);              // 0x00487190: argument-less byte getter (core/native_487190.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);         // 0x004F1A70: argument-less byte getter (script_state.hpp)
extern "C" std::uint8_t Core_GetFlagByte_413B40(void);            // 0x00413B40: argument-less byte getter (core/native_413b40.cpp)
extern "C" std::uint8_t Core_GetFlagByte_413B30(void);            // 0x00413B30: argument-less byte getter (core/native_413b30.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F2030(void);            // 0x004F2030: argument-less dword getter (sys/native_4f2030.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_4071D0(void);          // 0x004071D0: argument-less byte getter (sys/native_4071d0.cpp)
extern "C" void Script_QHashNodeRegister(void* arg1, void* value); // 0x004B8810 (script/native_4b8810.cpp)

extern "C" {

// 0x005BE33C: the "Binocular" task type id. Image value 0, overwritten by a 16-bit store from TaskType_Register's return value.
std::uint16_t g_5be33c = 0U;

void Task_DefineBinocular()
{
    // 0x00470610/0x00470612: push 0; push 0 (both inert arguments of the parent getter); 0x00470614: call 0x477BF0; 0x00470619: push eax;
    // 0x0047061A: push 0x1C8; 0x0047061F: push "Binocular"; 0x00470624: call 0x401900.
    const std::uint16_t binocular_id = TaskType_Register("Binocular", // 0x0047061F: push 0x0053FF6C
                                                         0x1C8U,       // 0x0047061A: push 0x1C8 (object size)
                                                         Sys_GetGlobal16_477BF0(), // 0x00470614: call 0x477BF0 (parent type)
                                                         0U);          // 0x00470612: push 0 (inert)

    // 0x00470629: push 0x00470760 (pre-pushed handler); 0x0047062E: push 0; 0x00470630: push eax;
    // 0x00470631: mov word [0x5BE33C],ax (the store lands between the pushes and the call); 0x00470637: call 0x401400.
    ::g_5be33c = binocular_id;
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be33c), // 0x0047063C: mov ax,[0x5BE33C]
                                0U,                                     // 0x0047062E: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00470760U))); // 0x00470629: push handler

    // 0x00470642: push 0x004707D0; 0x00470647: push 2; 0x00470649: push eax; 0x0047064A: call 0x401530;
    // 0x0047064F: add esp,0x2c - the eleven pushes above. No event 1 link exists in this body.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5be33c), // 0x0047063C: mov ax,[0x5BE33C]
                                           2U,                                     // 0x00470647: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004707D0U))); // 0x00470642: push handler

    // 0x00470652: push 0x004754C0 (the handler 0x00475380 also links for its 0x004F1FE0 event); 0x00470657: call 0x4F1FE0;
    // 0x0047065C: mov cx,[0x5BE33C]; 0x00470663: push eax; 0x00470664: push ecx; 0x00470665: call 0x401400; 0x0047066A: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be33c), // 0x0047065C: mov cx,[0x5BE33C]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_4F1FE0()), // 0x00470663: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004754C0U))); // 0x00470652: push handler

    // 0x0047066D: push 0x00470890; 0x00470672: call 0x460BF0; 0x00470677: mov dx,[0x5BE33C]; 0x0047067E: push eax; 0x0047067F: push edx;
    // 0x00470680: call 0x401400; 0x00470685: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be33c), // 0x00470677: mov dx,[0x5BE33C]
                                static_cast<std::uint32_t>(Task_GetCachedEventId_460BF0()), // 0x0047067E: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00470890U))); // 0x0047066D: push handler

    // 0x00470688: push 0x004D3710; 0x0047068D: call 0x460C10; 0x00470692: push eax; 0x00470693: mov ax,[0x5BE33C]; 0x00470699: push eax;
    // 0x0047069A: call 0x401400; 0x0047069F: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be33c), // 0x00470693: mov ax,[0x5BE33C]
                                static_cast<std::uint32_t>(Task_GetCachedEventId_460C10()), // 0x00470692: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004D3710U))); // 0x00470688: push handler

    // 0x004706A2: push 0x00470BC0; 0x004706A7: call 0x487190; 0x004706AC: mov cx,[0x5BE33C]; 0x004706B3: push eax; 0x004706B4: push ecx;
    // 0x004706B5: call 0x401400; 0x004706BA: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be33c), // 0x004706AC: mov cx,[0x5BE33C]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_487190()), // 0x004706B3: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00470BC0U))); // 0x004706A2: push handler

    // 0x004706BD: push 0x00470C80; 0x004706C2: call 0x4F1A70; 0x004706C7: mov dx,[0x5BE33C]; 0x004706CE: push eax; 0x004706CF: push edx;
    // 0x004706D0: call 0x401400; 0x004706D5: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be33c), // 0x004706C7: mov dx,[0x5BE33C]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()), // 0x004706CE: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00470C80U))); // 0x004706BD: push handler

    // 0x004706D8: push 0x00470CE0; 0x004706DD: call 0x413B40; 0x004706E2: push eax; 0x004706E3: mov ax,[0x5BE33C]; 0x004706E9: push eax;
    // 0x004706EA: call 0x401400; 0x004706EF: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be33c), // 0x004706E3: mov ax,[0x5BE33C]
                                static_cast<std::uint32_t>(Core_GetFlagByte_413B40()), // 0x004706E2: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00470CE0U))); // 0x004706D8: push handler

    // 0x004706F2: push 0x0051BAA0 (a handler shared by six call sites in the image); 0x004706F7: call 0x413B30;
    // 0x004706FC: mov cx,[0x5BE33C]; 0x00470703: push eax; 0x00470704: push ecx; 0x00470705: call 0x401400; 0x0047070A: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be33c), // 0x004706FC: mov cx,[0x5BE33C]
                                static_cast<std::uint32_t>(Core_GetFlagByte_413B30()), // 0x00470703: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0051BAA0U))); // 0x004706F2: push handler (above 0x0048FFFF)

    // 0x0047070D: push 0x00470840; 0x00470712: call 0x4F2030; 0x00470717: mov dx,[0x5BE33C]; 0x0047071E: push eax; 0x0047071F: push edx;
    // 0x00470720: call 0x401530 - this link uses 0x00401530, unlike the seven links before it; 0x00470725: add esp,0xc.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5be33c), // 0x00470717: mov dx,[0x5BE33C]
                                           static_cast<std::uint32_t>(Sys_GetGlobal32_4F2030()), // 0x0047071E: push eax (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00470840U))); // 0x0047070D: push handler

    // 0x00470728: push 0x00470D70; 0x0047072D: call 0x4071D0; 0x00470732: push eax; 0x00470733: mov ax,[0x5BE33C]; 0x00470739: push eax;
    // 0x0047073A: call 0x401400. This link has no cleanup of its own.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be33c), // 0x00470733: mov ax,[0x5BE33C]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_4071D0()), // 0x00470732: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00470D70U))); // 0x00470728: push handler

    // 0x0047073F: mov ecx,[0x5BE33C] (a 32-bit load); 0x00470745: and ecx,0xFFFF; 0x0047074B: push ecx;
    // 0x0047074C: push "TASKTYPE_BINOCULAR"; 0x00470751: call 0x4B8810; 0x00470756: add esp,0x14 - the last link's three pushes plus the
    // tail's two; 0x00470759: ret.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_BINOCULAR"), // 0x0047074C: push 0x0053FF58 (argument 1)
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(
                                 static_cast<std::uint32_t>(::g_5be33c)))); // 0x00470745: the id masked to 16 bits (argument 2)
} // end of Task_DefineBinocular

} // extern "C"
