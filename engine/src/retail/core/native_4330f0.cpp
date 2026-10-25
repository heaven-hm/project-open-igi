// Native: Task_DefinePlane (behavior-derived placeholder name from the byte-identical name literal; retail symbol unknown); retail VA 0x004330F0-0x004333E8.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 761 bytes, 1 block, 218 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 218 instructions / 761 bytes / 0 edges) plus
//   the objdump listing build/retail-dis.txt for the same extent (0x004330F0-0x004333E8; the ret is at 0x004333E8 and seven nops follow, and
//   0x004333F0 is the next function, which this body uses as its first handler); `ps` at 0x0053BFC4 for the name literal (byte-identical to
//   "Plane") and at 0x0053BFB4 for the script name ("TASKTYPE_PLANE"); `px 2 @ 0x0053BEE8` for the id slot and `px 4` at 0x0057BD54,
//   0x0057BD48, 0x0057BD4C and 0x0057BD50 for the container and the three-word block; a stack-pointer simulation over all 218 instructions in
//   which the `sub esp,0x888` frame, every push and every `add esp` were applied in order, which returns esp to its entry value at the ret
//   (the last cleanup `add esp,0x8AC` is the 0x888 frame plus the final append's six pushes 0x18, plus the script tail's two pushes 0x8, plus
//   the one-push tail call 0x4) and which resolved every `lea reg,[esp+N]` to a frame-relative address. The ten append deltas were generated
//   mechanically from the dump with assertions (esp exactly 0x18 below the frame base at every 0x004F1A80 site, and exactly one of the two
//   frame operands of every append resolving to 0x00), then re-checked against retail. Argument numbers were derived by counting the pushes
//   above each access, not by reading an [esp+N] offset as an argument number.
// Behavior/CFG: one straight-line body. It allocates a 0x888-byte frame with a plain `sub esp,0x888` (NO stack probe in this body), registers
//   the "Plane" task type of size 0x888 with parent Sys_GetGlobal16_4F45F0() -> id at 0x0053BEE8, links e0->0x004333F0 through 0x00401400,
//   e1->0x00433690 and e2->0x00433790 through 0x00401530, then links nine more handlers through 0x00401400: 0x004F1A60->0x004337D0,
//   0x004F1A70->0x00434770, 0x004E6610->0x00434A30, 0x004F40D0->0x004337E0, 0x004F40E0->0x00434A50, 0x004F4110->0x00432BD0,
//   0x004F4140->0x00434A80, 0x004F4130->0x00434CD0 and 0x004F4120->0x00434DA0. It then allocates a TEN-entry parameter container through
//   Script_TaskContainer_Alloc(0xA) into 0x0057BD54, appends ten records through 0x004F1A80 with recorded deltas 0x168, 0x18C, 0x740, 0x180,
//   0x154, 0x831, 0x830, 0x6D8, 0x6D9 and 0x838 (four of them typed by Camera_Get_4F1000, 0x004F1000), and finishes with the script-name tail
//   Script_QHashNodeRegister("TASKTYPE_PLANE", id & 0xFFFF) at 0x004B8810 followed by Sys_StoreThreeWords_4B4720(&0x0057BD48) at 0x004B4720.
//   One basic block, no branch, no loop, 46 call sites over 24 distinct callees.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The container is allocated with count 0xA = 10 and there are exactly 10 appends, so no entry is
//   left uninitialised by this body. The id slot 0x0053BEE8 holds image value 385 = 0x0181, the same value as the id slots of 0x004310F0 and
//   0x00466E40, so it is initialised data (`D`) and its accesses are 16-bit. 0x0057BD54, 0x0057BD48, 0x0057BD4C and 0x0057BD50 all hold image
//   value 0, so they are zero-initialised data (`B`). **Arg6 of 0x004F1A80 was checked site by site in this body and is 0 at all ten appends** -
//   unlike 0x004310F0, where two sites push real values; the sixth argument is a real field of the record (the callee stores it at record
//   +0x08), so it is transcribed per site here rather than defaulted. **The append call sites in this body use different registers from the ones
//   in 0x004310F0**: here retail loads the container into %ecx, pushes the getter result, then computes arg3 into %edx and arg2 into %eax,
//   where 0x004310F0 computes arg3 into %ecx and arg2 into %edx. The deltas below are the values each site's frame operand actually resolves
//   to, so the register swap changes nothing in the port, which passes arguments by position; it is recorded because it is the kind of local
//   variation that must not be generalised across bodies. 0x0057BD48 is a THREE-dword block (0x0057BD48-0x0057BD53) written by
//   Sys_StoreThreeWords_4B4720 at +0x00, +0x04 and +0x08, so it is declared here as g_57bd48[3] and passed by address; the three values are
//   that callee's own immediates, not this body's. **Deferred pushes**: at all twelve link sites the handler immediate is pushed before the
//   event-id getter call and consumed as the link's third argument, with the getter sitting in between and never reading it - the shape recorded
//   for this lane, so each site is transcribed as a genuine three-argument call with the handler as arg3. 0x00432BD0 is registered here as a
//   Plane handler and is also registered by 0x004310F0 (the Heli type), so a single handler address can belong to two task types. The handler
//   immediates 0x004333F0, 0x00433690, 0x00433790, 0x004337D0, 0x00434770, 0x00434A30, 0x004337E0, 0x00434A50, 0x00432BD0, 0x00434A80,
//   0x00434CD0 and 0x00434DA0 are raw code immediates with no implementation in the tree yet; they are values here, not symbols. Documented shape
//   differences, all value-preserving: (1) every getter callee is an argument-less load in retail that ignores the arguments the caller pushes,
//   and the tree's transcriptions take no arguments, so those inert pushes are not reproduced; (2) byte-valued event ids are zero-extended in the
//   port, which cannot change behaviour because every link fed by a byte getter goes through 0x00401400 or 0x00401530, which mask the event word
//   for the phase index; (3) retail's 0x888-byte frame is reproduced as a single local slot whose address stands in for the frame base, the same
//   convention the accepted 0x004682F0, 0x00466E40, 0x00466F70, 0x0046E370, 0x004813E0 and 0x004310F0 files use; (4) the cleanups are
//   reproduced implicitly by the tree's calling convention rather than by explicit `add esp` arithmetic, since the port never pushes the inert
//   arguments; the retail sizes 0x38, 0xC (ten times), 0x10 and 0x8AC are recorded so a future reviewer can check the arithmetic.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4F45F0(void);            // 0x004F45F0: `mov ax,[0x54E200]; ret` (parent type; image 385 = 0x0181)
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);         // 0x004F1A60: argument-less byte getter (script/native_4f1a60.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);         // 0x004F1A70: argument-less byte getter (script/native_4f1a70.cpp)
extern "C" std::uint8_t Task_GetCachedEventId_4E6610(void);       // 0x004E6610: lazy one-shot event-id allocator (core/native_004e6610.cpp)
extern "C" std::uint8_t Sys_GetGlobal8_4F40D0(void);              // 0x004F40D0: argument-less byte getter
extern "C" std::uint8_t Sys_GetGlobal8_4F40E0(void);              // 0x004F40E0: argument-less byte getter
extern "C" std::uint8_t Sys_GetGlobal8_4F4110(void);              // 0x004F4110: argument-less byte getter
extern "C" std::uint8_t Sys_GetGlobal8_4F4140(void);              // 0x004F4140: argument-less byte getter
extern "C" std::uint8_t Sys_GetGlobal8_4F4130(void);              // 0x004F4130: argument-less byte getter
extern "C" std::uint8_t Sys_GetGlobal8_4F4120(void);              // 0x004F4120: argument-less byte getter
extern "C" std::uint32_t Sys_GetGlobal32_4F1010(void);            // 0x004F1010: `mov eax,[0xA70C88]; ret`
extern "C" std::uint32_t Sys_GetGlobal32_4F0F70(void);            // 0x004F0F70: `mov eax,[0xA70C6C]; ret`
extern "C" std::uint32_t Sys_GetGlobal32_4F0F50(void);            // 0x004F0F50: `mov eax,[0xA71870]; ret`
extern "C" std::uint32_t Sys_GetGlobal32_4F0F60(void);            // 0x004F0F60: `mov eax,[0xA71880]; ret`
extern "C" std::uint32_t Sys_GetGlobal32_4F0FC0(void);            // 0x004F0FC0: `mov eax,[0xA70C9C]; ret`
extern "C" std::uint32_t Core_GetRenderWordB_4F0FD0(void);        // 0x004F0FD0: `mov eax,[0xA71884]; ret` (core/native_4f0fd0.cpp)
extern "C" std::uint32_t Camera_Get_4F1000(void);                 // 0x004F1000: `mov eax,[0xA70C90]; ret` (render/native_4f1000.cpp)
extern "C" void* Script_TaskContainer_Alloc(std::int32_t count);  // 0x004F1950 (script/native_4f1950.cpp)
extern "C" void Script_QHashNodeRegister(void* const arg1, void* const value); // 0x004B8810 (script/native_4b8810.cpp)
extern "C" void Sys_StoreThreeWords_4B4720(void* target);         // 0x004B4720: stores three immediate dwords at target+0x00/+0x04/+0x08
// 0x004F1A80 (script/native_4f1a80.cpp). SIX arguments: arg5 is read as a byte (record +0x0C) and arg6 as a dword (record +0x08).
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5, std::int32_t const arg6);

