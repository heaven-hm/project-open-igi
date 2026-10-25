// Native: Task_DefineRadio (behavior-derived placeholder name from the byte-identical name literal; retail symbol unknown); retail VA 0x0043AD80-0x0043AF19.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 410 bytes, 3 basic blocks, 121 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image - 3 blocks: 0x0043AD80-0x0043AE86 (71 instrs, falls through
//   to the loop), 0x0043AE87-0x0043AEC3 (21 instrs, jumps back to itself, exits to 0x0043AEC4) and 0x0043AEC4-0x0043AF19 (29 instrs, ends in ret) -
//   plus the objdump listing build/retail-dis.txt for the same extent, instruction by instruction. `ps` at 0x0053C2F4 for the name literal
//   (byte-identical to "Radio") and at 0x0053C2E4 for the loop's format string (byte-identical to "Radio sound %d"); `px 4` at 0x0053C2E0
//   (image value 0x00000181), 0x0057BEF8 (image value 0) and 0x0057BEFC (sixty bytes, all image value 0 - the buffer the loop formats into).
//   **0x004A53B3 is not a game routine: it is the statically linked MSVC CRT `sprintf`, already classified as CRT in
//   `scripts/native_coverage.py` and documented under that name in `engine/src/retail/error/error_show.cpp`, which states the port defines no body
//   for it and binds the name to the toolchain's CRT symbol.** This file therefore declares nothing for it and calls `::sprintf` at the call site,
//   exactly as `engine/src/retail/core/main.cpp` does; the census maps the port's `sprintf` relocation to retail VA 0x004A53B3.
//   Every other callee was swept against the tree's `// Native:` provenance index with no lane exemption and every one resolves to an existing file
//   under the same name, return type and linkage. A full push-by-push simulation, including the loop, closing exactly: the registration's
//   `add esp,0x38` is fourteen pushes, the container's `add esp,0x8` is its own two, each link's `add esp,0xC` is its three, the loop's
//   `add esp,0x18` is the six outstanding pushes (three saved registers plus arg6, arg5 and arg4), and the epilogue's `add esp,0x2E8` matches the
//   prologue with `pop edi`/`pop esi`/`pop ebx`.
// Behavior/CFG: a registrar with one loop. Prologue: `sub esp,0x2E8` (a plain frame, NO stack probe) then `push ebx; push esi; push edi`, so three
//   saved registers sit BELOW the frame and every frame-relative `lea` in this body is offset by twelve bytes - the second argument of each append
//   resolves to frame+0x00 only once that shift is taken into account.
//   It registers the "Radio" task type of size 0x2E8 with parent Sys_GetGlobal16_481340() -> id at 0x0053C2E0, stores the id, then links EIGHT
//   handlers: e0->0x0043AF20, e1->0x0043AF90 and e2->0x0043B110 with the literal event ids 0, 1 and 2, then 0x004F1A60->0x0043B160,
//   0x004F1A70->0x0043B170, 0x004F1210->0x0043B1C0, 0x004F1220->0x0043B250 and 0x00467D90->0x00443DC0. It grows an existing parameter container by
//   SIX entries - `Script_TaskContainer_Grow(Sys_GetGlobal32_481350(), 6)`, the container coming from the getter and the count being the pushed 6 -
//   and stores the returned pointer at 0x0057BEF8.
//   THE LOOP (0x0043AE87-0x0043AEC2) is a do-while whose induction is a data pointer, not a counter: `mov esi,0x57BEFC` sets the destination,
//   `xor edi,edi` sets the format argument, `lea ebx,[esp+0x2A0]` resolves to frame+0x294 for the record's third argument, and the back edge is
//   `add esi,0xF; inc edi; add ebx,0x10; cmp esi,0x57BF38; jl 0x0043AE87`. Each iteration formats "Radio sound %d" into the buffer at esi and then
//   appends one record. The trip count is therefore (0x57BF38 - 0x57BEFC) / 0xF = 0x3C / 0xF = FOUR, and the stride of 0xF with a 0x10 record
//   stride is why the formatted strings and the records advance at different rates. Two more appends follow the loop (deltas 0x2D4 and 0x244), so
//   this body writes 4 + 1 + 1 = SIX records through THREE static append sites - exactly the container's growth of six.
//   Call census: 24 call sites over 15 distinct callees - 0x00401400 x4, 0x00401530 x4, 0x004F1A80 x3, 0x004F0FD0 x2, and one each of 0x00401900,
//   0x00467D90, 0x00481340, 0x00481350, 0x004A53B3, 0x004F0FC0, 0x004F1210, 0x004F1220, 0x004F1980, 0x004F1A60 and 0x004F1A70.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. **The id slot 0x0053C2E0 is in writable `.data`** (the section table puts `.rdata` at VA
//   0x00533000 with virtual size 0x28F6, ending at 0x005358F6) and its image value is 0x00000181, so its initialiser is 0x0181U and its symbol class
//   is `D`. The container slot 0x0057BEF8 and the sixty-byte buffer 0x0057BEFC both have image value 0, so their initialisers are 0 and their symbol
//   class is `B`; this image has no `.bss` section, so a zero-image global is `.data` with a zero initialiser and `B`/`D` is the symbol
//   classification rather than the section. **The buffer 0x0057BEFC is written only through the CRT `sprintf` call at 0x0043AE8E, never by a direct
//   store, so it is defined here as the object whose address the loop advances; the retail buffer spans 0x0057BEFC-0x0057BF37 and the loop's bound
//   0x0057BF38 is the first byte past it.** The three-number check for this body is: three static append sites, six records written, container
//   grown by six - and here the trip count is not an immediate at all but the ratio of the bound span 0x3C to the stride 0xF, which is why the
//   container's six is the independent confirmation rather than the other way round. Arg6 is 0 at all three sites (checked per site). Deferred
//   pushes: at every getter-fed link site the handler immediate is pushed before the event-id getter call and consumed as the link's third argument;
//   the 0x00467D90 site pushes the handler immediate 0x00443DC0 in the same deferred position. Handler immediates 0x0043AF20, 0x0043AF90,
//   0x0043B110, 0x0043B160, 0x0043B170, 0x0043B1C0, 0x0043B250 and 0x00443DC0 are raw code immediates with no implementation in the tree yet.
//   Documented shape differences, all value-preserving: (1) every getter callee is an argument-less load in retail that ignores the arguments the
//   caller pushes, and the tree's transcriptions take no arguments, so those inert pushes are not reproduced; (2) the cleanups are reproduced
//   implicitly by the tree's calling convention rather than by explicit `add esp` arithmetic; the retail sizes 0x38, 0xC (four times), 0x8, 0x18
//   and 0x2E8 are recorded so a future reviewer can check the arithmetic; (3) the loop's three induction values are kept as three locals because
//   retail keeps them in three registers that advance at different rates, and collapsing them into one would change the buffer stride; (4) the
//   port's `::sprintf` is the same CRT entry retail calls, so the formatted bytes are produced by the same routine rather than reimplemented.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>
#include <cstdio>

