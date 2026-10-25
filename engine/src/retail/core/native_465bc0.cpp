// Native: Task_DefineDeathZone (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00465BC0-0x00465CC6.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 263 bytes, 1 block, 72 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 72 instructions / 263 bytes / 0 edges) plus
//   the objdump listing build/retail-dis.txt for the same extent (0x00465BC0-0x00465CC6; the ret is at 0x00465CC6, nine nops follow, and
//   0x00465CD0 is the next function, which this body uses as a handler); `ps` at 0x0053F514 and 0x0053F500 for the name literals; `px 8 @
//   0x0053F4FC` for the id slot and the byte after it (81 01 ff 00 54 41 53 4b = 0x0181, then 0xFF, then "TASK"), so both are initialised data;
//   a stack-pointer simulation over all 72 instructions, which returns esp to its entry value at the ret and accounts for every cleanup
//   (0x38 = fourteen pushes: the two zeros, the parent-getter result, the size, the name, and the three pushes of each of the first three links;
//   0xC four times; 0x18 once, covering the 0x00489610 link, the script-name tail and the allocation argument). Every callee was re-read at its
//   own VA, which is how 0x004D9FA0, 0x004D9FB0, 0x00416840, 0x00416860, 0x004D14D0 and 0x00489610 were established as argument-less loads
//   (0x004D14D0 and 0x00489610 are already implemented in this tree as Render_GetByte_4D14D0 and Sys_GetGlobal8_489610, so this file declares
//   those symbols rather than duplicating them)
//   despite being called with pushes. Argument numbers were derived by counting the pushes above each access.
// Behavior/CFG: one straight-line body with NO frame of its own. It registers the "DeathZone" task type of size 0x138 with parent
//   Sys_GetGlobal16_4D9FA0() -> id at 0x0053F4FC, then links TEN handlers/events to that id: e0->0x00465CD0 (0x00401400), e1->0x00465DE0 and
//   e2->0x00426860 (both 0x00401530), then the event ids from 0x004D9FB0->0x00465F50, from 0x00416840, 0x00416860 and 0x004D14D0 (all three
//   with a NULL handler), **event 9 with a NULL handler**, and from 0x00489610->0x0045F920, all through 0x00401400. It then registers the script
//   name "TASKTYPE_DEATHZONE" for its masked id through 0x004B8810, and finally calls QTask_AllocateEvent(0) and stores the returned low byte at
//   0x0053F4FE. One basic block, no branch, no loop, 18 call sites over 11 distinct callees. It allocates no parameter container and appends no
//   records.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. **Two globals here are initialised data because their image values are non-zero, although the
//   addresses sit in the 0x0053F4xx region among the name literals**: 0x0053F4FC holds 0x0181 and is the 16-bit id slot (written at 0x00465BE1
//   and read by every link), and 0x0053F4FE holds 0xFF and is the byte that receives QTask_AllocateEvent(0)'s result. `px 8 @ 0x0053F4FC` shows
//   81 01 ff 00 followed immediately by the "TASKTYPE_DEATHZONE" bytes, so the id slot, the event byte and the literal are adjacent - the same
//   layout habit as 0x0053F6D0/0x0053F6D4 in core/native_4682f0.cpp. The parent getter 0x004D9FA0 loads `[0x548648]`, which holds 385 = 0x0181
//   in the image - another member of the 0x0181 cluster. **Four of the ten links carry a NULL handler**: the pre-pushed argument of the 0x00416840,
//   0x00416860, 0x004D14D0 and event-9 links is the constant 0, so argument 3 is NULL and 0x00401400 is expected to accept it; they are
//   transcribed as the instructions order them rather than dropped. 0x00426860 has 11 references image-wide (0x00426676, 0x00426DD6, 0x004272E6,
//   0x00427876, 0x004399F8, 0x00443B89C and others) and lies below 0x00460000, so it is a shared handler owned by the other lane; 0x0045F920 is
//   also below 0x00460000 and is referenced 32 times image-wide. Handlers 0x00465CD0, 0x00465DE0 and 0x00465F50 have one reference each and are
//   raw code immediates in retail, not transcribed yet (native gaps in 0x0046xxxx-0x0048xxxx). **The allocation argument is 0**, not 0x181 as in
//   the three cache getters 0x00467D90/0x00467DB0/0x00467DD0, and the result is stored in a byte that no other instruction in the image reads - a
//   reserved event id whose purpose is unestablished; recorded, not guessed. Documented shape differences, all value-preserving: (1) the getters
//   are argument-less loads in retail that ignore the arguments the caller pushes, and the tree's transcriptions take no arguments, so those inert
//   pushes are not reproduced; each is simultaneously the next 0x00401400/0x00401530 link call's pre-pushed handler, which the port keeps as the
//   handler value in the same order - including the NULL ones; (2) byte-valued event ids are zero-extended in the port, which cannot change
//   behaviour because every link fed by a byte getter here goes through 0x00401400 or 0x00401530, which mask the event word for the phase index;
//   (3) the deferred cleanup is reproduced implicitly by the tree's calling convention rather than by explicit `add esp` arithmetic, since the
//   port never pushes the inert arguments; the retail size 0x38 is recorded here so a future reviewer can check the arithmetic. Domain: core/,
//   because it is one of the 0x00402930 startup registrars (call position 98 of 155).
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4D9FA0(void);            // 0x004D9FA0: `mov ax,[0x548648]; ret` (parent type; image 385 = 0x0181)
extern "C" std::uint8_t Sys_GetGlobal8_4D9FB0(void);              // 0x004D9FB0: `mov al,[0x54864A]; ret`
extern "C" std::uint8_t Sys_GetSettingByte_416840(void);          // 0x00416840: `mov al,[0x539549]; ret`
extern "C" std::uint8_t Sys_GetSettingByte_416860(void);          // 0x00416860: `mov al,[0x53954B]; ret`
extern "C" std::uint8_t Render_GetByte_4D14D0(void);              // 0x004D14D0: `mov al,[0x548440]; ret` (render/native_4d14d0.cpp)
extern "C" std::uint8_t Sys_GetGlobal8_489610(void);              // 0x00489610: `mov al,[0x54127C]; ret`
extern "C" void Script_QHashNodeRegister(void* const arg1, void* const value); // 0x004B8810 (script/native_4b8810.cpp)

