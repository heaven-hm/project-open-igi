// Native: Task_DefineCar (behavior-derived placeholder name from the byte-identical name literal; retail symbol unknown); retail VA 0x0042DCF0-0x0042E15B.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Radare2 6.2.0 `pd` over the exact image; VA 0x0042DCF0..0x0042E15B, 1132 bytes, 1 block, 318 instructions, 0 branches. Every push and cleanup was replayed with esp simulated, and the cleanups reconcile with zero slack: `add esp,0x38` at 0x0042DD4D closes 14 pending dwords, `add esp,0xC` closes 3 at each of the nineteen single-link sites, `add esp,0x10` at 0x0042DF34 closes the last link's three plus the allocation's one, `add esp,0x18` at each of the twelve append sites closes 6, and the epilogue `add esp,0x2EF8` at 0x0042E155 closes the 0x2ED8 frame plus the final append's six and the hash registration's two. Link counts were taken from the 0x00401400/0x00401530 census -- 17 and 4 -- not from handler immediates.
// Behavior/CFG: one basic block. The prologue is a compiler-emitted stack probe, not a retail call: `mov eax,0x2ED8; call 0x004A54A0` lowers esp by the amount in eax itself and returns through `push eax; ret`, so the frame base is set inside the probe and this body contains no `sub esp`. Registers "Car" (size 0x2ED8) through 0x00401900, storing the 16-bit id at 0x0053BDA8, then makes twenty-one handler links on it: three carry fixed event ids (0x00401400 to 0x0042E160 on event 0, 0x00401530 to 0x0042E320 on phase 1 and 0x00401530 to 0x0042E940 on phase 2) and eighteen take their event id from an argument-less getter with a fixed handler. It then allocates one container of capacity 13 through 0x004F1950 at 0x0057BD28, initialises 0x0057BD20 to 0 and 0x0057BD24 to the float bits 0x447A0000 (1000.0f), and performs thirteen 0x004F1A80 appends into it. It ends by reloading the id, masking it to 16 bits and registering "TASKTYPE_CAR" through 0x004B8810, returning at 0x0042E15B.
// Status/limits: STRUCTURALLY_VERIFIED. All twenty-one links, the thirteen appends, the registration and the hash registration are transcribed from their own pushes, and every cleanup sum reconciles with zero slack. Limits: no runtime observation of this native in either binary. The stack probe 0x004A54A0 is the MSVC __alloca_probe, classified CRT in scripts/native_coverage.py and never hand-transcribed; the port states the allocation and does not reproduce it. 0x0053BDA8 is owned by its writer in engine/src/retail/sys/native_42fdc0.cpp and is bound extern here rather than redefined. The thirteenth append pushes the address 0x0057BD20 as its sixth argument rather than a literal zero, so the two literals in this body must not be generalised. The two appends at frame+0x6D8 and frame+0x6D9 differ by one byte deliberately: they are adjacent bytes of the same buffer, not a transcription slip.
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

