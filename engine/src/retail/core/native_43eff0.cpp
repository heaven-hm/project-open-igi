// Native: Task_DefineWire (behavior-derived placeholder name from the byte-identical name literal; retail symbol unknown); retail VA 0x0043EFF0-0x0043F1E2.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 499 bytes, 1 block, 139 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 139 instructions / 499 bytes / 0 edges) plus the
//   objdump listing build/retail-dis.txt for the same extent, instruction by instruction (the ret is at 0x0043F1E2 and 0x0043F1F0 is the next
//   function, which this body uses as its first handler). `ps` at 0x0053C4DC (the name literal, byte-identical to "Wire"); `px 4` at 0x0057C144
//   (the id slot) and 0x0057C148 (the container slot), both image value 0. The prologue was read before any `lea` was resolved: `sub esp,0x138`
//   followed immediately by argument pushes, with NO saved-register pushes, so the frame-relative shift for this body is ZERO. A full push-by-push
//   simulation closes exactly: the registration's `add esp,0x38` is fourteen pushes (the five registration pushes plus the first three links), the
//   fourth to eleventh links' `add esp,0xC` is three each (eight of them), the twelfth and thirteenth links are cleaned together by the
//   `add esp,0x18` at 0x0043F14A, the container's `add esp,0x10` is its own count push plus the FOURTEENTH link's three pushes, the first two
//   appends' `add esp,0x18` is six each, and **the THIRD append has no cleanup of its own: its six pushes are still outstanding at the tail and are
//   removed by the single `add esp,0x150` at 0x0043F1DC** (0x150 = the 0x138 frame plus 0x18). Every callee was swept against the tree's
//   `// Native:` provenance index with no lane exemption and every one resolves to an existing file under the same name, return type and linkage.
// Behavior/CFG: one straight-line body with a 0x138-byte frame and no stack probe. It registers the "Wire" task type of size 0x138 with parent
//   Sys_GetGlobal16_4C48C0() -> id at 0x0057C144, stores the id, then links FOURTEEN handlers: e0->0x0043F1F0, e1->0x0043F340 and e2->0x0043F360
//   with the literal event ids 0, 1 and 2, then 0x004F1A60->0x0043F3C0, 0x004F1A70->0x0043F3D0, 0x004167F0->0x0043F560,
//   0x00416810->0x0043F6C0, 0x00416800->0x0043F6D0, 0x00416820->0x0043F6E0, 0x00416850->0x0045E3D0, 0x004F1210->0x0043F700,
//   0x004F1220->0x0043F780, then a link with the LITERAL event id 8 and a NULL handler, and finally 0x0047E6F0 with a NULL handler.
//   **TWO of the fourteen links have a handler immediate of ZERO** (0x0043F140 and 0x0043F14D), which is why a mechanical link detector keyed on a
//   handler immediate >= 0x400000 finds only twelve; the census - 0x00401400 x10 and 0x00401530 x4 - is what establishes fourteen, and the two
//   null links are transcribed by hand. It then allocates a parameter container through Script_TaskContainer_Alloc(0x3) into 0x0057C148 and appends
//   THREE records through 0x004F1A80 with recorded deltas 0xEC, 0x104 and 0x11C (from `lea 0xF8/0x110/0x128(%esp)` with three pushes
//   outstanding). One basic block, no branch, no loop, 33 call sites over 18 distinct callees. There is NO 0x004B8810 script-name registration and
//   NO 0x004169D0 lookup in this body.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. **The three-number check holds exactly here: capacity 0x3 = THREE, static append sites THREE, and
//   because the body is a single basic block the records written are THREE too.** Per the correction recorded in native_43b850.cpp the allocation
//   argument is a CAPACITY (`[container+4]`, with `[container]` a live count starting at zero and incremented by each append), so the correct general
//   statement is `records <= capacity`; here the two meet exactly, so the capacity does confirm the record count for this body.
//   Both the id slot 0x0057C144 and the container slot 0x0057C148 are in writable `.data` (the section table puts `.rdata` at VA 0x00533000 with
//   virtual size 0x28F6, ending at 0x005358F6 and `.data` at VA 0x00536000) and both have image value 0, so their initialisers are 0 and their symbol
//   class is `B`; this image has no `.bss` section, so a zero-image global is `.data` with a zero initialiser and `B`/`D` is the symbol
//   classification rather than the section. **The id is the WORD at 0x0057C144 (only `mov word [0x57C144],ax` ever writes it) and the container
//   pointer is the DWORD at the next address, 0x0057C148.** Arg6 of 0x004F1A80 is 0 at all three sites (checked per site), so this body takes the
//   address of none of its own slots. Deferred pushes: at every getter-fed link site the handler immediate is pushed before the event-id getter call
//   and consumed as the link's third argument - including both null-handler sites, where the pushed immediate is zero and the getter still ignores
//   it. Handler immediates 0x0043F1F0, 0x0043F340, 0x0043F360, 0x0043F3C0, 0x0043F3D0, 0x0043F560, 0x0043F6C0, 0x0043F6D0, 0x0043F6E0, 0x0045E3D0,
//   0x0043F700 and 0x0043F780 are raw code immediates; **0x0045E3D0 is the only handler outside this lane's band and it is NOT defined in the tree**
//   (no provenance header carries its VA - it appears only as a handler immediate in other files), so it is recorded as a raw immediate and no
//   implementation is assumed. Documented shape differences, all value-preserving: (1) every getter callee is an argument-less load in retail that
//   ignores the arguments the caller pushes, and the tree's transcriptions take no arguments, so those inert pushes are not reproduced; (2) the
//   cleanups are reproduced implicitly by the tree's calling convention rather than by explicit `add esp` arithmetic; the retail sizes 0x38, 0xC
//   (eight times), 0x10, 0x18 (three times) and 0x150 are recorded so a future reviewer can check the arithmetic; (3) byte-valued event ids are
//   zero-extended in the port, which cannot change behaviour because every link fed by a byte getter goes through 0x00401400 or 0x00401530, which
//   mask the event word for the phase index.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4C48C0(void);            // 0x004C48C0: parent type getter (sys/native_4c48c0.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);         // 0x004F1A60: argument-less byte getter (script/native_4f1a60.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);         // 0x004F1A70: argument-less byte getter (script/native_4f1a70.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_4167F0(void);          // 0x004167F0: argument-less byte getter (config/native_4167f0.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416800(void);          // 0x00416800: argument-less byte getter (config/native_416800.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416810(void);          // 0x00416810: argument-less byte getter (config/native_416810.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416820(void);          // 0x00416820: argument-less byte getter (config/native_416820.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416850(void);          // 0x00416850: argument-less byte getter (config/native_416850.cpp)
extern "C" std::uint8_t Sys_GetGlobal8_47E6F0(void);              // 0x0047E6F0: argument-less byte getter (sys/native_47e6f0.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1210(void);            // 0x004F1210: `mov eax,[0x54DDA8]; ret` (image 255)
extern "C" std::uint32_t Sys_GetGlobal32_4F1220(void);            // 0x004F1220: `mov eax,[0x54DDAC]; ret` (image 255)
extern "C" std::uint32_t Sys_GetGlobal32_4F1010(void);            // 0x004F1010: `mov eax,[0xA70C88]; ret` (image 0)
extern "C" std::uint32_t Core_GetRenderWordB_4F0FD0(void);        // 0x004F0FD0: `mov eax,[0xA71884]; ret` (core/native_4f0fd0.cpp)
extern "C" void* Script_TaskContainer_Alloc(std::int32_t count);  // 0x004F1950 (script/native_4f1950.cpp)
// 0x004F1A80 (script/native_4f1a80.cpp). SIX arguments: arg5 is read as a byte (record +0x0C) and arg6 as a dword (record +0x08).
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5, std::int32_t const arg6);

