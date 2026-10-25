// Native: Task_DefineLevelFlow (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00488BB0-0x00488DF3.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 580 bytes, 1 block, 171 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 171 instructions / 580 bytes / 0 edges; `pd`
//   stops at the ret at 0x00488DF3) plus the objdump listing build/retail-dis.txt for the same extent; `ps` at 0x00541148/0x00541154/0x00541134/
//   0x00541114/0x005410FC/0x005410E4 for the strings; a full stack-pointer simulation over the 171 instructions, which returns esp to its entry
//   value at the ret and confirms every `add esp` (0x38 = 14 pushes, 0xc = 3 each, 0x1c = 7, 0x18 = 6 each, 0x40 = 16, 0x138 = the 0x128 frame
//   plus the final 4 pushes). The seven 0x004F1A80 calls were also checked against 0x004F1A80's own bytes: it reads its second and third
//   arguments at [esp+8]/[esp+0xC], subtracts them at 0x004F1A88 and never dereferences either, so the values that reach the container are the
//   differences 0x20, 0x38, 0x6C, 0x7C, 0xCC, 0x70, 0x74.
// Behavior/CFG: registers the "LevelFlow" task type as TaskType_Register("LevelFlow", 0x128, Sys_GetGlobal16_5034D0(), 0) and stores the returned
//   16-bit id at 0x005C8AB8 after loading the model name "306_01_1" through 0x004CEBA0 into 0x005C8AB4; links the id to handler 0x00488ED0 on event
//   0 through 0x00401400, then to 0x00489290 on event 1 and 0x00489200 on event 2 through 0x00401530, then to 0x00489280 on the event id from
//   0x004F1A60 and 0x00489400 on the event id from 0x004F1A70 through 0x00401400, then to 0x00488F90 on the event id from 0x004F1210 and
//   0x004890F0 on the event id from 0x004F1220 through 0x00401530, and finally to a NULL handler on event 7 through 0x00401400 (retail really
//   pushes 0 there: 0x00488C8C `push 0`, 0x00488C8E `push 7`, 0x00488C90 `push eax`); allocates a 7-entry parameter container through 0x004F1950
//   into 0x005C8AA4 and appends seven descriptors to it through 0x004F1A80, with the type values coming from 0x004F1010, 0x004F0F70, 0x004F0F50,
//   0x004F0FC0 (twice), 0x004F1000 and 0x004F0F50 again and with 0x004F1CB0 called before the third one; then registers the script name
//   "TASKTYPE_LEVELFLOW" for the id through 0x004B8810 and the three script commands "LevelFlow_GetBreakCutSceneKey" (0x00488E00),
//   "LevelFlow_LevelFailed" (0x00488E50) and "LevelFlow_IsCountryUSA" (0x00488E90) through 0x004B8890. Straight-line: one basic block, no
//   branch, no loop, 35 call sites over 19 distinct callees.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Three documented shape differences, all value-preserving: (1) the 0x004F0F50/0x004F0F70/
//   0x004F0FC0/0x004F1000/0x004F1010/0x004F1210/0x004F1220 callees are single-instruction getters in retail that ignore the arguments the caller
//   pushes (verified: each body is `mov eax,[global]; ret`), and the tree's transcriptions take no arguments, so those inert pushes are not
//   reproduced and only the call order and values are kept; (2) the same applies to the two zeros pushed before 0x005034D0; (3) retail's
//   0x128-byte frame is not reproduced - only the two addresses whose difference 0x004F1A80 records are, via one addressable dword plus the
//   retail offsets, because that difference is the only thing the callee reads from them. Handlers 0x00488ED0, 0x00489290, 0x00489200,
//   0x00489280, 0x00489400, 0x00488F90, 0x004890F0, 0x00488E00, 0x00488E50 and 0x00488E90 are raw code immediates in retail and are not
//   transcribed yet (native gaps). 0x005C8AB4, 0x005C8AB8 and 0x005C8AA4 have no other owner and image value 0, so this file defines them. The
//   string 0x00541160 ("LOCAL:menusystem/ingamemenu.qsc") sits immediately after this function's strings in the image but is NOT referenced by
//   any of the 171 instructions, so it is not part of this native. Domain: core/, because it is one of the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint8_t* MTP_AddLODVirModel_4CEBA0(const char* name);   // 0x004CEBA0
extern "C" std::uint16_t Sys_GetGlobal16_5034D0(void);                  // 0x005034D0: argument-less word getter
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);               // 0x004F1A60: argument-less byte getter
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);               // 0x004F1A70: argument-less byte getter
extern "C" std::uint32_t Sys_GetGlobal32_4F1210(void);                  // 0x004F1210: argument-less dword getter
extern "C" std::uint32_t Sys_GetGlobal32_4F1220(void);                  // 0x004F1220: argument-less dword getter
extern "C" std::uint32_t Sys_GetGlobal32_4F1010(void);                  // 0x004F1010: argument-less dword getter
extern "C" std::uint32_t Sys_GetGlobal32_4F0F70(void);                  // 0x004F0F70: argument-less dword getter
extern "C" std::uint32_t Sys_GetGlobal32_4F0FC0(void);                  // 0x004F0FC0: argument-less dword getter
extern "C" std::uint32_t Sys_GetGlobal32_4F0F50(void);                  // 0x004F0F50: argument-less dword getter
extern "C" std::uint32_t Camera_Get_4F1000(void);                       // 0x004F1000: argument-less dword getter
extern "C" void* Sys_InitFloatPair_4F1CB0(void);                        // 0x004F1CB0: returns the address of the pair it initialises
extern "C" void* Script_TaskContainer_Alloc(std::int32_t const count);  // 0x004F1950
// 0x004F1A80 (script/native_4f1a80.cpp). SIX arguments, corrected: the callee reads arg1..arg4, arg5 as a byte (recorded at record +0x0C)
// and arg6 as a dword (recorded at record +0x08). The two reads are both `[esp+0x20]` in retail, at different push depths, with a `pop edi`
// between them (0x004F1AB1 with twelve bytes pushed = arg5, 0x004F1ACA with eight = arg6). Earlier revisions of this file declared five
// parameters and the tree's implementation wrote arg5 into both fields; both are now corrected. Every append below passes the sixth word that
// retail pushes for it, so the 0x004F1CB0 result at 0x00488CEE is a real argument rather than a discarded one.
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5, std::int32_t const arg6);  // 0x004F1A80 (copied from script/native_4f1a80.cpp)
extern "C" void Script_QHashNodeRegister(void* const arg1, void* const value); // 0x004B8810
extern "C" void Script_QHashNodeRegisterEx(void* const arg0, void* const arg1, char const* const arg2,
                                           void* const arg3); // 0x004B8890 (copied from script/native_4b8890.cpp)

