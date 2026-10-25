// Native: Task_DefineExplosionTypes (behavior-derived placeholder name; retail symbol unknown); retail VA 0x004801B0-0x00480416.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 615 bytes, 1 block, 164 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 164 instructions / 615 bytes / 0 edges; `pd`
//   stops at the ret at 0x00480416) plus the objdump listing build/retail-dis.txt for the same extent, read in three ranges
//   (0x004801B0-0x004802DE, 0x004802DE-0x004803A0, 0x004803A0-0x00480417) so no part of the body is transcribed from a summary; `p8` at
//   0x00540970, 0x00540964, 0x00540958 and 0x00540948 for the four class names, and `ps` at 0x00540918, 0x0053C130 and 0x0053C154 for the three
//   resource names (all byte-identical to the literals below); `pxw` at 0x005C1154, 0x005C1156, 0x005C1158, 0x005C115C, 0x005C1160,
//   0x005C1164 and 0x005C1168 for the image values; a full stack-pointer simulation over the 164 instructions, which returns esp to its entry
//   value at the ret and confirms every cleanup (0x38 = 14 pushes, 0x2c = 11 twice, 0xc = 3 eleven times, 0x30 = 12, 0x40 = 16, 0x4 = 1). Every
//   getter was re-read at its own VA and is a single-instruction argument-less load ending in a ret: 0x004018F0, 0x004C48C0, 0x00503BF0,
//   0x00416830, 0x00416850, 0x00416880 and 0x00489610, so every push that precedes them is inert - and each one is simultaneously the following
//   0x00401400/0x00401530 link call's pre-pushed handler slot, which the same stack simulation confirms (the pushed value is still on the stack
//   when the link call runs, at [esp+0xC], because the getter is __cdecl and cleans nothing).
// Behavior/CFG: one straight-line body that registers FOUR task types, in this order, each with its own id slot and link set:
//   (1) "Explosion0", size 0x88, parent QTask_GetCurrentType() (0x004018F0) -> id at 0x005C1154; links e0 -> 0x00480F30 through 0x00401400,
//       e1 -> 0x00480F40 and e2 -> 0x004D3710 through 0x00401530.
//   (2) "FireBall", size 0x118, parent Sys_GetGlobal16_4C48C0() -> id at 0x005C1168; links e0 -> 0x004805F0 and e4 -> 0x00480790 through
//       0x00401400, then seven more links on the event ids from 0x00416830, 0x00416850, 0x00416880, the constant 12 and 0x00489610 (all through
//       0x00401400, handler 0x0045F920) and on the constants 1 and 2 (through 0x00401530, handlers 0x004806E0 and 0x00480F00).
//   (3) "Fragment", size 0x118, parent Sys_GetGlobal16_503BF0() -> id at 0x005C1156; ONE link, e1 -> 0x004805B0 through 0x00401530 (there is no
//       e0 link for this type), plus five more links on the event ids from 0x00416830, 0x00416850, 0x00416880, the constant 12 and 0x00489610
//       with a NULL handler (the pre-pushed zero).
//   (4) "ExplosionSmoke", size 0x48, parent QTask_GetCurrentType() (0x004018F0) -> id at 0x005C115C; links e0 -> 0x004D3710 through
//       0x00401400, e1 -> 0x00480420 and e2 -> 0x004D3710 through 0x00401530.
//   It then performs three resource lookups in retail order: 0x00416930("burn1.tex") stored to 0x005C1164, 0x004169D0("smoke2.spr") stored to
//   0x005C1160, and 0x004169D0("fireball.spr") stored to 0x005C1158, with the first store landing before the second call and the second store after
//   the 0x40 cleanup, exactly as retail orders them. One basic block, no branch, no loop, no frame, no parameter container, 40 call sites over
//   12 distinct callees. There is no 0x004B8810/0x004B8890 script-name tail in this body.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Documented shape differences, all value-preserving: (1) every getter callee is an
//   argument-less load in retail that ignores the arguments the caller pushes, and the tree's transcriptions take no arguments, so those inert
//   pushes are not reproduced; each is simultaneously the next link call's pre-pushed handler, which the port keeps as the handler value in the
//   same order; (2) the inert pushes are NOT all zero in this native - 0x20 before the first registration, 0x60 before the second, 0xA0 before the
//   third and 0x40 before the fourth - and their values are recorded here as facts rather than normalised away, since what they were in the
//   original source is not established; (3) the two zeros before 0x004018F0 in the first and fourth registrations are the same kind of inert
//   push. The id slots 0x005C1154 and 0x005C1156 are two bytes apart in the image, so they are declared as plain 16-bit globals and every store to
//   them is a plain 16-bit store; the three record slots are declared as std::uint32_t and assigned from the callees' void* results the same way
//   core/native_0047e700.cpp does it for the same two callees. No global written here is owned elsewhere, and all seven have image value 0, so
//   this file defines them all. Handlers 0x00480F30, 0x00480F40, 0x004805F0, 0x00480790, 0x004806E0, 0x00480F00, 0x00480420, 0x004805B0 and
//   0x0045F920 are raw code immediates in retail and are not transcribed yet; 0x004D3710 is already transcribed in script/native_4D3710.cpp.
//   0x0045F920 lies OUTSIDE this agent's 0x0046xxxx-0x0048xxxx scope (it is below 0x00460000) and belongs to the 0x00400000-0x0045FFFF
//   owner, so it is reported as their gap and not written here. Domain: core/, because it is one of the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4C48C0(void);                  // 0x004C48C0: argument-less word getter
extern "C" std::uint16_t Sys_GetGlobal16_503BF0(void);                  // 0x00503BF0: argument-less word getter (sys/native_503bf0.cpp)
extern "C" std::uint8_t Sys_GetGlobal8_489610(void);                    // 0x00489610: argument-less byte getter (sys/native_489610.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416830(void);                // 0x00416830: argument-less byte getter
extern "C" std::uint8_t Sys_GetSettingByte_416850(void);                // 0x00416850: argument-less byte getter
extern "C" std::uint8_t Sys_GetSettingByte_416880(void);                // 0x00416880: argument-less byte getter
extern "C" void* Sys_LookupOrAppendRecord_416930(char const* name);     // 0x00416930 (config/native_416930.cpp)
extern "C" void* Sys_LookupOrAppendRecord_4169D0(char const* name);     // 0x004169D0 (config/native_4169d0.cpp)

