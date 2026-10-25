// Native: Task_DefineHumanPlayer_40FE30 (behavior-derived placeholder name; retail symbol unknown); retail VA 0x0040FE30.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x0040FE30-0x0041006E (575 bytes, 1 block, 0 edges, 152 instructions; the extent is measured from the image - the next byte 0x0041006F is nop padding and 0x00410070 is the next function's prologue).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image; every call site was decoded against its callee's own transcribed signature. 23 distinct callees: Core_GetGlobal16_460BE0 (0x00460BE0), TaskType_Register (0x00401900), TaskType_SetInheritedFlags_401FA0 (0x00401FA0), QTaskList_New (0x004C1800, core/qtask_list.cpp), QTask_LinkQTaskEventHandler (0x00401400, 14 calls), QTask_LinkQTaskEventHandlerNoContextFn (0x00401530, 2 calls), Script_PhaseIdGetE4_4F1A60 (0x004F1A60), Script_PhaseIdGetE5_4F1A70 (0x004F1A70), QTask_PhaseIdGet548177_4CEA20 (0x004CEA20), Sys_GetGlobal8_4F40D0/4F40E0/4F4140, Sys_GetSettingByte_416840/416860/416870, Core_GetGlobal8_460C40, Core_GetGlobal8_47E990, Sys_GetGlobal8_487190, Task_GetCachedEventId_467D90, Core_GetGlobal32_460C50, Script_TaskContainer_Grow (0x004F1980, script/native_4f1980.cpp), Script_QHashNodeRegisterEx (0x004B8890, 4 calls, script/native_4b8890.cpp) and AppMain_ParseCmdLineArgs (0x0048F360, core/app_main_commands.cpp). The string bytes are read from the image: "HumanPlayer" at 0x00538EFC, "UnlimitedAmmo" at 0x00538E74, "DefineHumanPlayerGeneral" at 0x00538EE0, "DefineHumanPlayerWeaponCycle" at 0x00538EC0, "DefineHumanPlayerWeaponCategory" at 0x00538EA0, "DefineHumanPlayerAmmoLimit" at 0x00538E84.
// Behavior/CFG: one straight-line body - no branch, no loop, no frame - that registers the "HumanPlayer" task type of object size 0xDF0 with the parent type Core_GetGlobal16_460BE0() returns, stores the returned id at 0x005385B0, sets inherited flags 1, allocates the task list with QTaskList_New(8) into 0x0056E210, links handler 0x00410370 for event 0 through 0x00401400 and handlers 0x00454EE0 and 0x00410840 for events 1 and 2 through 0x00401530, then links thirteen further 0x00401400 events in retail order (event ids from 0x004F1A60, 0x004F1A70, 0x004CEA20, 0x004F40D0, 0x004F40E0, 0x004F4140, 0x00416840, 0x00416860, 0x00460C40, 0x00416870, 0x0047E990, 0x00487190 and 0x00467D90 with handlers 0x004108B0, 0x004108C0, 0x00410900, 0x00410960, 0x004123F0, 0x00412420, 0x00412440, 0x00412990, 0x00412CE0, 0x00412F80, 0x00413090, 0x00413120 and 0x00413730), grows the container Core_GetGlobal32_460C50() by 1 into 0x0056E09C, registers four script commands through 0x004B8890 with the DefineHumanPlayer* names and type -1, and finally registers the command-line option "UnlimitedAmmo" with handler 0x00410360 through 0x0048F360.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The event-id getters are argument-less in the tree and retail's preceding pushes are inert for them, but each inert push is simultaneously the next 0x00401400 link call's handler, so the port keeps the pushed value as that call's handler in retail order rather than reproducing an inert argument. Byte-valued event ids widen to the std::uint32_t event_id parameter, which cannot change behaviour because 0x00401400 masks the event word to its low byte for the phase index. 0x005385B0 holds image value 0x0181, is written with a 16-bit store and read back with 16-bit loads, so it is declared std::uint16_t; 0x0056E210 and 0x0056E09C are zero in the image and are pointer slots. The twenty-one handler and option-handler addresses (0x00410070, 0x004101C0, 0x00410240, 0x004102D0, 0x00410360, 0x00410370, 0x00410840, 0x004108B0, 0x004108C0, 0x00410900, 0x00410960, 0x004123F0, 0x00412420, 0x00412440, 0x00412990, 0x00412CE0, 0x00412F80, 0x00413090, 0x00413120, 0x00413730 and 0x00454EE0) are raw code immediates in retail and are not transcribed yet, so they are passed as cast integers and create no link dependency; each is a new gap VA in the 0x00400000-0x0045FFFF range; three of them are transcribed since this file was written - 0x00410360 (core/native_410360.cpp), 0x004108B0 (core/native_4108b0.cpp) and 0x00454EE0 (core/native_454ee0.cpp) - and the other eighteen are still gaps. Domain: core/, because it is one of the 0x00402930 startup registrars. Hypothesis, not established by this body: the three dwords that follow the id slot - 0x005385B4, 0x005385B8 and 0x005385BC, holding 0x00408770, 0x00408A40 and 0x00408C30 in the image - look like handler tables of sibling 0x0040xxxx registrars; nothing here reads them, so this stays a lead to check, not a fact.
#include "dev_log.hpp"
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

