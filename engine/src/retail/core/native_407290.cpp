// Native: Task_DefineWeaponConfig_407290 (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00407290.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00407290-0x00407B4A (2235 bytes, 1 block, 0 edges, 703 instructions; sub esp,0x82c and epilogue add esp,0x844).
// Tools/evidence: radare2 6.2.0 pd/p8/afij over the exact image. 128 call sites, 21 distinct targets. Links: 0x00401400 x6 and 0x00401530 x0 - the context variant only, the no-context linker is absent, so the family's usual count does not apply here. Appends: 52 through 0x004F1A80, partitioned by container as 0x569208 -> 50 and 0x569204 -> 2, matching the two 0x004F1950 capacities (50 at 0x004073FA, 2 at 0x0040739E) exactly; totals, partition and capacities are three independent numbers that agree. 0x004F1CD0/0x004F1DB0 run once per container over 0x5690E0 and 0x5691F0, the same list-head/new-record pair the registrar engine/src/retail/core/native_413df0.cpp runs four times over 0x0056E248/238/218/228 - corroboration from an unrelated file.
// Behavior/CFG: straight-line, no branch of any kind. Allocates one event through 0x004017C0 (QTask_AllocateEvent, declared engine/include/igi/retail/core/task_types.hpp:200, defined engine/src/retail/core/task_types.cpp:490 - it is NOT a native_ file, the hazard that has caused three duplicate/undercount defects this session), registers "WeaponConfigContainer" (0x124, id at 0x005384C0) and "WeaponConfig" (0x708, id at 0x005384C2), links six context handlers, then appends 52 records. arg5 is 7 at 42 sites, 3 at two, 6 and 2 at one each, and six sites take an accessor's returned pointer; arg6 is 0 at the 46 literal sites. The zero must not be generalised across all 52.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 703-instruction listing; compile status recorded below. Runtime UNOBSERVED. All 21 callees resolve to a definition file (checked whole-file by provenance line, not by declaration pattern) - a clean negative, no consumer-only hole. The five globals 0x5690D8, 0x5690E0, 0x5691F0, 0x569204, 0x569208 have no other owner.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

std::uint32_t NATIVE_DECL QTask_AllocateEvent(std::uint32_t arg1);
std::uint16_t NATIVE_DECL QTask_GetCurrentType(void);
std::uint16_t NATIVE_DECL TaskType_Register(const char* name, std::uint32_t object_size,
                                            std::uint32_t parent, std::uint32_t flags);
std::uint32_t NATIVE_DECL QTask_LinkQTaskEventHandler(std::uint16_t class_id, std::uint32_t event_id,
                                                      void* handler);
std::uint32_t NATIVE_DECL Script_PhaseIdGetE4_4F1A60(void);
std::uint32_t NATIVE_DECL Script_PhaseIdGetE5_4F1A70(void);
std::uint32_t NATIVE_DECL Sys_InitGlobalListHead_4F1CD0(void* list_head);
std::uint32_t NATIVE_DECL Sys_NewListRecord_4F1DB0(void* list_head, const char* name);
std::uint32_t NATIVE_DECL Script_TaskContainer_Alloc(std::int32_t count);
std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F0EF0(void);
std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F0F50(void);
std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F0FF0(void);
std::uint32_t NATIVE_DECL Core_GetRenderWordA_4F0FA0(void);
std::uint32_t NATIVE_DECL Core_GetRenderWordB_4F0FD0(void);
std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F1E00(void);
std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F1E10(void);
std::uint32_t NATIVE_DECL Camera_Get_4F1000(void);
std::uint32_t NATIVE_DECL Sys_GetGlobalPtr_413DB0(void);
std::uint32_t NATIVE_DECL Sys_GetGlobalPtr_413DC0(void);
std::uint32_t NATIVE_DECL Sys_GetGlobalPtr_413DD0(void);
void* NATIVE_DECL Sys_InitFloatPair_4F1C70(void);
void* NATIVE_DECL Sys_InitFloatPair_4F1C90(void);
void* NATIVE_DECL Sys_InitFloatPair_4F1CB0(void);
void* NATIVE_DECL Script_TaskContainer_Append(std::int32_t* container, std::int32_t arg2,
                                              std::int32_t arg3, std::int32_t arg4,
                                              std::int32_t arg5, std::int32_t arg6);

