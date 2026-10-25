// Native: Task_DefineHumanAttach (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00465A10-0x00465ABD.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 174 bytes, 1 block, 47 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 47 instructions / 174 bytes / 0 edges) plus
//   the objdump listing build/retail-dis.txt for the same extent (0x00465A10-0x00465ABD; the ret is at 0x00465ABD, two nops follow, and
//   0x00465AC0 is the next function, which this body uses as a handler); `ps` at 0x0053F4F0 for the name literal and `pxw 2 @ 0x005BDC48` for the
//   id slot's image value 0; a stack-pointer simulation over all 47 instructions, which returns esp to its entry value at the ret and accounts for
//   every cleanup (0x2C = eleven pushes: the two zeros, the parent-getter result, the size, the name, and the three pushes of each of the first
//   two links; 0xC four times). Every callee was re-read at its own VA, which is how 0x00504DB0, 0x00416840, 0x00416850, 0x00416880 and
//   0x00489610 were established as argument-less loads despite being called with pushes. Argument numbers were derived by counting the pushes
//   above each access, not by reading an [esp+N] offset as an argument number.
// Behavior/CFG: one straight-line body with NO frame of its own. It registers the "HumanAttach" task type of size 0x140 with parent
//   Sys_GetGlobal16_504DB0() -> id at 0x005BDC48, then links SIX handlers to that id: e0->0x00465AC0 (0x00401400), e1->0x00465AE0
//   (0x00401530), the event id from 0x00416840->0x00465B00, and the event ids from 0x00416850, 0x00416880 and 0x00489610->0x0045F920,
//   0x0045F920 and 0x00465B80 respectively, all through 0x00401400. One basic block, no branch, no loop, 12 call sites over 8 distinct callees.
//   It allocates no parameter container, appends no records, allocates no event id of its own through QTask_AllocateEvent, and has NO 0x004B8810
//   script-name tail.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The id slot 0x005BDC48 holds image value 0 and is written by the 16-bit store at 0x00465A31
//   and read back by every link below it. **The parent getter 0x00504DB0 loads `[0xBA8084]`, which holds 0 in the image** - unlike the 0x0181
//   parents of the other registrars in this lane - so at load time this type is registered with parent 0 unless something writes that global
//   before the 0x00402930 sequence runs. That is recorded as an observation, not resolved; the same "image value may only be the pre-initialisation
//   state" caveat applies as for 0x0054E158 in the accepted 0x0046DB40. The handler 0x00465B80 has 2 references image-wide (0x00464360, inside the
//   accepted 0x00464260, and 0x00465AA2 here), so it is shared between the two registrars and is not this type's own. Handlers 0x00465AC0,
//   0x00465AE0 and 0x00465B00 are raw code immediates in retail and are not transcribed yet (native gaps in 0x0046xxxx-0x0048xxxx);
//   0x00465B80 lies in this lane's range as well, and 0x0045F920 lies below 0x00460000 and is referenced 32 times image-wide (other lane).
//   Documented shape differences, all value-preserving: (1) the getters are argument-less loads in retail that ignore the arguments the caller
//   pushes, and the tree's transcriptions take no arguments, so those inert pushes are not reproduced; each is simultaneously the next 0x00401400
//   link call's pre-pushed handler, which the port keeps as the handler value in the same order; (2) byte-valued event ids are zero-extended in
//   the port, which cannot change behaviour because every link fed by a byte getter here goes through 0x00401400 or 0x00401530, which mask the
//   event word for the phase index; (3) the deferred cleanup is reproduced implicitly by the tree's calling convention rather than by explicit
//   `add esp` arithmetic, since the port never pushes the inert arguments; the retail size 0x2C is recorded here so a future reviewer can check
//   the arithmetic. Domain: core/, because it is one of the 0x00402930 startup registrars (call position 97 of 155).
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_504DB0(void);            // 0x00504DB0: `mov ax,[0xBA8084]; ret` (parent type; image value 0)
extern "C" std::uint8_t Sys_GetSettingByte_416840(void);          // 0x00416840: `mov al,[0x539549]; ret`
extern "C" std::uint8_t Sys_GetSettingByte_416850(void);          // 0x00416850: `mov al,[0x539548]; ret`
extern "C" std::uint8_t Sys_GetSettingByte_416880(void);          // 0x00416880: `mov al,[0x53954D]; ret`
extern "C" std::uint8_t Sys_GetGlobal8_489610(void);              // 0x00489610: `mov al,[0x54127C]; ret`

