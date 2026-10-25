// Native: Task_DefineHumanView (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00464260-0x00464395.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 310 bytes, 1 block, 83 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 83 instructions / 310 bytes / 0 edges) plus
//   the objdump listing build/retail-dis.txt for the same extent (0x00464260-0x00464395; the ret is at 0x00464395, ten nops follow, and
//   0x004643A0 is the next function, which this body uses as a handler); `ps` at 0x0053F4D8 for the name literal and `pxw 2 @ 0x005BDC40` for the
//   id slot's image value 0; a stack-pointer simulation over all 83 instructions, which returns esp to its entry value at the ret and accounts
//   for every cleanup (0x44 = seventeen pushes: the two zeros, the parent-getter result, the size, the name, and the three pushes of each of the
//   first four links; 0xC six times; 0x18 once for the event-7 link and the one before it). Every callee was re-read at its own VA, which is how
//   0x004D9610, 0x004CEA20, 0x004CEA10, 0x00416830, 0x00416850, 0x00416880 and 0x00489610 were established as argument-less loads despite being
//   called with pushes. 0x004E6610 is NOT one of them: it is a lazy one-shot event-id allocation that compares 0x00548813 with 0xFF and, only on
//   0xFF, calls QTask_AllocateEvent(0) - the same body shape as 0x00467D90 but with allocation argument 0 - and it is already implemented in this
//   tree as Task_GetCachedEventId_4E6610 (core/native_004e6610.cpp), so this file declares that symbol rather than duplicating it. Argument numbers were derived by counting the pushes above each access, not by reading an [esp+N] offset as
//   an argument number.
// Behavior/CFG: one straight-line body with NO frame of its own. It registers the "HumanView" task type of size 0x328 with parent
//   Sys_GetGlobal16_4D9610() -> id at 0x005BDC40, then links TWELVE handlers to that id: e0->0x004643A0 and e4->0x004644A0 through 0x00401400,
//   e1->0x00488700 and e2->0x00464470 through 0x00401530, 0x004CEA20's event id->0x00464580 and 0x004CEA10's event id->0x004645B0 through
//   0x00401400, **event 7 with a NULL handler** through 0x00401400, and the event ids from the byte getters 0x00416830, 0x00416850 and 0x00416880
//   ->0x0045F920 (three times), 0x00489610->0x00465B80 and 0x004E6610's event id->0x004645D0 through 0x00401400. One basic block, no branch, no loop,
//   21 call sites over 11 distinct callees. It allocates no parameter container, appends no records, allocates no event id of its own through
//   QTask_AllocateEvent, and has NO 0x004B8810 script-name tail.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The id slot 0x005BDC40 holds image value 0 and is written by the 16-bit store at 0x00464281
//   and read back by every link below it. The parent getter 0x004D9610 loads `[0x548618]`, which holds 385 = 0x0181 in the image - another
//   member of the 0x0181 cluster, recorded as an observation; the parent is a 16-bit load and the port declares it as such. **The seventh link is
//   a NULL handler**: 0x00464303/0x00464305/0x00464307 push 0, then 7, then the id, so argument 3 is the constant 0, and 0x00401400 is expected to
//   accept it. This is the first NULL handler in this lane, and it is transcribed as the instructions order it rather than dropped. Two of the
//   handlers are shared, which is why they must not be treated as belonging to this type: **0x00488700 has 17 references image-wide** (0x004240EB,
//   0x00436202, 0x00464292 here, 0x004688DF and 0x00468E4F - the last two inside the next two targets in this lane's queue, 0x004688B0 and
//   0x00468E20), and **0x00465B80 has 2** (0x00464360 here and 0x00465AA2, inside 0x00465A10). Both are recorded as shared handlers. Handlers
//   0x004643A0 (207 bytes / 44 instructions), 0x00464470 (48 / 16), 0x004644A0 (221 / 67), 0x00464580 (46 / 16), 0x004645B0 (22 / 8),
//   0x004645D0 (33 / 8), 0x00488700 (104 / 36) and 0x00465B80 (44 / 20) are raw code immediates in retail and are not transcribed yet (native
//   gaps in 0x0046xxxx-0x0048xxxx); 0x0045F920 lies below 0x00460000 and is referenced 32 times image-wide (other lane). Documented shape
//   differences, all value-preserving: (1) the getters are argument-less loads in retail that ignore the arguments the caller pushes, and the
//   tree's transcriptions take no arguments, so those inert pushes are not reproduced; each is simultaneously the next 0x00401400/0x00401530
//   link call's pre-pushed handler, which the port keeps as the handler value in the same order; (2) byte-valued event ids are zero-extended in
//   the port, which cannot change behaviour because every link fed by a byte getter here goes through 0x00401400 or 0x00401530, which mask the
//   event word for the phase index; (3) the deferred cleanup is reproduced implicitly by the tree's calling convention rather than by explicit
//   `add esp` arithmetic, since the port never pushes the inert arguments; the retail size 0x44 is recorded here so a future reviewer can check
//   the arithmetic. Domain: core/, because it is one of the 0x00402930 startup registrars (call position 96 of 155).
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4D9610(void);            // 0x004D9610: `mov ax,[0x548618]; ret` (parent type; image 385 = 0x0181)
extern "C" std::uint8_t QTask_PhaseIdGet548177_4CEA20(void);      // 0x004CEA20: argument-less byte getter (script/native_4cea20.cpp)
extern "C" std::uint8_t QTask_PhaseIdGet548176_4CEA10(void);      // 0x004CEA10: argument-less byte getter (script/native_4cea10.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416830(void);          // 0x00416830: `mov al,[0x539547]; ret`
extern "C" std::uint8_t Sys_GetSettingByte_416850(void);          // 0x00416850: `mov al,[0x539548]; ret`
extern "C" std::uint8_t Sys_GetSettingByte_416880(void);          // 0x00416880: `mov al,[0x53954D]; ret`
extern "C" std::uint8_t Sys_GetGlobal8_489610(void);              // 0x00489610: `mov al,[0x54127C]; ret`
extern "C" std::uint8_t Task_GetCachedEventId_4E6610(void);      // 0x004E6610: lazy event-id allocation (core/native_004e6610.cpp)

