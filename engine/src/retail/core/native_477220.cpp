// Native: Task_DefineGunParts (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00477220-0x004775FE.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 991 bytes, 1 block, 267 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 267 instructions / 991 bytes / 0 edges; `pd`
//   stops at the ret at 0x004775FE) plus the objdump listing build/retail-dis.txt for the same extent, read in three explicit ranges
//   (0x00477220-0x00477345, 0x00477345-0x00477480, 0x0047747D-0x004775FF); `ps` at 0x005407AC, 0x005407A4, 0x00540794, 0x00540780,
//   0x0054076C and 0x00540754 for the six literals (byte-identical to "GunFlame", "GunClip", "GunMovingPart", "TASKTYPE_GUNFLAME",
//   "TASKTYPE_GUNCLIP", "TASKTYPE_GUNMOVINGPART"); `px 16` at 0x0054074C for the slot image (81 01 / 81 01 / 81 01 / 00 00 then the string);
//   a full stack-pointer simulation over all 267 instructions, which returns esp to its entry value at the ret and accounts for every cleanup
//   (0x38 = 14 pushes three times, 0xc twenty-nine times, 0x24 = 9 at the tail). Every getter callee was re-read at its own VA and is a
//   single-instruction argument-less load ending in a ret. Argument numbers were derived by counting the pushes above each access, not by
//   reading an [esp+N] offset as an argument number.
// Behavior/CFG: one straight-line body that registers THREE task types of size 0x140 with the same parent and then wires thirty-five handler
//   links plus three script-name registrations, in retail order:
//   (1) "GunFlame" -> id at 0x0054074C, e0->0x00477600 (0x00401400), e1->0x004D3710 and e2->0x004776A0 (0x00401530), then the event ids from
//       0x004F1A70->0x004D3710, 0x004C56D0->0x004D3710, 0x004C56E0->0x004EFA10, 0x004C56F0->0x004778E0, 0x004D9FB0->0x004776C0,
//       0x004D9850->0x004779E0, 0x004CEA20 with a NULL handler, 0x004CEA10 with a NULL handler, 0x0047E6F0 with a NULL handler, and
//       0x00416830->0x0045E3D0.
//   (2) "GunClip" -> id at 0x0054074E, e0->0x00477A70 (0x00401400), e1/e2->0x004D3710 (0x00401530), then 0x004F1A70->0x004D3710,
//       0x004C56D0->0x004D3710, 0x004C56E0->0x004EFA10, 0x004C56F0->0x00472880, 0x004CEA20 NULL, 0x004CEA10 NULL, 0x0047E6F0 NULL,
//       0x00416830->0x0045E3D0. This type has NO 0x004D9FB0 and NO 0x004D9850 link, and its 0x004C56F0 handler is 0x00472880 rather than
//       0x004778E0 - the three types are NOT the same link set, so the differences are transcribed rather than assumed.
//   (3) "GunMovingPart" -> id at 0x00540750, e0->0x00477A70 (0x00401400), e1/e2->0x004D3710 (0x00401530), then 0x004F1A70->0x004D3710,
//       0x004C56D0->0x004D3710, 0x004C56E0->0x004EFA10, 0x004C56F0->0x00477B10, 0x004CEA20 NULL, 0x004CEA10 NULL, 0x0047E6F0 NULL,
//       0x00416830->0x0045E3D0.
//   (4) three script-name tails through 0x004B8810: ("TASKTYPE_GUNFLAME", GunFlame id), ("TASKTYPE_GUNCLIP", GunClip id) and
//       ("TASKTYPE_GUNMOVINGPART", GunMovingPart id), each zero-extending its 16-bit id to 32 bits first.
//   One basic block, no branch, no loop, no frame, no parameter container, 70 call sites over 15 distinct callees.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. All three parent lookups are the same getter, 0x004D9FA0, called twice with two inert zero
//   arguments each time. Documented shape differences, all value-preserving: (1) every getter callee is an argument-less load in retail that
//   ignores the arguments the caller pushes, and the tree's transcriptions take no arguments, so those inert pushes are not reproduced; each is
//   simultaneously the next 0x00401400/0x00401530 link call's pre-pushed handler (or a NULL handler when the push is 0), which the port keeps
//   as the handler value in the same order; (2) byte-valued event ids are zero-extended in the port, which cannot change behaviour because every
//   link fed by a byte getter here goes through 0x00401400, which masks the event word to its low byte for the phase index. The three id slots
//   0x0054074C/0x0054074E/0x00540750 each hold image value 0x0181 before TaskType_Register overwrites them with the assigned id, and each is
//   written with a 16-bit store and read back with a 32-bit load masked by 0xFFFF, so each is declared as a plain std::uint16_t; the two bytes
//   at 0x00540752-0x00540753 are referenced nowhere in build/retail-dis.txt and are NOT modelled here. This is the fourth, fifth and sixth
//   observed image occurrence of 0x0181 in an id/parent slot (with 0x00548698, 0x00547F42, 0x00548648), which is evidence for 0x0181 being a
//   real type id that these slots are seeded with rather than a universal default. The three script-name tails use three DIFFERENT
//   zero-extension sequences in retail - `mov edx,[0x54074C]; and edx,0xFFFF` for the first, `xor eax,eax; mov ax,[0x54074E]` for the second,
//   `mov ecx,[0x540750]; and ecx,0xFFFF` for the third - all three are transcribed as the same zero-extended value, and the port keeps retail's
//   statement order. Handlers 0x00477600, 0x004776A0, 0x004778E0, 0x004776C0, 0x004779E0, 0x00477A70, 0x00472880 and 0x00477B10 are raw
//   code immediates in retail and are not transcribed yet (native gaps in 0x0046xxxx-0x0048xxxx); 0x004D3710 and 0x004EFA10 are already
//   transcribed. Handler 0x0045E3D0 is BELOW 0x00460000 and therefore belongs to the 0x00400000-0x0045FFFF owner, not to this file. Domain:
//   core/, because it is one of the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4D9FA0(void);              // 0x004D9FA0: argument-less word getter (parent type)
extern "C" std::uint8_t Sys_GetGlobal8_4D9FB0(void);                // 0x004D9FB0: argument-less byte getter
extern "C" std::uint8_t DynObj_GetByte_4C56D0(void);                // 0x004C56D0: argument-less byte getter (dynobj/native_4c56d0.cpp)
extern "C" std::uint8_t DynObj_GetByte_4C56E0(void);                // 0x004C56E0: argument-less byte getter (dynobj/native_4c56e0.cpp)
extern "C" std::uint8_t DynObj_GetByte_4C56F0(void);                // 0x004C56F0: argument-less byte getter (dynobj/native_4c56f0.cpp)
extern "C" std::uint8_t Sys_GetGlobal8_4D9850(void);                // 0x004D9850: argument-less byte getter (sys/native_4d9850.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);           // 0x004F1A70: argument-less byte getter (script/script_state.hpp)
extern "C" std::uint8_t QTask_PhaseIdGet548177_4CEA20(void);        // 0x004CEA20: argument-less byte getter (script/native_4cea20.cpp)
extern "C" std::uint8_t QTask_PhaseIdGet548176_4CEA10(void);        // 0x004CEA10: argument-less byte getter (script/native_4cea10.cpp)
extern "C" std::uint8_t Sys_GetGlobal8_47E6F0(void);                // 0x0047E6F0: argument-less byte getter (sys/native_47e6f0.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416830(void);            // 0x00416830: argument-less byte getter
extern "C" void Script_QHashNodeRegister(void* arg1, void* value);  // 0x004B8810 (script/native_4b8810.cpp)