extern "C" std::uint16_t Sys_GetGlobal16_481340(void);            // 0x00481340: parent type getter (sys/native_481340.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_481350(void);            // 0x00481350: the existing parameter container (sys/native_481350.cpp)
extern "C" std::uint8_t Task_GetCachedEventId_467D90(void);       // 0x00467D90: argument-less byte getter (core/native_00467d90.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0FC0(void);            // 0x004F0FC0: `mov eax,[0xA70C9C]; ret`
extern "C" std::uint32_t Core_GetRenderWordB_4F0FD0(void);        // 0x004F0FD0: `mov eax,[0xA71884]; ret` (core/native_4f0fd0.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1210(void);            // 0x004F1210: `mov eax,[0x54DDA8]; ret` (image 255)
extern "C" std::uint32_t Sys_GetGlobal32_4F1220(void);            // 0x004F1220: `mov eax,[0x54DDAC]; ret` (image 255)
// 0x004F1980 (script/native_4f1980.cpp): takes the container and a count, returns the container pointer.
extern "C" void* Script_TaskContainer_Grow(std::int32_t* container, std::int32_t count);
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);         // 0x004F1A60: argument-less byte getter (script/native_4f1a60.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);         // 0x004F1A70: argument-less byte getter (script/native_4f1a70.cpp)
// 0x004F1A80 (script/native_4f1a80.cpp). SIX arguments: arg5 is read as a byte (record +0x0C) and arg6 as a dword (record +0x08).
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5, std::int32_t const arg6);

