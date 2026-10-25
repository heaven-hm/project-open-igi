// Native: Task_DefineHumanAIConfig (behavior-derived placeholder name from the byte-identical name literal; retail symbol unknown); retail VA 0x00406450-0x00406B53.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Radare2 6.2.0 `pd` over the exact image with esp simulated from `sub esp,0x428`; the 35 `call 0x4F1A80` sites were derived by replaying every push, `lea reg,[esp+K]` and `mov reg,[global]` instead of by pattern, and the derived per-site arguments reproduce the retail call census exactly -- 0x4F1C70 x13, 0x4F1C90 x6, 0x4F1CB0 x5, constant 0x5690C0 x7, constant 0x5690B8 x1, zero x3, with arg4 producers 0x4F0F50 x31 / 0x4F0FF0 x2 / 0x4F0EF0 x1 / 0x4F0FA0 x1: the 15-target reconciliation for this function. Frame offsets are relative to the post-prologue esp and all lie inside the 0x428 frame.
// Behavior/CFG: one basic block, no branch of its own. `sub esp,0x428`; registers "HumanAIConfig" (size 0x180) and "HumanAIConfigItem" (size 0x2A8) through 0x00401900, storing the 16-bit ids at 0x005383D8 and 0x005383DA; makes nine handler links on those ids through 0x00401400 and 0x00401530 (0x004886F0 event 0, 0x004D3710 phase 2, 0x00406B60, 0x004D3710, 0x00406B70 event 0, 0x004D3710 phase 1, 0x004D3710 phase 2, 0x00406D00, 0x00406D10); allocates two containers through 0x004F1950 (count 1 -> 0x005690B0, count 0x22 -> 0x005690C8); writes 0x005690B8=1, 0x005690BC=0x3C, 0x005690C0=0, 0x005690C4=0x41200000 (5.0f) between the second allocation's push and its call; then makes 35 `call 0x004F1A80` appends in retail order. Epilogue `add esp,0x440` (0x428 frame plus 0x18 of pending arguments) and `ret`.
// Status/limits: STRUCTURALLY_VERIFIED. All 35 append sites, both registrations, all nine links, both allocations and the four config stores come from their own pushes; the per-site arguments were derived mechanically and independently of the earlier hand-built table, and the two agree except where that table aggregated the two appends inside its first group (0x004065ED is arg5=3, arg3=frame+0x128, arg2=frame+0x00, not the group's arg5=7). Limits: no runtime observation of this native in either binary; the two surplus zero dwords retail pushes before each argument-less 0x004018F0 call cannot be expressed as C++ arguments and are recorded rather than silently dropped; and the deferred-push pattern is written as one nested call, which preserves every argument value and the call order but not the literal instruction order.
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

extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);          // 0x004F1A60
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);          // 0x004F1A70
extern "C" void* Script_TaskContainer_Alloc(std::int32_t count);   // 0x004F1950
extern "C" std::uint32_t Sys_GetGlobal32_4F0EF0(void);             // 0x004F0EF0
extern "C" std::uint32_t Sys_GetGlobal32_4F0F50(void);             // 0x004F0F50
extern "C" std::uint32_t Sys_GetGlobal32_4F0FF0(void);             // 0x004F0FF0
extern "C" std::uint32_t Core_GetRenderWordA_4F0FA0(void);         // 0x004F0FA0
extern "C" std::int32_t Sys_InitFloatPair_4F1C70(void);            // 0x004F1C70
extern "C" std::int32_t Sys_InitFloatPair_4F1C90(void);            // 0x004F1C90
extern "C" std::int32_t Sys_InitFloatPair_4F1CB0(void);            // 0x004F1CB0

extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5, std::int32_t const arg6);