std::uint16_t Core_GetGlobal16_460BE0(void);                                          // 0x00460BE0: argument-less word getter (parent type)
void TaskType_SetInheritedFlags_401FA0(std::uint32_t type_id, std::uint32_t flags);   // 0x00401FA0 (core/native_00401fa0.cpp)
void* QTaskList_New(std::int32_t capacity);                                           // 0x004C1800 (core/qtask_list.cpp)
std::uint8_t Script_PhaseIdGetE4_4F1A60(void);                                        // 0x004F1A60: argument-less byte getter
std::uint8_t Script_PhaseIdGetE5_4F1A70(void);                                        // 0x004F1A70: argument-less byte getter
std::uint8_t QTask_PhaseIdGet548177_4CEA20(void);                                     // 0x004CEA20: argument-less byte getter
std::uint8_t Sys_GetGlobal8_4F40D0(void);                                             // 0x004F40D0: argument-less byte getter
std::uint8_t Sys_GetGlobal8_4F40E0(void);                                             // 0x004F40E0: argument-less byte getter
std::uint8_t Sys_GetGlobal8_4F4140(void);                                             // 0x004F4140: argument-less byte getter
std::uint8_t Sys_GetSettingByte_416840(void);                                         // 0x00416840: argument-less byte getter
std::uint8_t Sys_GetSettingByte_416860(void);                                         // 0x00416860: argument-less byte getter
std::uint8_t Sys_GetSettingByte_416870(void);                                         // 0x00416870: argument-less byte getter
std::uint8_t Core_GetGlobal8_460C40(void);                                            // 0x00460C40: argument-less byte getter
std::uint8_t Core_GetGlobal8_47E990(void);                                            // 0x0047E990: argument-less byte getter
std::uint8_t Sys_GetGlobal8_487190(void);                                             // 0x00487190: argument-less byte getter
std::uint8_t Task_GetCachedEventId_467D90(void);                                      // 0x00467D90: argument-less byte getter
std::uint32_t Core_GetGlobal32_460C50(void);                                          // 0x00460C50: argument-less dword getter
void* Script_TaskContainer_Grow(std::int32_t* container, std::int32_t count);         // 0x004F1980 (script/native_4f1980.cpp)
void Script_QHashNodeRegisterEx(void* arg0, void* arg1, char const* arg2, void* arg3); // 0x004B8890 (script/native_4b8890.cpp)
void* AppMain_ParseCmdLineArgs(char* option_name, std::int32_t(NATIVE_DECL* handler)(char* suffix)); // 0x0048F360 (core/app_main_commands.cpp)

// 0x005385B0: the "HumanPlayer" task type id. Image value 0x0181, overwritten by the 16-bit store from TaskType_Register's return value.
std::uint16_t g_5385b0 = 0x0181U;
// 0x0056E210: the HumanPlayer task list returned by QTaskList_New; zero in the image.
void* g_56e210 = nullptr;
// 0x0056E09C: the script task container returned by Script_TaskContainer_Grow; zero in the image.
void* g_56e09c = nullptr;

