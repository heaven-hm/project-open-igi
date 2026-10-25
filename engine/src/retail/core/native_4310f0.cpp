// Native: Task_DefineHeli (behavior-derived placeholder name from the byte-identical name literal; retail symbol unknown); retail VA 0x004310F0-0x00431537.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 1096 bytes, 1 block, 312 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 312 instructions / 1096 bytes / 0 edges) plus
//   the objdump listing build/retail-dis.txt for the same extent (0x004310F0-0x00431537; the ret is at 0x00431537 and eight nops follow, and
//   0x00431540 is the next function, which this body uses as its first handler); `ps` at 0x0053BED0 for the name literal (byte-identical to
//   "Heli") and at 0x0053BEC0 for the script name ("TASKTYPE_HELI"); `px 2 @ 0x0053BE58`, `px 4 @ 0x0057BD40`, `px 4 @ 0x0057BD38` and
//   `px 4 @ 0x0057BD3C` for the four globals; a stack-pointer simulation over all 312 instructions in which the `mov eax,0x2D00; call
//   0x004A54A0` allocation was applied as a 0x2D00-byte esp drop and every push/add esp was applied in order, which returns esp to its entry
//   value at the ret and resolves every `lea reg,[esp+N]` to a frame-relative address. The 13 append deltas were generated mechanically from the
//   dump with assertions (esp exactly 0x18 below the frame base at every 0x004F1A80 site, the frame+0x00 operand of every append resolving to
//   0x00), then re-checked against retail. Argument numbers were derived by counting the pushes above each access.
// Behavior/CFG: one straight-line body. It allocates a 0x2D00-byte frame with the MSVC 6 stack-probe helper (`mov eax,0x2D00; call 0x004A54A0`
//   at 0x004310F0, with no `sub esp`), registers the "Heli" task type of size 0x2D00 with parent Sys_GetGlobal16_4F45F0() -> id at 0x0053BE58,
//   links e0->0x00431540 through 0x00401400, e1->0x004316D0 and e2->0x00431AE0 through 0x00401530, then links seventeen more handlers through
//   0x00401400: 0x004F1A60->0x00431B60, 0x004F1A70->0x00432950, 0x004E6610->0x00432BB0, 0x00416840->0x004D3710, 0x00416860->0x004D3710,
//   0x004F40D0->0x00431B70, 0x004F40E0->0x00432BE0, 0x004F4110->0x00432BD0, 0x004F4140->0x00432C10, 0x004F4130->0x00432F20,
//   0x004F4120->0x00432F40, 0x004EE0E0->0x00432F60, 0x004EE0F0->0x00432F80, 0x004CEA20->0x00432F90, 0x00489610->0x0042FC50,
//   0x0046AB20->0x004E4280 and 0x00489630->0x00433000. It then zeroes 0x0057BD38, writes 0x447A0000 to 0x0057BD3C, allocates a THIRTEEN-entry
//   parameter container through Script_TaskContainer_Alloc(0xD) into 0x0057BD40, appends thirteen records through 0x004F1A80 with recorded
//   deltas 0x168, 0x18C, 0x740, 0x180, 0x154, 0x6D8, 0x6D9, 0x758, 0x7A8, 0x7F8, 0x2CF0, 0x2CF4 and 0x2CF8, and finishes with the script-name
//   tail Script_QHashNodeRegister("TASKTYPE_HELI", id & 0xFFFF) at 0x004B8810. One basic block, no branch, no loop, 69 call sites over 34
//   distinct callees in retail; 68 over 33 in the port, the single difference being the stack-probe call (see Status/limits).
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The container is allocated with count 0xD = 13 and there are exactly 13 appends, so no entry
//   is left uninitialised by this body. The id slot 0x0053BE58 holds image value 385 = 0x0181, so it is initialised data (`D`) and its accesses
//   are 16-bit. 0x0057BD40, 0x0057BD38 and 0x0057BD3C all hold image value 0, so they are zero-initialised data (`B`); 0x0057BD3C receives the
//   raw bit pattern 0x447A0000, which is the 32-bit float 1000.0f - recorded as the raw dword the retail instruction stores, not as a claim about
//   what the field means. **The 0x004F1A80 call sites are not uniform in their sixth argument, and this is the first body in this lane where
//   that argument is non-zero.** Eleven appends push 0 as arg6, but append 3 (0x00431384) pushes the result of `call 0x004F1C90`
//   (Sys_InitFloatPair_4F1C90, which stores 0 and 0x40000000 at 0x00A76BF0/0x00A76BF4 and returns 0x00A76BF0) and append 13 (0x004314F5) pushes
//   the immediate address 0x0057BD38. Both are reproduced as real values rather than as 0. Because 0x004F1A80 reads arg6 as a dword (record
//   +0x08), the difference is observable in the container and must not be flattened. The 0x004F1C90 call is a real call with side effects whose
//   result retail keeps in %eax across the next getter call and two constant pushes; the port holds that result in a local so that the retail
//   call order 0x004F1C90 -> 0x004F0F50 -> 0x004F1A80 is fixed at compile time instead of depending on argument evaluation order. **Deferred
//   pushes**: at every one of the seventeen 0x00401400 links after the first, the handler immediate is pushed before the event-id getter call and
//   consumed as the link's third argument, with the getter sitting in between and never reading it - the shape recorded for this lane, so each
//   such site is transcribed as a genuine three-argument call with the handler as arg3. **Stack-probe shape**: `mov eax,0x2D00; call 0x004A54A0`
//   with no `sub esp`, following the precedent in core/native_46e370.cpp; the C++ frame replaces the allocation, which is why the port's call
//   table has 68 sites where retail has 69. 0x004D3710 is already implemented in this tree as QTask_NoOpHandler (script/native_4d3710.cpp) and is
//   still passed here as the raw immediate retail pushes, so the file does not depend on that symbol's signature. The other handler immediates
//   (0x00431540, 0x004316D0, 0x00431AE0, 0x00431B60, 0x00432950, 0x00432BB0, 0x00431B70, 0x00432BE0, 0x00432BD0, 0x00432C10, 0x00432F20,
//   0x00432F40, 0x00432F60, 0x00432F80, 0x00432F90, 0x00433000), 0x0042FC50 (another lane, below 0x00460000) and 0x004E4280 (above
//   0x0048FFFF) are raw code immediates with no implementation anywhere in the tree yet; they are values here, not symbols. Documented shape
//   differences, all value-preserving: (1) every getter callee is an argument-less load in retail that ignores the arguments the caller pushes,
//   and the tree's transcriptions take no arguments, so those inert pushes are not reproduced; (2) byte-valued event ids are zero-extended in the
//   port, which cannot change behaviour because every link fed by a byte getter here goes through 0x00401400 or 0x00401530, which mask the event
//   word for the phase index; (3) retail's 0x2D00-byte frame is reproduced as a single local slot whose address stands in for the frame base, the
//   same convention the accepted 0x004682F0, 0x00466E40, 0x00466F70, 0x0046E370 and 0x004813E0 files use; (4) the deferred cleanups are
//   reproduced implicitly by the tree's calling convention rather than by explicit `add esp` arithmetic, since the port never pushes the inert
//   arguments; the retail sizes 0x38, 0xC (sixteen times), 0x10 and 0x2D20 are recorded so a future reviewer can check the arithmetic - the last
//   one is the 0x2D00 frame plus the final append's six pushes (0x18) plus the script tail's two pushes (0x8). Domain: core/, because this is a
//   Task_Define* task-type registrar, the same subsystem as the other registrars on the startup list; the only 0x0043xxxx registrar already in the
//   tree (Task_DefineGenericPhysicsMagicObj, 0x0043E4F0) sits in dynobj/ because it registers a physics object, which this body is not.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4F45F0(void);            // 0x004F45F0: `mov ax,[0x54E200]; ret` (parent type; image 385 = 0x0181)
extern "C" std::uint8_t Sys_GetGlobal8_4F40D0(void);              // 0x004F40D0: argument-less byte getter
extern "C" std::uint8_t Sys_GetGlobal8_4F40E0(void);              // 0x004F40E0: argument-less byte getter
extern "C" std::uint8_t Sys_GetGlobal8_4F4110(void);              // 0x004F4110: argument-less byte getter
extern "C" std::uint8_t Sys_GetGlobal8_4F4140(void);              // 0x004F4140: argument-less byte getter
extern "C" std::uint8_t Sys_GetGlobal8_4F4130(void);              // 0x004F4130: argument-less byte getter
extern "C" std::uint8_t Sys_GetGlobal8_4F4120(void);              // 0x004F4120: argument-less byte getter
extern "C" std::uint8_t Sys_GetGlobal8_4EE0E0(void);              // 0x004EE0E0: argument-less byte getter
extern "C" std::uint8_t Sys_GetGlobal8_4EE0F0(void);              // 0x004EE0F0: argument-less byte getter
extern "C" std::uint8_t Sys_GetGlobal8_489610(void);              // 0x00489610: argument-less byte getter
extern "C" std::uint8_t Sys_GetGlobal8_489630(void);              // 0x00489630: argument-less byte getter
extern "C" std::uint8_t Sys_GetGlobal8_46AB20(void);              // 0x0046AB20: argument-less byte getter
extern "C" std::uint8_t Sys_GetSettingByte_416840(void);          // 0x00416840: `mov al,[0x539544]; ret` (event id; ignores its push)
extern "C" std::uint8_t Sys_GetSettingByte_416860(void);          // 0x00416860: `mov al,[0x53954B]; ret` (event id; ignores its push)
extern "C" std::uint8_t Task_GetCachedEventId_4E6610(void);       // 0x004E6610: lazy one-shot event-id allocator (core/native_004e6610.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);         // 0x004F1A60: argument-less byte getter (script/native_4f1a60.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);         // 0x004F1A70: argument-less byte getter (script/native_4f1a70.cpp)
extern "C" std::uint8_t QTask_PhaseIdGet548177_4CEA20(void);      // 0x004CEA20: argument-less byte getter (script/native_4cea20.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1010(void);            // 0x004F1010: `mov eax,[0xA70C88]; ret`
extern "C" std::uint32_t Sys_GetGlobal32_4F0F70(void);            // 0x004F0F70: `mov eax,[0xA70C6C]; ret`
extern "C" std::uint32_t Sys_GetGlobal32_4F0F50(void);            // 0x004F0F50: `mov eax,[0xA71870]; ret`
extern "C" std::uint32_t Sys_GetGlobal32_4F0F60(void);            // 0x004F0F60: `mov eax,[0xA70C68]; ret`
extern "C" std::uint32_t Sys_GetGlobal32_4F0F30(void);            // 0x004F0F30: `mov eax,[0xA70CA4]; ret`
extern "C" std::uint32_t Sys_GetGlobal32_4F0FC0(void);            // 0x004F0FC0: `mov eax,[0xA70C9C]; ret`
extern "C" std::uint32_t Core_GetRenderWordB_4F0FD0(void);        // 0x004F0FD0: `mov eax,[0xA71884]; ret` (core/native_4f0fd0.cpp)
extern "C" std::uint32_t Camera_Get_4F1000(void);                 // 0x004F1000: `mov eax,[0xA70C90]; ret` (render/native_4f1000.cpp)
extern "C" void* Sys_InitFloatPair_4F1C90(void);                  // 0x004F1C90: stores 0/0x40000000 at 0xA76BF0/0xA76BF4, returns 0xA76BF0
extern "C" void* Script_TaskContainer_Alloc(std::int32_t count);  // 0x004F1950 (script/native_4f1950.cpp)
extern "C" void Script_QHashNodeRegister(void* const arg1, void* const value); // 0x004B8810 (script/native_4b8810.cpp)
// 0x004F1A80 (script/native_4f1a80.cpp). SIX arguments: arg5 is read as a byte (record +0x0C) and arg6 as a dword (record +0x08).
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5, std::int32_t const arg6);