extern "C" {

// 0x0053C2E0: the "Radio" task type id, in writable `.data`; image value 0x00000181, so its initialiser is 0x0181U and its symbol class is `D`.
// Only the low 16 bits are ever written and read.
std::uint16_t g_53c2e0 = 0x0181U;

// 0x0057BEF8: the grown parameter container. Image value 0, so the initialiser is 0 and the symbol class is `B`.
std::uint32_t g_57bef8 = 0U;

// 0x0057BEFC-0x0057BF37: the sixty-byte buffer the loop formats "Radio sound %d" into, one 15-byte slot per iteration. Image value 0 throughout, so
// the initialiser is 0 and the symbol class is `B`. It is written only through the CRT sprintf call below, never by a direct store.
std::uint8_t g_57befc[0x3C] = {};

void Task_DefineRadio()
{
    // 0x0043AD80: sub esp,0x2E8 - a plain frame; this body has NO stack probe. 0x0043AD86/0x0043AD87/0x0043AD88: push ebx; push esi; push edi -
    // three callee-saved registers saved BELOW the frame, which is why every frame-relative address in this body is offset by twelve bytes.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00 (the address of the `sub esp,0x2E8` result)
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x0043AD89: push 0 (TaskType_Register's fourth argument, pushed first); 0x0043AD8B: push 0 (the parent getter's inert argument);
    // 0x0043AD8D: call 0x481340; 0x0043AD92: push eax; 0x0043AD93: push 0x2E8; 0x0043AD98: push "Radio"; 0x0043AD9D: call 0x401900.
    const std::uint16_t radio_id = TaskType_Register("Radio",                 // 0x0043AD98: push 0x0053C2F4
                                                     0x2E8U,                  // 0x0043AD93: push 0x2E8 (object size, equal to the frame)
                                                     Sys_GetGlobal16_481340(), // 0x0043AD8D: call 0x481340 (parent type)
                                                     0U);                     // 0x0043AD8B: push 0 (fourth argument)
    ::g_53c2e0 = radio_id; // 0x0043ADAA: mov word [0x53C2E0],ax - between the pushes of the first link

    // 0x0043ADA2: push 0x0043AF20 (handler, deferred argument 3); 0x0043ADA7: push 0x0 (argument 2, literal event id);
    // 0x0043ADA9: push eax (argument 1); 0x0043ADB0: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c2e0), 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043AF20U)));

    // 0x0043ADB5: mov ax,[0x53C2E0]; 0x0043ADBB: push 0x0043AF90 (handler); 0x0043ADC0: push 0x1 (event id); 0x0043ADC2: push eax (argument 1);
    // 0x0043ADC3: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53c2e0), 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043AF90U)));

    // 0x0043ADC8: mov cx,[0x53C2E0]; 0x0043ADCF: push 0x0043B110 (handler); 0x0043ADD4: push 0x2 (event id); 0x0043ADD6: push ecx (argument 1);
    // 0x0043ADD7: call 0x401530; 0x0043ADDC: add esp,0x38 - fourteen pushes.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53c2e0), 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043B110U)));

    // 0x0043ADDF: push 0x0043B160 (handler, deferred argument 3); 0x0043ADE4: call 0x4F1A60 (event id; the pushed handler is not read);
    // 0x0043ADE9: mov dx,[0x53C2E0]; 0x0043ADF0: push eax (argument 2); 0x0043ADF1: push edx (argument 1); 0x0043ADF2: call 0x401400;
    // 0x0043ADF7: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c2e0),
                                static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()), // 0x0043ADF0: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043B160U)));

    // 0x0043ADFA: push 0x0043B170 (handler, deferred argument 3); 0x0043ADFF: call 0x4F1A70; 0x0043AE04: push eax (argument 2);
    // 0x0043AE05: mov ax,[0x53C2E0]; 0x0043AE0B: push eax (argument 1); 0x0043AE0C: call 0x401400; 0x0043AE11: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c2e0),
                                static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()), // 0x0043AE04: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043B170U)));

    // 0x0043AE14: push 0x0043B1C0 (handler, deferred argument 3); 0x0043AE19: call 0x4F1210; 0x0043AE1E: mov cx,[0x53C2E0];
    // 0x0043AE25: push eax (argument 2); 0x0043AE26: push ecx (argument 1); 0x0043AE27: call 0x401530; 0x0043AE2C: add esp,0xC.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53c2e0),
                                           static_cast<std::uint32_t>(Sys_GetGlobal32_4F1210()), // 0x0043AE25: push eax (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043B1C0U)));

    // 0x0043AE2F: push 0x0043B250 (handler, deferred argument 3); 0x0043AE34: call 0x4F1220; 0x0043AE39: mov dx,[0x53C2E0];
    // 0x0043AE40: push eax (argument 2); 0x0043AE41: push edx (argument 1); 0x0043AE42: call 0x401530; 0x0043AE47: add esp,0xC.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53c2e0),
                                           static_cast<std::uint32_t>(Sys_GetGlobal32_4F1220()), // 0x0043AE40: push eax (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043B250U)));

    // 0x0043AE4A: push 0x00443DC0 (handler, deferred argument 3); 0x0043AE4F: call 0x467D90 (event id; the pushed handler is not read);
    // 0x0043AE54: push eax (argument 2); 0x0043AE55: mov ax,[0x53C2E0]; 0x0043AE5B: push eax (argument 1); 0x0043AE5C: call 0x401400;
    // 0x0043AE61: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c2e0),
                                static_cast<std::uint32_t>(Task_GetCachedEventId_467D90()), // 0x0043AE54: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00443DC0U)));

    // 0x0043AE64: push 0x6 (argument 2 = the number of entries to grow by); 0x0043AE66: call 0x481350 (argument 1 = the existing container);
    // 0x0043AE6B: push eax (argument 1); 0x0043AE6C: call 0x4F1980; 0x0043AE71: add esp,0x8; 0x0043AE74: mov [0x57BEF8],eax.
    ::g_57bef8 = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(
        Script_TaskContainer_Grow(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(Sys_GetGlobal32_481350())), 6))); // 0x0043AE64: push 0x6

    // 0x0043AE79: xor edi,edi (the format argument); 0x0043AE7B: lea ebx,[esp+0x2A0] -> frame+0x294 (the running third argument);
    // 0x0043AE82: mov esi,0x0057BEFC (the running destination for the formatted string).
    char* radio_sound_name = reinterpret_cast<char*>(&::g_57befc[0]); // retail esi (0x0043AE82: mov esi,0x57BEFC)
    std::int32_t radio_sound_index = 0;                               // retail edi (0x0043AE79: xor edi,edi)
    std::int32_t radio_sound_delta = 0x294;                           // retail ebx (0x0043AE7B: lea ebx,[esp+0x2A0])

    // 0x0043AE87-0x0043AEC2: do-while. The back edge is `add esi,0xF; inc edi; add ebx,0x10; cmp esi,0x57BF38; jl 0x0043AE87`, so the loop runs
    // while the destination pointer is below the first byte past the sixty-byte buffer: (0x57BF38 - 0x57BEFC) / 0xF = 0x3C / 0xF = 4 iterations.
    do
    {
        // 0x0043AE87: push edi (argument 3, the format argument); 0x0043AE88: push 0x0053C2E4 ("Radio sound %d", argument 2);
        // 0x0043AE8D: push esi (argument 1, the destination); 0x0043AE8E: call 0x4A53B3 (the CRT sprintf); 0x0043AE93: add esp,0xC.
        ::sprintf(radio_sound_name, "Radio sound %d", radio_sound_index); // 0x0043AE8E: call 0x4A53B3

        // 0x0043AE96: push 0 (arg6); 0x0043AE98: push 0x7 (arg5); 0x0043AE9A: call 0x4F0FD0 (arg4); 0x0043AE9F: mov edx,[0x57BEF8] (arg1);
        // 0x0043AEA5: push eax (arg4); 0x0043AEA6: lea ecx,[esp+0x18] -> frame+0x00 (arg2); 0x0043AEAA: push ebx (arg3);
        // 0x0043AEAB: push ecx (arg2); 0x0043AEAC: push edx (arg1); 0x0043AEAD: call 0x4F1A80; 0x0043AEB2: add esp,0x18.
        Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bef8)),
                                    append_frame, append_frame + radio_sound_delta,
                                    static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);

        radio_sound_name += 0xF;      // 0x0043AEB5: add esi,0xF (the buffer stride, NOT the record stride)
        ++radio_sound_index;          // 0x0043AEB8: inc edi
        radio_sound_delta += 0x10;    // 0x0043AEB9: add ebx,0x10 (the record stride)
    } while (radio_sound_name < reinterpret_cast<char*>(&::g_57befc[0]) + 0x3C); // 0x0043AEBC: cmp esi,0x57BF38; 0x0043AEC2: jl

    // 0x0043AEC4: push 0 (arg6); 0x0043AEC6: push 0x7 (arg5); 0x0043AEC8: call 0x4F0FD0 (arg4); 0x0043AECD: mov edx,[0x57BEF8] (arg1);
    // 0x0043AED3: push eax (arg4); 0x0043AED4: lea eax,[esp+0x2EC] -> frame+0x2D4 (arg3); 0x0043AEDB: lea ecx,[esp+0x18] -> frame+0x00 (arg2);
    // 0x0043AEE2: call 0x4F1A80; 0x0043AEE7: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bef8)),
                                append_frame, append_frame + 0x2D4,
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);

    // 0x0043AEEA: push 0 (arg6); 0x0043AEEC: push 0x7 (arg5); 0x0043AEEE: call 0x4F0FC0 (arg4); 0x0043AEF3: mov edx,[0x57BEF8] (arg1);
    // 0x0043AEF9: push eax (arg4); 0x0043AEFA: lea eax,[esp+0x25C] -> frame+0x244 (arg3); 0x0043AF01: lea ecx,[esp+0x18] -> frame+0x00 (arg2);
    // 0x0043AF08: call 0x4F1A80; 0x0043AF0D: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bef8)),
                                append_frame, append_frame + 0x244,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // 0x0043AF10: pop edi; 0x0043AF11: pop esi; 0x0043AF12: pop ebx; 0x0043AF13: add esp,0x2E8; 0x0043AF19: ret - the frame and the three
    // saved registers are restored here; the port's locals take their place.
} // end of Task_DefineRadio

} // extern "C"
