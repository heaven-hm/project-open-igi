// Native: Task_DefineSubLevel (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00488620-0x004886E9.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 202 bytes, 1 block, 56 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 56 instructions / 202 bytes / 0 edges; `pd`
//   stops at the ret at 0x004886E9) plus the objdump listing build/retail-dis.txt for the same extent; `p8`/`ps` at 0x005410C4 for the class name
//   (bytes 5375624c6576656c00 = "SubLevel", byte-identical to the literal below); `pxw` at 0x005C8A90 and 0x005C8A94 for the image values of the
//   two globals; a full stack-pointer simulation over the 56 instructions, which returns esp to its entry value at the ret and confirms every
//   `add esp` (0x2c = 11 pushes, 0xc = 3 each, 0x10 = 4, 0x138 = the 0x120 frame plus the final 6 pushes). The one 0x004F1A80 call was checked
//   against 0x004F1A80's own bytes: it reads its second and third arguments at [esp+8]/[esp+0xC], subtracts them at 0x004F1A88 and never
//   dereferences either, so the value that reaches the container is the difference 0x20.
// Behavior/CFG: registers the "SubLevel" task type as TaskType_Register("SubLevel", 0x120, QTask_GetCurrentType(), 0) and stores the returned
//   16-bit id at 0x005C8A90; links the id to handler 0x004886F0 on event 0 through 0x00401400 and to handler 0x00488700 on event 1 through
//   0x00401530; links handler 0x004E0890 on the event id from DynObj_GetByte_4C56F0, handler 0x00488770 on the event id from 0x004F1A60 and
//   handler 0x00488780 on the event id from 0x004F1A70, all through 0x00401400; allocates a 1-entry parameter container through 0x004F1950 into
//   0x005C8A94 and appends one descriptor to it through 0x004F1A80 with the type value from 0x004F0FF0. Straight-line: one basic block, no branch,
//   no loop, 13 call sites over 10 distinct callees. There is no 0x004B8810/0x004B8890 script-name tail in this body.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Documented shape differences, all value-preserving: (1) 0x004F0FF0 is a single-instruction
//   getter in retail that ignores the arguments the caller pushes (verified: `mov eax,[0xA7186C]; ret`), and the tree's transcription takes no
//   arguments, so those inert pushes are not reproduced and only the call order and value are kept; (2) the same applies to the two zeros pushed
//   before 0x004018F0 and to the 0x004E0890 push, which is the getter 0x004C56F0's inert argument and simultaneously the next link call's
//   pre-pushed handler slot - the port keeps both the getter call and the handler value in the same order; (3) retail's 0x120-byte frame is not
//   reproduced - only the two addresses whose difference 0x004F1A80 records are, via one addressable dword plus the retail offset, because that
//   difference is the only thing the callee reads from them. The id slot 0x005C8A90 is declared as the two 16-bit words it holds (image dword
//   0x00000000), so retail's 16-bit store at 0x00488647 and its three 16-bit re-loads are plain 16-bit accesses with no type punning. Handlers
//   0x004886F0, 0x00488770 and 0x00488780 are raw code immediates in retail and are not transcribed yet (native gaps, all inside
//   0x0046xxxx-0x0048xxxx); 0x00488700 is already transcribed in core/native_488700.cpp and 0x004E0890 is still a gap in the 0x004E range.
//   0x005C8A90 and 0x005C8A94 have no other owner and both have image value 0, so this file defines them. Domain: core/, because it is one of the
//   0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint8_t DynObj_GetByte_4C56F0(void);                    // 0x004C56F0: argument-less byte getter (dynobj/native_4c56f0.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);               // 0x004F1A60: argument-less byte getter
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);               // 0x004F1A70: argument-less byte getter
extern "C" std::uint32_t Sys_GetGlobal32_4F0FF0(void);                  // 0x004F0FF0: argument-less dword getter
extern "C" void* Script_TaskContainer_Alloc(std::int32_t const count);  // 0x004F1950
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5, std::int32_t const arg6);
// 0x004F1A80 (script/native_4f1a80.cpp). SIX arguments, corrected: the callee reads arg1..arg4, arg5 as a byte (recorded at record +0x0C)
// and arg6 as a dword (recorded at record +0x08). The two reads are both `[esp+0x20]` in retail, at different push depths, with a `pop edi`
// between them (0x004F1AB1 with twelve bytes pushed = arg5, 0x004F1ACA with eight = arg6). Earlier revisions of this file declared five
// parameters and the tree's implementation wrote arg5 into both fields; both are now corrected. Every append below passes the sixth word that
// retail pushes for it.  // 0x004F1A80 (copied from script/native_4f1a80.cpp)

