// Native: Task_DefineRudder (behavior-derived placeholder name from the byte-identical name literal; retail symbol unknown); retail VA 0x004361D0-0x004362BD.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 238 bytes, 1 block, 62 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 62 instructions / 238 bytes / 0 edges) plus
//   the objdump listing build/retail-dis.txt for the same extent (0x004361D0-0x004362BD; the ret is at 0x004362BD and 0x004362C0 is the next
//   function, which this body uses as its fourth handler); `ps` at 0x0053C084 for the name literal (byte-identical to "Rudder") and at 0x0053C074,
//   0x0053C05C, 0x0053C044, 0x0053C034 and 0x0053C024 for the five script names; `px 2 @ 0x0057BD74` for the id slot; a push-by-push simulation
//   of the whole body, which starts at the entry esp (this body has NO `sub esp` and allocates NO local frame) and whose two cleanups close exactly:
//   `add esp,0x38` after the first three links is fourteen pushes (the two zeros, the parent-getter result, the size, the name and three pushes for
//   each of the first three links) and the final `add esp,0x34` is the last link's three pushes (0xC) plus five registrations of two pushes each
//   (0x28). Argument numbers were derived by counting the pushes above each access. Every callee was re-read at its own VA.
// Behavior/CFG: one straight-line body with no local frame. It registers the "Rudder" task type of size 0x140 with parent Sys_GetGlobal16_4D9FA0()
//   (0x004D9FA0: `mov ax,[0x548648]; ret`, image value 385 = 0x0181) -> id at 0x0057BD74, links e0->0x004360C0 through 0x00401400, e1->0x00488700
//   and e2->0x0051B940 through 0x00401530, then links three more handlers through 0x00401400: 0x004D9FB0->0x004362C0, 0x004C56D0->0x00436740
//   and 0x004C56E0->0x00427F80. It then makes FIVE hash-node registrations through 0x004B8810 - "TASKTYPE_RUDDER" with the masked id, then
//   "RUDDER_TAILERON_LEFT" with 1, "RUDDER_TAILERON_RIGHT" with 2, "RUDDER_AILERON" with 0 and "RUDDER_GAMMA" with 3, in that order. One basic
//   block, no branch, no loop, 16 call sites over 8 distinct callees. There is NO parameter container and NO 0x004F1A80 append in this body.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The id slot 0x0057BD74 holds image value 0, so it is zero-initialised data (`B`) and its
//   accesses are 16-bit; it continues the same 0x0057BDxx block as 0x0057BD5C/0x0057BD64/0x0057BD6C. **Deferred pushes**: at all six link sites
//   the handler immediate is pushed before the event-id getter call and consumed as the link's third argument, with the getter in between never
//   reading it. **The four extra registrations are NOT links and must not be folded into them**: they are five `Script_QHashNodeRegister` calls
//   whose second argument is an immediate small integer, and retail pushes them with no intervening getter call, so the handler-shaped immediates
//   in this tail (there are none - the tail has no 0x004xxxxx push at all) cannot be confused with them. The four constants 1, 2, 0 and 3 are
//   transcribed as pushed; the fourth registration carries 0 and the fifth carries 3, so the values are not in ascending order and the order of
//   the calls is part of the body. **Two of this body's handlers are already-implemented natives outside this lane**: e1 is 0x00488700 =
//   `Task_WalkAndDispatch_488700` (core/native_488700.cpp) and the last link's handler is 0x00427F80 = `Script_SetConstant_427F80`
//   (script/native_427f80.cpp). Both are passed here as the raw immediates retail pushes, so this file does not depend on either signature. The
//   remaining handler immediates 0x004360C0, 0x004362C0, 0x00436740 and 0x0051B940 are raw code immediates with no implementation in the tree
//   yet. Documented shape differences, all value-preserving: (1) every getter callee is an argument-less load in retail that ignores the
//   arguments the caller pushes, and the tree's transcriptions take no arguments, so those inert pushes are not reproduced; (2) byte-valued event
//   ids are zero-extended in the port, which cannot change behaviour because every link fed by a byte getter goes through 0x00401400 or
//   0x00401530, which mask the event word for the phase index; (3) the cleanups are reproduced implicitly by the tree's calling convention rather
//   than by explicit `add esp` arithmetic; the retail sizes 0x38 and 0x34 are recorded so a future reviewer can check the arithmetic.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4D9FA0(void);            // 0x004D9FA0: `mov ax,[0x548648]; ret` (parent type; image 385 = 0x0181)
extern "C" std::uint8_t Sys_GetGlobal8_4D9FB0(void);              // 0x004D9FB0: `mov al,[0x54864A]; ret` (image 255)
extern "C" std::uint8_t DynObj_GetByte_4C56D0(void);              // 0x004C56D0: argument-less byte accessor (dynobj/native_4c56d0.cpp)
extern "C" std::uint8_t DynObj_GetByte_4C56E0(void);              // 0x004C56E0: argument-less byte accessor (dynobj/native_4c56e0.cpp)
extern "C" void Script_QHashNodeRegister(void* const arg1, void* const value); // 0x004B8810 (script/native_4b8810.cpp)

