// Native: Task_DefineMenuManager_4170A0 (behavior-derived placeholder name; retail symbol unknown); retail VA 0x004170A0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004170A0-0x0041725C (445 bytes, 1 block, 0 edges, 128 instructions; epilogue add esp,0x28B0 at 0x00417256 balancing the 0x2898 probe allocation plus the tail's six outstanding words).
// Tools/evidence: radare2 6.2.0 pd/p8/afij over the exact image. 28 call sites, 17 distinct targets: 0x00401530 x5 and 0x00401400 x3, five appends through 0x004F1A80, one 0x004F1950 allocation with count 5. The five append sites are the only five records the container ever receives, so records and capacity meet exactly; totals, partition and capacities are one number here rather than three, unlike the 52-site body where they were independent. The body's boundary is NOP padding at 0x0041725D-0x0041725F with a different prologue at 0x00417260, and that address is also the handler the first link registers - the next function starts immediately after the padding because it is the link's target, which is a positive confirmation of the boundary rather than an absence of code.
// Behavior/CFG: one straight-line body. Allocates a 0x2898-byte frame at 0x004170A0 with `mov eax,0x2898; call 0x004A54A0` - the MSVC 6 4KB stack probe, which lowers esp itself and returns via `push eax; ret`, so the frame base is set inside the probe and there is no `sub esp` here. That call is a compiler-emitted CRT helper, classified in scripts/native_coverage.py as "MSVC __chkstk 4KB stack probe (compiler-emitted, never hand-transcribed)" - as with core/native_4310f0.cpp, the C++ frame replaces the allocation and no native is ever transcribed for it. Because this prologue pushes no callee-saved register, there is no displacement shift: every `lea reg,[esp+N]` resolves directly against the probe-set base. Registers the "MENUSYS:"/"LOCAL:menusystem_" alias through 0x004B12F0, then "MenuManager" of size 0x2898 via 0x00401900 with id at 0x00539824, links three context handlers (0x00401400: 0x00417260 event 0, 0x00418070 event e4, 0x00417CF0 event e5) and five no-context handlers (0x00401530: 0x00417A20 phase 2, 0x00418080 phase 1, 0x00418970 e, 0x00418A80, 0x00418AA0), then appends five records into a container of capacity 5 at 0x0057BAE0.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 128-instruction listing; compile status recorded below. Runtime UNOBSERVED. The three handler immediates that are pushed before the argument-less phase-id getters (0x00418070, 0x00417CF0, 0x00418970) are consumed as the following link's third argument - the deferred-push shape recorded for this lane, transcribed as genuine three-argument calls, never as an invented parameter on the getter. Both defined globals 0x00539824 and 0x0057BAE0 were swept whole-file first and have no other owner. The capacity check is exact here but has no slack to exercise, so it confirms rather than proves the count.
#include "dev_log.hpp"
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

void NATIVE_DECL QFile_RegisterAlias_4B12F0(const char* arg1, const char* arg2);
std::uint16_t NATIVE_DECL QTask_GetCurrentType(void);
std::uint16_t NATIVE_DECL TaskType_Register(const char* name, std::uint32_t object_size,
                                            std::uint32_t parent, std::uint32_t flags);
std::uint32_t NATIVE_DECL QTask_LinkQTaskEventHandler(std::uint16_t class_id, std::uint32_t event_id,
                                                      void* handler);
std::uint32_t NATIVE_DECL QTask_LinkQTaskEventHandlerNoContextFn(std::uint16_t class_id,
                                                                std::uint32_t event_id, void* handler);
std::uint32_t NATIVE_DECL Script_PhaseIdGetE4_4F1A60(void);
std::uint32_t NATIVE_DECL Script_PhaseIdGetE5_4F1A70(void);
std::uint32_t NATIVE_DECL Core_GetEventWord_4E7530(void);
std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F1210(void);
std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F1220(void);
std::uint32_t NATIVE_DECL Script_TaskContainer_Alloc(std::int32_t count);
std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F0EF0(void);
std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F0FF0(void);
std::uint32_t NATIVE_DECL Core_GetRenderWordA_4F0FA0(void);
std::uint32_t NATIVE_DECL Camera_Get_4F1000(void);
void* NATIVE_DECL Script_TaskContainer_Append(std::int32_t* container, std::int32_t arg2,
                                              std::int32_t arg3, std::int32_t arg4,
                                              std::int32_t arg5, std::int32_t arg6);

std::uint16_t g_539824 = 0x181U;
std::uint32_t g_57bae0 = 0U;