extern "C" {

// Retail 0x005C8A90 is a 4-byte slot whose image dword is 0x00000000, and retail stores and re-loads only its low word (0x00488647
// `mov word [0x5C8A90],ax`, then `mov ax/cx/dx,[0x5C8A90]` at 0x00488652/0x00488672/0x0048868D/0x004886A9). It is declared as the two 16-bit
// words the slot actually holds, so the byte layout is identical and every store and load is a plain 16-bit access.
std::uint16_t g_5c8a90[2] = {0x0000U, 0x0000U}; // retail 0x005C8A90 (low word: the "SubLevel" task type id) and 0x005C8A92 (high word)
void* g_5c8a94 = nullptr;                       // retail 0x005C8A94: the parameter container from 0x004F1950 (image value 0)

void Task_DefineSubLevel()
{
    // 0x00488620: sub esp,0x120 - retail's 288-byte frame. The 0x004F1A80 call below passes two addresses inside it: its first dword (the
    // `lea edx,[esp+0xc]` at 0x004886D7 runs with 12 bytes of arguments already pushed, so the address is frame+0x00) and the slot at retail
    // offset 0x20. 0x004F1A80 reads those two arguments at [esp+8]/[esp+0xC], subtracts them at 0x004F1A88 and never dereferences them, so the
    // port keeps one addressable dword and passes that address plus the same retail offset.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x00488626/0x00488628: push 0; push 0 (inert arguments of the getter, and TaskType_Register's fourth/fifth slots);
    // 0x0048862A: call 0x4018F0; 0x0048862F: push eax; 0x00488630: push 0x120; 0x00488635: push "SubLevel"; 0x0048863A: call 0x401900.
    const std::uint16_t class_id = TaskType_Register("SubLevel", // 0x00488635: push 0x005410C4
                                                     0x120U,      // 0x00488630: push 0x120 (object size)
                                                     QTask_GetCurrentType(), // 0x0048862A: call 0x4018F0 (parent type)
                                                     0U);        // 0x00488628: push 0 (task list count)

    // 0x0048863F: push 0x004886F0 (pre-pushed third argument); 0x00488644: push 0; 0x00488646: push eax;
    // 0x00488647: mov word [0x5C8A90],ax; 0x0048864D: call 0x401400.
    ::g_5c8a90[0] = class_id; // 0x00488647: the id is stored between the argument pushes and the call, exactly as retail orders it
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5c8a90[0]), // 0x00488652: mov ax,[0x5C8A90] (retail re-loads the word)
                                0U,                                     // 0x00488644: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004886F0U))); // 0x0048863F: push handler

    // 0x00488658: push 0x00488700 (pre-pushed third argument); 0x0048865D: push 1; 0x0048865F: push eax; 0x00488660: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5c8a90[0]), // 0x00488652: mov ax,[0x5C8A90]
                                           1U,                                     // 0x0048865D: push 1
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00488700U))); // 0x00488658: push handler
    // 0x00488665: add esp,0x2c - 11 pushes above (2 inert zeros + 3 registration arguments + three 3-argument links).

    // 0x00488668: push 0x004E0890 (pre-pushed third argument, and simultaneously the getter's inert argument); 0x0048866D: call 0x4C56F0
    // (event id); 0x00488672: mov cx,[0x5C8A90]; 0x00488679: push eax; 0x0048867A: push ecx; 0x0048867B: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5c8a90[0]), // 0x00488672: mov cx,[0x5C8A90]
                                static_cast<std::uint32_t>(DynObj_GetByte_4C56F0()), // 0x00488679: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004E0890U))); // 0x00488668: push handler (argument 3)
    // 0x00488680: add esp,0xc - 3 pushes above.

    // 0x00488683: push 0x00488770 (pre-pushed third argument); 0x00488688: call 0x4F1A60 (event id);
    // 0x0048868D: mov dx,[0x5C8A90]; 0x00488694: push eax; 0x00488695: push edx; 0x00488696: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5c8a90[0]), // 0x0048868D: mov dx,[0x5C8A90]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()), // 0x00488694: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00488770U))); // 0x00488683: push handler (argument 3)
    // 0x0048869B: add esp,0xc - 3 pushes above.

    // 0x0048869E: push 0x00488780 (pre-pushed third argument); 0x004886A3: call 0x4F1A70 (event id);
    // 0x004886A8: push eax; 0x004886A9: mov ax,[0x5C8A90]; 0x004886AF: push eax; 0x004886B0: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5c8a90[0]), // 0x004886A9: mov ax,[0x5C8A90]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()), // 0x004886A8: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00488780U))); // 0x0048869E: push handler (argument 3)

    // 0x004886B5: push 1; 0x004886B7: call 0x4F1950; 0x004886BC: add esp,0x10 (4 pushes: the link's 3 plus this 1);
    // 0x004886BF: mov [0x5C8A94],eax.
    ::g_5c8a94 = Script_TaskContainer_Alloc(1); // 0x004886B5: push 1 (the container's entry count; one descriptor follows)

    // 0x004886C4/0x004886C6: push 0; push 7 (inert arguments); 0x004886C8: call 0x4F0FF0; 0x004886CD: lea ecx,[esp+0x28] (frame+0x20);
    // 0x004886D1: push eax; 0x004886D7: lea edx,[esp+0xc] (frame+0x00); 0x004886DB/0x004886DC/0x004886DD: the three argument pushes;
    // 0x004886DE: call 0x4F1A80 (no cleanup of its own); 0x004886E3: add esp,0x138 - the 0x120 frame plus this group's 6 pushes; ret.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c8a94),  // argument 1: the container
                                append_frame,                                 // argument 2: retail frame+0x00
                                append_frame + 0x20,                          // argument 3: retail frame+0x20 (recorded difference 0x20)
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FF0()), // argument 4: the type from 0x4F0FF0
                                7, 0);                                           // argument 5: retail's leftover 7
} // end of Task_DefineSubLevel

} // extern "C"