extern "C" {

// 0x005BDC40: the "HumanView" task type id. Image value 0, overwritten by a 16-bit store from TaskType_Register's return value.
std::uint16_t g_5bdc40 = 0U;

void Task_DefineHumanView()
{
    // 0x00464260/0x00464262: push 0; push 0 (the parent getter's inert argument, then TaskType_Register's fourth argument);
    // 0x00464264: call 0x4D9610; 0x00464269: push eax; 0x0046426A: push 0x328; 0x0046426F: push "HumanView"; 0x00464274: call 0x401900.
    const std::uint16_t human_view_id = TaskType_Register("HumanView", // 0x0046426F: push 0x0053F4D8
                                                         0x328U,        // 0x0046426A: push 0x328 (object size)
                                                         Sys_GetGlobal16_4D9610(), // 0x00464264: call 0x4D9610 (parent type)
                                                         0U);           // 0x00464262: push 0 (fourth argument)

    // 0x00464279: push 0x004643A0 (pre-pushed handler); 0x0046427E: push 0; 0x00464280: push eax; 0x00464281: mov word [0x5BDC40],ax (the id
    // store lands between the pushes); 0x00464287: call 0x401400.
    ::g_5bdc40 = human_view_id;
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdc40), // 0x00464281: mov ax,[0x5BDC40]
                                0U,                                     // 0x0046427E: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004643A0U))); // 0x00464279: push handler

    // 0x0046428C: mov ax,[0x5BDC40]; 0x00464292: push 0x00488700; 0x00464297: push 1; 0x00464299: push eax; 0x0046429A: call 0x401530.
    // 0x00488700 is shared: it has 17 references image-wide, including two in the next two targets of this lane's queue.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5bdc40), // 0x0046428C: mov ax,[0x5BDC40]
                                           1U,                                     // 0x00464297: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00488700U))); // 0x00464292: push handler

    // 0x0046429F: mov cx,[0x5BDC40]; 0x004642A6: push 0x00464470; 0x004642AB: push 2; 0x004642AD: push ecx; 0x004642AE: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5bdc40), // 0x0046429F: mov cx,[0x5BDC40]
                                           2U,                                     // 0x004642AB: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00464470U))); // 0x004642A6: push handler

    // 0x004642B3: mov dx,[0x5BDC40]; 0x004642BA: push 0x004644A0; 0x004642BF: push 4; 0x004642C1: push edx; 0x004642C2: call 0x401400;
    // 0x004642C7: add esp,0x44 - seventeen pushes: the two zeros, the parent-getter result, the size, the name, and the three pushes of each of
    // the first four links. NOTE the event ids are 0, 1, 2 and 4: there is NO event-3 handler in this body.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdc40), // 0x004642B3: mov dx,[0x5BDC40]
                                4U,                                     // 0x004642BF: push 4 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004644A0U))); // 0x004642BA: push handler

    // 0x004642CA: push 0x00464580; 0x004642CF: call 0x4CEA20; 0x004642D4: push eax; 0x004642D5: mov ax,[0x5BDC40]; 0x004642DB: push eax;
    // 0x004642DC: call 0x401400; 0x004642E1: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdc40), // 0x004642D5: mov ax,[0x5BDC40]
                                static_cast<std::uint32_t>(QTask_PhaseIdGet548177_4CEA20()), // 0x004642D4: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00464580U))); // 0x004642CA: push handler

    // 0x004642E4: push 0x004645B0; 0x004642E9: call 0x4CEA10; 0x004642EE: mov cx,[0x5BDC40]; 0x004642F5: push eax; 0x004642F6: push ecx;
    // 0x004642F7: call 0x401400. This link has no cleanup of its own: the next add esp,0x18 covers it together with the event-7 link.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdc40), // 0x004642EE: mov cx,[0x5BDC40]
                                static_cast<std::uint32_t>(QTask_PhaseIdGet548176_4CEA10()), // 0x004642F5: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004645B0U))); // 0x004642E4: push handler

    // 0x004642FC: mov dx,[0x5BDC40]; 0x00464303: push 0; 0x00464305: push 7; 0x00464307: push edx; 0x00464308: call 0x401400;
    // 0x0046430D: add esp,0x18. The handler for event 7 is the constant 0: argument 3 is NULL, and 0x00401400 is expected to accept it.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdc40), // 0x004642FC: mov dx,[0x5BDC40]
                                7U,                                     // 0x00464305: push 7 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0U)));      // 0x00464303: push 0 (argument 3, NULL handler)

    // 0x00464310: push 0x0045F920 (the handler, and the inert argument of the byte getter); 0x00464315: call 0x416830;
    // 0x0046431A: push eax; 0x0046431B: mov ax,[0x5BDC40]; 0x00464321: push eax; 0x00464322: call 0x401400; 0x00464327: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdc40), // 0x0046431B: mov ax,[0x5BDC40]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416830()), // 0x0046431A: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0045F920U))); // 0x00464310: push handler (below 0x00460000)

    // 0x0046432A: push 0x0045F920; 0x0046432F: call 0x416850; 0x00464334: mov cx,[0x5BDC40]; 0x0046433B: push eax; 0x0046433C: push ecx;
    // 0x0046433D: call 0x401400; 0x00464342: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdc40), // 0x00464334: mov cx,[0x5BDC40]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416850()), // 0x0046433B: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0045F920U))); // 0x0046432A: push handler (below 0x00460000)

    // 0x00464345: push 0x0045F920; 0x0046434A: call 0x416880; 0x0046434F: mov dx,[0x5BDC40]; 0x00464356: push eax; 0x00464357: push edx;
    // 0x00464358: call 0x401400; 0x0046435D: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdc40), // 0x0046434F: mov dx,[0x5BDC40]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416880()), // 0x00464356: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0045F920U))); // 0x00464345: push handler (below 0x00460000)

    // 0x00464360: push 0x00465B80; 0x00464365: call 0x489610; 0x0046436A: push eax; 0x0046436B: mov ax,[0x5BDC40]; 0x00464371: push eax;
    // 0x00464372: call 0x401400; 0x00464377: add esp,0xC. 0x00465B80 has 2 references image-wide: this one and 0x00465AA2, inside the next
    // target of this lane's queue (0x00465A10).
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdc40), // 0x0046436B: mov ax,[0x5BDC40]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_489610()), // 0x0046436A: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00465B80U))); // 0x00464360: push handler

    // 0x0046437A: push 0x004645D0; 0x0046437F: call 0x4E6610; 0x00464384: mov cx,[0x5BDC40]; 0x0046438B: push eax; 0x0046438C: push ecx;
    // 0x0046438D: call 0x401400; 0x00464392: add esp,0xC; 0x00464395: ret.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdc40), // 0x00464384: mov cx,[0x5BDC40]
                                static_cast<std::uint32_t>(Task_GetCachedEventId_4E6610()), // 0x0046438B: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004645D0U))); // 0x0046437A: push handler
} // end of Task_DefineHumanView

} // extern "C"