extern "C" {

// 0x0053F4FC: the "DeathZone" task type id. Image value 0x0181, so it is initialised data (`D`); written by a 16-bit store and read back by
// every link below it.
std::uint16_t g_53f4fc = 0x0181U;

// 0x0053F4FE: image value 0xFF. Receives the low byte of QTask_AllocateEvent(0)'s result at 0x00465CC1.
std::uint8_t g_53f4fe = 0xFFU;

void Task_DefineDeathZone()
{
    // 0x00465BC0/0x00465BC2: push 0; push 0 (the parent getter's inert argument, then TaskType_Register's fourth argument);
    // 0x00465BC4: call 0x4D9FA0; 0x00465BC9: push eax; 0x00465BCA: push 0x138; 0x00465BCF: push "DeathZone"; 0x00465BD4: call 0x401900.
    const std::uint16_t death_zone_id = TaskType_Register("DeathZone", // 0x00465BCF: push 0x0053F514
                                                         0x138U,       // 0x00465BCA: push 0x138 (object size)
                                                         Sys_GetGlobal16_4D9FA0(), // 0x00465BC4: call 0x4D9FA0 (parent type)
                                                         0U);          // 0x00465BC2: push 0 (fourth argument)

    // 0x00465BD9: push 0x00465CD0 (pre-pushed handler); 0x00465BDE: push 0; 0x00465BE0: push eax; 0x00465BE1: mov word [0x53F4FC],ax (the id
    // store lands between the pushes); 0x00465BE7: call 0x401400.
    ::g_53f4fc = death_zone_id;
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53f4fc), // 0x00465BE1: mov ax,[0x53F4FC]
                                0U,                                     // 0x00465BDE: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00465CD0U))); // 0x00465BD9: push handler

    // 0x00465BEC: mov ax,[0x53F4FC]; 0x00465BF2: push 0x00465DE0; 0x00465BF7: push 1; 0x00465BF9: push eax; 0x00465BFA: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53f4fc), // 0x00465BEC: mov ax,[0x53F4FC]
                                           1U,                                     // 0x00465BF7: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00465DE0U))); // 0x00465BF2: push handler

    // 0x00465BFF: mov cx,[0x53F4FC]; 0x00465C06: push 0x00426860; 0x00465C0B: push 2; 0x00465C0D: push ecx; 0x00465C0E: call 0x401530;
    // 0x00465C13: add esp,0x38 - fourteen pushes: the two zeros, the parent-getter result, the size, the name, and the three pushes of each of
    // the first three links. 0x00426860 has 11 references image-wide and lies below 0x00460000.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53f4fc), // 0x00465BFF: mov cx,[0x53F4FC]
                                           2U,                                     // 0x00465C0B: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00426860U))); // 0x00465C06: push handler (below 0x00460000)

    // 0x00465C16: push 0x00465F50; 0x00465C1B: call 0x4D9FB0; 0x00465C20: mov dx,[0x53F4FC]; 0x00465C27: push eax; 0x00465C28: push edx;
    // 0x00465C29: call 0x401400; 0x00465C2E: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53f4fc), // 0x00465C20: mov dx,[0x53F4FC]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_4D9FB0()), // 0x00465C27: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00465F50U))); // 0x00465C16: push handler

    // 0x00465C31: push 0 (argument 3, NULL handler); 0x00465C33: call 0x416840; 0x00465C38: push eax; 0x00465C39: mov ax,[0x53F4FC];
    // 0x00465C3F: push eax; 0x00465C40: call 0x401400; 0x00465C45: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53f4fc), // 0x00465C39: mov ax,[0x53F4FC]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416840()), // 0x00465C38: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0U)));      // 0x00465C31: push 0 (argument 3, NULL handler)

    // 0x00465C48: push 0 (argument 3, NULL handler); 0x00465C4A: call 0x416860; 0x00465C4F: mov cx,[0x53F4FC]; 0x00465C56: push eax;
    // 0x00465C57: push ecx; 0x00465C58: call 0x401400; 0x00465C5D: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53f4fc), // 0x00465C4F: mov cx,[0x53F4FC]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416860()), // 0x00465C56: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0U)));      // 0x00465C48: push 0 (argument 3, NULL handler)

    // 0x00465C60: push 0 (argument 3, NULL handler); 0x00465C62: call 0x4D14D0; 0x00465C67: mov dx,[0x53F4FC]; 0x00465C6E: push eax;
    // 0x00465C6F: push edx; 0x00465C70: call 0x401400. This link has no cleanup of its own: the next add esp,0x18 covers it together with the
    // event-9 link.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53f4fc), // 0x00465C67: mov dx,[0x53F4FC]
                                static_cast<std::uint32_t>(Render_GetByte_4D14D0()), // 0x00465C6E: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0U)));      // 0x00465C60: push 0 (argument 3, NULL handler)

    // 0x00465C75: mov ax,[0x53F4FC]; 0x00465C7B: push 0 (argument 3, NULL handler); 0x00465C7D: push 9; 0x00465C7F: push eax;
    // 0x00465C80: call 0x401400; 0x00465C85: add esp,0x18.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53f4fc), // 0x00465C75: mov ax,[0x53F4FC]
                                9U,                                     // 0x00465C7D: push 9 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0U)));      // 0x00465C7B: push 0 (argument 3, NULL handler)

    // 0x00465C88: push 0x0045F920 (the handler, and the inert argument of the byte getter); 0x00465C8D: call 0x489610;
    // 0x00465C92: mov cx,[0x53F4FC]; 0x00465C99: push eax; 0x00465C9A: push ecx; 0x00465C9B: call 0x401400. 0x0045F920 is below 0x00460000
    // and is referenced 32 times image-wide.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53f4fc), // 0x00465C92: mov cx,[0x53F4FC]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_489610()), // 0x00465C99: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0045F920U))); // 0x00465C88: push handler (below 0x00460000)

    // 0x00465CA0: mov edx,[0x53F4FC]; 0x00465CA6: and edx,0xFFFF (retail masks the id explicitly here); 0x00465CAC: push edx;
    // 0x00465CAD: push "TASKTYPE_DEATHZONE"; 0x00465CB2: call 0x4B8810.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_DEATHZONE"), // 0x00465CAD: push 0x0053F500 (argument 1)
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(
                                 static_cast<std::uint32_t>(::g_53f4fc) & 0xFFFFU))); // 0x00465CAC: push edx (argument 2, masked id)

    // 0x00465CB7: push 0 (QTask_AllocateEvent's argument - 0, not the 0x181 the three cache getters pass);
    // 0x00465CB9: call 0x4017C0; 0x00465CBE: add esp,0x18 - the 0x00489610 link's three pushes, this tail's two and this argument;
    // 0x00465CC1: mov byte [0x53F4FE],al; 0x00465CC6: ret.
    ::g_53f4fe = static_cast<std::uint8_t>(QTask_AllocateEvent(0U));
} // end of Task_DefineDeathZone

} // extern "C"
