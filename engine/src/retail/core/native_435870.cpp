// Native: Task_DefineGear (behavior-derived placeholder name from the byte-identical name literal; retail symbol unknown); retail VA 0x00435870-0x0043592D.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 190 bytes, 1 block, 50 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 50 instructions / 190 bytes / 0 edges) plus
//   the objdump listing build/retail-dis.txt for the same extent (0x00435870-0x0043592D; the ret is at 0x0043592D and 0x0x00435930 is the next function, which this
//   body uses as its second handler); `ps` at 0xGearVA for the name literal (byte-identical to "Gear") and at 0xTASKTYPE_GEARVA for the script name
//   ("TASKTYPE_GEAR"); `px 2 @ 0x0057BD64` for the id slot; a push-by-push simulation of the whole body, which starts at the entry esp (this body has NO
//   `sub esp` and allocates NO local frame - the 0x140 it registers is never reserved on the stack) and ends with the final `add esp,0x14`, which
//   is exactly the last link's three pushes (0xC) plus the script tail's two pushes (0x8). Argument numbers were derived by counting the pushes
//   above each access. Every callee was re-read at its own VA. This body is instruction-for-instruction the shape of 0x004351E0 (CockpitShield)
//   apart from its name, its id slot and its handler immediates, which were compared site by site rather than assumed.
// Behavior/CFG: one straight-line body with no local frame. It registers the "Gear" task type of size 0x140 with parent Sys_GetGlobal16_4D9FA0()
//   (0x004D9FA0: `mov ax,[0x548648]; ret`, image value 385 = 0x0181) -> id at 0x0057BD64, links e0->0x00435460 through 0x00401400, e1->0x00435930 and
//   e2->0x0051B940 through 0x00401530, then links three more handlers through 0x00401400: 0x004D9FB0->0x00435A80, 0x004C56D0->0x00435BD0 and
//   0x004C56E0->0x004EFF20. It then ends with the script-name tail Script_QHashNodeRegister("TASKTYPE_GEAR", id & 0xFFFF) at 0x004B8810. One basic block,
//   no branch, no loop, 12 call sites over 8 distinct callees. There is NO parameter container and NO 0x004F1A80 append in this body.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The id slot 0x0057BD64 holds image value 0, so it is zero-initialised data (`B`) and its accesses
//   are 16-bit; it is the same 0x0057BDxx block the sibling registrars 0x004330F0, 0x004351E0 and 0x00435D90 use, each one task type further along.
//   **Deferred pushes**: at all six link sites the handler immediate is pushed before the event-id getter call and consumed as the link's third
//   argument, with the getter sitting in between and never reading it. Two of the three getters are not byte getters of the 0x004F4xxx family but
//   0x004D9FB0 (`mov al,[0x54864A]; ret`, image value 255) and the dynobj accessors 0x004C56D0/0x004C56E0, which is why the callee set of this
//   body is unusual for a registrar; they are transcribed as the argument-less calls retail makes. **0x004EFF20 is already implemented in this tree as Script_SetConstant_4EFF20 (script/native_4eff20.cpp)**, above 0x0048FFFF and therefore outside this lane; it is still passed here as the raw immediate retail pushes, so this file does not depend on that symbol's signature. The remaining handler immediates
//   0x00435460, 0x00435930, 0x00435A80, 0x00435BD0 are raw code immediates with no implementation in the tree yet. Documented shape differences, all value-preserving: (1) every
//   getter callee is an argument-less load in retail that ignores the arguments the caller pushes, and the tree's transcriptions take no
//   arguments, so those inert pushes are not reproduced; (2) byte-valued event ids are zero-extended in the port, which cannot change behaviour
//   because every link fed by a byte getter goes through 0x00401400 or 0x00401530, which mask the event word for the phase index; (3) the
//   cleanups are reproduced implicitly by the tree's calling convention rather than by explicit `add esp` arithmetic; the retail sizes 0x38 and
//   0x14 are recorded so a future reviewer can check the arithmetic. 0x0051B940 is registered as a handler by 0x004351E0, by 0x00435870 and by
//   0x00435D90, so one handler address serves three task types.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4D9FA0(void);            // 0x004D9FA0: `mov ax,[0x548648]; ret` (parent type; image 385 = 0x0181)
extern "C" std::uint8_t Sys_GetGlobal8_4D9FB0(void);              // 0x004D9FB0: `mov al,[0x54864A]; ret` (image 255)
extern "C" std::uint8_t DynObj_GetByte_4C56D0(void);              // 0x004C56D0: argument-less byte accessor (dynobj/native_4c56d0.cpp)
extern "C" std::uint8_t DynObj_GetByte_4C56E0(void);              // 0x004C56E0: argument-less byte accessor (dynobj/native_4c56e0.cpp)
extern "C" void Script_QHashNodeRegister(void* const arg1, void* const value); // 0x004B8810 (script/native_4b8810.cpp)