extern "C" {

// All seven globals below have image value 0 and no other owner in the tree. The two id slots 0x005C1154 and 0x005C1156 are exactly two bytes
// apart in the image, so they are 16-bit slots and retail writes them with 16-bit stores (`mov word [0x5C1154],ax` at 0x004801D1,
// `mov %ax,0x5C1156` at 0x00480312, `mov %ax,0x5C115C` at 0x004803B1).
std::uint16_t g_5c1154 = 0U; // retail 0x005C1154: the "Explosion0" task type id
std::uint16_t g_5c1156 = 0U; // retail 0x005C1156: the "Fragment" task type id
std::uint16_t g_5c115c = 0U; // retail 0x005C115C: the "ExplosionSmoke" task type id
std::uint16_t g_5c1168 = 0U; // retail 0x005C1168: the "FireBall" task type id

std::uint32_t g_5c1158 = 0U; // retail 0x005C1158: 0x004169D0("fireball.spr"), stored as a dword
std::uint32_t g_5c1160 = 0U; // retail 0x005C1160: 0x004169D0("smoke2.spr"), stored as a dword
std::uint32_t g_5c1164 = 0U; // retail 0x005C1164: 0x00416930("burn1.tex"), stored as a dword

void Task_DefineExplosionTypes()
{
    // ---- Registration 1: "Explosion0" --------------------------------------------------------------------------------------------------
    // 0x004801B0/0x004801B2: push 0; push 0x20 (both inert arguments of the getter; 0x20 is a retail value, not normalised);
    // 0x004801B4: call 0x4018F0; 0x004801B9: push eax; 0x004801BA: push 0x88; 0x004801BF: push "Explosion0"; 0x004801C4: call 0x401900.
    const std::uint16_t explosion0_id = TaskType_Register("Explosion0", // 0x004801BF: push 0x00540970
                                                          0x88U,          // 0x004801BA: push 0x88 (object size)
                                                          QTask_GetCurrentType(), // 0x004801B4: call 0x4018F0 (parent type)
                                                          0U);            // 0x004801B2: push 0x20 (inert; the register call's fourth slot is 0)

    // 0x004801C9: push 0x00480F30 (pre-pushed third argument); 0x004801CE: push 0; 0x004801D0: push eax;
    // 0x004801D1: mov word [0x5C1154],ax; 0x004801D7: call 0x401400.
    ::g_5c1154 = explosion0_id; // 0x004801D1: a plain 16-bit store into the 16-bit slot
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5c1154), // 0x004801DC: mov ax,[0x5C1154] (retail re-loads the word)
                                0U,                                     // 0x004801CE: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00480F30U))); // 0x004801C9: push handler

    // 0x004801E2: push 0x00480F40 (pre-pushed third argument); 0x004801E7: push 1; 0x004801E9: push eax; 0x004801EA: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5c1154), // 0x004801DC: mov ax,[0x5C1154]
                                           1U,                                     // 0x004801E7: push 1
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00480F40U))); // 0x004801E2: push handler

    // 0x004801F6: push 0x004D3710 (pre-pushed third argument); 0x004801FB: push 2; 0x004801FD: push ecx; 0x004801FE: call 0x401530;
    // 0x00480203: add esp,0x38 - 14 pushes above (the two inert zeros/0x20, the three registration arguments and three 3-argument links).
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5c1154), // 0x004801EF: mov cx,[0x5C1154]
                                           2U,                                     // 0x004801FB: push 2
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004D3710U))); // 0x004801F6: push handler

    // ---- Registration 2: "FireBall" ----------------------------------------------------------------------------------------------------
    // 0x00480206/0x00480208: push 0; push 0x60 (both inert arguments of the getter);
    // 0x0048020A: call 0x4C48C0; 0x0048020F: push eax; 0x00480210: push 0x118; 0x00480215: push "FireBall"; 0x0048021A: call 0x401900.
    const std::uint16_t fireball_id = TaskType_Register("FireBall", // 0x00480215: push 0x00540964
                                                        0x118U,       // 0x00480210: push 0x118 (object size)
                                                        Sys_GetGlobal16_4C48C0(), // 0x0048020A: call 0x4C48C0 (parent type)
                                                        0U);          // 0x00480208: push 0x60 (inert)

    // 0x0048021F: push 0x004805F0 (pre-pushed third argument); 0x00480224: push 0; 0x00480226: push eax;
    // 0x00480227: mov word [0x5C1168],ax; 0x0048022D: call 0x401400.
    ::g_5c1168 = fireball_id; // 0x00480227: a plain 16-bit store into the 16-bit slot
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5c1168), // 0x00480232: mov dx,[0x5C1168]
                                0U,                                     // 0x00480224: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004805F0U))); // 0x0048021F: push handler

    // 0x00480239: push 0x00480790 (pre-pushed third argument); 0x0048023E: push 4; 0x00480240: push edx; 0x00480241: call 0x401400;
    // 0x00480246: add esp,0x2c - 11 pushes above.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5c1168), // 0x00480232: mov dx,[0x5C1168]
                                4U,                                     // 0x0048023E: push 4 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00480790U))); // 0x00480239: push handler

    // FireBall link 3: 0x00480249: push 0x0045F920 (the pre-pushed handler, and the getter's inert argument);
    // 0x0048024E: call 0x416830 (event id); 0x00480253: push eax; 0x00480254: mov ax,[0x5C1168]; 0x0048025A: push eax;
    // 0x0048025B: call 0x401400; 0x00480260: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5c1168), // 0x00480254: mov ax,[0x5C1168]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416830()), // 0x00480253: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0045F920U))); // 0x00480249: push handler (argument 3)

    // FireBall link 4: 0x00480263: push 0x0045F920; 0x00480268: call 0x416850 (event id); 0x0048026D: mov cx,[0x5C1168];
    // 0x00480274: push eax; 0x00480275: push ecx; 0x00480276: call 0x401400; 0x0048027B: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5c1168), // 0x0048026D: mov cx,[0x5C1168]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416850()), // 0x00480274: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0045F920U))); // 0x00480263: push handler (argument 3)

    // FireBall link 5: 0x0048027E: push 0x0045F920; 0x00480283: call 0x416880 (event id); 0x00480288: mov dx,[0x5C1168];
    // 0x0048028F: push eax; 0x00480290: push edx; 0x00480291: call 0x401400 (no cleanup of its own).
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5c1168), // 0x00480288: mov dx,[0x5C1168]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416880()), // 0x0048028F: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0045F920U))); // 0x0048027E: push handler (argument 3)

    // FireBall link 6 (constant event 12): 0x0048029C: push 0x0045F920; 0x004802A1: push 0xc; 0x004802A3: push eax;
    // 0x004802A4: call 0x401400 (no cleanup of its own).
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5c1168), // 0x00480296: mov ax,[0x5C1168]
                                12U,                                    // 0x004802A1: push 0xc (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0045F920U))); // 0x0048029C: push handler (argument 3)

    // FireBall link 7: 0x004802B0: push 0x004806E0 (pre-pushed third argument); 0x004802B5: push 1; 0x004802B7: push ecx;
    // 0x004802B8: call 0x401530 (no cleanup of its own).
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5c1168), // 0x004802A9: mov cx,[0x5C1168]
                                           1U,                                     // 0x004802B5: push 1
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004806E0U))); // 0x004802B0: push handler

    // FireBall link 8: 0x004802C4: push 0x00480F00 (pre-pushed third argument); 0x004802C9: push 2; 0x004802CB: push edx;
    // 0x004802CC: call 0x401530; 0x004802D1: add esp,0x30 - 12 pushes above (four 3-argument links with no intermediate cleanup).
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5c1168), // 0x004802BD: mov dx,[0x5C1168]
                                           2U,                                     // 0x004802C9: push 2
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00480F00U))); // 0x004802C4: push handler

    // FireBall link 9: 0x004802D4: push 0x0045F920; 0x004802D9: call 0x489610 (event id); 0x004802DE: push eax;
    // 0x004802DF: mov ax,[0x5C1168]; 0x004802E5: push eax; 0x004802E6: call 0x401400; 0x004802EB: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5c1168), // 0x004802DF: mov ax,[0x5C1168]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_489610()), // 0x004802DE: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0045F920U))); // 0x004802D4: push handler (argument 3)

    // ---- Registration 3: "Fragment" ----------------------------------------------------------------------------------------------------
    // 0x004802EE/0x004802F0: push 0; push 0xa0 (both inert arguments of the getter);
    // 0x004802F5: call 0x503BF0; 0x004802FA: push eax; 0x004802FB: push 0x118; 0x00480300: push "Fragment"; 0x00480305: call 0x401900.
    const std::uint16_t fragment_id = TaskType_Register("Fragment", // 0x00480300: push 0x00540958
                                                        0x118U,       // 0x004802FB: push 0x118 (object size)
                                                        Sys_GetGlobal16_503BF0(), // 0x004802F5: call 0x503BF0 (parent type)
                                                        0U);          // 0x004802F0: push 0xa0 (inert)

    // 0x0048030A: push 0x004805B0 (pre-pushed third argument); 0x0048030F: push 1; 0x00480311: push eax;
    // 0x00480312: mov word [0x5C1156],ax; 0x00480318: call 0x401530. Note there is no event-0 link for this type.
    ::g_5c1156 = fragment_id; // 0x00480312: a plain 16-bit store into the 16-bit slot
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5c1156), // 0x0048031D: mov cx,[0x5C1156]
                                           1U,                                     // 0x0048030F: push 1
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004805B0U))); // 0x0048030A: push handler

    // Fragment link 2 (constant event 12): 0x00480324: push 0x0045F920 (pre-pushed third argument);
    // 0x00480329: push 0xc; 0x0048032B: push ecx; 0x0048032C: call 0x401400; 0x00480331: add esp,0x2c - 11 pushes above.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5c1156), // 0x0048031D: mov cx,[0x5C1156]
                                12U,                                    // 0x00480329: push 0xc (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0045F920U))); // 0x00480324: push handler (argument 3)

    // Fragment link 3: 0x00480334: push 0 (the pre-pushed third argument is NULL here, and the getter's inert argument);
    // 0x00480336: call 0x416830 (event id); 0x0048033B: mov dx,[0x5C1156]; 0x00480342: push eax; 0x00480343: push edx;
    // 0x00480344: call 0x401400; 0x00480349: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5c1156), // 0x0048033B: mov dx,[0x5C1156]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416830()), // 0x00480342: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0U))); // 0x00480334: push 0 (argument 3: a NULL handler)

    // Fragment link 4: 0x0048034C: push 0; 0x0048034E: call 0x416850 (event id); 0x00480353: push eax;
    // 0x00480354: mov ax,[0x5C1156]; 0x0048035A: push eax; 0x0048035B: call 0x401400; 0x00480360: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5c1156), // 0x00480354: mov ax,[0x5C1156]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416850()), // 0x00480353: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0U))); // 0x0048034C: push 0 (argument 3: a NULL handler)

    // Fragment link 5: 0x00480363: push 0; 0x00480365: call 0x416880 (event id); 0x0048036A: mov cx,[0x5C1156];
    // 0x00480371: push eax; 0x00480372: push ecx; 0x00480373: call 0x401400; 0x00480378: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5c1156), // 0x0048036A: mov cx,[0x5C1156]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416880()), // 0x00480371: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0U))); // 0x00480363: push 0 (argument 3: a NULL handler)

    // Fragment link 6: 0x0048037B: push 0; 0x0048037D: call 0x489610 (event id); 0x00480382: mov dx,[0x5C1156];
    // 0x00480389: push eax; 0x0048038A: push edx; 0x0048038B: call 0x401400; 0x00480390: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5c1156), // 0x00480382: mov dx,[0x5C1156]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_489610()), // 0x00480389: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0U))); // 0x0048037B: push 0 (argument 3: a NULL handler)

    // ---- Registration 4: "ExplosionSmoke" ----------------------------------------------------------------------------------------------
    // 0x00480393/0x00480395: push 0; push 0x40 (both inert arguments of the getter);
    // 0x00480397: call 0x4018F0; 0x0048039C: push eax; 0x0048039D: push 0x48; 0x0048039F: push "ExplosionSmoke"; 0x004803A4: call 0x401900.
    const std::uint16_t explosion_smoke_id = TaskType_Register("ExplosionSmoke", // 0x0048039F: push 0x00540948
                                                               0x48U,             // 0x0048039D: push 0x48 (object size)
                                                               QTask_GetCurrentType(), // 0x00480397: call 0x4018F0 (parent type)
                                                               0U);               // 0x00480395: push 0x40 (inert)

    // 0x004803A9: push 0x004D3710 (pre-pushed third argument); 0x004803AE: push 0; 0x004803B0: push eax;
    // 0x004803B1: mov word [0x5C115C],ax; 0x004803B7: call 0x401400.
    ::g_5c115c = explosion_smoke_id; // 0x004803B1: a plain 16-bit store into the 16-bit slot
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5c115c), // 0x004803BC: mov ax,[0x5C115C]
                                0U,                                     // 0x004803AE: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004D3710U))); // 0x004803A9: push handler

    // 0x004803C2: push 0x00480420 (pre-pushed third argument); 0x004803C7: push 1; 0x004803C9: push eax; 0x004803CA: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5c115c), // 0x004803BC: mov ax,[0x5C115C]
                                           1U,                                     // 0x004803C7: push 1
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00480420U))); // 0x004803C2: push handler

    // 0x004803D6: push 0x004D3710 (pre-pushed third argument); 0x004803DB: push 2; 0x004803DD: push ecx; 0x004803DE: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5c115c), // 0x004803CF: mov cx,[0x5C115C]
                                           2U,                                     // 0x004803DB: push 2
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004D3710U))); // 0x004803D6: push handler

    // ---- The three resource lookups ----------------------------------------------------------------------------------------------------
    // 0x004803E3: push "burn1.tex"; 0x004803E8: call 0x416930; 0x004803F2: mov dword [0x5C1164],eax - the first store lands before the second
    // call, exactly as retail orders it.
    ::g_5c1164 = static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(Sys_LookupOrAppendRecord_416930("burn1.tex"))); // 0x004803E3: push 0x00540918

    // 0x004803ED: push "smoke2.spr"; 0x004803F7: call 0x4169D0; 0x004803FC: add esp,0x40 - 16 pushes above (14 from the fourth registration
    // plus the two resource names); 0x004803FF: mov dword [0x5C1160],eax - the second store lands after the cleanup, as retail orders it.
    const void* const smoke_record = Sys_LookupOrAppendRecord_4169D0("smoke2.spr"); // 0x004803ED: push 0x0053C130
    ::g_5c1160 = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(smoke_record)); // 0x004803FF

    // 0x00480404: push "fireball.spr"; 0x00480409: call 0x4169D0; 0x0048040E: add esp,0x4 - 1 push; 0x00480411: mov dword [0x5C1158],eax;
    // 0x00480416: ret.
    ::g_5c1158 = static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(Sys_LookupOrAppendRecord_4169D0("fireball.spr"))); // 0x00480404: push 0x0053C154
} // end of Task_DefineExplosionTypes

} // extern "C"
