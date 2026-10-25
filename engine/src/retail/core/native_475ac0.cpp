// Native: Task_DefineMine (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00475AC0-0x00475C0C.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 333 bytes, 1 block, 91 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj with bin.relocs.apply over the exact image (1 block / 91 instructions / 333 bytes / 0 edges) plus the
//   objdump listing build/retail-dis.txt for the same extent (0x00475AC0-0x00475C0C; the ret is at 0x00475C0C, three nops follow, and
//   0x00475C10 is the next function, which this body uses as a handler); `ps` at 0x00540628 and 0x00540618 for the two literals (byte-identical
//   to "Mine" and "TASKTYPE_MINE"); `pxw` at 0x005BE3A0, 0x005BE3A8 and 0x005BE39C for the image values; a full stack-pointer simulation over all
//   91 instructions, which returns esp to its entry value + 4 at the ret and accounts for every cleanup (the 0x250 frame, 0x38 = 14 pushes, 0xc
//   five times, 0x10 = the link's three pushes plus the 0x40, 0x8 = the container and the leftover 2, 0x18 = the first append's six, and
//   0x270 = 0x250 frame + 0x18 last append + 0x8 tail). Every callee was re-read at its own VA. Argument numbers were derived by counting the
//   pushes above each access, not by reading an [esp+N] offset as an argument number.
// Behavior/CFG: one straight-line body with a 0x250-byte frame - the same size it registers - that registers the "Mine" task type with parent
//   Sys_GetGlobal16_481340() -> id at 0x005BE3A0, then wires seven handler links: e0->0x00475C10 (0x00401400), e1->0x00475CC0 and
//   e2->0x00475D30 (0x00401530), then the event ids from 0x004F1A60->0x00475D80, 0x004F1A70->0x00475D90, 0x004167D0->0x00475DF0 and
//   0x00467D90->0x00475F30 (all 0x00401400). It then builds a task list with QTaskList_New(0x40) into 0x005BE3A8, grows the 0x005C1184 task
//   container by 2 through Script_TaskContainer_Grow(0x005C1184, 2) into 0x005BE39C, appends two parameter records to that container through
//   0x004F1A80 (type from 0x004F0F50; frame+0x244 and frame+0x240 against frame+0x00), and registers the script name
//   ("TASKTYPE_MINE", Mine id) through 0x004B8810. One basic block, no branch, no loop, 21 call sites over 14 distinct callees.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Documented shape differences, all value-preserving: (1) retail's 0x250-byte frame is
//   reproduced as a single local slot whose address stands in for the frame base; the two 0x004F1A80 calls pass frame+0x00 and frame+0x244 /
//   frame+0x240, exactly the addresses retail computes, and nothing reads the intervening bytes; (2) every getter callee is an argument-less load
//   in retail that ignores the arguments the caller pushes, and the tree's transcriptions take no arguments, so those inert pushes are not
//   reproduced; each is simultaneously the next 0x00401400 link call's pre-pushed handler, which the port keeps as the handler value in the same
//   order; (3) byte-valued event ids are zero-extended in the port, which cannot change behaviour because every link fed by a byte getter here
//   goes through 0x00401400, which masks the event word to its low byte for the phase index; (4) Script_TaskContainer_Grow's second argument is
//   pushed at 0x00475B93 BEFORE the argument-less 0x00481350 call and is left on the stack for the 0x004F1980 call to read, so retail reaches
//   it by stack reuse - the port passes it as the declared second parameter, the same value at the same call. The 0x005BE3A0 id slot holds image
//   value 0 and is written with a 16-bit store and read back at the tail with a 32-bit load masked by 0xFFFF, so it is a plain std::uint16_t; the
//   six bytes at 0x005BE3A2-0x005BE3A7 are referenced nowhere in build/retail-dis.txt and are NOT modelled here. 0x005BE3A8 and 0x005BE39C are
//   dword pointers written and read as dwords, so they are declared as std::uint32_t. Handlers 0x00475C10, 0x00475CC0, 0x00475D30, 0x00475D80,
//   0x00475D90, 0x00475DF0 and 0x00475F30 are raw code immediates in retail and are not transcribed yet (native gaps in
//   0x0046xxxx-0x0048xxxx). Domain: core/, because it is one of the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"
#include "igi/retail/core/qtask_list.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_481340(void);              // 0x00481340: argument-less word getter (parent type)
extern "C" std::uint32_t Sys_GetGlobal32_481350(void);              // 0x00481350: argument-less dword getter (the 0x005C1184 container)
extern "C" std::uint8_t Sys_GetSettingByte_4167D0(void);            // 0x004167D0: argument-less byte getter
extern "C" std::uint8_t Task_GetCachedEventId_467D90(void);         // 0x00467D90: lazily allocated event id (core/native_00467d90.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0F50(void);              // 0x004F0F50: argument-less dword getter (the appended type)
extern "C" void* Script_TaskContainer_Grow(std::int32_t* const container, std::int32_t const count); // 0x004F1980
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);           // 0x004F1A60: argument-less byte getter (script_state.hpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);           // 0x004F1A70: argument-less byte getter (script_state.hpp)
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5); // 0x004F1A80 (script/native_4f1a80.cpp)
extern "C" void Script_QHashNodeRegister(void* arg1, void* value);  // 0x004B8810 (script/native_4b8810.cpp)