void NATIVE_DECL Task_DefineMenuManager_4170A0(void)
{
    NATIVE_LOG_ENTER("Task_DefineMenuManager_4170A0", "0x004170A0");
    // 0x004170A0: mov eax,0x2898; 0x004170A5: call 0x004A54A0 - the compiler-emitted MSVC 6 stack
    // probe (CRT, never hand-transcribed). This array is the frame it allocates; the probe's esp
    // drop is what sets the frame base, and every offset below is relative to that base.
    std::uint8_t frame_storage[0x2898];
    std::uint8_t* const f = frame_storage;

    ::QFile_RegisterAlias_4B12F0("MENUSYS:", "LOCAL:menusystem_");         // 0x004170AA/0x004170AF push str; 0x004170B4 call
    ::g_539824 = ::TaskType_Register("MenuManager", 0x2898,                // 0x004170C0 call 0x4018F0; 0x004170CB push str; 0x004170D0 call 0x401900
                                     static_cast<std::uint32_t>(::QTask_GetCurrentType()), 0);
    ::QTask_LinkQTaskEventHandler(::g_539824, 0,                           // 0x004170D5 push 0x417260; 0x004170E3 call 0x401400
                                  reinterpret_cast<void*>(0x00417260U));
    ::QTask_LinkQTaskEventHandlerNoContextFn(::g_539824, 2,                // 0x004170EE push 0x417A20; 0x004170F6 call 0x401530
                                             reinterpret_cast<void*>(0x00417A20U));
    ::QTask_LinkQTaskEventHandler(::g_539824, ::Script_PhaseIdGetE4_4F1A60(),  // 0x004170FE push 0x418070; 0x00417111 call 0x401400
                                  reinterpret_cast<void*>(0x00418070U));
    ::QTask_LinkQTaskEventHandler(::g_539824, ::Script_PhaseIdGetE5_4F1A70(),  // 0x00417119 push 0x417CF0; 0x0041712C call 0x401400
                                  reinterpret_cast<void*>(0x00417CF0U));
    ::QTask_LinkQTaskEventHandlerNoContextFn(::g_539824, 1,                // 0x00417137 push 0x418080; 0x0041713F call 0x401530
                                             reinterpret_cast<void*>(0x00418080U));
    ::QTask_LinkQTaskEventHandlerNoContextFn(::g_539824,
                                             ::Core_GetEventWord_4E7530(),  // 0x00417147 push 0x418970; 0x0041715A call 0x401530
                                             reinterpret_cast<void*>(0x00418970U));
    ::QTask_LinkQTaskEventHandlerNoContextFn(::g_539824,
                                             ::Sys_GetGlobal32_4F1210(),   // 0x00417162 push 0x418A80; 0x00417175 call 0x401530
                                             reinterpret_cast<void*>(0x00418A80U));
    ::QTask_LinkQTaskEventHandlerNoContextFn(::g_539824,
                                             ::Sys_GetGlobal32_4F1220(),   // 0x0041717D push 0x418AA0; 0x0041718F call 0x401530
                                             reinterpret_cast<void*>(0x00418AA0U));
    ::g_57bae0 = ::Script_TaskContainer_Alloc(5);                          // 0x00417194 push 5; 0x00417196 call 0x4F1950; 0x0041719E store

    ::Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bae0)),
                                  static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(f + 0x8)),
                                  static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(f + 0x26C4)),
                                  static_cast<std::int32_t>(::Sys_GetGlobal32_4F0EF0()), 6, 0);   // 0x004171A3..0x004171C5
    ::Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bae0)),
                                  static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(f + 0x8)),
                                  static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(f + 0x28)),
                                  static_cast<std::int32_t>(::Core_GetRenderWordA_4F0FA0()), 7, 0);   // 0x004171C8..0x004171E7
    ::Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bae0)),
                                  static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(f + 0x8)),
                                  static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(f + 0xDC)),
                                  static_cast<std::int32_t>(::Sys_GetGlobal32_4F0FF0()), 7, 0);   // 0x004171EA..0x0041720C
    ::Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bae0)),
                                  static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(f + 0x8)),
                                  static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(f + 0x26CD)),
                                  static_cast<std::int32_t>(::Sys_GetGlobal32_4F0FF0()), 6, 0);   // 0x0041720F..0x00417231
    ::Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bae0)),
                                  static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(f + 0x8)),
                                  static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(f + 0x26C9)),
                                  static_cast<std::int32_t>(::Camera_Get_4F1000()), 7, 0);   // 0x00417234..0x00417251
} // 0x00417256: add esp,0x28B0 - 0x2898 probe frame plus the tail append's six words; 0x0041725C: ret

} // extern "C"

#undef NATIVE_DECL
