// Native: Task_DefineGunMP5SD (behavior-derived placeholder name; retail symbol unknown); retail VA 0x004739D0-0x00473AAD.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 222 bytes, 1 block, 59 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 59 instructions / 222 bytes / 0 edges) plus
//   the objdump listing build/retail-dis.txt for the same extent (0x004739D0-0x00473AAD; the ret is at 0x00473AAD, two nops follow, and
//   0x00473AB0 is the next function, which this body uses as a handler); `ps` at 0x00540500 and 0x005404EC for the two literals (byte-identical
//   to "GunMP5SD" and "TASKTYPE_GUNMP5SD"); `pxw` at 0x005BE374 for the image value; a full stack-pointer simulation over all 59 instructions,
//   which returns esp to its entry value at the ret and accounts for every cleanup (0x2c = 11 pushes for the two inert arguments, the
//   registration and the first two links; 0xc five times; 0x14 = the last link's three plus the tail's two). Every callee was re-read at its own
//   VA. Argument numbers were derived by counting the pushes above each access, not by reading an [esp+N] offset as an argument number.
// Behavior/CFG: one straight-line body that registers the "GunMP5SD" task type of size 0x1B8 with parent Sys_GetGlobal16_477BF0() -> id at
//   0x005BE374, then wires seven handler links: e0->0x00473AB0 (0x00401400), e2->0x004754A0 (0x00401530), then the event ids from
//   0x00460BF0->0x00473AF0, 0x00413B40->0x004732F0, 0x00413B30->0x00427D70 and 0x004071D0->0x00473C80 (all 0x00401400) and
//   0x004F2030->0x00473990 (0x00401530), and finishes with the script-name tail ("TASKTYPE_GUNMP5SD", GunMP5SD id) through 0x004B8810. One
//   basic block, no branch, no loop, no frame, no parameter container, 15 call sites over 10 distinct callees.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The parent getter 0x00477BF0 is `mov ax, word [0x005BE3C6]` - the Gun type id that
//   0x00477E30 stores - so this type is parented onto Gun, and its size 0x1B8 differs from the 0x1B0 that 0x00475380 registers for
//   ProximityMine while both share the same parent. This body registers NO event 1: its two 0x00401530/0x00401400 links use events 0 and 2
//   only, and that gap is transcribed rather than filled. Handler 0x004754A0 is the SAME immediate that 0x00475380 links for its own event 2,
//   so a handler identity does not imply a single owning task type; both call sites are transcribed as retail writes them. The last link goes
//   through 0x00401530 rather than 0x00401400, unlike the five links before it - the distinction is retail's and is preserved. The id slot
//   0x005BE374 holds image value 0, is written with a 16-bit store and is read back at the tail with a 32-bit load masked by 0xFFFF, so it is
//   declared as a plain std::uint16_t; it is one of the image-zero slots in the 0x005BE3xx cluster. Documented shape differences, all
//   value-preserving: (1) every getter callee is an argument-less load in retail that ignores the arguments the caller pushes, and the tree's
//   transcriptions take no arguments, so those inert pushes are not reproduced; each is simultaneously the next 0x00401400/0x00401530 link
//   call's pre-pushed handler, which the port keeps as the handler value in the same order; (2) byte-valued event ids are zero-extended in the
//   port, which cannot change behaviour because every link fed by a byte getter here goes through 0x00401400 or 0x00401530, which mask the
//   event word for the phase index. Handlers 0x00473AB0, 0x004754A0, 0x00473AF0, 0x004732F0, 0x00473C80 and 0x00473990 are raw code
//   immediates in retail and are not transcribed yet (native gaps in 0x0046xxxx-0x0048xxxx). Handler 0x00427D70 is BELOW 0x00460000 and is
//   already implemented by the 0x00400000-0x0045FFFF owner, not by this file. Domain: core/, because it is one of the 0x00402930 startup
//   registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_477BF0(void);            // 0x00477BF0: argument-less word getter (the Gun type id)
extern "C" std::uint8_t Task_GetCachedEventId_460BF0(void);       // 0x00460BF0: lazily allocated event id (core/native_00460bf0.cpp)
extern "C" std::uint8_t Core_GetFlagByte_413B40(void);            // 0x00413B40: argument-less byte getter (core/native_413b40.cpp)
extern "C" std::uint8_t Core_GetFlagByte_413B30(void);            // 0x00413B30: argument-less byte getter (core/native_413b30.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_4071D0(void);          // 0x004071D0: argument-less byte getter (sys/native_4071d0.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F2030(void);            // 0x004F2030: argument-less dword getter (sys/native_4f2030.cpp)
extern "C" void Script_QHashNodeRegister(void* arg1, void* value); // 0x004B8810 (script/native_4b8810.cpp)