extern "C" {

// 0x0057BD74: the "Rudder" task type id. Image value 0, so it is zero-initialised data (`B`); all accesses are 16-bit.
std::uint16_t g_57bd74 = 0U;

void Task_DefineRudder()
{
    // 0x004361D0: push 0; 0x004361D2: push 0 (the parent getter's inert argument, then TaskType_Register's fourth argument);
    // 0x004361D4: call 0x4D9FA0; 0x004361D9: push eax; 0x004361DA: push 0x140; 0x004361DF: push "Rudder"; 0x004361E4: call 0x401900.
    const std::uint16_t rudder_id = TaskType_Register("Rudder",                  // 0x004361DF: push 0x0053C084
                                                      0x140U,                    // 0x004361DA: push 0x140 (object size)
                                                      Sys_GetGlobal16_4D9FA0(),  // 0x004361D4: call 0x4D9FA0 (parent type)
                                                      0U);                       // 0x004361D2: push 0 (fourth argument)
    ::g_57bd74 = rudder_id; // 0x004361F1: mov word [0x57BD74],ax - the id store lands between the pushes of the first link

    // 0x004361E9: push 0x004360C0 (handler, deferred argument 3); 0x004361EE: push 0x0 (argument 2); 0x004361F7: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bd74), 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x004360C0U)));

    // 0x004361FC: mov ax,[0x57BD74]; 0x00436202: push 0x00488700 (handler, deferred argument 3); 0x00436207: push 0x1 (argument 2);
    // 0x0043620A: call 0x401530. 0x00488700 is the already-implemented Task_WalkAndDispatch_488700, but retail pushes it as a plain
    // code immediate here, so it is transcribed as the immediate.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57bd74), 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00488700U)));

    // 0x0043620F: mov cx,[0x57BD74]; 0x00436216: push 0x0051B940 (handler, deferred argument 3); 0x0043621B: push 0x2 (argument 2);
    // 0x0043621E: call 0x401530; 0x00436223: add esp,0x38 - fourteen pushes.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57bd74), 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0051B940U)));

    // 0x00436226: push 0x004362C0 (handler, deferred argument 3); 0x0043622B: call 0x4D9FB0 (event id; the pushed handler is not read);
    // 0x00436230: mov dx,[0x57BD74]; 0x00436237: push eax (argument 2); 0x00436238: push edx (argument 1); 0x00436239: call 0x401400;
    // 0x0043623E: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bd74),
                                static_cast<std::uint32_t>(Sys_GetGlobal8_4D9FB0()), // 0x00436237: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x004362C0U)));

    // 0x00436241: push 0x00436740 (handler, deferred argument 3); 0x00436246: call 0x4C56D0; 0x0043624B: push eax (argument 2);
    // 0x0043624C: mov ax,[0x57BD74]; 0x00436252: push eax (argument 1); 0x00436253: call 0x401400; 0x00436258: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bd74),
                                static_cast<std::uint32_t>(DynObj_GetByte_4C56D0()), // 0x0043624B: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00436740U)));

    // 0x0043625B: push 0x00427F80 (handler, deferred argument 3); 0x00436260: call 0x4C56E0; 0x00436265: mov cx,[0x57BD74];
    // 0x0043626C: push eax (argument 2); 0x0043626D: push ecx (argument 1); 0x0043626E: call 0x401400. This link has no cleanup of its own:
    // the tail's add esp,0x34 covers it together with the five registrations.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bd74),
                                static_cast<std::uint32_t>(DynObj_GetByte_4C56E0()), // 0x0043626C: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00427F80U)));

    // 0x00436273: mov edx,[0x57BD74]; 0x00436279: and edx,0xFFFF; 0x0043627F: push edx (argument 2); 0x00436280: push 0x0053C074
    // (argument 1); 0x00436285: call 0x4B8810.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_RUDDER"), // 0x00436280: push 0x0053C074
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(
                                 static_cast<std::uint32_t>(::g_57bd74) & 0xFFFFU))); // 0x0043627F: push edx

    // 0x0043628A: push 0x1 (argument 2); 0x0043628C: push 0x0053C05C (argument 1); 0x00436291: call 0x4B8810.
    Script_QHashNodeRegister(const_cast<char*>("RUDDER_TAILERON_LEFT"), // 0x0043628C: push 0x0053C05C
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // 0x0043628A: push 0x1

    // 0x00436296: push 0x2 (argument 2); 0x00436298: push 0x0053C044 (argument 1); 0x0043629D: call 0x4B8810.
    Script_QHashNodeRegister(const_cast<char*>("RUDDER_TAILERON_RIGHT"), // 0x00436298: push 0x0053C044
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(2U))); // 0x00436296: push 0x2

    // 0x004362A2: push 0x0 (argument 2); 0x004362A4: push 0x0053C034 (argument 1); 0x004362A9: call 0x4B8810.
    Script_QHashNodeRegister(const_cast<char*>("RUDDER_AILERON"), // 0x004362A4: push 0x0053C034
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // 0x004362A2: push 0x0

    // 0x004362AE: push 0x3 (argument 2); 0x004362B0: push 0x0053C024 (argument 1); 0x004362B5: call 0x4B8810;
    // 0x004362BA: add esp,0x34 - the last link's three pushes (0xC) plus these five registrations of two pushes each (0x28).
    Script_QHashNodeRegister(const_cast<char*>("RUDDER_GAMMA"), // 0x004362B0: push 0x0053C024
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(3U))); // 0x004362AE: push 0x3
} // end of Task_DefineRudder

} // extern "C"
