// Native: Task_DefineGrenade (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00476730-0x0047689D.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 366 bytes, 1 block, 95 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 95 instructions / 366 bytes / 0 edges; `pd`
//   stops at the ret at 0x0047689D, and the two nops at 0x0047689E-0x0047689F are followed by the next function's `sub $0xc,%esp` at
//   0x004768A0, so the extent is exact) plus the objdump listing build/retail-dis.txt for the same extent, read in two explicit ranges
//   (0x00476730-0x00476889, 0x00476886-0x004768A0); `ps` at 0x005406C4, 0x005406B0, 0x00540690 and 0x00540670 for the four literals
//   (byte-identical to "Grenade", "TASKTYPE_GRENADE", "GRENADE_EXPLOSIONTYPE_NORMAL", "GRENADE_EXPLOSIONTYPE_FLASHBANG"); `pxw` at 0x0054066C
//   and 0x005BE3B8 for the image values; a full stack-pointer simulation over all 95 instructions, which returns esp to its entry value at the
//   ret and accounts for every cleanup (0x3c = 15 pushes, 0xc nine times, 0x24 = 9 at the tail). Every getter callee was re-read at its own VA
//   and is a single-instruction argument-less load ending in a ret. Argument numbers were derived by counting the pushes above each access,
//   not by reading an [esp+N] offset as an argument number.
// Behavior/CFG: one straight-line body that registers the "Grenade" task type of size 0x738 with parent Sys_GetGlobal16_4FB5E0() -> id at
//   0x0054066C, allocates one event id with QTask_AllocateEvent(0) and caches its low byte at 0x005BE3B8, then wires eleven handler links:
//   e0->0x004769B0 (0x00401400), e1->0x00476AC0 and e2->0x00476E50 (0x00401530), then the event ids from 0x00416840->0x00476EA0,
//   0x00416860->0x00476EB0, 0x00416850->0x0045F920, 0x00416880->0x0045F920, 0x004EE0E0->0x00476F70, 0x004EE0F0->0x00427F80,
//   0x00413B20->0x00476FA0 and 0x004071D0->0x004768A0, all through 0x00401400. Then three script-name tails through 0x004B8810:
//   ("TASKTYPE_GRENADE", the Grenade id zero-extended by `mov edx,[0x54066C]; and edx,0xFFFF`), ("GRENADE_EXPLOSIONTYPE_NORMAL", 0) and
//   ("GRENADE_EXPLOSIONTYPE_FLASHBANG", 1). One basic block, no branch, no loop, no frame, no parameter container, 25 call sites over 14
//   distinct callees. The 0x005BE3B8 byte this body writes is never read by this body; its reader is elsewhere.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The id slot 0x0054066C holds image value 0x0181 before TaskType_Register overwrites it with
//   the assigned id, and it is written with a 16-bit store and read back at the tail with a 32-bit load masked by 0xFFFF, so it is declared as a
//   plain std::uint16_t - this is the seventh observed image occurrence of 0x0181 in an id or parent slot (0x00548698, 0x00547F42, 0x00548648,
//   0x0054074C, 0x0054074E, 0x00540750, 0x0054066C), with 0x00503BF0 returning 0 as the counter-witness, so it is a pattern and not a proven
//   universal meaning. Documented shape differences, all value-preserving: (1) every getter callee is an argument-less load in retail that
//   ignores the arguments the caller pushes, and the tree's transcriptions take no arguments, so those inert pushes are not reproduced; each is
//   simultaneously the next 0x00401400 link call's pre-pushed handler, which the port keeps as the handler value in the same order; (2)
//   byte-valued event ids are zero-extended in the port, which cannot change behaviour because every link fed by a byte getter here goes through
//   0x00401400, which masks the event word to its low byte for the phase index; (3) the two constant tails pass 0 and 1 as 0x004B8810's second
//   argument exactly as retail pushes them. Handlers 0x004769B0, 0x00476AC0, 0x00476E50, 0x00476EA0, 0x00476EB0, 0x00476F70, 0x00476FA0 and
//   0x004768A0 are raw code immediates in retail and are not transcribed yet (native gaps in 0x0046xxxx-0x0048xxxx). Handlers 0x0045F920 and
//   0x00427F80 are BELOW 0x00460000 and therefore belong to the 0x00400000-0x0045FFFF owner, not to this file; 0x00427F80 is already
//   transcribed there. Domain: core/, because it is one of the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4FB5E0(void);              // 0x004FB5E0: argument-less word getter (parent type)
extern "C" std::uint8_t Sys_GetSettingByte_416840(void);            // 0x00416840: argument-less byte getter (config/native_416840.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416850(void);            // 0x00416850: argument-less byte getter
extern "C" std::uint8_t Sys_GetSettingByte_416860(void);            // 0x00416860: argument-less byte getter
extern "C" std::uint8_t Sys_GetSettingByte_416880(void);            // 0x00416880: argument-less byte getter
extern "C" std::uint8_t Sys_GetGlobal8_4EE0E0(void);                // 0x004EE0E0: argument-less byte getter
extern "C" std::uint8_t Sys_GetGlobal8_4EE0F0(void);                // 0x004EE0F0: argument-less byte getter
extern "C" std::uint8_t Core_GetFlagByte_413B20(void);              // 0x00413B20: argument-less byte getter
extern "C" std::uint8_t Sys_GetSettingByte_4071D0(void);            // 0x004071D0: argument-less byte getter
extern "C" void Script_QHashNodeRegister(void* arg1, void* value);  // 0x004B8810 (script/native_4b8810.cpp)

