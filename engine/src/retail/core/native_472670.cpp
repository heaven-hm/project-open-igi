// Native: Task_DefineGunCABarrelAndClip (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00472670-0x004727E7.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 376 bytes, 1 block, 99 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 99 instructions / 376 bytes / 0 edges) plus
//   the objdump listing build/retail-dis.txt for the same extent (0x00472670-0x004727E7; the ret is at 0x004727E7, one nop follows, and
//   0x004727F0 is the next function, which this body uses as a handler); `ps` at 0x00540330, 0x00540324, 0x00540314 and 0x00540300 for the four
//   literals (byte-identical to "GunCABarrel", "GunCAClip", "TASKTYPE_CACLIP" and "TASKTYPE_CABARREL"); `pxw` at 0x005402FC, which reads
//   0x01810181 - i.e. BOTH id slots hold image value 0x0181 (0x005402FC and 0x005402FE); the four literals also sit in .rdata in the REVERSE
//   order the code loads them (0x00540300 "TASKTYPE_CABARREL", 0x00540314 "TASKTYPE_CACLIP", 0x00540324 "GunCAClip", 0x00540330
//   "GunCABarrel"), so load order is not inferable from data layout; a full stack-pointer simulation over all 99
//   instructions, which returns esp to its entry value at the ret and accounts for every cleanup (0x38 twice = 14 pushes each for one
//   registration block's two inert arguments, registration and first two links; 0xc six times; 0x1c once = the last 0x00416830 link's three
//   pushes plus the two 0x004B8810 tails' two each). Every callee was re-read at its own VA. Argument numbers were derived by counting the
//   pushes above each access, not by reading an [esp+N] offset as an argument number.
// Behavior/CFG: ONE body that performs TWO complete registrations. First "GunCABarrel" (0x00540330) of size 0x138 with parent
//   Sys_GetGlobal16_4D9FA0() -> id at 0x005402FE, linking e0->0x004727F0 (0x00401400), e1->0x004D3710 and e2->0x004D3710 (0x00401530), then
//   0x004C56F0->0x00472880, 0x00460C10->0x00472930 and 0x00416830->0x0045E3D0. Second "GunCAClip" (0x00540324), same size and parent -> id at
//   0x005402FC, linking e0->0x004727F0 (the SAME handler as the first registration), e1->0x004D3710 and e2->0x004D3710, then
//   0x004C56F0->0x00477B10, 0x00460C10->0x004D3710 and 0x00416830->0x0045E3D0. It then writes two 0x004B8810 script-name tails in the order
//   ("TASKTYPE_CACLIP", clip id) first and ("TASKTYPE_CABARREL", barrel id) second - i.e. the tail order is the reverse of the registration
//   order. One basic block, no branch, no loop, no frame, no parameter container, 24 call sites over 8 distinct callees.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Both id slots are in the 0x005402xx cluster and both hold image 0x0181, which are two more
//   sightings of that value and the first in this cluster; they are written with 16-bit stores and read back at the tails with a 16-bit load
//   (the first tail reads 0x005402FC with `mov ecx,[abs]; and ecx,0xFFFF`, the second reads 0x005402FE with `xor edx,edx; mov dx,[abs]`), so
//   both are declared as std::uint16_t. Because their image value is 0x0181 rather than 0 they are the first id slots in this lane that land in
//   .data rather than .bss - nm on the compiled object reports both as `D`, against `B` for the image-zero 0x005BE3xx slots. The parent getter
//   0x004D9FA0 is the same parent that 0x00477220 uses for three registrations, but the
//   size here is 0x138 against 0x140 there, so the size is a per-registration fact taken from each body's own push immediate. Handler
//   0x004727F0 is linked as e0 by BOTH registrations in this body, and 0x004D3710 appears as the handler of e1/e2 for both registrations AND
//   as the event-linked handler of the second registration's 0x00460C10 event - the same immediate serving as handler and as link target.
//   Documented shape differences, all value-preserving: (1) every getter callee is an argument-less load in retail that ignores the arguments
//   the caller pushes, and the tree's transcriptions take no arguments, so those inert pushes are not reproduced; each is simultaneously the
//   next 0x00401400/0x00401530 link call's pre-pushed handler, which the port keeps as the handler value in the same order; (2) byte-valued
//   event ids are zero-extended in the port, which cannot change behaviour because every link fed by a byte getter here goes through
//   0x00401400 or 0x00401530, which mask the event word for the phase index; (3) the second tail reads its id with `xor edx,edx; mov dx,[abs]`
//   and the first with a dword load masked by 0xFFFF - both are reproduced as the same 16-bit value. Handlers 0x004727F0, 0x00472880,
//   0x00472930, 0x0045E3D0 and 0x00477B10 are raw code immediates in retail and are not transcribed yet; 0x0045E3D0 is BELOW 0x00460000 and
//   belongs to the 0x00400000-0x0045FFFF owner, while 0x004727F0, 0x00472880, 0x00472930 and 0x00477B10 are native gaps in
//   0x0046xxxx-0x0048xxxx. Handler 0x004D3710 is above 0x0048FFFF and is already implemented elsewhere. Domain: core/, because it is one of the
//   0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4D9FA0(void);            // 0x004D9FA0: argument-less word getter (parent type)
extern "C" std::uint8_t DynObj_GetByte_4C56F0(void);              // 0x004C56F0: argument-less byte getter (dynobj/native_4c56f0.cpp)
extern "C" std::uint8_t Task_GetCachedEventId_460C10(void);       // 0x00460C10: lazily allocated event id (core/native_460c10.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416830(void);          // 0x00416830: argument-less byte getter (sys/native_416830.cpp)
extern "C" void Script_QHashNodeRegister(void* arg1, void* value); // 0x004B8810 (script/native_4b8810.cpp)