extern "C" {

std::uint8_t* g_5c8ab4 = nullptr; // retail 0x005C8AB4: the "306_01_1" model pointer from 0x004CEBA0 (image value 0)
std::uint16_t g_5c8ab8 = 0U;      // retail 0x005C8AB8: the "LevelFlow" task type id (image value 0)
void* g_5c8aa4 = nullptr;         // retail 0x005C8AA4: the parameter container from 0x004F1950 (image value 0)

void Task_DefineLevelFlow()
{
    // 0x00488BB0: sub esp,0x128 - retail's 296-byte frame. The seven 0x004F1A80 calls below pass two addresses inside it: its first dword
    // (each `lea edx,[esp+0xc]` runs with 12 bytes of arguments already pushed, so the address is frame+0x00) and one later slot at retail
    // offsets 0x20, 0x38, 0x6C, 0x7C, 0xCC, 0x70 and 0x74. 0x004F1A80 reads those two arguments at [esp+8]/[esp+0xC], subtracts them at
    // 0x004F1A88 and never dereferences them, so the port keeps one addressable dword and passes that address plus the same retail offset.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x00488BB6: push 0x00541154 ("306_01_1"); 0x00488BBB: call 0x4CEBA0; 0x00488BC0: add esp,4; 0x00488BC3: mov [0x5C8AB4],eax.
    ::g_5c8ab4 = MTP_AddLODVirModel_4CEBA0("306_01_1");

    // 0x00488BC8/0x00488BCA: push 0; push 0 (inert arguments of the getter, and TaskType_Register's fourth/fifth slots);
    // 0x00488BCC: call 0x5034D0; 0x00488BD1: push eax; 0x00488BD2: push 0x128; 0x00488BD7: push "LevelFlow"; 0x00488BDC: call 0x401900.
    const std::uint16_t class_id = TaskType_Register("LevelFlow", // 0x00488BD7: push 0x00541148
                                                     0x128U,       // 0x00488BD2: push 0x128 (object size)
                                                     Sys_GetGlobal16_5034D0(), // 0x00488BCC: call 0x5034D0 (parent type)
                                                     0U);          // 0x00488BCA: push 0 (task list count)

    // 0x00488BE1: push 0x00488ED0 (pre-pushed third argument); 0x00488BE6: push 0; 0x00488BE8: push eax;
    // 0x00488BE9: mov word [0x5C8AB8],ax; 0x00488BEF: call 0x401400.
    ::g_5c8ab8 = class_id; // 0x00488BE9: the id is stored between the argument pushes and the call, exactly as retail orders it
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5c8ab8), // 0x00488BF4: mov ax,[0x5C8AB8] (retail re-loads the word)
                                0U,                                     // 0x00488BE6: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00488ED0U))); // 0x00488BE1: push handler

    // 0x00488BFA: push 0x00489290 (pre-pushed third argument); 0x00488BFF: push 1; 0x00488C01: push eax; 0x00488C02: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5c8ab8), // 0x00488C07: mov ax,[0x5C8AB8]
                                           1U,                                     // 0x00488BFF: push 1
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00489290U))); // 0x00488BFA: push handler

    // 0x00488C0E: push 0x00489200 (pre-pushed third argument); 0x00488C13: push 2; 0x00488C15: push ecx; 0x00488C16: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5c8ab8), // 0x00488C07: mov cx,[0x5C8AB8]
                                           2U,                                     // 0x00488C13: push 2
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00489200U))); // 0x00488C0E: push handler
    // 0x00488C1B: add esp,0x38 - 14 pushes above (2 inert zeros + 3 registration arguments + three 3-argument links).

    // 0x00488C1E: push 0x00489280 (pre-pushed third argument); 0x00488C23: call 0x4F1A60 (event id);
    // 0x00488C2F: push eax; 0x00488C30: push edx; 0x00488C31: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5c8ab8),      // 0x00488C28: mov dx,[0x5C8AB8]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()), // 0x00488C2F: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00489280U))); // 0x00488C1E: push handler (argument 3)
    // 0x00488C36: add esp,0xc - 3 pushes above.

    // 0x00488C39: push 0x00489400 (pre-pushed third argument); 0x00488C3E: call 0x4F1A70 (event id);
    // 0x00488C43: push eax; 0x00488C4A: push eax; 0x00488C4B: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5c8ab8),      // 0x00488C44: mov ax,[0x5C8AB8]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()), // 0x00488C43: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00489400U))); // 0x00488C39: push handler (argument 3)
    // 0x00488C50: add esp,0xc - 3 pushes above.

    // 0x00488C53: push 0x00488F90 (pre-pushed third argument); 0x00488C58: call 0x4F1210 (event id);
    // 0x00488C64: push eax; 0x00488C65: push ecx; 0x00488C66: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5c8ab8), // 0x00488C5D: mov cx,[0x5C8AB8]
                                           static_cast<std::uint32_t>(Sys_GetGlobal32_4F1210()), // 0x00488C64: push eax
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00488F90U))); // 0x00488C53: push handler
    // 0x00488C6B: add esp,0xc - 3 pushes above.

    // 0x00488C6E: push 0x004890F0 (pre-pushed third argument); 0x00488C73: call 0x4F1220 (event id);
    // 0x00488C7F: push eax; 0x00488C80: push edx; 0x00488C81: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5c8ab8), // 0x00488C78: mov dx,[0x5C8AB8]
                                           static_cast<std::uint32_t>(Sys_GetGlobal32_4F1220()), // 0x00488C7F: push eax
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004890F0U))); // 0x00488C6E: push handler
    // 0x00488C86/0x00488C8C/0x00488C8E/0x00488C90/0x00488C91: the NULL handler on event 7.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5c8ab8), // 0x00488C86: mov ax,[0x5C8AB8]
                                7U,                                     // 0x00488C8E: push 7 (argument 2)
                                nullptr);                               // 0x00488C8C: push 0 (argument 3; retail registers a NULL handler here)

    // 0x00488C96: push 7; 0x00488C98: call 0x4F1950; 0x00488C9D: add esp,0x1c (7 pushes); 0x00488CA0: mov [0x5C8AA4],eax.
    ::g_5c8aa4 = Script_TaskContainer_Alloc(7); // 0x00488C96: push 7 (the container's entry count)

    // 0x00488CA5/0x00488CA7: push 0; push 7 (inert arguments); 0x00488CA9: call 0x4F1010; 0x00488CAE: lea ecx,[esp+0x28] (frame+0x20);
    // 0x00488CB2: push eax; 0x00488CB8: lea edx,[esp+0xc] (frame+0x00); 0x00488CBC/0x00488CBD/0x00488CBE: the three argument pushes;
    // 0x00488CBF: call 0x4F1A80; 0x00488CC4: add esp,0x18 (6 pushes).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c8aa4),  // argument 1: the container
                                append_frame,                                 // argument 2: retail frame+0x00
                                append_frame + 0x20,                          // argument 3: retail frame+0x20 (recorded difference 0x20)
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1010()), // argument 4: the type from 0x4F1010
                                7, 0);                                           // argument 5: retail's leftover 7

    // 0x00488CC7/0x00488CC9: push 0; push 7 (inert); 0x00488CCB: call 0x4F0F70; 0x00488CD0: push eax; 0x00488CD1: lea ecx,[esp+0x44]
    // (frame+0x38); 0x00488CDA: lea edx,[esp+0xc]; 0x00488CE1: call 0x4F1A80; 0x00488CE6: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c8aa4),
                                append_frame,
                                append_frame + 0x38,                          // 0x00488CD1: frame+0x38 (recorded difference 0x38)
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F70()),
                                7, 0);

    // 0x00488CE9: call 0x4F1CB0; 0x00488CEE: push eax; 0x00488CEF: push 7; 0x00488CF1: call 0x4F0F50; 0x00488CF6: lea ecx,[esp+0x74]
    // (frame+0x6C); 0x00488D00: lea edx,[esp+0xc]; 0x00488D07: call 0x4F1A80; 0x00488D0C: add esp,0x18.
    // 0x00488CE9 returns the address of the pair it initialises (0x00A76BE8), and retail pushes that result first at 0x00488CEE, which makes
    // it the SIXTH and last argument of the 0x4F1A80 call below; the side effect of the initialisation is kept as well.
    const std::uint32_t float_pair_address =
        static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Sys_InitFloatPair_4F1CB0())); // 0x00488CE9: call 0x4F1CB0
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c8aa4),
                                append_frame,
                                append_frame + 0x6C,                          // 0x00488CF6: frame+0x6C (recorded difference 0x6C)
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()),
                                7,                                            // argument 5: retail's leftover 7
                                static_cast<std::int32_t>(float_pair_address)); // argument 6: 0x00A76BE8

    // 0x00488D0F/0x00488D11: push 0; push 7 (inert); 0x00488D13: call 0x4F0FC0; 0x00488D18: lea ecx,[esp+0x84] (frame+0x7C);
    // 0x00488D25: lea edx,[esp+0xc]; 0x00488D2C: call 0x4F1A80; 0x00488D31: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c8aa4),
                                append_frame,
                                append_frame + 0x7C,                          // 0x00488D18: frame+0x7C (recorded difference 0x7C)
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()),
                                7, 0);

    // 0x00488D34/0x00488D36: push 0; push 7 (inert); 0x00488D38: call 0x4F0FC0; 0x00488D3D: lea ecx,[esp+0xd4] (frame+0xCC);
    // 0x00488D4A: lea edx,[esp+0xc]; 0x00488D51: call 0x4F1A80; 0x00488D56: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c8aa4),
                                append_frame,
                                append_frame + 0xCC,                          // 0x00488D3D: frame+0xCC (recorded difference 0xCC)
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), // second use of the same getter, exactly as retail
                                7, 0);

    // 0x00488D59/0x00488D5B: push 0; push 7 (inert); 0x00488D5D: call 0x4F1000; 0x00488D62: lea ecx,[esp+0x78] (frame+0x70);
    // 0x00488D6C: lea edx,[esp+0xc]; 0x00488D73: call 0x4F1A80; 0x00488D78: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c8aa4),
                                append_frame,
                                append_frame + 0x70,                          // 0x00488D62: frame+0x70 (recorded difference 0x70)
                                static_cast<std::int32_t>(Camera_Get_4F1000()),
                                7, 0);

    // 0x00488D7B/0x00488D7D: push 0; push 7 (inert); 0x00488D7F: call 0x4F0F50; 0x00488D84: lea ecx,[esp+0x7c] (frame+0x74);
    // 0x00488D8E: lea edx,[esp+0xc]; 0x00488D95: call 0x4F1A80 (no cleanup of its own: 0x00488DD7 cleans this group together with the next two).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5c8aa4),
                                append_frame,
                                append_frame + 0x74,                          // 0x00488D84: frame+0x74 (recorded difference 0x74)
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), // second use of the same getter, exactly as retail
                                7, 0);

    // 0x00488D9A: mov ecx,[0x5C8AB8]; 0x00488DA0: and ecx,0xffff; 0x00488DA6: push ecx; 0x00488DA7: push "TASKTYPE_LEVELFLOW"; 0x00488DAC: call 0x4B8810.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_LEVELFLOW"), // 0x00488DA7: push 0x00541134
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(
                                 static_cast<std::uint32_t>(::g_5c8ab8) & 0xFFFFU))); // 0x00488DA6: push ecx

    // 0x00488DB1/0x00488DB3/0x00488DB5/0x00488DBA/0x00488DBF: the first script command.
    Script_QHashNodeRegisterEx(const_cast<char*>("LevelFlow_GetBreakCutSceneKey"), // 0x00488DBA: push 0x00541114
                               reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00488E00U)), // 0x00488DB5: push 0x00488E00
                               nullptr,                                        // 0x00488DB3: push 0
                               nullptr);                                       // 0x00488DB1: push 0

    // 0x00488DC4/0x00488DC6/0x00488DC8/0x00488DCD/0x00488DD2: the second script command.
    Script_QHashNodeRegisterEx(const_cast<char*>("LevelFlow_LevelFailed"), // 0x00488DCD: push 0x005410FC
                               reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00488E50U)), // 0x00488DC8: push 0x00488E50
                               nullptr,
                               nullptr);
    // 0x00488DD7: add esp,0x40 - 16 pushes above (6 from the last 0x4F1A80 group + 2 + 4 + 4).

    // 0x00488DDA/0x00488DDC/0x00488DDE/0x00488DE3/0x00488DE8: the third script command.
    Script_QHashNodeRegisterEx(const_cast<char*>("LevelFlow_IsCountryUSA"), // 0x00488DE3: push 0x005410E4
                               reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00488E90U)), // 0x00488DDE: push 0x00488E90
                               nullptr,
                               nullptr);
    // 0x00488DED: add esp,0x138 - the 0x128 frame plus the 4 pushes above; 0x00488DF3: ret.
} // end of Task_DefineLevelFlow

} // extern "C"
