// Native: Task_DefineAIStationaryGun (behavior-derived placeholder name; retail symbol unknown); retail VA 0x0046F2B0-0x0046F386.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 215 bytes, 1 block, 57 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 57 instructions / 215 bytes / 0 edges) plus
//   the objdump listing build/retail-dis.txt for the same extent (0x0046F2B0-0x0046F386; the ret is at 0x0046F386, nine nops follow, and
//   0x0046F390 is the next function, which this body uses as a handler); `ps` at 0x0053FEE0 and 0x0053FEC4 for the two literals (byte-identical
//   to "AIStationaryGun" and "TASKTYPE_AISTATIONARYGUN"); `pxw` at 0x0053FEC0, which reads 0x0181 - so the id slot holds image value 0x0181, and
//   the script-name string at 0x0053FEC4 begins immediately after that four-byte-aligned slot; a full stack-pointer simulation over all 57
//   instructions, which returns esp to its entry value at the ret and accounts for every cleanup (0x38 = 14 pushes for the two inert arguments,
//   the registration and the first two links; 0xc three times; 0x14 once = the last link's three pushes plus the tail's two). Every callee was
//   re-read at its own VA. Argument numbers were derived by counting the pushes above each access, not by reading an [esp+N] offset as an
//   argument number.
// Behavior/CFG: one straight-line body that registers the "AIStationaryGun" task type of size 0x1E8 with parent Sys_GetGlobal16_4D9FA0() -> id
//   at 0x0053FEC0, then wires seven handler links: e0->0x0046EAE0 (0x00401400), e1->0x0046F390 and e2->0x00426860 (0x00401530), then the event
//   ids from 0x004C56D0->0x0046FA50, 0x004C56E0->0x0046FB50, 0x00489610->0x0046FB60 and 0x004D9FB0->0x0046FBA0 (all 0x00401400), and finishes
//   with the script-name tail ("TASKTYPE_AISTATIONARYGUN", AIStationaryGun id) through 0x004B8810. One basic block, no branch, no loop, no
//   frame, no parameter container, 14 call sites over 9 distinct callees.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The id slot 0x0053FEC0 holds image value **0x0181** and is written with a 16-bit store, so it
//   is declared as std::uint16_t with the image value; because 0x0181 is non-zero the slot belongs in .data rather than .bss, which is the
//   falsifiable prediction recorded for the 0x0181 pattern and which the compiled object confirms (nm reports it as `D`). This is the twelfth
//   sighting of that value and it is in the 0x0053FExx cluster, distinct from the 0x005401xx and 0x005402xx sightings from this lane. The
//   script-name string at 0x0053FEC4 starts four bytes after the id slot, so the two are adjacent in the image - the tail reads the id and
//   pushes the string that immediately follows it. The parent getter 0x004D9FA0 is the same parent 0x00477220 and 0x00472670 use, but the size
//   here is 0x1E8 against their 0x140 and 0x138, so the size is a per-registration fact taken from this body's own push immediate. Handlers
//   0x0046EAE0, 0x0046F390, 0x0046FA50, 0x0046FB60 and 0x0046FBA0 are referenced exactly once in the image - by this body - while 0x00426860 is
//   referenced eleven times and 0x0046FB50 twice (0x004383E4 and 0x0046F321 here), so the latter two are shared across registrars, one of them
//   from the 0x0043xxxx band below this lane. All are transcribed as retail writes them. Documented shape differences, all value-preserving:
//   (1) every getter callee is an argument-less load in retail that ignores the arguments the caller pushes, and the tree's transcriptions take
//   no arguments, so those inert pushes are not reproduced; each is simultaneously the next 0x00401400/0x00401530 link call's pre-pushed
//   handler, which the port keeps as the handler value in the same order; (2) byte-valued event ids are zero-extended in the port, which cannot
//   change behaviour because every link fed by a byte getter here goes through 0x00401400 or 0x00401530, which mask the event word for the phase
//   index. Handlers 0x0046EAE0, 0x0046F390, 0x0046FA50, 0x0046FB50, 0x0046FB60 and 0x0046FBA0 are raw code immediates in retail and are not
//   transcribed yet (native gaps in 0x0046xxxx-0x0048xxxx); handler 0x00426860 is BELOW 0x00460000 and belongs to the
//   0x00400000-0x0045FFFF owner. Domain: core/, because it is one of the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4D9FA0(void);            // 0x004D9FA0: argument-less word getter (parent type)
extern "C" std::uint8_t DynObj_GetByte_4C56D0(void);              // 0x004C56D0: argument-less byte getter (dynobj/native_4c56d0.cpp)
extern "C" std::uint8_t DynObj_GetByte_4C56E0(void);              // 0x004C56E0: argument-less byte getter (dynobj/native_4c56e0.cpp)
extern "C" std::uint8_t Sys_GetGlobal8_489610(void);              // 0x00489610: argument-less byte getter (core/native_489610.cpp)
extern "C" std::uint8_t Sys_GetGlobal8_4D9FB0(void);              // 0x004D9FB0: argument-less byte getter (core/native_4d9fb0.cpp)
extern "C" void Script_QHashNodeRegister(void* arg1, void* value); // 0x004B8810 (script/native_4b8810.cpp)