extern "C" {

// 0x0053BE58: the "Heli" task type id. Image value 385 = 0x0181, so it is initialised data (`D`); all accesses are 16-bit.
std::uint16_t g_53be58 = 0x0181U;

// 0x0057BD40: the thirteen-entry parameter container. Image value 0, written and read as a dword.
std::uint32_t g_57bd40 = 0U;

// 0x0057BD38: zeroed by this body and later pushed by address as the thirteenth append's arg6. Image value 0.
std::uint32_t g_57bd38 = 0U;

// 0x0057BD3C: receives the raw bit pattern 0x447A0000 (the 32-bit float 1000.0f). Image value 0.
std::uint32_t g_57bd3c = 0U;

void Task_DefineHeli()
{
    // 0x004310F0: mov eax,0x2D00; 0x004310F5: call 0x004A54A0 - the MSVC 6 stack probe, which lowers esp by 0x2D00 itself. The C++ frame
    // replaces it, following the precedent in core/native_46e370.cpp; the 0x2D00 bytes are also the registered object size.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x004310FA/0x004310FC: push 0; push 0 (the parent getter's inert argument, then TaskType_Register's fourth argument);
    // 0x004310FE: call 0x4F45F0; 0x00431103: push eax; 0x00431104: push 0x2D00; 0x00431109: push "Heli";
    // 0x0043110E: call 0x401900.
    const std::uint16_t heli_id = TaskType_Register("Heli",                   // 0x00431109: push 0x0053BED0
                                                    0x2D00U,                  // 0x00431104: push 0x2D00 (object size, equal to the frame)
                                                    Sys_GetGlobal16_4F45F0(), // 0x004310FE: call 0x4F45F0 (parent type)
                                                    0U);                      // 0x004310FC: push 0 (fourth argument)

    // 0x00431113: push 0x00431540 (pre-pushed handler); 0x00431118: push 0; 0x0043111A: push eax; 0x0043111B: mov word [0x53BE58],ax (the
    // id store lands between the pushes); 0x00431121: call 0x401400.
    ::g_53be58 = heli_id;
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53be58), // 0x0043111B: mov ax,[0x53BE58]
                                0U,                                     // 0x00431118: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00431540U))); // 0x00431113: push handler

    // 0x00431126: mov ax,[0x53BE58]; 0x0043112C: push 0x004316D0; 0x00431131: push 1; 0x00431133: push eax; 0x00431134: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53be58), // 0x00431126: mov ax,[0x53BE58]
                                           1U,                                     // 0x00431131: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004316D0U))); // 0x0043112C: push handler

    // 0x00431139: mov cx,[0x53BE58]; 0x00431140: push 0x00431AE0; 0x00431145: push 2; 0x00431147: push ecx; 0x00431148: call 0x401530;
    // 0x0043114D: add esp,0x38 - fourteen pushes: the two zeros, the parent-getter result, the size, the name and the three pushes of each of
    // the first three links.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53be58), // 0x00431139: mov cx,[0x53BE58]
                                           2U,                                     // 0x00431145: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00431AE0U))); // 0x00431140: push handler

    // 0x00431150: push 0x00431B60 (deferred arg3); 0x00431155: call 0x4F1A60 (event id; the pushed handler is not read);
    // 0x0043115A: mov dx,[0x53BE58]; 0x00431161: push eax (argument 2); 0x00431162: push edx (argument 1); 0x00431163: call 0x401400;
    // 0x00431168: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53be58), // 0x0043115A: mov dx,[0x53BE58]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()), // 0x00431161: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00431B60U))); // 0x00431150: push handler

    // 0x0043116B: push 0x00432950 (deferred arg3); 0x00431170: call 0x4F1A70; 0x00431175: push eax (argument 2);
    // 0x00431176: mov ax,[0x53BE58]; 0x0043117C: push eax (argument 1); 0x0043117D: call 0x401400; 0x00431182: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53be58), // 0x00431176: mov ax,[0x53BE58]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()), // 0x00431175: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00432950U))); // 0x0043116B: push handler

    // 0x00431185: push 0x00432BB0 (deferred arg3); 0x0043118A: call 0x4E6610 (lazy event-id allocation; the pushed handler is not read);
    // 0x0043118F: mov cx,[0x53BE58]; 0x00431196: push eax (argument 2); 0x00431197: push ecx (argument 1); 0x00431198: call 0x401400;
    // 0x0043119D: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53be58), // 0x0043118F: mov cx,[0x53BE58]
                                static_cast<std::uint32_t>(Task_GetCachedEventId_4E6610()), // 0x00431196: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00432BB0U))); // 0x00431185: push handler

    // 0x004311A0: push 0x004D3710 (deferred arg3; this address is already implemented in the tree as QTask_NoOpHandler, but retail pushes it
    // as a raw immediate and it is kept as one); 0x004311A5: call 0x416840 (setting byte; the pushed handler is not read);
    // 0x004311AA: mov dx,[0x53BE58]; 0x004311B1: push eax (argument 2); 0x004311B2: push edx (argument 1); 0x004311B3: call 0x401400;
    // 0x004311B8: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53be58), // 0x004311AA: mov dx,[0x53BE58]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416840()), // 0x004311B1: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004D3710U))); // 0x004311A0: push handler

    // 0x004311BB: push 0x004D3710 (deferred arg3); 0x004311C0: call 0x416860; 0x004311C5: push eax (argument 2);
    // 0x004311C6: mov ax,[0x53BE58]; 0x004311CC: push eax (argument 1); 0x004311CD: call 0x401400; 0x004311D2: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53be58), // 0x004311C6: mov ax,[0x53BE58]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416860()), // 0x004311C5: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004D3710U))); // 0x004311BB: push handler

    // 0x004311D5: push 0x00431B70 (deferred arg3); 0x004311DA: call 0x4F40D0; 0x004311DF: mov cx,[0x53BE58]; 0x004311E6: push eax;
    // 0x004311E7: push ecx; 0x004311E8: call 0x401400; 0x004311ED: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53be58), // 0x004311DF: mov cx,[0x53BE58]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_4F40D0()), // 0x004311E6: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00431B70U))); // 0x004311D5: push handler

    // 0x004311F0: push 0x00432BE0 (deferred arg3); 0x004311F5: call 0x4F40E0; 0x004311FA: mov dx,[0x53BE58]; 0x00431201: push eax;
    // 0x00431202: push edx; 0x00431203: call 0x401400; 0x00431208: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53be58), // 0x004311FA: mov dx,[0x53BE58]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_4F40E0()), // 0x00431201: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00432BE0U))); // 0x004311F0: push handler

    // 0x0043120B: push 0x00432BD0 (deferred arg3); 0x00431210: call 0x4F4110; 0x00431215: push eax (argument 2);
    // 0x00431216: mov ax,[0x53BE58]; 0x0043121C: push eax (argument 1); 0x0043121D: call 0x401400; 0x00431222: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53be58), // 0x00431216: mov ax,[0x53BE58]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_4F4110()), // 0x00431215: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00432BD0U))); // 0x0043120B: push handler

    // 0x00431225: push 0x00432C10 (deferred arg3); 0x0043122A: call 0x4F4140; 0x0043122F: mov cx,[0x53BE58]; 0x00431236: push eax;
    // 0x00431237: push ecx; 0x00431238: call 0x401400; 0x0043123D: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53be58), // 0x0043122F: mov cx,[0x53BE58]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_4F4140()), // 0x00431236: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00432C10U))); // 0x00431225: push handler

    // 0x00431240: push 0x00432F20 (deferred arg3); 0x00431245: call 0x4F4130; 0x0043124A: mov dx,[0x53BE58]; 0x00431251: push eax;
    // 0x00431252: push edx; 0x00431253: call 0x401400; 0x00431258: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53be58), // 0x0043124A: mov dx,[0x53BE58]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_4F4130()), // 0x00431251: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00432F20U))); // 0x00431240: push handler

    // 0x0043125B: push 0x00432F40 (deferred arg3); 0x00431260: call 0x4F4120; 0x00431265: push eax (argument 2);
    // 0x00431266: mov ax,[0x53BE58]; 0x0043126C: push eax (argument 1); 0x0043126D: call 0x401400; 0x00431272: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53be58), // 0x00431266: mov ax,[0x53BE58]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_4F4120()), // 0x00431265: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00432F40U))); // 0x0043125B: push handler

    // 0x00431275: push 0x00432F60 (deferred arg3); 0x0043127A: call 0x4EE0E0; 0x0043127F: mov cx,[0x53BE58]; 0x00431286: push eax;
    // 0x00431287: push ecx; 0x00431288: call 0x401400; 0x0043128D: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53be58), // 0x0043127F: mov cx,[0x53BE58]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_4EE0E0()), // 0x00431286: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00432F60U))); // 0x00431275: push handler

    // 0x00431290: push 0x00432F80 (deferred arg3); 0x00431295: call 0x4EE0F0; 0x0043129A: mov dx,[0x53BE58]; 0x004312A1: push eax;
    // 0x004312A2: push edx; 0x004312A3: call 0x401400; 0x004312A8: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53be58), // 0x0043129A: mov dx,[0x53BE58]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_4EE0F0()), // 0x004312A1: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00432F80U))); // 0x00431290: push handler

    // 0x004312AB: push 0x00432F90 (deferred arg3); 0x004312B0: call 0x4CEA20; 0x004312B5: push eax (argument 2);
    // 0x004312B6: mov ax,[0x53BE58]; 0x004312BC: push eax (argument 1); 0x004312BD: call 0x401400; 0x004312C2: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53be58), // 0x004312B6: mov ax,[0x53BE58]
                                static_cast<std::uint32_t>(QTask_PhaseIdGet548177_4CEA20()), // 0x004312B5: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00432F90U))); // 0x004312AB: push handler

    // 0x004312C5: push 0x0042FC50 (deferred arg3; below 0x00460000, another lane); 0x004312CA: call 0x489610; 0x004312CF: mov cx,[0x53BE58];
    // 0x004312D6: push eax (argument 2); 0x004312D7: push ecx (argument 1); 0x004312D8: call 0x401400; 0x004312DD: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53be58), // 0x004312CF: mov cx,[0x53BE58]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_489610()), // 0x004312D6: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0042FC50U))); // 0x004312C5: push handler

    // 0x004312E0: push 0x004E4280 (deferred arg3; above 0x0048FFFF, no implementation in the tree); 0x004312E5: call 0x46AB20;
    // 0x004312EA: mov dx,[0x53BE58]; 0x004312F1: push eax (argument 2); 0x004312F2: push edx (argument 1); 0x004312F3: call 0x401400;
    // 0x004312F8: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53be58), // 0x004312EA: mov dx,[0x53BE58]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_46AB20()), // 0x004312F1: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004E4280U))); // 0x004312E0: push handler

    // 0x004312FB: push 0x00433000 (deferred arg3); 0x00431300: call 0x489630; 0x00431305: push eax (argument 2);
    // 0x00431306: mov ax,[0x53BE58]; 0x0043130C: push eax (argument 1); 0x0043130D: call 0x401400. This link has no cleanup of its own: the
    // next add esp,0x10 covers it together with the container argument.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53be58), // 0x00431306: mov ax,[0x53BE58]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_489630()), // 0x00431305: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00433000U))); // 0x004312FB: push handler

    // 0x00431312: push 0xD (13 - Script_TaskContainer_Alloc's argument, and exactly the number of appends below);
    // 0x00431314: call 0x4F1950; 0x00431319: add esp,0x10; 0x0043131C: mov [0x57BD40],eax.
    ::g_57bd40 = reinterpret_cast<std::uint32_t>(Script_TaskContainer_Alloc(0xD)); // 0x00431312: push 0xD

    ::g_57bd38 = 0U;          // 0x00431321: mov dword [0x57BD38],0
    ::g_57bd3c = 0x447A0000U; // 0x0043132B: mov dword [0x57BD3C],0x447A0000 (the 32-bit float 1000.0f)

    // Append 1: 0x4310FA: push $0x0 (arg6); 0x431339: call Sys_GetGlobal32_4F1010 (the record type, arg4); 0x431352: call 0x4F1A80 with frame+0x00
    // and frame+0x168; 0x431357: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd40)),
                                append_frame, append_frame + 0x168,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1010()), 7, 0);

    // Append 2: 0x43135A: push $0x0 (arg6); 0x43135E: call Sys_GetGlobal32_4F0F70 (the record type, arg4); 0x431377: call 0x4F1A80 with frame+0x00
    // and frame+0x18C; 0x43137C: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd40)),
                                append_frame, append_frame + 0x18C,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F70()), 7, 0);

    // 0x43137F: call 0x4F1C90 (Sys_InitFloatPair_4F1C90): retail keeps its result in %eax and pushes it as this append's arg6,
    // so the call is a real call, not an inert push. The port holds the result in a local so that the retail call order
    // 0x4F1C90 -> 0x4F0F50 -> 0x4F1A80 is fixed at compile time instead of depending on argument evaluation order.
    void* const float_pair = Sys_InitFloatPair_4F1C90();
    // Append 3: 0x431384: push %eax (arg6); 0x431387: call Sys_GetGlobal32_4F0F50 (the record type, arg4); 0x4313A0: call 0x4F1A80 with frame+0x00
    // and frame+0x740; 0x4313A5: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd40)),
                                append_frame, append_frame + 0x740,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(float_pair)));

    // Append 4: 0x4313A8: push $0x0 (arg6); 0x4313AC: call Sys_GetGlobal32_4F0F60 (the record type, arg4); 0x4313C5: call 0x4F1A80 with frame+0x00
    // and frame+0x180; 0x4313CA: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd40)),
                                append_frame, append_frame + 0x180,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F60()), 7, 0);

    // Append 5: 0x4313CD: push $0x0 (arg6); 0x4313D1: call Core_GetRenderWordB_4F0FD0 (the record type, arg4); 0x4313EA: call 0x4F1A80 with frame+0x00
    // and frame+0x154; 0x4313EF: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd40)),
                                append_frame, append_frame + 0x154,
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);

    // Append 6: 0x4313F2: push $0x0 (arg6); 0x4313F6: call Camera_Get_4F1000 (the record type, arg4); 0x43140F: call 0x4F1A80 with frame+0x00
    // and frame+0x6D8; 0x431414: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd40)),
                                append_frame, append_frame + 0x6D8,
                                static_cast<std::int32_t>(Camera_Get_4F1000()), 7, 0);

    // Append 7: 0x431417: push $0x0 (arg6); 0x43141B: call Camera_Get_4F1000 (the record type, arg4); 0x431434: call 0x4F1A80 with frame+0x00
    // and frame+0x6D9; 0x431439: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd40)),
                                append_frame, append_frame + 0x6D9,
                                static_cast<std::int32_t>(Camera_Get_4F1000()), 7, 0);

    // Append 8: 0x43143C: push $0x0 (arg6); 0x431440: call Sys_GetGlobal32_4F0FC0 (the record type, arg4); 0x431459: call 0x4F1A80 with frame+0x00
    // and frame+0x758; 0x43145E: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd40)),
                                append_frame, append_frame + 0x758,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // Append 9: 0x431461: push $0x0 (arg6); 0x431465: call Sys_GetGlobal32_4F0FC0 (the record type, arg4); 0x43147E: call 0x4F1A80 with frame+0x00
    // and frame+0x7A8; 0x431483: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd40)),
                                append_frame, append_frame + 0x7A8,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // Append 10: 0x431486: push $0x0 (arg6); 0x43148A: call Sys_GetGlobal32_4F0FC0 (the record type, arg4); 0x4314A3: call 0x4F1A80 with frame+0x00
    // and frame+0x7F8; 0x4314A8: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd40)),
                                append_frame, append_frame + 0x7F8,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // Append 11: 0x4314AB: push $0x0 (arg6); 0x4314AF: call Sys_GetGlobal32_4F0F30 (the record type, arg4); 0x4314C8: call 0x4F1A80 with frame+0x00
    // and frame+0x2CF0; 0x4314CD: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd40)),
                                append_frame, append_frame + 0x2CF0,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F30()), 7, 0);

    // Append 12: 0x4314D0: push $0x0 (arg6); 0x4314D4: call Sys_GetGlobal32_4F0F30 (the record type, arg4); 0x4314ED: call 0x4F1A80 with frame+0x00
    // and frame+0x2CF4; 0x4314F2: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd40)),
                                append_frame, append_frame + 0x2CF4,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F30()), 7, 0);

    // Append 13: 0x4314F5: push $0x57bd38 (arg6); 0x4314FC: call Sys_GetGlobal32_4F0F50 (the record type, arg4); 0x431515: call 0x4F1A80 with frame+0x00
    // and frame+0x2CF8; 0x43151A: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd40)),
                                append_frame, append_frame + 0x2CF8,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57bd38)));


    // 0x0043151A: mov ecx,[0x53BE58]; 0x00431520: and ecx,0xFFFF; 0x00431526: push ecx (argument 2); 0x00431527: push 0x0053BEC0
    // (argument 1); 0x0043152C: call 0x4B8810; 0x00431531: add esp,0x2D20 - the 0x2D00 frame plus the final append's six pushes (0x18) plus
    // these two pushes (0x8).
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_HELI"), // 0x00431527: push 0x0053BEC0
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(
                                 static_cast<std::uint32_t>(::g_53be58) & 0xFFFFU))); // 0x00431526: push ecx
} // end of Task_DefineHeli

} // extern "C"
