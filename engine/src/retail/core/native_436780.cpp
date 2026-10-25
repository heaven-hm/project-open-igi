// Native: Task_DefineRainEffect (behavior-derived placeholder name from the byte-identical name literal; retail symbol unknown); retail VA 0x00436780-0x00436945.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 442 bytes, 1 block, 123 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 123 instructions / 442 bytes / 0 edges) plus
//   the objdump listing build/retail-dis.txt for the same extent (0x00436780-0x00436945; the ret is at 0x00436945 and 0x00436950 is the next
//   function, which this body uses as its first handler); `ps` at 0x0053C098 for the name literal (byte-identical to "RainEffect") and at 0x0053C08C
//   for the texture name ("snow.tex"); `px 4` at 0x0057BD80/0x0057BD84/0x0057BD88/0x0057BD90/0x0057BD94/0x0057BD98/0x0057BD9C (all image value 0);
//   0x004018F0 re-read at its own VA (`mov ax,[0x54F928]; ret`, image 0) for the parent; 0x004F1210 and 0x004F1220 re-read at their own VAs
//   (`mov eax,[0x54DDA8]; ret` and `mov eax,[0x54DDAC]; ret`, both image 255) because these two links feed them as the event id getters. A
//   push-by-push simulation of the whole body: `sub esp,0x88`, every push, every `add esp` in order, ending with the final `add esp,0xA4` =
//   0x88 frame + the last append's six pushes (0x18) + the tail call's one push (0x4); every `lea reg,[esp+N]` was resolved to frame+0x00 for the
//   second argument and to the recorded delta for the third. The five deltas were generated mechanically with assertions (esp exactly 0x18 below
//   the frame base at every 0x004F1A80 site, and exactly one frame operand resolving to 0x00) and then re-checked against retail. Argument
//   numbers come from counting pushes, not from reading [esp+N] as an argument number.
// Behavior/CFG: one straight-line body. It allocates a 0x88-byte frame with `sub esp,0x88` (no stack probe), and its first four instructions
//   initialise four dwords of the 0x0057BD90 block before anything else: 0x40A00000 (5.0f) at 0x0057BD98, 0x43160000 (150.0f) at 0x0057BD9C,
//   0 at 0x0057BD90 and 0x3F800000 (1.0f) at 0x0057BD94 - the third of the four stores is interleaved between the pushes of the registration, so
//   the store order 98, 9C, 90, 94 is transcribed as retail has it. It then registers the "RainEffect" task type of size 0x88 with parent
//   QTask_GetCurrentType() -> id at 0x0057BD84, and links seven handlers: e0->0x00436950 through 0x00401400, e2->0x004369E0 through
//   0x00401530, 0x004F1A60->0x00436A40 and 0x004F1A70->0x00436A50 through 0x00401400, e1->0x00436990 through 0x00401530, and
//   0x004F1210->0x004D3710 and 0x004F1220->0x004D3710 through 0x00401530. It then allocates a FIVE-entry parameter container through
//   Script_TaskContainer_Alloc(5) into 0x0057BD88 and appends five records through 0x004F1A80 with recorded deltas 0x24, 0x28, 0x2C, 0x38 and
//   0x30. It finishes by calling Sys_LookupOrAppendRecord_416930("snow.tex") and storing the returned pointer at 0x0057BD80. One basic block, no
//   branch, no loop, 25 call sites over 14 distinct callees.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. All seven globals hold image value 0, so they are zero-initialised data (`B`). The container is
//   allocated with count 5 and there are exactly 5 appends. **Arg6 of 0x004F1A80 was derived per site and is NOT constant here**: 0 at appends 1
//   and 4, the address 0x0057BD98 at appends 2 and 3, and the address 0x0057BD90 at append 5 - so this body pushes real global addresses as the
//   sixth argument, which is the third distinct arg6 pattern found in this lane (0x004310F0 pushes a getter result and a global address, this body
//   pushes two different global addresses, and 0x004330F0 pushes 0 at every site). **The body does NOT use 0x004B8810 at all**; its only
//   name-to-value registration is the 0x00416930 call in the tail, which takes one argument and returns a pointer, and that pointer - not the id -
//   is what gets stored. 0x004D3710 (`QTask_NoOpHandler`) is pushed twice as a handler; it is a real code address that is also the tree's no-op
//   handler, so the two links that use it are transcribed with the raw immediate like every other handler here. The two getters 0x004F1210 and
//   0x004F1220 return the DWORD image value 255 from 0x0054DDA8 and 0x0054DDAC, so their results are ints, not bytes, and are widened to
//   32 bits without truncation in the port. **The parent getter is 0x004018F0, not a 0x004F4xxx getter**: it is `mov ax,[0x54F928]; ret` at a
//   core address, and its image value is 0, so the "RainEffect" type is registered with parent 0 rather than with a real type id - recorded as an
//   observation about the data, not as a behaviour. Deferred pushes: at all seven link sites the handler immediate is pushed before the getter
//   call and consumed as the link's third argument. Handler immediates 0x00436950, 0x004369E0, 0x00436A40, 0x00436A50 and 0x00436990 are raw code
//   immediates with no implementation in the tree yet. Documented shape differences, all value-preserving: (1) every getter callee is an
//   argument-less load in retail that ignores the arguments the caller pushes, and the tree's transcriptions take no arguments, so those inert
//   pushes are not reproduced; (2) the cleanups are reproduced implicitly by the tree's calling convention rather than by explicit `add esp`
//   arithmetic; the retail sizes 0x2C, 0xC (twice), 0x18, 0x10 and 0xA4 are recorded so a future reviewer can check the arithmetic; (3) the four
//   float stores are transcribed as their exact 32-bit bit patterns in `std::uint32_t` slots, because retail stores immediates into dwords and no
//   floating-point operation follows them in this body.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" void* Sys_LookupOrAppendRecord_416930(void* name);     // 0x00416930 (config/native_416930.cpp): returns a record pointer
extern "C" std::uint32_t Sys_GetGlobal32_4F0F50(void);            // 0x004F0F50: `mov eax,[0xA71870]; ret`
extern "C" std::uint32_t Sys_GetGlobal32_4F0FC0(void);            // 0x004F0FC0: `mov eax,[0xA70C9C]; ret`
extern "C" std::uint32_t Camera_Get_4F1000(void);                 // 0x004F1000: `mov eax,[0xA70C90]; ret` (render/native_4f1000.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1210(void);            // 0x004F1210: `mov eax,[0x54DDA8]; ret` (image 255)
extern "C" std::uint32_t Sys_GetGlobal32_4F1220(void);            // 0x004F1220: `mov eax,[0x54DDAC]; ret` (image 255)
extern "C" void* Script_TaskContainer_Alloc(std::int32_t count);  // 0x004F1950 (script/native_4f1950.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);         // 0x004F1A60: argument-less byte getter (script/native_4f1a60.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);         // 0x004F1A70: argument-less byte getter (script/native_4f1a70.cpp)
// 0x004F1A80 (script/native_4f1a80.cpp). SIX arguments: arg5 is read as a byte (record +0x0C) and arg6 as a dword (record +0x08).
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5, std::int32_t const arg6);