extern "C" {

// 0x005BE3A0: the "Mine" task type id. Image value 0, overwritten by a 16-bit store from TaskType_Register's return value.
std::uint16_t g_5be3a0 = 0U;

// 0x005BE3A8: the task list built by QTaskList_New(0x40). Image value 0, written and read as a dword.
std::uint32_t g_5be3a8 = 0U;

// 0x005BE39C: the task container grown from 0x005C1184. Image value 0, written and read as a dword.
std::uint32_t g_5be39c = 0U;

void Task_DefineMine()
{
    // 0x00475AC0: sub esp,0x250 - retail's 592-byte frame, the same size it passes to TaskType_Register as the object size. The two
    // 0x004F1A80 calls below pass two addresses inside it: its first dword (each `lea ecx,[esp+0xc]` runs with 12 bytes of arguments already
    // pushed, so the address is frame+0x00) and one later slot each. 0x004F1A80 reads those two arguments at [esp+8]/[esp+0xC], subtracts them at
    // 0x004F1A9D and stores the difference, so the two recorded deltas are 0x244 and 0x240.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x00475AC6/0x00475AC8: push 0; push 0 (both inert arguments of the parent getter); 0x00475ACA: call 0x481340; 0x00475ACF: push eax;
    // 0x00475AD0: push 0x250; 0x00475AD5: push "Mine"; 0x00475ADA: call 0x401900.
    const std::uint16_t mine_id = TaskType_Register("Mine", // 0x00475AD5: push 0x00540628
                                                    0x250U, // 0x00475AD0: push 0x250 (object size)
                                                    Sys_GetGlobal16_481340(), // 0x00475ACA: call 0x481340 (parent type)
                                                    0U);    // 0x00475AC8: push 0 (inert)

    // 0x00475ADF: push 0x00475C10 (pre-pushed handler); 0x00475AE4: push 0; 0x00475AE6: push eax;
    // 0x00475AE7: mov word [0x5BE3A0],ax (the store lands between the pushes and the call); 0x00475AED: call 0x401400.
    ::g_5be3a0 = mine_id;
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be3a0), // 0x00475AF2: mov ax,[0x5BE3A0]
                                0U,                                     // 0x00475AE4: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00475C10U))); // 0x00475ADF: push handler

    // 0x00475AF8: push 0x00475CC0; 0x00475AFD: push 1; 0x00475AFF: push eax; 0x00475B00: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5be3a0), // 0x00475AF2: mov ax,[0x5BE3A0]
                                           1U,                                     // 0x00475AFD: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00475CC0U))); // 0x00475AF8: push handler

    // 0x00475B05: mov cx,[0x5BE3A0]; 0x00475B0C: push 0x00475D30; 0x00475B11: push 2; 0x00475B13: push ecx; 0x00475B14: call 0x401530;
    // 0x00475B19: add esp,0x38 - 14 pushes above.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5be3a0), // 0x00475B05: mov cx,[0x5BE3A0]
                                           2U,                                     // 0x00475B11: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00475D30U))); // 0x00475B0C: push handler

    // 0x00475B1C: push 0x00475D80; 0x00475B21: call 0x4F1A60; 0x00475B26: mov dx,[0x5BE3A0]; 0x00475B2D: push eax; 0x00475B2E: push edx;
    // 0x00475B2F: call 0x401400; 0x00475B34: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be3a0), // 0x00475B26: mov dx,[0x5BE3A0]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()), // 0x00475B2D: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00475D80U))); // 0x00475B1C: push handler

    // 0x00475B37: push 0x00475D90; 0x00475B3C: call 0x4F1A70; 0x00475B41: push eax; 0x00475B42: mov ax,[0x5BE3A0]; 0x00475B48: push eax;
    // 0x00475B49: call 0x401400; 0x00475B4E: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be3a0), // 0x00475B42: mov ax,[0x5BE3A0]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()), // 0x00475B41: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00475D90U))); // 0x00475B37: push handler

    // 0x00475B51: push 0x00475DF0; 0x00475B56: call 0x4167D0; 0x00475B5B: mov cx,[0x5BE3A0]; 0x00475B62: push eax; 0x00475B63: push ecx;
    // 0x00475B64: call 0x401400; 0x00475B69: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be3a0), // 0x00475B5B: mov cx,[0x5BE3A0]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_4167D0()), // 0x00475B62: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00475DF0U))); // 0x00475B51: push handler

    // 0x00475B6C: push 0x00475F30; 0x00475B71: call 0x467D90; 0x00475B76: mov dx,[0x5BE3A0]; 0x00475B7D: push eax; 0x00475B7E: push edx;
    // 0x00475B7F: call 0x401400; 0x00475B84: push 0x40; 0x00475B86: call 0x4C1800 (QTaskList_New); 0x00475B8B: add esp,0x10 - the link's
    // three pushes plus the 0x40.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be3a0), // 0x00475B76: mov dx,[0x5BE3A0]
                                static_cast<std::uint32_t>(Task_GetCachedEventId_467D90()), // 0x00475B7D: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00475F30U))); // 0x00475B6C: push handler

    // 0x00475B8E: mov [0x5BE3A8],eax.
    ::g_5be3a8 = reinterpret_cast<std::uint32_t>(QTaskList_New(0x40)); // 0x00475B84: push 0x40

    // 0x00475B93: push 2 (Script_TaskContainer_Grow's second argument, pushed BEFORE the argument-less getter and left on the stack);
    // 0x00475B95: call 0x481350; 0x00475B9A: push eax (the 0x005C1184 container); 0x00475B9B: call 0x4F1980; 0x00475BA0: add esp,0x8.
    ::g_5be39c = reinterpret_cast<std::uint32_t>(Script_TaskContainer_Grow(
        reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(Sys_GetGlobal32_481350())), // 0x00475B9A: push eax
        2)); // 0x00475B93: push 2

    // Append 1: 0x00475BA8/0x00475BAA: push 0/7 (inert); 0x00475BAC: call 0x4F0F50; 0x00475BB1: mov edx,[0x5BE39C]; 0x00475BB7: push eax;
    // 0x00475BB8: lea eax,[esp+0x250] (frame+0x244); 0x00475BBF: lea ecx,[esp+0xc] (frame+0x00); 0x00475BC3: push eax; 0x00475BC4: push ecx;
    // 0x00475BC5: push edx; 0x00475BC6: call 0x4F1A80; 0x00475BCB: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5be39c)),
                                append_frame,                                 // argument 2: retail frame+0x00
                                append_frame + 0x244,                         // argument 3: retail frame+0x244 (recorded difference 0x244)
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), // argument 4: the type from 0x4F0F50
                                7);                                           // argument 5: retail's leftover 7

    // Append 2: 0x00475BCE/0x00475BD0: push 0/7; 0x00475BD2: call 0x4F0F50; 0x00475BD7: push eax; 0x00475BD8: lea eax,[esp+0x24c]
    // (frame+0x240); 0x00475BDF: lea ecx,[esp+0xc] (frame+0x00); 0x00475BE3: push eax; 0x00475BE4: push ecx; 0x00475BE5: mov edx,[0x5BE39C];
    // 0x00475BEB: push edx; 0x00475BEC: call 0x4F1A80. This append has no cleanup of its own: 0x00475C06's add esp,0x270 covers its six
    // pushes, the tail's two, and the frame.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5be39c)),
                                append_frame,
                                append_frame + 0x240,                         // 0x00475BD8: frame+0x240 (recorded difference 0x240)
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()),
                                7);

    // 0x00475BF1: mov eax,[0x5BE3A0] (a 32-bit load); 0x00475BF6: and eax,0xFFFF; 0x00475BFB: push eax;
    // 0x00475BFC: push "TASKTYPE_MINE"; 0x00475C01: call 0x4B8810; 0x00475C06: add esp,0x270; 0x00475C0C: ret.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_MINE"), // 0x00475BFC: push 0x00540618 (argument 1)
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(
                                 static_cast<std::uint32_t>(::g_5be3a0)))); // 0x00475BF6: the id masked to 16 bits (argument 2)
} // end of Task_DefineMine

} // extern "C"