extern "C" {

// 0x0053FEC0: the "AIStationaryGun" task type id. Image value 0x0181, overwritten by a 16-bit store from TaskType_Register's return value.
std::uint16_t g_53fec0 = 0x0181U;

void Task_DefineAIStationaryGun()
{
    // 0x0046F2B0/0x0046F2B2: push 0; push 0 (both inert arguments of the parent getter); 0x0046F2B4: call 0x4D9FA0; 0x0046F2B9: push eax;
    // 0x0046F2BA: push 0x1E8; 0x0046F2BF: push "AIStationaryGun"; 0x0046F2C4: call 0x401900.
    const std::uint16_t ai_stationary_gun_id = TaskType_Register("AIStationaryGun", // 0x0046F2BF: push 0x0053FEE0
                                                                 0x1E8U, // 0x0046F2BA: push 0x1E8 (object size)
                                                                 Sys_GetGlobal16_4D9FA0(), // 0x0046F2B4: call 0x4D9FA0 (parent type)
                                                                 0U);     // 0x0046F2B2: push 0 (inert)

    // 0x0046F2C9: push 0x0046EAE0 (pre-pushed handler); 0x0046F2CE: push 0; 0x0046F2D0: push eax;
    // 0x0046F2D1: mov word [0x53FEC0],ax (the store lands between the pushes and the call); 0x0046F2D7: call 0x401400.
    ::g_53fec0 = ai_stationary_gun_id;
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53fec0), // 0x0046F2DC: mov ax,[0x53FEC0]
                                0U,                                     // 0x0046F2CE: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0046EAE0U))); // 0x0046F2C9: push handler

    // 0x0046F2E2: push 0x0046F390; 0x0046F2E7: push 1; 0x0046F2E9: push eax; 0x0046F2EA: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53fec0), // 0x0046F2DC: mov ax,[0x53FEC0]
                                           1U,                                     // 0x0046F2E7: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0046F390U))); // 0x0046F2E2: push handler

    // 0x0046F2EF: mov cx,[0x53FEC0]; 0x0046F2F6: push 0x00426860; 0x0046F2FB: push 2; 0x0046F2FD: push ecx; 0x0046F2FE: call 0x401530;
    // 0x0046F303: add esp,0x38 - the fourteen pushes above.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53fec0), // 0x0046F2EF: mov cx,[0x53FEC0]
                                           2U,                                     // 0x0046F2FB: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00426860U))); // 0x0046F2F6: push handler (below 0x00460000)

    // 0x0046F306: push 0x0046FA50; 0x0046F30B: call 0x4C56D0; 0x0046F310: mov dx,[0x53FEC0]; 0x0046F317: push eax; 0x0046F318: push edx;
    // 0x0046F319: call 0x401400; 0x0046F31E: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53fec0), // 0x0046F310: mov dx,[0x53FEC0]
                                static_cast<std::uint32_t>(DynObj_GetByte_4C56D0()), // 0x0046F317: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0046FA50U))); // 0x0046F306: push handler

    // 0x0046F321: push 0x0046FB50 (the handler a 0x0043xxxx body also references at 0x004383E4); 0x0046F326: call 0x4C56E0;
    // 0x0046F32B: push eax; 0x0046F32C: mov ax,[0x53FEC0]; 0x0046F332: push eax; 0x0046F333: call 0x401400; 0x0046F338: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53fec0), // 0x0046F32C: mov ax,[0x53FEC0]
                                static_cast<std::uint32_t>(DynObj_GetByte_4C56E0()), // 0x0046F32B: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0046FB50U))); // 0x0046F321: push handler

    // 0x0046F33B: push 0x0046FB60; 0x0046F340: call 0x489610; 0x0046F345: mov cx,[0x53FEC0]; 0x0046F34C: push eax; 0x0046F34D: push ecx;
    // 0x0046F34E: call 0x401400; 0x0046F353: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53fec0), // 0x0046F345: mov cx,[0x53FEC0]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_489610()), // 0x0046F34C: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0046FB60U))); // 0x0046F33B: push handler

    // 0x0046F356: push 0x0046FBA0; 0x0046F35B: call 0x4D9FB0; 0x0046F360: mov dx,[0x53FEC0]; 0x0046F367: push eax; 0x0046F368: push edx;
    // 0x0046F369: call 0x401400. This link has no cleanup of its own.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53fec0), // 0x0046F360: mov dx,[0x53FEC0]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_4D9FB0()), // 0x0046F367: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0046FBA0U))); // 0x0046F356: push handler

    // 0x0046F36E: mov eax,[0x53FEC0] (a 32-bit load); 0x0046F373: and eax,0xFFFF; 0x0046F378: push eax;
    // 0x0046F379: push "TASKTYPE_AISTATIONARYGUN" (0x0053FEC4, the four bytes that follow the id slot);
    // 0x0046F37E: call 0x4B8810; 0x0046F383: add esp,0x14 - the last link's three pushes plus the tail's two; 0x0046F386: ret.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_AISTATIONARYGUN"), // 0x0046F379: push 0x0053FEC4 (argument 1)
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(
                                 static_cast<std::uint32_t>(::g_53fec0)))); // 0x0046F373: the id masked to 16 bits (argument 2)
} // end of Task_DefineAIStationaryGun

} // extern "C"