extern "C" {

// 0x0057BD64: the "Gear" task type id. Image value 0, so it is zero-initialised data (`B`); all accesses are 16-bit.
std::uint16_t g_57bd64 = 0U;

void Task_DefineGear()
{
    // 0x00435870: push 0; 0x004358702: push 0 (the parent getter's inert argument, then TaskType_Register's fourth argument);
    // 0x004358703: call 0x4D9FA0; 0x004358704: push eax; 0x004358705: push 0x140; 0x004358706: push "Gear"; 0x004358707: call 0x401900.
    const std::uint16_t gear_id = TaskType_Register("Gear",                  // 0x004358706: push 0xGearVA
                                                    0x140U,                   // 0x004358705: push 0x140 (object size)
                                                    Sys_GetGlobal16_4D9FA0(), // 0x004358703: call 0x4D9FA0 (parent type)
                                                    0U);                      // 0x004358702: push 0 (fourth argument)
    ::g_57bd64 = gear_id; // 0x00435891: mov word [0x0057BD64],ax - the id store lands between the pushes of the first link

    // 0x00435889: push 0x00435460 (handler, deferred argument 3); 0x00435889B: push 0x0 (argument 2); 0x00435889C: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bd64), 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00435460U)));

    // 0x0043589C: push 0x00435930 (handler, deferred argument 3); 0x0043589CB: push 0x1 (argument 2); 0x0043589CC: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57bd64), 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00435930U)));

    // 0x004358AF: push 0x0051B940 (handler, deferred argument 3); 0x004358AFB: push 0x2 (argument 2); 0x004358AFC: call 0x401530;
    // 0x004358AFD: add esp,0x38 - fourteen pushes: the two zeros, the parent-getter result, the size, the name and the three pushes of each of the
    // first three links.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57bd64), 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0051B940U)));

    // 0x004358C6: push 0x00435A80 (handler, deferred argument 3); 0x004358C6B: call 0x4D9FB0 (event id; the pushed handler is not read);
    // 0x004358C6C: mov dx,[0x0057BD64]; 0x004358C6D: push eax (argument 2); 0x004358C6E: push edx (argument 1); 0x004358C6F: call 0x401400; 0x004358C6G: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bd64),
                                static_cast<std::uint32_t>(Sys_GetGlobal8_4D9FB0()), // 0x004358C6D: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00435A80U)));

    // 0x004358E1: push 0x00435BD0 (handler, deferred argument 3); 0x004358E1B: call 0x4C56D0; 0x004358E1C: push eax (argument 2);
    // 0x004358E1D: mov ax,[0x0057BD64]; 0x004358E1E: push eax (argument 1); 0x004358E1F: call 0x401400; 0x004358E1G: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bd64),
                                static_cast<std::uint32_t>(DynObj_GetByte_4C56D0()), // 0x004358E1C: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00435BD0U)));

    // 0x004358FB: push 0x004EFF20 (handler, deferred argument 3); 0x004358FBB: call 0x4C56E0; 0x004358FBC: mov cx,[0x0057BD64]; 0x004358FBD: push eax (argument 2);
    // 0x004358FBE: push ecx (argument 1); 0x004358FBF: call 0x401400. This link has no cleanup of its own: the tail's add esp,0x14 covers it together
    // with the script-name pushes.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bd64),
                                static_cast<std::uint32_t>(DynObj_GetByte_4C56E0()), // 0x004358FBD: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x004EFF20U)));

    // 0x00435913: mov edx,[0x0057BD64]; 0x00435919: and edx,0xFFFF; 0x0043591F: push edx (argument 2); 0x00435920: push 0xTASKTYPE_GEARVA
    // (argument 1); 0x00435925: call 0x4B8810; 0x0043592A: add esp,0x14 - the last link's three pushes (0xC) plus these two (0x8).
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_GEAR"), // 0x00435920: push 0xTASKTYPE_GEARVA
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(
                                 static_cast<std::uint32_t>(::g_57bd64) & 0xFFFFU))); // 0x0043591F: push edx
} // end of Task_DefineGear

} // extern "C"