extern "C" {

// 0x0054066C: the "Grenade" task type id. Retail image value 0x0181, overwritten by a 16-bit store from TaskType_Register's return value.
std::uint16_t g_54066c = 0x0181U;

// 0x005BE3B8: the one event id allocated by this body, stored as a byte (0x00476756 stores AL). Image value 0.
std::uint8_t g_5be3b8 = 0U;

void Task_DefineGrenade()
{
    // 0x00476730/0x00476732: push 0; push 0 (both inert arguments of the parent getter); 0x00476734: call 0x4FB5E0; 0x00476739: push eax;
    // 0x0047673A: push 0x738; 0x0047673F: push "Grenade"; 0x00476744: call 0x401900.
    const std::uint16_t grenade_id = TaskType_Register("Grenade", // 0x0047673F: push 0x005406C4
                                                       0x738U,      // 0x0047673A: push 0x738 (object size)
                                                       Sys_GetGlobal16_4FB5E0(), // 0x00476734: call 0x4FB5E0 (parent type)
                                                       0U);         // 0x00476732: push 0 (inert)

    // 0x00476749: push 0 (the allocation's argument); 0x0047674B: mov word [0x54066C],ax (the id store lands before the call);
    // 0x00476751: call 0x4017C0; 0x00476756: mov byte [0x5BE3B8],al.
    ::g_54066c = grenade_id;
    ::g_5be3b8 = static_cast<std::uint8_t>(QTask_AllocateEvent(0U)); // 0x00476756: the allocated event id's low byte

    // 0x0047675B: mov ax,[0x54066C]; 0x00476761: push 0x004769B0; 0x00476766: push 0; 0x00476768: push eax; 0x00476769: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54066c), // 0x0047675B: mov ax,[0x54066C]
                                0U,                                     // 0x00476766: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004769B0U))); // 0x00476761: push handler

    // 0x0047676E: mov cx,[0x54066C]; 0x00476775: push 0x00476AC0; 0x0047677A: push 1; 0x0047677C: push ecx; 0x0047677D: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_54066c), // 0x0047676E: mov cx,[0x54066C]
                                           1U,                                     // 0x0047677A: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00476AC0U))); // 0x00476775: push handler

    // 0x00476782: mov dx,[0x54066C]; 0x00476789: push 0x00476E50; 0x0047678E: push 2; 0x00476790: push edx; 0x00476791: call 0x401530;
    // 0x00476796: add esp,0x3c - 15 pushes above.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_54066c), // 0x00476782: mov dx,[0x54066C]
                                           2U,                                     // 0x0047678E: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00476E50U))); // 0x00476789: push handler

    // 0x00476799: push 0x00476EA0; 0x0047679E: call 0x416840; 0x004767A3: push eax; 0x004767A4: mov ax,[0x54066C]; 0x004767AA: push eax;
    // 0x004767AB: call 0x401400; 0x004767B0: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54066c), // 0x004767A4: mov ax,[0x54066C]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416840()), // 0x004767A3: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00476EA0U))); // 0x00476799: push handler

    // 0x004767B3: push 0x00476EB0; 0x004767B8: call 0x416860; 0x004767BD: mov cx,[0x54066C]; 0x004767C4: push eax; 0x004767C5: push ecx;
    // 0x004767C6: call 0x401400; 0x004767CB: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54066c), // 0x004767BD: mov cx,[0x54066C]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416860()), // 0x004767C4: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00476EB0U))); // 0x004767B3: push handler

    // 0x004767CE: push 0x0045F920; 0x004767D3: call 0x416850; 0x004767D8: mov dx,[0x54066C]; 0x004767DF: push eax; 0x004767E0: push edx;
    // 0x004767E1: call 0x401400; 0x004767E6: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54066c), // 0x004767D8: mov dx,[0x54066C]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416850()), // 0x004767DF: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0045F920U))); // 0x004767CE: push handler (below 0x00460000)

    // 0x004767E9: push 0x0045F920; 0x004767EE: call 0x416880; 0x004767F3: push eax; 0x004767F4: mov ax,[0x54066C]; 0x004767FA: push eax;
    // 0x004767FB: call 0x401400; 0x00476800: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54066c), // 0x004767F4: mov ax,[0x54066C]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416880()), // 0x004767F3: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0045F920U))); // 0x004767E9: push handler (below 0x00460000)

    // 0x00476803: push 0x00476F70; 0x00476808: call 0x4EE0E0; 0x0047680D: mov cx,[0x54066C]; 0x00476814: push eax; 0x00476815: push ecx;
    // 0x00476816: call 0x401400; 0x0047681B: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54066c), // 0x0047680D: mov cx,[0x54066C]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_4EE0E0()), // 0x00476814: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00476F70U))); // 0x00476803: push handler

    // 0x0047681E: push 0x00427F80; 0x00476823: call 0x4EE0F0; 0x00476828: mov dx,[0x54066C]; 0x0047682F: push eax; 0x00476830: push edx;
    // 0x00476831: call 0x401400; 0x00476836: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54066c), // 0x00476828: mov dx,[0x54066C]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_4EE0F0()), // 0x0047682F: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00427F80U))); // 0x0047681E: push handler (below 0x00460000)

    // 0x00476839: push 0x00476FA0; 0x0047683E: call 0x413B20; 0x00476843: push eax; 0x00476844: mov ax,[0x54066C]; 0x0047684A: push eax;
    // 0x0047684B: call 0x401400; 0x00476850: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54066c), // 0x00476844: mov ax,[0x54066C]
                                static_cast<std::uint32_t>(Core_GetFlagByte_413B20()), // 0x00476843: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00476FA0U))); // 0x00476839: push handler

    // 0x00476853: push 0x004768A0; 0x00476858: call 0x4071D0; 0x0047685D: mov cx,[0x54066C]; 0x00476864: push eax; 0x00476865: push ecx;
    // 0x00476866: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_54066c), // 0x0047685D: mov cx,[0x54066C]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_4071D0()), // 0x00476864: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004768A0U))); // 0x00476853: push handler

    // 0x0047686B: mov edx,[0x54066C] (a 32-bit load); 0x00476871: and edx,0xFFFF; 0x00476877: push edx;
    // 0x00476878: push "TASKTYPE_GRENADE"; 0x0047687D: call 0x4B8810.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_GRENADE"), // 0x00476878: push 0x005406B0 (argument 1)
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(
                                 static_cast<std::uint32_t>(::g_54066c)))); // 0x00476871: the id masked to 16 bits (argument 2)

    // 0x00476882: push 0; 0x00476884: push "GRENADE_EXPLOSIONTYPE_NORMAL"; 0x00476889: call 0x4B8810.
    Script_QHashNodeRegister(const_cast<char*>("GRENADE_EXPLOSIONTYPE_NORMAL"), // 0x00476884: push 0x00540690 (argument 1)
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // 0x00476882: push 0 (argument 2)

    // 0x0047688E: push 1; 0x00476890: push "GRENADE_EXPLOSIONTYPE_FLASHBANG"; 0x00476895: call 0x4B8810;
    // 0x0047689A: add esp,0x24 - the last link's three pushes plus the three tails' two each; 0x0047689D: ret.
    Script_QHashNodeRegister(const_cast<char*>("GRENADE_EXPLOSIONTYPE_FLASHBANG"), // 0x00476890: push 0x00540670 (argument 1)
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // 0x0047688E: push 1 (argument 2)
} // end of Task_DefineGrenade

} // extern "C"