extern "C" {

// 0x005402FC: the "GunCAClip" task type id. Image value 0x0181, overwritten by a 16-bit store from TaskType_Register's return value.
std::uint16_t g_5402fc = 0x0181U;

// 0x005402FE: the "GunCABarrel" task type id. Image value 0x0181, overwritten by a 16-bit store from TaskType_Register's return value.
std::uint16_t g_5402fe = 0x0181U;

void Task_DefineGunCABarrelAndClip()
{
    // ===== Registration 1: GunCABarrel =====
    // 0x00472670/0x00472672: push 0; push 0 (both inert arguments of the parent getter); 0x00472674: call 0x4D9FA0; 0x00472679: push eax;
    // 0x0047267A: push 0x138; 0x0047267F: push "GunCABarrel"; 0x00472684: call 0x401900.
    const std::uint16_t gun_ca_barrel_id = TaskType_Register("GunCABarrel", // 0x0047267F: push 0x00540330
                                                             0x138U,        // 0x0047267A: push 0x138 (object size)
                                                             Sys_GetGlobal16_4D9FA0(), // 0x00472674: call 0x4D9FA0 (parent type)
                                                             0U);           // 0x00472672: push 0 (inert)

    // 0x00472689: push 0x004727F0 (pre-pushed handler, shared with registration 2); 0x0047268E: push 0; 0x00472690: push eax;
    // 0x00472691: mov word [0x5402FE],ax; 0x00472697: call 0x401400.
    ::g_5402fe = gun_ca_barrel_id;
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5402fe), // 0x0047269C: mov ax,[0x5402FE]
                                0U,                                     // 0x0047268E: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004727F0U))); // 0x00472689: push handler

    // 0x004726A2: push 0x004D3710; 0x004726A7: push 1; 0x004726A9: push eax; 0x004726AA: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5402fe), // 0x0047269C: mov ax,[0x5402FE]
                                           1U,                                     // 0x004726A7: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004D3710U))); // 0x004726A2: push handler

    // 0x004726AF: mov cx,[0x5402FE]; 0x004726B6: push 0x004D3710; 0x004726BB: push 2; 0x004726BD: push ecx; 0x004726BE: call 0x401530;
    // 0x004726C3: add esp,0x38 - the fourteen pushes above (two inert, three registration, three per link).
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5402fe), // 0x004726AF: mov cx,[0x5402FE]
                                           2U,                                     // 0x004726BB: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004D3710U))); // 0x004726B6: push handler

    // 0x004726C6: push 0x00472880; 0x004726CB: call 0x4C56F0; 0x004726D0: mov dx,[0x5402FE]; 0x004726D7: push eax; 0x004726D8: push edx;
    // 0x004726D9: call 0x401400; 0x004726DE: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5402fe), // 0x004726D0: mov dx,[0x5402FE]
                                static_cast<std::uint32_t>(DynObj_GetByte_4C56F0()), // 0x004726D7: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00472880U))); // 0x004726C6: push handler

    // 0x004726E1: push 0x00472930; 0x004726E6: call 0x460C10; 0x004726EB: push eax; 0x004726EC: mov ax,[0x5402FE]; 0x004726F2: push eax;
    // 0x004726F3: call 0x401400; 0x004726F8: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5402fe), // 0x004726EC: mov ax,[0x5402FE]
                                static_cast<std::uint32_t>(Task_GetCachedEventId_460C10()), // 0x004726EB: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00472930U))); // 0x004726E1: push handler

    // 0x004726FB: push 0x0045E3D0; 0x00472700: call 0x416830; 0x00472705: mov cx,[0x5402FE]; 0x0047270C: push eax; 0x0047270D: push ecx;
    // 0x0047270E: call 0x401400; 0x00472713: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5402fe), // 0x00472705: mov cx,[0x5402FE]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416830()), // 0x0047270C: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0045E3D0U))); // 0x004726FB: push handler (below 0x00460000)

    // ===== Registration 2: GunCAClip =====
    // 0x00472716/0x00472718: push 0; push 0 (inert); 0x0047271A: call 0x4D9FA0; 0x0047271F: push eax; 0x00472720: push 0x138;
    // 0x00472725: push "GunCAClip"; 0x0047272A: call 0x401900.
    const std::uint16_t gun_ca_clip_id = TaskType_Register("GunCAClip", // 0x00472725: push 0x00540324
                                                           0x138U,      // 0x00472720: push 0x138 (the same object size)
                                                           Sys_GetGlobal16_4D9FA0(), // 0x0047271A: call 0x4D9FA0
                                                           0U);         // 0x00472718: push 0 (inert)

    // 0x0047272F: push 0x004727F0 (the SAME handler registration 1 linked for its event 0); 0x00472734: push 0; 0x00472736: push eax;
    // 0x00472737: mov word [0x5402FC],ax; 0x0047273D: call 0x401400.
    ::g_5402fc = gun_ca_clip_id;
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5402fc), // 0x00472742: mov dx,[0x5402FC]
                                0U,                                     // 0x00472734: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004727F0U))); // 0x0047272F: push handler

    // 0x00472749: push 0x004D3710; 0x0047274E: push 1; 0x00472750: push edx; 0x00472751: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5402fc), // 0x00472742: mov dx,[0x5402FC]
                                           1U,                                     // 0x0047274E: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004D3710U))); // 0x00472749: push handler

    // 0x00472756: mov ax,[0x5402FC]; 0x0047275C: push 0x004D3710; 0x00472761: push 2; 0x00472763: push eax; 0x00472764: call 0x401530;
    // 0x00472769: add esp,0x38.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5402fc), // 0x00472756: mov ax,[0x5402FC]
                                           2U,                                     // 0x00472761: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004D3710U))); // 0x0047275C: push handler

    // 0x0047276C: push 0x00477B10; 0x00472771: call 0x4C56F0; 0x00472776: mov cx,[0x5402FC]; 0x0047277D: push eax; 0x0047277E: push ecx;
    // 0x0047277F: call 0x401400; 0x00472784: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5402fc), // 0x00472776: mov cx,[0x5402FC]
                                static_cast<std::uint32_t>(DynObj_GetByte_4C56F0()), // 0x0047277D: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00477B10U))); // 0x0047276C: push handler

    // 0x00472787: push 0x004D3710; 0x0047278C: call 0x460C10; 0x00472791: mov dx,[0x5402FC]; 0x00472798: push eax; 0x00472799: push edx;
    // 0x0047279A: call 0x401400; 0x0047279F: add esp,0xc. Here 0x004D3710 is the EVENT-LINKED handler, not an e1/e2 handler.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5402fc), // 0x00472791: mov dx,[0x5402FC]
                                static_cast<std::uint32_t>(Task_GetCachedEventId_460C10()), // 0x00472798: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004D3710U))); // 0x00472787: push handler

    // 0x004727A2: push 0x0045E3D0; 0x004727A7: call 0x416830; 0x004727AC: push eax; 0x004727AD: mov ax,[0x5402FC]; 0x004727B3: push eax;
    // 0x004727B4: call 0x401400. This link has no cleanup of its own.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5402fc), // 0x004727AD: mov ax,[0x5402FC]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416830()), // 0x004727AC: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0045E3D0U))); // 0x004727A2: push handler (below 0x00460000)

    // ===== The two script-name tails, in retail's order: CLIP first, BARREL second =====
    // 0x004727B9: mov ecx,[0x5402FC] (a 32-bit load); 0x004727BF: and ecx,0xFFFF; 0x004727C5: push ecx;
    // 0x004727C6: push "TASKTYPE_CACLIP"; 0x004727CB: call 0x4B8810.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_CACLIP"), // 0x004727C6: push 0x00540314 (argument 1)
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(
                                 static_cast<std::uint32_t>(::g_5402fc)))); // 0x004727BF: the clip id masked to 16 bits

    // 0x004727D0: xor edx,edx; 0x004727D2: mov dx,[0x5402FE] (a 16-bit load, the other zero-extension shape); 0x004727D9: push edx;
    // 0x004727DA: push "TASKTYPE_CABARREL"; 0x004727DF: call 0x4B8810; 0x004727E4: add esp,0x1c - the last 0x00416830 link's three pushes
    // plus this tail's two and the previous tail's two; 0x004727E7: ret.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_CABARREL"), // 0x004727DA: push 0x00540300 (argument 1)
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(
                                 static_cast<std::uint32_t>(::g_5402fe)))); // 0x004727D2: mov dx,[0x5402FE] (argument 2)
} // end of Task_DefineGunCABarrelAndClip

} // extern "C"