extern "C" {

// 0x005BE374: the "GunMP5SD" task type id. Image value 0, overwritten by a 16-bit store from TaskType_Register's return value.
std::uint16_t g_5be374 = 0U;

void Task_DefineGunMP5SD()
{
    // 0x004739D0/0x004739D2: push 0; push 0 (both inert arguments of the parent getter); 0x004739D4: call 0x477BF0; 0x004739D9: push eax;
    // 0x004739DA: push 0x1B8; 0x004739DF: push "GunMP5SD"; 0x004739E4: call 0x401900.
    const std::uint16_t gun_mp5sd_id = TaskType_Register("GunMP5SD", // 0x004739DF: push 0x00540500
                                                         0x1B8U,      // 0x004739DA: push 0x1B8 (object size)
                                                         Sys_GetGlobal16_477BF0(), // 0x004739D4: call 0x477BF0 (parent type)
                                                         0U);         // 0x004739D2: push 0 (inert)

    // 0x004739E9: push 0x00473AB0 (pre-pushed handler); 0x004739EE: push 0; 0x004739F0: push eax;
    // 0x004739F1: mov word [0x5BE374],ax (the store lands between the pushes and the call); 0x004739F7: call 0x401400.
    ::g_5be374 = gun_mp5sd_id;
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be374), // 0x004739FC: mov ax,[0x5BE374]
                                0U,                                     // 0x004739EE: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00473AB0U))); // 0x004739E9: push handler

    // 0x00473A02: push 0x004754A0 (the same handler 0x00475380 links for its event 2); 0x00473A07: push 2; 0x00473A09: push eax;
    // 0x00473A0A: call 0x401530; 0x00473A0F: add esp,0x2c - the eleven pushes above. No event 1 link exists in this body.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5be374), // 0x004739FC: mov ax,[0x5BE374]
                                           2U,                                     // 0x00473A07: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004754A0U))); // 0x00473A02: push handler

    // 0x00473A12: push 0x00473AF0; 0x00473A17: call 0x460BF0; 0x00473A1C: mov cx,[0x5BE374]; 0x00473A23: push eax; 0x00473A24: push ecx;
    // 0x00473A25: call 0x401400; 0x00473A2A: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be374), // 0x00473A1C: mov cx,[0x5BE374]
                                static_cast<std::uint32_t>(Task_GetCachedEventId_460BF0()), // 0x00473A23: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00473AF0U))); // 0x00473A12: push handler

    // 0x00473A2D: push 0x004732F0; 0x00473A32: call 0x413B40; 0x00473A37: mov dx,[0x5BE374]; 0x00473A3E: push eax; 0x00473A3F: push edx;
    // 0x00473A40: call 0x401400; 0x00473A45: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be374), // 0x00473A37: mov dx,[0x5BE374]
                                static_cast<std::uint32_t>(Core_GetFlagByte_413B40()), // 0x00473A3E: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004732F0U))); // 0x00473A2D: push handler

    // 0x00473A48: push 0x00427D70; 0x00473A4D: call 0x413B30; 0x00473A52: push eax; 0x00473A53: mov ax,[0x5BE374]; 0x00473A59: push eax;
    // 0x00473A5A: call 0x401400; 0x00473A5F: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be374), // 0x00473A53: mov ax,[0x5BE374]
                                static_cast<std::uint32_t>(Core_GetFlagByte_413B30()), // 0x00473A52: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00427D70U))); // 0x00473A48: push handler (below 0x00460000)

    // 0x00473A62: push 0x00473C80; 0x00473A67: call 0x4071D0; 0x00473A6C: mov cx,[0x5BE374]; 0x00473A73: push eax; 0x00473A74: push ecx;
    // 0x00473A75: call 0x401400; 0x00473A7A: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be374), // 0x00473A6C: mov cx,[0x5BE374]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_4071D0()), // 0x00473A73: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00473C80U))); // 0x00473A62: push handler

    // 0x00473A7D: push 0x00473990; 0x00473A82: call 0x4F2030; 0x00473A87: mov dx,[0x5BE374]; 0x00473A8E: push eax; 0x00473A8F: push edx;
    // 0x00473A90: call 0x401530 - this link uses 0x00401530, unlike the five links before it.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5be374), // 0x00473A87: mov dx,[0x5BE374]
                                            static_cast<std::uint32_t>(Sys_GetGlobal32_4F2030()), // 0x00473A8E: push eax (argument 2)
                                            reinterpret_cast<igi::retail::core::FrameHandler>(
                                                static_cast<std::uintptr_t>(0x00473990U))); // 0x00473A7D: push handler

    // 0x00473A95: mov eax,[0x5BE374] (a 32-bit load); 0x00473A9A: and eax,0xFFFF; 0x00473A9F: push eax;
    // 0x00473AA0: push "TASKTYPE_GUNMP5SD"; 0x00473AA5: call 0x4B8810; 0x00473AAA: add esp,0x14 - the last link's three pushes plus the
    // tail's two; 0x00473AAD: ret.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_GUNMP5SD"), // 0x00473AA0: push 0x005404EC (argument 1)
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(
                                 static_cast<std::uint32_t>(::g_5be374)))); // 0x00473A9A: the id masked to 16 bits (argument 2)
} // end of Task_DefineGunMP5SD

} // extern "C"