extern "C" {

// 0x0057BD80: the pointer Sys_LookupOrAppendRecord_416930("snow.tex") returns. Image value 0, written and read as a dword.
std::uint32_t g_57bd80 = 0U;

// 0x0057BD84: the "RainEffect" task type id. Image value 0, so it is zero-initialised data (`B`); all accesses are 16-bit.
std::uint16_t g_57bd84 = 0U;

// 0x0057BD88: the five-entry parameter container. Image value 0, written and read as a dword.
std::uint32_t g_57bd88 = 0U;

// 0x0057BD90: the first of four dwords initialised by this body (0 at 0x0057BD90, then the float 1.0f, 5.0f and 150.0f at 0x0057BD94, 0x0057BD98
// and 0x0057BD9C). Image values are all 0, so the block is zero-initialised data (`B`) that this body overwrites at startup.
std::uint32_t g_57bd90 = 0U;
std::uint32_t g_57bd94 = 0U;
std::uint32_t g_57bd98 = 0U;
std::uint32_t g_57bd9c = 0U;

void Task_DefineRainEffect()
{
    // 0x00436780: sub esp,0x88 - a plain frame; this body has NO stack probe.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    ::g_57bd98 = 0x40A00000U; // 0x00436786: mov dword [0x57BD98],0x40A00000 (the float 5.0f as a raw bit pattern)
    ::g_57bd9c = 0x43160000U; // 0x00436790: mov dword [0x57BD9C],0x43160000 (the float 150.0f as a raw bit pattern)
    ::g_57bd90 = 0U;          // 0x0043679A: mov dword [0x57BD90],0

    // 0x004367A4: push 0 (TaskType_Register's fourth argument, pushed first); 0x004367A6: push 0 (the parent getter's inert argument);
    ::g_57bd94 = 0x3F800000U; // 0x004367A8: mov dword [0x57BD94],0x3F800000 - between the pushes, exactly as retail has it
    // 0x004367B2: call 0x4018F0; 0x004367B7: push eax; 0x004367B8: push 0x88; 0x004367BD: push "RainEffect"; 0x004367C2: call 0x401900.
    const std::uint16_t rain_effect_id = TaskType_Register("RainEffect",             // 0x004367BD: push 0x0053C098
                                                           0x88U,                    // 0x004367B8: push 0x88 (object size, equal to the frame)
                                                           QTask_GetCurrentType(), // 0x004367B2: call 0x4018F0 (parent type)
                                                           0U);                      // 0x004367A4: push 0 (fourth argument)
    ::g_57bd84 = rain_effect_id; // 0x004367CF: mov word [0x57BD84],ax - the id store lands between the pushes of the first link

    // 0x4367C7: push 0x00436950 (handler, deferred argument 3); 0x4367CC: push 0x0 (argument 2); 0x4367D5: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bd84), 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00436950U)));

    // 0x4367E0: push 0x004369E0 (handler, deferred argument 3); 0x4367E5: push 0x2 (argument 2); 0x4367E8: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57bd84), 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x004369E0U)));

    // 0x4367F0: push 0x00436A40 (handler, deferred argument 3); 0x4367F5: call 0x4F1A60 -> argument 2; 0x436803: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bd84), static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00436A40U)));

    // 0x43680B: push 0x00436A50 (handler, deferred argument 3); 0x436810: call 0x4F1A70 -> argument 2; 0x43681E: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bd84), static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00436A50U)));

    // 0x436829: push 0x00436990 (handler, deferred argument 3); 0x43682E: push 0x1 (argument 2); 0x436831: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57bd84), 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00436990U)));

    // 0x436839: push 0x004D3710 (handler, deferred argument 3); 0x43683E: call 0x4F1210 -> argument 2; 0x43684C: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57bd84), static_cast<std::uint32_t>(Sys_GetGlobal32_4F1210()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x004D3710U)));

    // 0x436854: push 0x004D3710 (handler, deferred argument 3); 0x436859: call 0x4F1220 -> argument 2; 0x436867: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57bd84), static_cast<std::uint32_t>(Sys_GetGlobal32_4F1220()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x004D3710U)));

    // 0x0043686C: push 5 (the container entry count, exactly the number of appends below); 0x0043686E: call 0x4F1950;
    // 0x00436873: add esp,0x10 - the last link's three pushes (0xC) plus this count push; 0x00436876: mov [0x57BD88],eax.
    ::g_57bd88 = reinterpret_cast<std::uint32_t>(Script_TaskContainer_Alloc(5)); // 0x0043686C: push 5

    // 0x43687B: push $0x0 (arg6); 0x43687F: call Camera_Get_4F1000 (arg4); 0x436896: call 0x004F1A80 (frame+0x00, frame+0x24).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd88)),
                                append_frame, append_frame + 0x24,
                                static_cast<std::int32_t>(Camera_Get_4F1000()), 7, 0);

    // 0x43689E: push $0x57bd98 (arg6); 0x4368A5: call Sys_GetGlobal32_4F0F50 (arg4); 0x4368BC: call 0x004F1A80 (frame+0x00, frame+0x28).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd88)),
                                append_frame, append_frame + 0x28,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57bd98)));

    // 0x4368C4: push $0x57bd98 (arg6); 0x4368CB: call Sys_GetGlobal32_4F0F50 (arg4); 0x4368E2: call 0x004F1A80 (frame+0x00, frame+0x2C).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd88)),
                                append_frame, append_frame + 0x2C,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57bd98)));

    // 0x4368EA: push $0x0 (arg6); 0x4368EE: call Sys_GetGlobal32_4F0FC0 (arg4); 0x436905: call 0x004F1A80 (frame+0x00, frame+0x38).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd88)),
                                append_frame, append_frame + 0x38,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // 0x43690D: push $0x57bd90 (arg6); 0x436914: call Sys_GetGlobal32_4F0F50 (arg4); 0x43692B: call 0x004F1A80 (frame+0x00, frame+0x30).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd88)),
                                append_frame, append_frame + 0x30,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57bd90)));
    // 0x00436930: push 0x0053C08C ("snow.tex", argument 1); 0x00436935: call 0x416930; 0x0043693A: mov [0x57BD80],eax;
    // 0x0043693F: add esp,0xA4 - the 0x88 frame plus the last append's six pushes (0x18) plus this call's one push (0x4).
    ::g_57bd80 = reinterpret_cast<std::uint32_t>(Sys_LookupOrAppendRecord_416930(
        const_cast<char*>("snow.tex"))); // 0x00436930: push 0x0053C08C
} // end of Task_DefineRainEffect

} // extern "C"