extern "C" std::uint16_t Sys_GetGlobal16_4F45F0(void);   // 0x004F45F0
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void); // 0x004F1A60
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void); // 0x004F1A70
extern "C" std::uint8_t Task_GetCachedEventId_4E6610(void); // 0x004E6610
extern "C" std::uint8_t Sys_GetSettingByte_416860(void); // 0x00416860
extern "C" std::uint8_t Sys_GetSettingByte_416890(void); // 0x00416890
extern "C" std::uint8_t Sys_GetGlobal8_4F40D0(void);     // 0x004F40D0
extern "C" std::uint8_t Sys_GetGlobal8_4F40E0(void);     // 0x004F40E0
extern "C" std::uint8_t Sys_GetGlobal8_4F4110(void);     // 0x004F4110
extern "C" std::uint8_t Sys_GetGlobal8_4F4140(void);     // 0x004F4140
extern "C" std::uint8_t Sys_GetGlobal8_4F4130(void);     // 0x004F4130
extern "C" std::uint8_t Sys_GetGlobal8_4F4120(void);     // 0x004F4120
extern "C" std::uint8_t Sys_GetGlobal8_489610(void);     // 0x00489610
extern "C" std::uint8_t Sys_GetGlobal8_4F4850(void);     // 0x004F4850
extern "C" std::uint8_t Sys_GetGlobal8_46AB20(void);     // 0x0046AB20
extern "C" std::uint8_t Sys_GetGlobal8_489630(void);     // 0x00489630
extern "C" std::uint8_t QTask_PhaseIdGet548177_4CEA20(void); // 0x004CEA20
extern "C" std::uint32_t Sys_GetGlobal32_4F1210(void);   // 0x004F1210
extern "C" std::uint32_t Sys_GetGlobal32_4F1220(void);   // 0x004F1220
extern "C" std::uint32_t Sys_GetGlobal32_4F1010(void);   // 0x004F1010
extern "C" std::uint32_t Sys_GetGlobal32_4F0F70(void);   // 0x004F0F70
extern "C" std::uint32_t Sys_GetGlobal32_4F0F60(void);   // 0x004F0F60
extern "C" std::uint32_t Core_GetRenderWordB_4F0FD0(void); // 0x004F0FD0
extern "C" std::uint32_t Camera_Get_4F1000(void);        // 0x004F1000
extern "C" std::uint32_t Sys_GetGlobal32_4F0FC0(void);   // 0x004F0FC0
extern "C" std::uint32_t Sys_GetGlobal32_4F0F30(void);   // 0x004F0F30
extern "C" std::uint32_t Sys_GetGlobal32_4F0F50(void);   // 0x004F0F50
extern "C" std::uint32_t Script_TaskContainer_Alloc(std::int32_t count); // 0x004F1950
extern "C" void Script_QHashNodeRegister(void* arg1, void* value); // 0x004B8810
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5, std::int32_t const arg6); // 0x004F1A80

// Retail 0x0053BDA8 is owned by its writer, engine/src/retail/sys/native_42fdc0.cpp.
extern "C" std::uint16_t g_53bda8;
std::uint32_t g_57bd28 = 0U;   // retail 0x0057BD28: the append container
std::uint32_t g_57bd20 = 0U;   // retail 0x0057BD20: zero-initialised, address taken by append 13
std::uint32_t g_57bd24 = 0x447A0000U;   // retail 0x0057BD24: image bits for 1000.0f