extern "C" {

// 0x005BDC48: the "HumanAttach" task type id. Image value 0, overwritten by a 16-bit store from TaskType_Register's return value.
std::uint16_t g_5bdc48 = 0U;

void Task_DefineHumanAttach()
{
    // 0x00465A10/0x00465A12: push 0; push 0 (the parent getter's inert argument, then TaskType_Register's fourth argument);
    // 0x00465A14: call 0x504DB0; 0x00465A19: push eax; 0x00465A1A: push 0x140; 0x00465A1F: push "HumanAttach"; 0x00465A24: call 0x401900.
    const std::uint16_t human_attach_id = TaskType_Register("HumanAttach", // 0x00465A1F: push 0x0053F4F0
                                                           0x140U,         // 0x00465A1A: push 0x140 (object size)
                                                           Sys_GetGlobal16_504DB0(), // 0x00465A14: call 0x504DB0 (parent type; image 0)
                                                           0U);            // 0x00465A12: push 0 (fourth argument)

    // 0x00465A29: push 0x00465AC0 (pre-pushed handler); 0x00465A2E: push 0; 0x00465A30: push eax; 0x00465A31: mov word [0x5BDC48],ax (the id
    // store lands between the pushes); 0x00465A37: call 0x401400.
    ::g_5bdc48 = human_attach_id;
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdc48), // 0x00465A31: mov ax,[0x5BDC48]
                                0U,                                     // 0x00465A2E: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00465AC0U))); // 0x00465A29: push handler

    // 0x00465A3C: mov ax,[0x5BDC48]; 0x00465A42: push 0x00465AE0; 0x00465A47: push 1; 0x00465A49: push eax; 0x00465A4A: call 0x401530;
    // 0x00465A4F: add esp,0x2C - eleven pushes: the two zeros, the parent-getter result, the size, the name, and the three pushes of each of the
    // first two links.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5bdc48), // 0x00465A3C: mov ax,[0x5BDC48]
                                           1U,                                     // 0x00465A47: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00465AE0U))); // 0x00465A42: push handler

    // 0x00465A52: push 0x00465B00; 0x00465A57: call 0x416840; 0x00465A5C: mov cx,[0x5BDC48]; 0x00465A63: push eax; 0x00465A64: push ecx;
    // 0x00465A65: call 0x401400; 0x00465A6A: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdc48), // 0x00465A5C: mov cx,[0x5BDC48]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416840()), // 0x00465A63: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00465B00U))); // 0x00465A52: push handler

    // 0x00465A6D: push 0x0045F920 (the handler, and the inert argument of the byte getter); 0x00465A72: call 0x416850;
    // 0x00465A77: mov dx,[0x5BDC48]; 0x00465A7E: push eax; 0x00465A7F: push edx; 0x00465A80: call 0x401400; 0x00465A85: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdc48), // 0x00465A77: mov dx,[0x5BDC48]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416850()), // 0x00465A7E: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0045F920U))); // 0x00465A6D: push handler (below 0x00460000)

    // 0x00465A88: push 0x0045F920; 0x00465A8D: call 0x416880; 0x00465A92: push eax; 0x00465A93: mov ax,[0x5BDC48]; 0x00465A99: push eax;
    // 0x00465A9A: call 0x401400; 0x00465A9F: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdc48), // 0x00465A93: mov ax,[0x5BDC48]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416880()), // 0x00465A92: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0045F920U))); // 0x00465A88: push handler (below 0x00460000)

    // 0x00465AA2: push 0x00465B80; 0x00465AA7: call 0x489610; 0x00465AAC: mov cx,[0x5BDC48]; 0x00465AB3: push eax; 0x00465AB4: push ecx;
    // 0x00465AB5: call 0x401400; 0x00465ABA: add esp,0xC; 0x00465ABD: ret. 0x00465B80 has 2 references image-wide: this one and 0x00464360,
    // inside the accepted 0x00464260.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdc48), // 0x00465AAC: mov cx,[0x5BDC48]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_489610()), // 0x00465AB3: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00465B80U))); // 0x00465AA2: push handler
} // end of Task_DefineHumanAttach

} // extern "C"