void NATIVE_DECL Task_DefineHumanPlayer_40FE30()
{
    NATIVE_LOG_ENTER("Task_DefineHumanPlayer_40FE30", "0x0040FE30");
    // 0x0040FE30/0x0040FE32: push 0; push 0 (both inert arguments of the parent getter, the second also the register call's fourth slot).
    // 0x0040FE34: call 0x460BE0; 0x0040FE39: push eax; 0x0040FE3A: push 0xDF0; 0x0040FE3F: push 0x00538EFC; 0x0040FE44: call 0x401900.
    const std::uint16_t human_player_id = TaskType_Register("HumanPlayer",        // 0x0040FE3F: push 0x00538EFC
                                                            0xDF0U,               // 0x0040FE3A: push 0xDF0 (object size)
                                                            Core_GetGlobal16_460BE0(), // 0x0040FE34: call 0x460BE0 (parent type)
                                                            0U);                  // 0x0040FE32: push 0 (inert)

    // 0x0040FE49: push 1; 0x0040FE4B: push eax; 0x0040FE4C: mov word [0x005385B0],ax; 0x0040FE52: call 0x401FA0.
    ::g_5385b0 = human_player_id;
    TaskType_SetInheritedFlags_401FA0(static_cast<std::uint32_t>(::g_5385b0), 1U); // 0x0040FE49: push 1 (flags)

    // 0x0040FE57: push 8; 0x0040FE59: call 0x4C1800; 0x0040FE5E: mov [0x0056E210],eax.
    ::g_56e210 = QTaskList_New(8);                                                 // 0x0040FE57: push 8 (capacity)

    // 0x0040FE69: push 0x00410370 (pre-pushed handler); 0x0040FE6E: push 0 (event 0); 0x0040FE70: push eax; 0x0040FE71: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5385b0), // 0x0040FE70: push ax ([0x005385B0])
                                0U,                                     // 0x0040FE6E: push 0 (event)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00410370U))); // 0x0040FE69: push handler

    // 0x0040FE7D: push 0x00454EE0; 0x0040FE82: push 1; 0x0040FE84: push ecx; 0x0040FE85: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5385b0), // 0x0040FE84: push cx ([0x005385B0])
                                           1U,                                     // 0x0040FE82: push 1 (event)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00454EE0U))); // 0x0040FE7D: push handler

    // 0x0040FE91: push 0x00410840; 0x0040FE96: push 2; 0x0040FE98: push edx; 0x0040FE99: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5385b0), // 0x0040FE98: push dx ([0x005385B0])
                                           2U,                                     // 0x0040FE96: push 2 (event)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00410840U))); // 0x0040FE91: push handler

    // The thirteen remaining event links, in retail order. Each pre-pushed handler immediate is also the inert argument of the
    // argument-less getter call that supplies the event id.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5385b0), Script_PhaseIdGetE4_4F1A60(), // 0x0040FEA1/0x0040FEA6
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004108B0U)));                       // 0x0040FEB3: call 0x401400
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5385b0), Script_PhaseIdGetE5_4F1A70(), // 0x0040FEBB/0x0040FEC0
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004108C0U)));                       // 0x0040FECE: call 0x401400
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5385b0), QTask_PhaseIdGet548177_4CEA20(), // 0x0040FED6/0x0040FEDB
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00410900U)));                       // 0x0040FEE9: call 0x401400
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5385b0), Sys_GetGlobal8_4F40D0(), // 0x0040FEF1/0x0040FEF6
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00410960U)));                       // 0x0040FF03: call 0x401400
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5385b0), Sys_GetGlobal8_4F40E0(), // 0x0040FF0B/0x0040FF10
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004123F0U)));                       // 0x0040FF1E: call 0x401400
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5385b0), Sys_GetGlobal8_4F4140(), // 0x0040FF26/0x0040FF2B
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00412420U)));                       // 0x0040FF39: call 0x401400
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5385b0), Sys_GetSettingByte_416840(), // 0x0040FF41/0x0040FF46
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00412440U)));                       // 0x0040FF53: call 0x401400
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5385b0), Sys_GetSettingByte_416860(), // 0x0040FF5B/0x0040FF60
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00412990U)));                       // 0x0040FF6E: call 0x401400
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5385b0), Core_GetGlobal8_460C40(), // 0x0040FF76/0x0040FF7B
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00412CE0U)));                       // 0x0040FF89: call 0x401400
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5385b0), Sys_GetSettingByte_416870(), // 0x0040FF91/0x0040FF96
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00412F80U)));                       // 0x0040FFA3: call 0x401400
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5385b0), Core_GetGlobal8_47E990(), // 0x0040FFAB/0x0040FFB0
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00413090U)));                       // 0x0040FFBE: call 0x401400
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5385b0), Sys_GetGlobal8_487190(), // 0x0040FFC6/0x0040FFCB
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00413120U)));                       // 0x0040FFD9: call 0x401400
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5385b0), Task_GetCachedEventId_467D90(), // 0x0040FFE1/0x0040FFE6
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00413730U)));                       // 0x0040FFF3: call 0x401400

    // 0x0040FFFB: push 1 (inert for the argument-less getter, and the grow count for 0x004F1980); 0x0040FFFD: call 0x460C50;
    // 0x00410002: push eax; 0x00410003: call 0x4F1980; 0x00410016: mov [0x0056E09C],eax.
    const std::uint32_t container = Core_GetGlobal32_460C50();                     // 0x0040FFFD: call 0x460C50
    ::g_56e09c = Script_TaskContainer_Grow(
        reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(container)), 1); // 0x00410003: call 0x4F1980

    // 0x00410008: push -1; 0x0041000A: push 0; 0x0041000C: push 0x00410070; 0x00410011: push 0x00538EE0; 0x0041001B: call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("DefineHumanPlayerGeneral"),      // 0x00410011: push 0x00538EE0
                               reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00410070U)), // handler
                               nullptr,                                            // 0x0041000A: push 0
                               reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xFFFFFFFFU))); // 0x00410008: push -1
    // 0x00410020: push -1; 0x00410022: push 0; 0x00410024: push 0x004101C0; 0x00410029: push 0x00538EC0; 0x0041002E: call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("DefineHumanPlayerWeaponCycle"),  // 0x00410029: push 0x00538EC0
                               reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x004101C0U)), // handler
                               nullptr,                                            // 0x00410022: push 0
                               reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xFFFFFFFFU))); // 0x00410020: push -1
    // 0x00410033: push -1; 0x00410035: push 0; 0x00410037: push 0x00410240; 0x0041003C: push 0x00538EA0; 0x00410041: call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("DefineHumanPlayerWeaponCategory"), // 0x0041003C: push 0x00538EA0
                               reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00410240U)), // handler
                               nullptr,                                            // 0x00410035: push 0
                               reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xFFFFFFFFU))); // 0x00410033: push -1
    // 0x00410046: push -1; 0x00410048: push 0; 0x0041004A: push 0x004102D0; 0x0041004F: push 0x00538E84; 0x00410054: call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("DefineHumanPlayerAmmoLimit"),    // 0x0041004F: push 0x00538E84
                               reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x004102D0U)), // handler
                               nullptr,                                            // 0x00410048: push 0
                               reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xFFFFFFFFU))); // 0x00410046: push -1

    // 0x0041005C: push 0x00410360 (handler); 0x00410061: push 0x00538E74; 0x00410066: call 0x48F360; 0x0041006E: ret.
    AppMain_ParseCmdLineArgs(const_cast<char*>("UnlimitedAmmo"),                   // 0x00410061: push 0x00538E74
                             reinterpret_cast<std::int32_t(NATIVE_DECL*)(char*)>(
                                 static_cast<std::uintptr_t>(0x00410360U)));       // 0x0041005C: push handler
}

} // extern "C"

#undef NATIVE_DECL