extern "C" {

// 0x0057C144: the "Wire" task type id, in writable `.data`. Image value 0, so the initialiser is 0 and the symbol class is `B`. Only the low 16 bits
// are ever stored; the container pointer lives at the next dword, 0x0057C148.
std::uint16_t g_57c144 = 0U;

// 0x0057C148: the parameter container. Image value 0, so the initialiser is 0 and the symbol class is `B`.
std::uint32_t g_57c148 = 0U;

void Task_DefineWire()
{
    // 0x0043EFF0: sub esp,0x138 - a plain frame; this body has NO stack probe and NO saved-register pushes, so no frame-relative shift applies.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x0043EFF6: push 0 (TaskType_Register's fourth argument, pushed first); 0x0043EFF8: push 0 (the parent getter's inert argument);
    // 0x0043EFFA: call 0x4C48C0; 0x0043EFFF: push eax; 0x0043F000: push 0x138; 0x0043F005: push "Wire"; 0x0043F00A: call 0x401900.
    const std::uint16_t wire_id = TaskType_Register("Wire",                   // 0x0043F005: push 0x0053C4DC
                                                    0x138U,                   // 0x0043F000: push 0x138 (object size, equal to the frame)
                                                    Sys_GetGlobal16_4C48C0(), // 0x0043EFFA: call 0x4C48C0 (parent type)
                                                    0U);                      // 0x0043EFF8: push 0 (fourth argument)
    ::g_57c144 = wire_id; // 0x0043F017: mov word [0x57C144],ax - between the pushes of the first link

    // 0x43F00F: push 0x43F1F0 (handler, deferred argument 3); 0x43F014: push $0 (argument 2, literal event id);
    // 0x43F01D: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c144), 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x43F1F0U)));

    // 0x43F028: push 0x43F340 (handler, deferred argument 3); 0x43F02D: push $1 (argument 2, literal event id);
    // 0x43F030: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c144), 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x43F340U)));

    // 0x43F03C: push 0x43F360 (handler, deferred argument 3); 0x43F041: push $2 (argument 2, literal event id);
    // 0x43F044: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c144), 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x43F360U)));

    // 0x43F04C: push 0x43F3C0 (handler, deferred argument 3); the Script_PhaseIdGetE4_4F1A60 getter supplies argument 2;
    // 0x43F05F: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c144),
        static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x43F3C0U)));

    // 0x43F067: push 0x43F3D0 (handler, deferred argument 3); the Script_PhaseIdGetE5_4F1A70 getter supplies argument 2;
    // 0x43F079: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c144),
        static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x43F3D0U)));

    // 0x43F081: push 0x43F560 (handler, deferred argument 3); the Sys_GetSettingByte_4167F0 getter supplies argument 2;
    // 0x43F094: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c144),
        static_cast<std::uint32_t>(Sys_GetSettingByte_4167F0()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x43F560U)));

    // 0x43F09C: push 0x43F6C0 (handler, deferred argument 3); the Sys_GetSettingByte_416810 getter supplies argument 2;
    // 0x43F0AF: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c144),
        static_cast<std::uint32_t>(Sys_GetSettingByte_416810()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x43F6C0U)));

    // 0x43F0B7: push 0x43F6D0 (handler, deferred argument 3); the Sys_GetSettingByte_416800 getter supplies argument 2;
    // 0x43F0C9: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c144),
        static_cast<std::uint32_t>(Sys_GetSettingByte_416800()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x43F6D0U)));

    // 0x43F0D1: push 0x43F6E0 (handler, deferred argument 3); the Sys_GetSettingByte_416820 getter supplies argument 2;
    // 0x43F0E4: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c144),
        static_cast<std::uint32_t>(Sys_GetSettingByte_416820()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x43F6E0U)));

    // 0x43F0EC: push 0x45E3D0 (handler, deferred argument 3); the Sys_GetSettingByte_416850 getter supplies argument 2;
    // 0x43F0FF: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c144),
        static_cast<std::uint32_t>(Sys_GetSettingByte_416850()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45E3D0U)));

    // 0x43F107: push 0x43F700 (handler, deferred argument 3); the Sys_GetGlobal32_4F1210 getter supplies argument 2;
    // 0x43F119: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c144),
        static_cast<std::uint32_t>(Sys_GetGlobal32_4F1210()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x43F700U)));

    // 0x43F121: push 0x43F780 (handler, deferred argument 3); the Sys_GetGlobal32_4F1220 getter supplies argument 2;
    // 0x43F134: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c144),
        static_cast<std::uint32_t>(Sys_GetGlobal32_4F1220()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x43F780U)));

    // 0x43F140: push $0x0 - **THE HANDLER IMMEDIATE IS ZERO: this is a NULL-HANDLER link**, and a mechanical link detector keyed on a handler
    //   immediate >= 0x400000 does not see it. 0x43F142: push $0x8 (argument 2, LITERAL event id 8); 0x43F144: push edx (argument 1, the reloaded id);
    //   0x43F145: call 0x401400. 0x43F14A: add esp,0x18 cleans this link together with the twelfth.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c144), 8U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0U))); // 0x43F140: push 0x0

    // 0x43F14D: push $0x0 (the handler immediate is ZERO again - the SECOND NULL-HANDLER link in this body); 0x43F14F: call 0x47E6F0 (event id;
    //   the pushed zero is not read); 0x43F154: push eax (argument 2); 0x43F155: mov ax,[0x57C144]; 0x43F15B: push eax (argument 1);
    //   0x43F15C: call 0x401400; 0x43F161: push $0x3 (the container capacity, next) - the count push and this link's three pushes are cleaned by the
    //   single `add esp,0x10` at 0x43F168.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c144),
        static_cast<std::uint32_t>(Sys_GetGlobal8_47E6F0()), // 0x43F154: push eax (argument 2)
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0U))); // 0x43F14D: push 0x0


    // 0x0043F161: push 0x3 (the container CAPACITY - three slots, matching the three appends below); 0x0043F163: call 0x4F1950;
    // 0x0043F168: add esp,0x10 - the fourteenth link's three pushes plus this count push; 0x0043F16B: mov [0x57C148],eax.
    ::g_57c148 = reinterpret_cast<std::uint32_t>(Script_TaskContainer_Alloc(0x3)); // 0x0043F161: push 0x3

    // 0x43F170: push $0x0 (arg6); 0x43F172: push $0x7 (arg5); 0x43F174: call 0x4F1010 (arg4);
    // 0x43F179: lea 0xF8(%esp),%ecx resolves to frame+0xEC (arg3, three pushes outstanding); 0x43F186: lea 0xc(%esp),%edx
    // resolves to frame+0x00 (arg2); 0x43F18D: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c148)),
                                append_frame, append_frame + 0xEC,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1010()), 7, 0);

    // 0x43F195: push $0x0 (arg6); 0x43F197: push $0x7 (arg5); 0x43F199: call 0x4F1010 (arg4);
    // 0x43F19E: lea 0x110(%esp),%ecx resolves to frame+0x104 (arg3, three pushes outstanding); 0x43F1AB: lea 0xc(%esp),%edx
    // resolves to frame+0x00 (arg2); 0x43F1B2: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c148)),
                                append_frame, append_frame + 0x104,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1010()), 7, 0);

    // 0x43F1BA: push $0x0 (arg6); 0x43F1BC: push $0x7 (arg5); 0x43F1BE: call 0x4F0FD0 (arg4);
    // 0x43F1C3: lea 0x128(%esp),%ecx resolves to frame+0x11C (arg3, three pushes outstanding); 0x43F1D0: lea 0xc(%esp),%edx
    // resolves to frame+0x00 (arg2); 0x43F1D7: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c148)),
                                append_frame, append_frame + 0x11C,
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);


    // 0x0043F1DC: add esp,0x150 - the 0x138 frame plus the third append's six outstanding pushes; 0x0043F1E2: ret. The port's locals take the
    // frame's place.
} // end of Task_DefineWire

} // extern "C"