std::uint16_t g_5383d8 = 0U;   // retail 0x005383D8: "HumanAIConfig" type id
std::uint16_t g_5383da = 0U;   // retail 0x005383DA: "HumanAIConfigItem" type id
std::uint32_t g_5690b0 = 0U;   // retail 0x005690B0: first container dword (a pointer, stored as a dword)
std::uint32_t g_5690c8 = 0U;   // retail 0x005690C8: second container dword
std::int32_t g_5690b8 = 0;     // retail 0x005690B8
std::int32_t g_5690bc = 0;     // retail 0x005690BC
std::int32_t g_5690c0 = 0;     // retail 0x005690C0
std::int32_t g_5690c4 = 0;     // retail 0x005690C4

std::int32_t NATIVE_DECL Task_DefineHumanAIConfig()
{
    NATIVE_LOG_ENTER("Task_DefineHumanAIConfig", "0x00406450");
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x00406456/0x0040645A: retail pushes two surplus zero dwords and then calls the
    // argument-less getter 0x004018F0. The shared `add esp,0x14` at 0x0040646F is one
    // dword larger than this four-argument 0x00401900 call, which is where the surplus
    // is visible. The second registration's two zeros are cleaned by `add esp,0x2C`.
    g_5383d8 = TaskType_Register("HumanAIConfig", 0x180U, QTask_GetCurrentType(), 0U);      // 0x0040646A
    g_5383da = TaskType_Register("HumanAIConfigItem", 0x2A8U, QTask_GetCurrentType(), 0U);  // 0x0040648C

    QTask_LinkQTaskEventHandler(g_5383d8, 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(0x004886F0));                     // 0x004064A5
    QTask_LinkQTaskEventHandlerNoContextFn(g_5383d8, 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(0x004D3710));                     // 0x004064B9
    QTask_LinkQTaskEventHandler(g_5383d8, Script_PhaseIdGetE4_4F1A60(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00406B60));                     // 0x004064D4
    QTask_LinkQTaskEventHandler(g_5383d8, Script_PhaseIdGetE5_4F1A70(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x004D3710));                     // 0x004064EE
    g_5690b0 = reinterpret_cast<std::uint32_t>(Script_TaskContainer_Alloc(1));                                               // 0x004064F5

    // 0x00406522: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690b0)),
                                append_frame + 0x2A8, append_frame + 0x2C8,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0EF0()), 7, 0);

    QTask_LinkQTaskEventHandler(g_5383da, 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00406B70));                     // 0x00406536
    QTask_LinkQTaskEventHandlerNoContextFn(g_5383da, 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(0x004D3710));                     // 0x0040654A
    QTask_LinkQTaskEventHandlerNoContextFn(g_5383da, 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(0x004D3710));                     // 0x0040655D
    QTask_LinkQTaskEventHandler(g_5383da, Script_PhaseIdGetE4_4F1A60(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00406D00));                     // 0x00406578
    QTask_LinkQTaskEventHandler(g_5383da, Script_PhaseIdGetE5_4F1A70(),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00406D10));                     // 0x00406593

    // 0x0040659A..0x004065C2: the four config dwords are stored between the second
    // allocation's `push 0x22` and its call, which is retail's instruction order.
    g_5690b8 = 1;           // 0x0040659A
    g_5690bc = 0x3C;        // 0x004065A4
    g_5690c0 = 0;           // 0x004065AE
    g_5690c4 = 0x41200000;  // 0x004065B8 (5.0f)
    g_5690c8 = reinterpret_cast<std::uint32_t>(Script_TaskContainer_Alloc(0x22));                                            // 0x004065C2

    // 0x004065ED: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690c8)),
                                append_frame, append_frame + 0x128,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FF0()), 3, 0);

    // 0x00406610: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690c8)),
                                append_frame, append_frame + 0x28,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FF0()), 3, 0);

    // 0x00406639: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690c8)),
                                append_frame, append_frame + 0x2A0,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(::g_5690c0));

    // 0x00406662: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690c8)),
                                append_frame, append_frame + 0x2A4,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(::g_5690c0));

    // 0x0040668C: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690c8)),
                                append_frame, append_frame + 0x22C,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(Sys_InitFloatPair_4F1C90()));

    // 0x004066B6: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690c8)),
                                append_frame, append_frame + 0x230,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(Sys_InitFloatPair_4F1C90()));

    // 0x004066E0: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690c8)),
                                append_frame, append_frame + 0x268,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(Sys_InitFloatPair_4F1CB0()));

    // 0x0040670A: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690c8)),
                                append_frame, append_frame + 0x270,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(Sys_InitFloatPair_4F1CB0()));

    // 0x00406734: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690c8)),
                                append_frame, append_frame + 0x26C,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(Sys_InitFloatPair_4F1CB0()));

    // 0x0040675E: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690c8)),
                                append_frame, append_frame + 0x234,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(Sys_InitFloatPair_4F1C70()));

    // 0x00406788: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690c8)),
                                append_frame, append_frame + 0x238,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(Sys_InitFloatPair_4F1C70()));

    // 0x004067B2: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690c8)),
                                append_frame, append_frame + 0x23C,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(Sys_InitFloatPair_4F1C70()));

    // 0x004067DC: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690c8)),
                                append_frame, append_frame + 0x240,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(Sys_InitFloatPair_4F1C70()));

    // 0x00406806: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690c8)),
                                append_frame, append_frame + 0x244,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(Sys_InitFloatPair_4F1C70()));

    // 0x00406830: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690c8)),
                                append_frame, append_frame + 0x248,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(Sys_InitFloatPair_4F1C70()));

    // 0x0040685A: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690c8)),
                                append_frame, append_frame + 0x24C,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(Sys_InitFloatPair_4F1C70()));

    // 0x00406883: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690c8)),
                                append_frame, append_frame + 0x228,
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, static_cast<std::int32_t>(::g_5690b8));

    // 0x004068AD: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690c8)),
                                append_frame, append_frame + 0x274,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(Sys_InitFloatPair_4F1C70()));

    // 0x004068D7: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690c8)),
                                append_frame, append_frame + 0x250,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(Sys_InitFloatPair_4F1C70()));

    // 0x00406901: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690c8)),
                                append_frame, append_frame + 0x254,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(Sys_InitFloatPair_4F1C70()));

    // 0x0040692B: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690c8)),
                                append_frame, append_frame + 0x258,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(Sys_InitFloatPair_4F1C90()));

    // 0x00406955: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690c8)),
                                append_frame, append_frame + 0x25C,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(Sys_InitFloatPair_4F1C90()));

    // 0x0040697F: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690c8)),
                                append_frame, append_frame + 0x260,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(Sys_InitFloatPair_4F1C90()));

    // 0x004069A9: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690c8)),
                                append_frame, append_frame + 0x264,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(Sys_InitFloatPair_4F1C90()));

    // 0x004069D3: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690c8)),
                                append_frame, append_frame + 0x278,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(Sys_InitFloatPair_4F1C70()));

    // 0x004069FD: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690c8)),
                                append_frame, append_frame + 0x27C,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(Sys_InitFloatPair_4F1C70()));

    // 0x00406A27: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690c8)),
                                append_frame, append_frame + 0x280,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(Sys_InitFloatPair_4F1CB0()));

    // 0x00406A51: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690c8)),
                                append_frame, append_frame + 0x284,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(Sys_InitFloatPair_4F1CB0()));

    // 0x00406A7A: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690c8)),
                                append_frame, append_frame + 0x288,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(::g_5690c0));

    // 0x00406AA3: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690c8)),
                                append_frame, append_frame + 0x28C,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(::g_5690c0));

    // 0x00406ACC: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690c8)),
                                append_frame, append_frame + 0x290,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(::g_5690c0));

    // 0x00406AF5: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690c8)),
                                append_frame, append_frame + 0x294,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(::g_5690c0));

    // 0x00406B1F: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690c8)),
                                append_frame, append_frame + 0x298,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(Sys_InitFloatPair_4F1C70()));

    // 0x00406B48: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5690c8)),
                                append_frame, append_frame + 0x29C,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(::g_5690c0));

    return 0;   // 0x00406B53: this path returns without setting EAX
}

} // extern "C"