std::int32_t NATIVE_DECL Task_DefineCar()
{
    std::int32_t append_frame_base = 0;
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x0042DCF0/0x0042DCF5: `mov eax,0x2ED8; call 0x004A54A0` -- the MSVC 6 stack probe,
    // which lowers esp by 0x2ED8 itself and returns through `push eax; ret`. No sub esp here.
    // 0x0042DCFA/0x0042DCFC: the two surplus zeros of the argument-less 0x004F45F0.
    g_53bda8 = TaskType_Register("Car", 0x2ED8U, Sys_GetGlobal16_4F45F0(), 0U);        // 0x0042DD0E

    QTask_LinkQTaskEventHandler(g_53bda8, 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042E160));                // 0x0042DD21
    QTask_LinkQTaskEventHandlerNoContextFn(g_53bda8, 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042E320));                // 0x0042DD34
    QTask_LinkQTaskEventHandlerNoContextFn(g_53bda8, 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042E940));                // 0x0042DD48

    // The eighteen deferred-handler links: in each, the handler immediate is pushed before the
    // argument-less getter and is consumed as the link's third argument.
    QTask_LinkQTaskEventHandler(g_53bda8, Script_PhaseIdGetE4_4F1A60(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042EA00));                // 0x0042DD63
    QTask_LinkQTaskEventHandler(g_53bda8, Script_PhaseIdGetE5_4F1A70(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042F530));                // 0x0042DD7D
    QTask_LinkQTaskEventHandler(g_53bda8, Task_GetCachedEventId_4E6610(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042F850));                // 0x0042DD98
    QTask_LinkQTaskEventHandler(g_53bda8, Sys_GetSettingByte_416860(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042FB40));                // 0x0042DDB3
    QTask_LinkQTaskEventHandler(g_53bda8, Sys_GetSettingByte_416890(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042FC00));                // 0x0042DDCD
    QTask_LinkQTaskEventHandler(g_53bda8, Sys_GetGlobal8_4F40D0(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042EA10));                // 0x0042DDE8
    QTask_LinkQTaskEventHandler(g_53bda8, Sys_GetGlobal8_4F40E0(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042F870));                // 0x0042DE03
    QTask_LinkQTaskEventHandler(g_53bda8, Sys_GetGlobal8_4F4110(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00432BD0));                // 0x0042DE1D
    QTask_LinkQTaskEventHandler(g_53bda8, Sys_GetGlobal8_4F4140(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042F8A0));                // 0x0042DE38
    QTask_LinkQTaskEventHandler(g_53bda8, Sys_GetGlobal8_4F4130(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042FA60));                // 0x0042DE53
    QTask_LinkQTaskEventHandler(g_53bda8, Sys_GetGlobal8_4F4120(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042FB10));                // 0x0042DE6D
    QTask_LinkQTaskEventHandler(g_53bda8, Sys_GetGlobal8_489610(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042FC50));                // 0x0042DE88
    QTask_LinkQTaskEventHandler(g_53bda8, Sys_GetGlobal8_4F4850(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042FC40));                // 0x0042DEA3
    QTask_LinkQTaskEventHandler(g_53bda8, Sys_GetGlobal8_46AB20(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x004E4280));                // 0x0042DEBD
    QTask_LinkQTaskEventHandler(g_53bda8, Sys_GetGlobal8_489630(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042FC70));                // 0x0042DED8
    QTask_LinkQTaskEventHandler(g_53bda8, QTask_PhaseIdGet548177_4CEA20(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042FC80));                // 0x0042DEF3
    QTask_LinkQTaskEventHandlerNoContextFn(g_53bda8, Sys_GetGlobal32_4F1210(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042FCE0));                // 0x0042DF0D
    QTask_LinkQTaskEventHandlerNoContextFn(g_53bda8, Sys_GetGlobal32_4F1220(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042FD40));                // 0x0042DF28

    g_57bd28 = static_cast<std::uint32_t>(static_cast<std::uintptr_t>(
        Script_TaskContainer_Alloc(13)));                                              // 0x0042DF2F
    g_57bd20 = 0U;                                                                     // 0x0042DF3C
    g_57bd24 = 0x447A0000U;                                                            // 0x0042DF46

    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd28)), append_frame, append_frame + 0x168, static_cast<std::int32_t>(Sys_GetGlobal32_4F1010()), 7, 0);  // 0x0042DF6E
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd28)), append_frame, append_frame + 0x18C, static_cast<std::int32_t>(Sys_GetGlobal32_4F0F70()), 7, 0);  // 0x0042DF94
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd28)), append_frame, append_frame + 0x740, static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, 0);  // 0x0042DFBA
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd28)), append_frame, append_frame + 0x180, static_cast<std::int32_t>(Sys_GetGlobal32_4F0F60()), 7, 0);  // 0x0042DFE0
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd28)), append_frame, append_frame + 0x154, static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);  // 0x0042E006
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd28)), append_frame, append_frame + 0x6D8, static_cast<std::int32_t>(Camera_Get_4F1000()), 7, 0);  // 0x0042E02C
    // 0x0042E052: the next byte of the same buffer. 0x6D9 is deliberately adjacent to 0x6D8.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd28)), append_frame, append_frame + 0x6D9, static_cast<std::int32_t>(Camera_Get_4F1000()), 7, 0);  // 0x0042E052
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd28)), append_frame, append_frame + 0x928, static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);  // 0x0042E078
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd28)), append_frame, append_frame + 0x978, static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);  // 0x0042E09E
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd28)), append_frame, append_frame + 0x9C8, static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);  // 0x0042E0C4
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd28)), append_frame, append_frame + 0x2EC0, static_cast<std::int32_t>(Sys_GetGlobal32_4F0F30()), 7, 0);  // 0x0042E0EA
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd28)), append_frame, append_frame + 0x2EC4, static_cast<std::int32_t>(Sys_GetGlobal32_4F0F30()), 7, 0);  // 0x0042E110
    // 0x0042E118: this append's sixth argument is the ADDRESS 0x0057BD20, not a literal zero.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bd28)), append_frame, append_frame + 0x2EC8, static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(static_cast<std::uintptr_t>(0x0057BD20U)));  // 0x0042E139

    // 0x0042E13E: reload the id at 32-bit width and mask it, rather than reusing the local.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_CAR"),
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(g_53bda8 & 0xFFFFU)));     // 0x0042E150

    return 0;   // 0x0042E15B: this path returns without setting EAX
}

} // extern "C"