// Retail 0x005690D8 is owned by its writer, engine/src/retail/config/native_4071d0.cpp.
// Defining it here was a duplicate; this file reads and writes the existing storage.
extern "C" std::uint32_t g_5690d8;   // type follows the owner in config/native_4071d0.cpp
std::uint8_t g_5690e0[0x10] = {};
std::uint8_t g_5691f0[0x10] = {};
std::uint32_t g_569204 = 0U;
std::uint32_t g_569208 = 0U;
std::uint16_t g_5384c0 = 0U;
std::uint16_t g_5384c2 = 0U;

void NATIVE_DECL Task_DefineWeaponConfig_407290(void)
{
    std::uint8_t frame_storage[0x82c];
    std::uint8_t* const append_frame = frame_storage;

    ::g_5690d8 = static_cast<std::uint8_t>(::QTask_AllocateEvent(0));         // 0x00407296 push 0; 0x00407298 call; 0x004072A0 mov byte [0x5690D8],al
    ::g_5384c0 = ::TaskType_Register("WeaponConfigContainer", 0x124,          // 0x004072A9 call 0x4018F0; 0x004072B4 push str; 0x004072B9 call 0x401900
                                     static_cast<std::uint32_t>(::QTask_GetCurrentType()), 0);
    ::QTask_LinkQTaskEventHandler(::g_5384c0, 0,                              // 0x004072BE push 0x407B50; 0x004072CC call 0x401400
                                  reinterpret_cast<void*>(0x00407B50U));
    ::QTask_LinkQTaskEventHandler(::g_5384c0, ::Script_PhaseIdGetE5_4F1A70(), // 0x004072D4 push 0x4D3710; 0x004072E6 call 0x401400
                                  reinterpret_cast<void*>(0x004D3710U));
    ::QTask_LinkQTaskEventHandler(::g_5384c0, ::Script_PhaseIdGetE4_4F1A60(), // 0x004072EE push 0x407B90; 0x00407301 call 0x401400
                                  reinterpret_cast<void*>(0x00407B90U));
    ::g_5384c2 = ::TaskType_Register("WeaponConfig", 0x708,                   // 0x0040730D call 0x4018F0; 0x00407318 push str; 0x0040731D call 0x401900
                                     static_cast<std::uint32_t>(::QTask_GetCurrentType()), 0);
    ::QTask_LinkQTaskEventHandler(::g_5384c2, 0,                              // 0x00407322 push 0x407BA0; 0x00407330 call 0x401400
                                  reinterpret_cast<void*>(0x00407BA0U));
    ::QTask_LinkQTaskEventHandler(::g_5384c2, ::Script_PhaseIdGetE5_4F1A70(), // 0x00407338 push 0x407BE0; 0x0040734B call 0x401400
                                  reinterpret_cast<void*>(0x00407BE0U));
    ::QTask_LinkQTaskEventHandler(::g_5384c2, ::Script_PhaseIdGetE4_4F1A60(), // 0x00407353 push 0x407CF0; 0x00407365 call 0x401400
                                  reinterpret_cast<void*>(0x00407CF0U));
    ::Sys_InitGlobalListHead_4F1CD0(::g_5690e0);                              // 0x0040736A push 0x5690E0; 0x0040736F call 0x4F1CD0
    ::Sys_NewListRecord_4F1DB0(::g_5690e0, "HUMANAI_DETECTIONEVENT_._RANGE"); // 0x00407374 push str; 0x0040737E call 0x4F1DB0
    ::Sys_InitGlobalListHead_4F1CD0(::g_5691f0);                              // 0x00407383 push 0x5691F0; 0x00407388 call 0x4F1CD0
    ::Sys_NewListRecord_4F1DB0(::g_5691f0, "TASKTYPE_.*");                    // 0x0040738D push str; 0x00407397 call 0x4F1DB0
    ::g_569204 = ::Script_TaskContainer_Alloc(2);                             // 0x0040739C push 2; 0x0040739E call 0x4F1950; 0x004073A6 store
    ::g_569208 = ::Script_TaskContainer_Alloc(0x32);                          // 0x004073F8 push 0x32; 0x004073FA call 0x4F1950; 0x00407402 store

    // 0x004073cb
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569204)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x708)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x728)),
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0EF0()), 6, 0);
    // 0x004073f3
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569204)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x708)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x729)),
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FF0()), 2, 0);
    // 0x00407421
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x20)),
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 3, 0);
    // 0x00407443
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x30)),
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FF0()), 3, 0);
    // 0x00407468
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x130)),
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FF0()), 7, 0);
    // 0x0040748d
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x330)),
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FF0()), 7, 0);
    // 0x004074b2
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x230)),
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FF0()), 7, 0);
    // 0x004074db
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x630)),
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1E00()), 7, static_cast<std::int32_t>(Sys_GetGlobalPtr_413DB0()));
    // 0x00407504
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x634)),
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1E00()), 7, static_cast<std::int32_t>(Sys_GetGlobalPtr_413DC0()));
    // 0x0040752d
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x638)),
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1E00()), 7, static_cast<std::int32_t>(Sys_GetGlobalPtr_413DD0()));
    // 0x0040754f
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x24)),
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);
    // 0x00407574
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x65C)),
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);
    // 0x00407599
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x674)),
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, 0);
    // 0x004075be
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x678)),
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, 0);
    // 0x004075e3
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x67C)),
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, 0);
    // 0x00407608
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x680)),
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, 0);
    // 0x0040762d
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x660)),
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);
    // 0x00407652
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x664)),
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);
    // 0x00407677
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x668)),
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);
    // 0x0040769c
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x66C)),
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);
    // 0x004076c1
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x684)),
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, 0);
    // 0x004076e6
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x688)),
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, 0);
    // 0x0040770b
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x68C)),
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, 0);
    // 0x00407730
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x690)),
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, 0);
    // 0x00407755
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x694)),
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, 0);
    // 0x0040777a
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x698)),
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, 0);
    // 0x0040779f
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x430)),
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FF0()), 7, 0);
    // 0x004077c4
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x670)),
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, 0);
    // 0x004077e9
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x530)),
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FF0()), 7, 0);
    // 0x0040780b
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x28)),
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);
    // 0x0040782d
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x2C)),
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);
    // 0x00407852
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x63C)),
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);
    // 0x00407877
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x64C)),
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);
    // 0x0040789c
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x69C)),
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);
    // 0x004078c1
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x6A0)),
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);
    // 0x004078e6
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x6A4)),
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);
    // 0x0040790b
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x6A8)),
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);
    // 0x00407930
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x6AC)),
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);
    // 0x00407955
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x6B0)),
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);
    // 0x0040797a
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x6B4)),
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);
    // 0x0040799f
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x6B8)),
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);
    // 0x004079c4
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x6C0)),
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);
    // 0x004079e9
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x6C4)),
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);
    // 0x00407a0e
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x6BC)),
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);
    // 0x00407a33
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x6C8)),
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);
    // 0x00407a58
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x6CC)),
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);
    // 0x00407a7d
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x6D0)),
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);
    // 0x00407aa2
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x6E0)),
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);
    // 0x00407aca
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x6F0)),
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1E10()), 7, 0x5690e0);
    // 0x00407af2
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x6F4)),
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1E00()), 7, 0x5691f0);
    // 0x00407b1a
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x6F8)),
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1E00()), 7, 0x5691f0);
    // 0x00407b3f
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_569208)),
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x0)), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x6FC)),
                                static_cast<std::int32_t>(Camera_Get_4F1000()), 7, 0);

    // 0x00407B44: epilogue - add esp,0x844 is the 0x82c frame plus the 0x18 of outstanding
    // pushes at the tail (the last append's six words).
} // Task_DefineWeaponConfig_407290

} // extern "C"

#undef NATIVE_DECL