extern "C" {

// 0x0054074C, 0x0054074E, 0x00540750: the three task type ids. Retail image value 0x0181 for each (px 16 at 0x0054074C shows 81 01 / 81 01 /
// 81 01 / 00 00), each overwritten by a 16-bit store from TaskType_Register's return value.
std::uint16_t g_54074c = 0x0181U;
std::uint16_t g_54074e = 0x0181U;
std::uint16_t g_540750 = 0x0181U;

void Task_DefineGunParts()
{
    // ---- Registration 1: "GunFlame" ----------------------------------------------------------------------------------------------------
    // 0x00477220/0x00477222: push 0; push 0 (both inert arguments of the parent getter); 0x00477224: call 0x4D9FA0; 0x00477229: push eax;
    // 0x0047722A: push 0x140; 0x0047722F: push "GunFlame"; 0x00477234: call 0x401900.
    const std::uint16_t gun_flame_id = TaskType_Register("GunFlame", // 0x0047722F: push 0x005407AC
                                                         0x140U,      // 0x0047722A: push 0x140 (object size)
                                                         Sys_GetGlobal16_4D9FA0(), // 0x00477224: call 0x4D9FA0 (parent type)
                                                         0U);         // 0x00477222: push 0 (inert)

    // 0x00477239: push 0x00477600 (pre-pushed handler); 0x0047723E: push 0; 0x00477240: push eax;
    // 0x00477241: mov word [0x54074C],ax (the store lands between the pushes and the call); 0x00477247: call 0x401400.
    ::g_54074c = gun_flame_id;
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54074c), // 0x0047724C: mov ax,[0x54074C]
                                0U,                                     // 0x0047723E: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00477600U))); // 0x00477239: push handler

    // 0x00477252: push 0x004D3710; 0x00477257: push 1; 0x00477259: push eax; 0x0047725A: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_54074c), // 0x0047724C: mov ax,[0x54074C]
                                           1U,                                     // 0x00477257: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004D3710U))); // 0x00477252: push handler

    // 0x0047725F: mov cx,[0x54074C]; 0x00477266: push 0x004776A0; 0x0047726B: push 2; 0x0047726D: push ecx; 0x0047726E: call 0x401530;
    // 0x00477273: add esp,0x38 - 14 pushes above.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_54074c), // 0x0047725F: mov cx,[0x54074C]
                                           2U,                                     // 0x0047726B: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004776A0U))); // 0x00477266: push handler

    // 0x00477276: push 0x004D3710 (pre-pushed handler); 0x0047727B: call 0x4F1A70 (event id); 0x00477280: mov dx,[0x54074C];
    // 0x00477287: push eax; 0x00477288: push edx; 0x00477289: call 0x401400; 0x0047728E: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54074c), // 0x00477280: mov dx,[0x54074C]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()), // 0x00477287: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004D3710U))); // 0x00477276: push handler

    // 0x00477291: push 0x004D3710; 0x00477296: call 0x4C56D0; 0x0047729B: push eax; 0x0047729C: mov ax,[0x54074C]; 0x004772A2: push eax;
    // 0x004772A3: call 0x401400; 0x004772A8: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54074c), // 0x0047729C: mov ax,[0x54074C]
                                static_cast<std::uint32_t>(DynObj_GetByte_4C56D0()), // 0x0047729B: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004D3710U))); // 0x00477291: push handler

    // 0x004772AB: push 0x004EFA10; 0x004772B0: call 0x4C56E0; 0x004772B5: mov cx,[0x54074C]; 0x004772BC: push eax; 0x004772BD: push ecx;
    // 0x004772BE: call 0x401400; 0x004772C3: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54074c), // 0x004772B5: mov cx,[0x54074C]
                                static_cast<std::uint32_t>(DynObj_GetByte_4C56E0()), // 0x004772BC: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004EFA10U))); // 0x004772AB: push handler (already transcribed)

    // 0x004772C6: push 0x004778E0; 0x004772CB: call 0x4C56F0; 0x004772D0: mov dx,[0x54074C]; 0x004772D7: push eax; 0x004772D8: push edx;
    // 0x004772D9: call 0x401400; 0x004772DE: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54074c), // 0x004772D0: mov dx,[0x54074C]
                                static_cast<std::uint32_t>(DynObj_GetByte_4C56F0()), // 0x004772D7: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004778E0U))); // 0x004772C6: push handler

    // 0x004772E1: push 0x004776C0; 0x004772E6: call 0x4D9FB0; 0x004772EB: push eax; 0x004772EC: mov ax,[0x54074C]; 0x004772F2: push eax;
    // 0x004772F3: call 0x401400; 0x004772F8: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54074c), // 0x004772EC: mov ax,[0x54074C]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_4D9FB0()), // 0x004772EB: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004776C0U))); // 0x004772E1: push handler

    // 0x004772FB: push 0x004779E0; 0x00477300: call 0x4D9850; 0x00477305: mov cx,[0x54074C]; 0x0047730C: push eax; 0x0047730D: push ecx;
    // 0x0047730E: call 0x401400; 0x00477313: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54074c), // 0x00477305: mov cx,[0x54074C]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_4D9850()), // 0x0047730C: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004779E0U))); // 0x004772FB: push handler

    // 0x00477316: push 0 (a NULL handler, and the getter's inert argument); 0x00477318: call 0x4CEA20; 0x0047731D: mov dx,[0x54074C];
    // 0x00477324: push eax; 0x00477325: push edx; 0x00477326: call 0x401400; 0x0047732B: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54074c), // 0x0047731D: mov dx,[0x54074C]
                                static_cast<std::uint32_t>(QTask_PhaseIdGet548177_4CEA20()), // 0x00477324: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0U))); // 0x00477316: push 0 (argument 3: a NULL handler)

    // 0x0047732E: push 0; 0x00477330: call 0x4CEA10; 0x00477335: push eax; 0x00477336: mov ax,[0x54074C]; 0x0047733C: push eax;
    // 0x0047733D: call 0x401400; 0x00477342: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54074c), // 0x00477336: mov ax,[0x54074C]
                                static_cast<std::uint32_t>(QTask_PhaseIdGet548176_4CEA10()), // 0x00477335: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0U))); // 0x0047732E: push 0 (argument 3: a NULL handler)

    // 0x00477345: push 0; 0x00477347: call 0x47E6F0; 0x0047734C: mov cx,[0x54074C]; 0x00477353: push eax; 0x00477354: push ecx;
    // 0x00477355: call 0x401400; 0x0047735A: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54074c), // 0x0047734C: mov cx,[0x54074C]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_47E6F0()), // 0x00477353: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0U))); // 0x00477345: push 0 (argument 3: a NULL handler)

    // 0x0047735D: push 0x0045E3D0; 0x00477362: call 0x416830; 0x00477367: mov dx,[0x54074C]; 0x0047736E: push eax; 0x0047736F: push edx;
    // 0x00477370: call 0x401400; 0x00477375: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54074c), // 0x00477367: mov dx,[0x54074C]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416830()), // 0x0047736E: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0045E3D0U))); // 0x0047735D: push handler (below 0x00460000)

    // ---- Registration 2: "GunClip" -----------------------------------------------------------------------------------------------------
    // 0x00477378/0x0047737A: push 0; push 0; 0x0047737C: call 0x4D9FA0; 0x00477381: push eax; 0x00477382: push 0x140;
    // 0x00477387: push "GunClip"; 0x0047738C: call 0x401900.
    const std::uint16_t gun_clip_id = TaskType_Register("GunClip", // 0x00477387: push 0x005407A4
                                                        0x140U,      // 0x00477382: push 0x140
                                                        Sys_GetGlobal16_4D9FA0(), // 0x0047737C: call 0x4D9FA0
                                                        0U);         // 0x0047737A: push 0 (inert)

    // 0x00477391: push 0x00477A70; 0x00477396: push 0; 0x00477398: push eax; 0x00477399: mov word [0x54074E],ax; 0x0047739F: call 0x401400.
    ::g_54074e = gun_clip_id;
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54074e), // 0x004773A4: mov ax,[0x54074E]
                                0U,                                     // 0x00477396: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00477A70U))); // 0x00477391: push handler

    // 0x004773AA: push 0x004D3710; 0x004773AF: push 1; 0x004773B1: push eax; 0x004773B2: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_54074e), // 0x004773A4: mov ax,[0x54074E]
                                           1U,                                     // 0x004773AF: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004D3710U))); // 0x004773AA: push handler

    // 0x004773B7: mov cx,[0x54074E]; 0x004773BE: push 0x004D3710; 0x004773C3: push 2; 0x004773C5: push ecx; 0x004773C6: call 0x401530;
    // 0x004773CB: add esp,0x38.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_54074e), // 0x004773B7: mov cx,[0x54074E]
                                           2U,                                     // 0x004773C3: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004D3710U))); // 0x004773BE: push handler

    // 0x004773CE: push 0x004D3710; 0x004773D3: call 0x4F1A70; 0x004773D8: mov dx,[0x54074E]; 0x004773DF: push eax; 0x004773E0: push edx;
    // 0x004773E1: call 0x401400; 0x004773E6: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54074e), // 0x004773D8: mov dx,[0x54074E]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()), // 0x004773DF: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004D3710U))); // 0x004773CE: push handler

    // 0x004773E9: push 0x004D3710; 0x004773EE: call 0x4C56D0; 0x004773F3: push eax; 0x004773F4: mov ax,[0x54074E]; 0x004773FA: push eax;
    // 0x004773FB: call 0x401400; 0x00477400: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54074e), // 0x004773F4: mov ax,[0x54074E]
                                static_cast<std::uint32_t>(DynObj_GetByte_4C56D0()), // 0x004773F3: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004D3710U))); // 0x004773E9: push handler

    // 0x00477403: push 0x004EFA10; 0x00477408: call 0x4C56E0; 0x0047740D: mov cx,[0x54074E]; 0x00477414: push eax; 0x00477415: push ecx;
    // 0x00477416: call 0x401400; 0x0047741B: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54074e), // 0x0047740D: mov cx,[0x54074E]
                                static_cast<std::uint32_t>(DynObj_GetByte_4C56E0()), // 0x00477414: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004EFA10U))); // 0x00477403: push handler

    // 0x0047741E: push 0x00472880; 0x00477423: call 0x4C56F0; 0x00477428: mov dx,[0x54074E]; 0x0047742F: push eax; 0x00477430: push edx;
    // 0x00477431: call 0x401400; 0x00477436: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54074e), // 0x00477428: mov dx,[0x54074E]
                                static_cast<std::uint32_t>(DynObj_GetByte_4C56F0()), // 0x0047742F: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00472880U))); // 0x0047741E: push handler (NOT 0x004778E0: this type differs)

    // 0x00477439: push 0; 0x0047743B: call 0x4CEA20; 0x00477440: push eax; 0x00477441: mov ax,[0x54074E]; 0x00477447: push eax;
    // 0x00477448: call 0x401400; 0x0047744D: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54074e), // 0x00477441: mov ax,[0x54074E]
                                static_cast<std::uint32_t>(QTask_PhaseIdGet548177_4CEA20()), // 0x00477440: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0U))); // 0x00477439: push 0 (argument 3: a NULL handler)

    // 0x00477450: push 0; 0x00477452: call 0x4CEA10; 0x00477457: mov cx,[0x54074E]; 0x0047745E: push eax; 0x0047745F: push ecx;
    // 0x00477460: call 0x401400; 0x00477465: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54074e), // 0x00477457: mov cx,[0x54074E]
                                static_cast<std::uint32_t>(QTask_PhaseIdGet548176_4CEA10()), // 0x0047745E: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0U))); // 0x00477450: push 0 (argument 3: a NULL handler)

    // 0x00477468: push 0; 0x0047746A: call 0x47E6F0; 0x0047746F: mov dx,[0x54074E]; 0x00477476: push eax; 0x00477477: push edx;
    // 0x00477478: call 0x401400; 0x0047747D: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54074e), // 0x0047746F: mov dx,[0x54074E]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_47E6F0()), // 0x00477476: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0U))); // 0x00477468: push 0 (argument 3: a NULL handler)

    // 0x00477480: push 0x0045E3D0; 0x00477485: call 0x416830; 0x0047748A: push eax; 0x0047748B: mov ax,[0x54074E]; 0x00477491: push eax;
    // 0x00477492: call 0x401400; 0x00477497: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54074e), // 0x0047748B: mov ax,[0x54074E]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416830()), // 0x0047748A: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0045E3D0U))); // 0x00477480: push handler (below 0x00460000)

    // ---- Registration 3: "GunMovingPart" -----------------------------------------------------------------------------------------------
    // 0x0047749A/0x0047749C: push 0; push 0; 0x0047749E: call 0x4D9FA0; 0x004774A3: push eax; 0x004774A4: push 0x140;
    // 0x004774A9: push "GunMovingPart"; 0x004774AE: call 0x401900.
    const std::uint16_t gun_moving_part_id = TaskType_Register("GunMovingPart", // 0x004774A9: push 0x00540794
                                                               0x140U,            // 0x004774A4: push 0x140
                                                               Sys_GetGlobal16_4D9FA0(), // 0x0047749E: call 0x4D9FA0
                                                               0U);               // 0x0047749C: push 0 (inert)

    // 0x004774B3: push 0x00477A70; 0x004774B8: push 0; 0x004774BA: push eax; 0x004774BB: mov word [0x540750],ax; 0x004774C1: call 0x401400.
    ::g_540750 = gun_moving_part_id;
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_540750), // 0x004774C6: mov cx,[0x540750]
                                0U,                                     // 0x004774B8: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00477A70U))); // 0x004774B3: push handler

    // 0x004774CD: push 0x004D3710; 0x004774D2: push 1; 0x004774D4: push ecx; 0x004774D5: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_540750), // 0x004774C6: mov cx,[0x540750]
                                           1U,                                     // 0x004774D2: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004D3710U))); // 0x004774CD: push handler

    // 0x004774DA: mov dx,[0x540750]; 0x004774E1: push 0x004D3710; 0x004774E6: push 2; 0x004774E8: push edx; 0x004774E9: call 0x401530;
    // 0x004774EE: add esp,0x38.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_540750), // 0x004774DA: mov dx,[0x540750]
                                           2U,                                     // 0x004774E6: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004D3710U))); // 0x004774E1: push handler

    // 0x004774F1: push 0x004D3710; 0x004774F6: call 0x4F1A70; 0x004774FB: push eax; 0x004774FC: mov ax,[0x540750]; 0x00477502: push eax;
    // 0x00477503: call 0x401400; 0x00477508: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_540750), // 0x004774FC: mov ax,[0x540750]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()), // 0x004774FB: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004D3710U))); // 0x004774F1: push handler

    // 0x0047750B: push 0x004D3710; 0x00477510: call 0x4C56D0; 0x00477515: mov cx,[0x540750]; 0x0047751C: push eax; 0x0047751D: push ecx;
    // 0x0047751E: call 0x401400; 0x00477523: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_540750), // 0x00477515: mov cx,[0x540750]
                                static_cast<std::uint32_t>(DynObj_GetByte_4C56D0()), // 0x0047751C: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004D3710U))); // 0x0047750B: push handler

    // 0x00477526: push 0x004EFA10; 0x0047752B: call 0x4C56E0; 0x00477530: mov dx,[0x540750]; 0x00477537: push eax; 0x00477538: push edx;
    // 0x00477539: call 0x401400; 0x0047753E: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_540750), // 0x00477530: mov dx,[0x540750]
                                static_cast<std::uint32_t>(DynObj_GetByte_4C56E0()), // 0x00477537: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004EFA10U))); // 0x00477526: push handler

    // 0x00477541: push 0x00477B10; 0x00477546: call 0x4C56F0; 0x0047754B: push eax; 0x0047754C: mov ax,[0x540750]; 0x00477552: push eax;
    // 0x00477553: call 0x401400; 0x00477558: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_540750), // 0x0047754C: mov ax,[0x540750]
                                static_cast<std::uint32_t>(DynObj_GetByte_4C56F0()), // 0x0047754B: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00477B10U))); // 0x00477541: push handler

    // 0x0047755B: push 0; 0x0047755D: call 0x4CEA20; 0x00477562: mov cx,[0x540750]; 0x00477569: push eax; 0x0047756A: push ecx;
    // 0x0047756B: call 0x401400; 0x00477570: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_540750), // 0x00477562: mov cx,[0x540750]
                                static_cast<std::uint32_t>(QTask_PhaseIdGet548177_4CEA20()), // 0x00477569: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0U))); // 0x0047755B: push 0 (argument 3: a NULL handler)

    // 0x00477573: push 0; 0x00477575: call 0x4CEA10; 0x0047757A: mov dx,[0x540750]; 0x00477581: push eax; 0x00477582: push edx;
    // 0x00477583: call 0x401400; 0x00477588: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_540750), // 0x0047757A: mov dx,[0x540750]
                                static_cast<std::uint32_t>(QTask_PhaseIdGet548176_4CEA10()), // 0x00477581: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0U))); // 0x00477573: push 0 (argument 3: a NULL handler)

    // 0x0047758B: push 0; 0x0047758D: call 0x47E6F0; 0x00477592: push eax; 0x00477593: mov ax,[0x540750]; 0x00477599: push eax;
    // 0x0047759A: call 0x401400; 0x0047759F: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_540750), // 0x00477593: mov ax,[0x540750]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_47E6F0()), // 0x00477592: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0U))); // 0x0047758B: push 0 (argument 3: a NULL handler)

    // 0x004775A2: push 0x0045E3D0; 0x004775A7: call 0x416830; 0x004775AC: mov cx,[0x540750]; 0x004775B3: push eax; 0x004775B4: push ecx;
    // 0x004775B5: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_540750), // 0x004775AC: mov cx,[0x540750]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416830()), // 0x004775B3: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0045E3D0U))); // 0x004775A2: push handler (below 0x00460000)

    // ---- The three script-name tails ---------------------------------------------------------------------------------------------------
    // 0x004775BA: mov edx,[0x54074C] (a 32-bit load); 0x004775C0: and edx,0xFFFF; 0x004775C6: push edx; 0x004775C7: push "TASKTYPE_GUNFLAME";
    // 0x004775CC: call 0x4B8810.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_GUNFLAME"), // 0x004775C7: push 0x00540780 (argument 1)
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(
                                 static_cast<std::uint32_t>(::g_54074c)))); // 0x004775C0: the id masked to 16 bits (argument 2)

    // 0x004775D1: xor eax,eax; 0x004775D3: mov ax,[0x54074E] (retail's second, different zero-extension); 0x004775D9: push eax;
    // 0x004775DA: push "TASKTYPE_GUNCLIP"; 0x004775DF: call 0x4B8810.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_GUNCLIP"), // 0x004775DA: push 0x0054076C (argument 1)
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(
                                 static_cast<std::uint32_t>(::g_54074e)))); // 0x004775D3: the zero-extended id (argument 2)

    // 0x004775E4: mov ecx,[0x540750]; 0x004775EA: and ecx,0xFFFF; 0x004775F0: push ecx; 0x004775F1: push "TASKTYPE_GUNMOVINGPART";
    // 0x004775F6: call 0x4B8810; 0x004775FB: add esp,0x24 - 9 pushes above; 0x004775FE: ret.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_GUNMOVINGPART"), // 0x004775F1: push 0x00540754 (argument 1)
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(
                                 static_cast<std::uint32_t>(::g_540750)))); // 0x004775EA: the id masked to 16 bits (argument 2)
} // end of Task_DefineGunParts

} // extern "C"