extern "C" {

// 0x0053BEE8: the "Plane" task type id. Image value 385 = 0x0181, so it is initialised data (`D`); all accesses are 16-bit.
std::uint16_t g_53bee8 = 0x0181U;

// 0x0057BD48: a three-dword block (0x0057BD48-0x0057BD53) written by Sys_StoreThreeWords_4B4720 at +0x00/+0x04/+0x08. Image value 0.
std::uint32_t g_57bd48[3] = {};

// 0x0057BD54: the ten-entry parameter container. Image value 0, written and read as a dword.
std::uint32_t g_57bd54 = 0U;

void Task_DefinePlane()
{
    // 0x004330F0: sub esp,0x888 - a plain frame; this body has NO stack probe (contrast 0x004310F0).
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x004330F6: push 0; 0x004330F8: push 0 (the parent getter's inert argument, then TaskType_Register's fourth argument);
    // 0x004330FA: call 0x4F45F0; 0x004330FF: push eax; 0x00433100: push 0x888; 0x00433105: push "Plane"; 0x0043310A: call 0x401900.
    const std::uint16_t plane_id = TaskType_Register("Plane",                   // 0x00433105: push 0x0053BFC4
                                                     0x888U,                    // 0x00433100: push 0x888 (object size, equal to the frame)
                                                     Sys_GetGlobal16_4F45F0(),  // 0x004330FA: call 0x4F45F0 (parent type)
                                                     0U);                       // 0x004330F8: push 0 (fourth argument)
    ::g_53bee8 = plane_id; // 0x00433117: mov word [0x53BEE8],ax - the id store lands between the pushes of the first link

    // 0x43310F: push 0x004333F0 (handler, deferred argument 3); 0x433114: push 0x0 (argument 2); 0x43311D: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53bee8), 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x004333F0U)));

    // 0x433128: push 0x00433690 (handler, deferred argument 3); 0x43312D: push 0x1 (argument 2); 0x433130: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53bee8), 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00433690U)));

    // 0x43313C: push 0x00433790 (handler, deferred argument 3); 0x433141: push 0x2 (argument 2); 0x433144: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53bee8), 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00433790U)));

    // 0x43314C: push 0x004337D0 (handler, deferred argument 3); 0x43315F: call 0x4F1A60 -> argument 2; 0x43315F: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53bee8), static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x004337D0U)));

    // 0x433167: push 0x00434770 (handler, deferred argument 3); 0x433179: call 0x4F1A70 -> argument 2; 0x433179: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53bee8), static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00434770U)));

    // 0x433181: push 0x00434A30 (handler, deferred argument 3); 0x433194: call 0x4E6610 -> argument 2; 0x433194: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53bee8), static_cast<std::uint32_t>(Task_GetCachedEventId_4E6610()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00434A30U)));

    // 0x43319C: push 0x004337E0 (handler, deferred argument 3); 0x4331AF: call 0x4F40D0 -> argument 2; 0x4331AF: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53bee8), static_cast<std::uint32_t>(Sys_GetGlobal8_4F40D0()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x004337E0U)));

    // 0x4331B7: push 0x00434A50 (handler, deferred argument 3); 0x4331C9: call 0x4F40E0 -> argument 2; 0x4331C9: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53bee8), static_cast<std::uint32_t>(Sys_GetGlobal8_4F40E0()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00434A50U)));

    // 0x4331D1: push 0x00432BD0 (handler, deferred argument 3); 0x4331E4: call 0x4F4110 -> argument 2; 0x4331E4: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53bee8), static_cast<std::uint32_t>(Sys_GetGlobal8_4F4110()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00432BD0U)));

    // 0x4331EC: push 0x00434A80 (handler, deferred argument 3); 0x4331FF: call 0x4F4140 -> argument 2; 0x4331FF: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53bee8), static_cast<std::uint32_t>(Sys_GetGlobal8_4F4140()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00434A80U)));

    // 0x433207: push 0x00434CD0 (handler, deferred argument 3); 0x433219: call 0x4F4130 -> argument 2; 0x433219: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53bee8), static_cast<std::uint32_t>(Sys_GetGlobal8_4F4130()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00434CD0U)));

    // 0x433221: push 0x00434DA0 (handler, deferred argument 3); 0x433234: call 0x4F4120 -> argument 2; 0x433234: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53bee8), static_cast<std::uint32_t>(Sys_GetGlobal8_4F4120()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00434DA0U)));

    // 0x00433239: push 0xA (10 - Script_TaskContainer_Alloc's argument, and exactly the number of appends below);
    // 0x0043323B: call 0x4F1950; 0x00433240: add esp,0x10; 0x00433243: mov [0x57BD54],eax.
    ::g_57bd54 = reinterpret_cast<std::uint32_t>(Script_TaskContainer_Alloc(0xA)); // 0x00433239: push 0xA

    // 0x433248: push $0x0 (arg6); 0x43324C: call Sys_GetGlobal32_4F1010 (arg4); 0x433266: call 0x004F1A80 (frame+0x00, frame+0x168).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd54)),
                                append_frame, append_frame + 0x168,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1010()), 7, 0);

    // 0x43326E: push $0x0 (arg6); 0x433272: call Sys_GetGlobal32_4F0F70 (arg4); 0x43328C: call 0x004F1A80 (frame+0x00, frame+0x18C).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd54)),
                                append_frame, append_frame + 0x18C,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F70()), 7, 0);

    // 0x433294: push $0x0 (arg6); 0x433298: call Sys_GetGlobal32_4F0F50 (arg4); 0x4332B2: call 0x004F1A80 (frame+0x00, frame+0x740).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd54)),
                                append_frame, append_frame + 0x740,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, 0);

    // 0x4332BA: push $0x0 (arg6); 0x4332BE: call Sys_GetGlobal32_4F0F60 (arg4); 0x4332D8: call 0x004F1A80 (frame+0x00, frame+0x180).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd54)),
                                append_frame, append_frame + 0x180,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F60()), 7, 0);

    // 0x4332E0: push $0x0 (arg6); 0x4332E4: call Core_GetRenderWordB_4F0FD0 (arg4); 0x4332FE: call 0x004F1A80 (frame+0x00, frame+0x154).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd54)),
                                append_frame, append_frame + 0x154,
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);

    // 0x433306: push $0x0 (arg6); 0x43330A: call Camera_Get_4F1000 (arg4); 0x433324: call 0x004F1A80 (frame+0x00, frame+0x831).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd54)),
                                append_frame, append_frame + 0x831,
                                static_cast<std::int32_t>(Camera_Get_4F1000()), 7, 0);

    // 0x43332C: push $0x0 (arg6); 0x433330: call Camera_Get_4F1000 (arg4); 0x43334A: call 0x004F1A80 (frame+0x00, frame+0x830).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd54)),
                                append_frame, append_frame + 0x830,
                                static_cast<std::int32_t>(Camera_Get_4F1000()), 7, 0);

    // 0x433352: push $0x0 (arg6); 0x433356: call Camera_Get_4F1000 (arg4); 0x433370: call 0x004F1A80 (frame+0x00, frame+0x6D8).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd54)),
                                append_frame, append_frame + 0x6D8,
                                static_cast<std::int32_t>(Camera_Get_4F1000()), 7, 0);

    // 0x433378: push $0x0 (arg6); 0x43337C: call Camera_Get_4F1000 (arg4); 0x433396: call 0x004F1A80 (frame+0x00, frame+0x6D9).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd54)),
                                append_frame, append_frame + 0x6D9,
                                static_cast<std::int32_t>(Camera_Get_4F1000()), 7, 0);

    // 0x43339E: push $0x0 (arg6); 0x4333A2: call Sys_GetGlobal32_4F0FC0 (arg4); 0x4333BC: call 0x004F1A80 (frame+0x00, frame+0x838).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd54)),
                                append_frame, append_frame + 0x838,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);
    // 0x004333C1: mov edx,[0x53BEE8]; 0x004333C7: and edx,0xFFFF; 0x004333CD: push edx (argument 2); 0x004333CE: push 0x0053BFB4
    // (argument 1); 0x004333D3: call 0x4B8810; 0x004333D8: push 0x0057BD48 (argument 1); 0x004333DD: call 0x4B4720;
    // 0x004333E2: add esp,0x8AC - the 0x888 frame plus the final append's six pushes (0x18) plus the script tail's two pushes (0x8) plus
    // the last call's one push (0x4).
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_PLANE"), // 0x004333CE: push 0x0053BFB4
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(
                                 static_cast<std::uint32_t>(::g_53bee8) & 0xFFFFU))); // 0x004333CD: push edx
    Sys_StoreThreeWords_4B4720(&::g_57bd48[0]); // 0x004333D8: push 0x0057BD48
} // end of Task_DefinePlane

} // extern "C"
