// Native: Task_RegisterAIActions_44CCA0 (behavior-derived placeholder name: it registers every AI action and AI function script entry point into the script registry; retail symbol unknown); retail VA 0x0044CCA0-0x0044D0DF.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 1088 bytes (0x440), 1 block, 0 edges, 290 instructions, 55 call sites, counted with af + afbj over
//   the exact image. The body contains no branch of any kind.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf and the objdump listing build/retail-dis.txt over the same extent. `psz` at all 55 name addresses
//   gives the byte-identical literals below, and every handler argument is a push immediate, so all 55 handler entry points are fixed by the
//   image rather than inferred. A push-by-push simulation closes EXACTLY with no frame and no saved registers: 880 bytes of pushes equals the
//   0x370 total of the fourteen `add esp` cleanups (thirteen 0x40 and one 0x20 - the last group holds a single registration). The three callees
//   are the register family already transcribed in script/: 0x004B8890 (41 calls), 0x004B88C0 (10) and 0x004B88F0 (4); all three take
//   (name, handler, char const*, value) and differ only in one constant forwarded to 0x004BAAC0.
// Behavior/CFG: one straight-line block of 55 registrations in retail order - 18 AIAction_ names then 37 AIFunction_ names - each call carrying
//   (name literal, handler address, 0, and the value retail pushes first: 0, 1, 2, 3 or 5). The value is transcribed as the literal retail pushes;
//   nothing establishes what it selects. Each handler argument is the address of its declaration rather than a bare integer so the linker is
//   forced to resolve the body retail's immediate points at; the emitted value is that address either way. No frame, no stack probe, no return value.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. NOT LINKABLE: the 55 handler bodies do not exist in the tree yet, so they are declared here with
//   the tree's handler ABI (void NATIVE_DECL <name>(void* context), the convention core/native_508fa0.cpp uses for AIGraph_Handler_5092A0) and
//   are NOT stubbed. Each keeps the placeholder form Task_Handler_<VA> until its own body is read: the name literal in a registration pair names
//   the EVENT being registered, not the handler's behaviour, so it is deliberately not used as the handler's name.
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

// 0x004B8890 / 0x004B88C0 / 0x004B88F0: the three register variants, transcribed in script/native_4b8890.cpp, script/native_4b88c0.cpp and
// script/native_4b88f0.cpp. Retail pushes the value first and the name last, so the name is argument 0 and the handler is argument 1.
void Script_QHashNodeRegisterEx(void* arg0, void* arg1, char const* arg2, void* arg3);
void Script_QHashNodeRegisterMode1_4B88C0(void* arg0, void* arg1, char const* arg2, void* arg3);
void Script_QHashNodeRegisterMode2_4B88F0(void* arg0, void* arg1, char const* arg2, void* arg3);

// The 55 handler entry points this body registers, one per push immediate. Each is its own native in this band; none is implemented yet and
// none is stubbed.
void NATIVE_DECL Task_Handler_44D0E0(void* context);   // 0x0x44D0E0: the handler registered for "AIAction_Patrol"
void NATIVE_DECL Task_Handler_44D160(void* context);   // 0x0x44D160: the handler registered for "AIAction_Combat"
void NATIVE_DECL Task_Handler_44D260(void* context);   // 0x0x44D260: the handler registered for "AIAction_Dead"
void NATIVE_DECL Task_Handler_44D2C0(void* context);   // 0x0x44D2C0: the handler registered for "AIAction_FallFlat"
void NATIVE_DECL Task_Handler_44D420(void* context);   // 0x0x44D420: the handler registered for "AIAction_Activate"
void NATIVE_DECL Task_Handler_44D4A0(void* context);   // 0x0x44D4A0: the handler registered for "AIAction_WalkToNode"
void NATIVE_DECL Task_Handler_44D510(void* context);   // 0x0x44D510: the handler registered for "AIAction_RunToNode"
void NATIVE_DECL Task_Handler_44D580(void* context);   // 0x0x44D580: the handler registered for "AIAction_FireAtNode"
void NATIVE_DECL Task_Handler_44D630(void* context);   // 0x0x44D630: the handler registered for "AIAction_FireAtTask"
void NATIVE_DECL Task_Handler_44D6E0(void* context);   // 0x0x44D6E0: the handler registered for "AIAction_PlayAnimation"
void NATIVE_DECL Task_Handler_44D750(void* context);   // 0x0x44D750: the handler registered for "AIAction_PlaySound"
void NATIVE_DECL Task_Handler_44D7D0(void* context);   // 0x0x44D7D0: the handler registered for "AIAction_MoveToEvent"
void NATIVE_DECL Task_Handler_44D870(void* context);   // 0x0x44D870: the handler registered for "AIAction_LookAtEvent"
void NATIVE_DECL Task_Handler_44D8F0(void* context);   // 0x0x44D8F0: the handler registered for "AIAction_Stunned"
void NATIVE_DECL Task_Handler_44D960(void* context);   // 0x0x44D960: the handler registered for "AIAction_KickGrenade"
void NATIVE_DECL Task_Handler_44DA00(void* context);   // 0x0x44DA00: the handler registered for "AIAction_RunPanicking"
void NATIVE_DECL Task_Handler_44DA80(void* context);   // 0x0x44DA80: the handler registered for "AIAction_Idle"
void NATIVE_DECL Task_Handler_44DAE0(void* context);   // 0x0x44DAE0: the handler registered for "AIAction_SetCombat"
void NATIVE_DECL Task_Handler_44E060(void* context);   // 0x0x44E060: the handler registered for "AIFunction_DefaultHandler"
void NATIVE_DECL Task_Handler_44DBC0(void* context);   // 0x0x44DBC0: the handler registered for "AIFunction_RemoveAlarmActions"
void NATIVE_DECL Task_Handler_44DC40(void* context);   // 0x0x44DC40: the handler registered for "AIFunction_SetViewLength"
void NATIVE_DECL Task_Handler_44DCD0(void* context);   // 0x0x44DCD0: the handler registered for "AIFunction_SetAlarmViewLength"
void NATIVE_DECL Task_Handler_44DD30(void* context);   // 0x0x44DD30: the handler registered for "AIFunction_SetViewAlpha"
void NATIVE_DECL Task_Handler_44DD90(void* context);   // 0x0x44DD90: the handler registered for "AIFunction_SetViewGamma"
void NATIVE_DECL Task_Handler_44DDF0(void* context);   // 0x0x44DDF0: the handler registered for "AIFunction_SetSecondaryViewLength"
void NATIVE_DECL Task_Handler_44DE80(void* context);   // 0x0x44DE80: the handler registered for "AIFunction_SetSecondaryAlarmViewLength"
void NATIVE_DECL Task_Handler_44DEE0(void* context);   // 0x0x44DEE0: the handler registered for "AIFunction_SetSecondaryViewAlpha"
void NATIVE_DECL Task_Handler_44DF40(void* context);   // 0x0x44DF40: the handler registered for "AIFunction_SetSecondaryViewGamma"
void NATIVE_DECL Task_Handler_44E0C0(void* context);   // 0x0x44E0C0: the handler registered for "AIFunction_SetEventPriority"
void NATIVE_DECL Task_Handler_44E150(void* context);   // 0x0x44E150: the handler registered for "AIFunction_SetInvulnerability"
void NATIVE_DECL Task_Handler_44E1B0(void* context);   // 0x0x44E1B0: the handler registered for "AIFunction_SetInstantDeath"
void NATIVE_DECL Task_Handler_44E210(void* context);   // 0x0x44E210: the handler registered for "AIFunction_SetDeathAnimation"
void NATIVE_DECL Task_Handler_44E270(void* context);   // 0x0x44E270: the handler registered for "AIFunction_SetAlarmTriggerID"
void NATIVE_DECL Task_Handler_44E2D0(void* context);   // 0x0x44E2D0: the handler registered for "AIFunction_SetAlarmControlID"
void NATIVE_DECL Task_Handler_44E330(void* context);   // 0x0x44E330: the handler registered for "AIFunction_SetAlarmAccess"
void NATIVE_DECL Task_Handler_44E390(void* context);   // 0x0x44E390: the handler registered for "AIFunction_SetGunnerID"
void NATIVE_DECL Task_Handler_44E6D0(void* context);   // 0x0x44E6D0: the handler registered for "AIFunction_SetScriptIntegerValue"
void NATIVE_DECL Task_Handler_44E790(void* context);   // 0x0x44E790: the handler registered for "AIFunction_SetScriptRealValue"
void NATIVE_DECL Task_Handler_44E3F0(void* context);   // 0x0x44E3F0: the handler registered for "AIFunction_GetAlarmTriggerID"
void NATIVE_DECL Task_Handler_44E430(void* context);   // 0x0x44E430: the handler registered for "AIFunction_GetAlarmControlID"
void NATIVE_DECL Task_Handler_44E470(void* context);   // 0x0x44E470: the handler registered for "AIFunction_GetAlarmAccess"
void NATIVE_DECL Task_Handler_44E4B0(void* context);   // 0x0x44E4B0: the handler registered for "AIFunction_GetGunnerID"
void NATIVE_DECL Task_Handler_44E4F0(void* context);   // 0x0x44E4F0: the handler registered for "AIFunction_GetAlarmControlStatus"
void NATIVE_DECL Task_Handler_44E5E0(void* context);   // 0x0x44E5E0: the handler registered for "AIFunction_GetGunnerStatus"
void NATIVE_DECL Task_Handler_44E740(void* context);   // 0x0x44E740: the handler registered for "AIFunction_GetScriptIntegerValue"
void NATIVE_DECL Task_Handler_44DFA0(void* context);   // 0x0x44DFA0: the handler registered for "AIFunction_GetCurrentEventType"
void NATIVE_DECL Task_Handler_44E860(void* context);   // 0x0x44E860: the handler registered for "AIFunction_IsEventBehind"
void NATIVE_DECL Task_Handler_44E800(void* context);   // 0x0x44E800: the handler registered for "AIFunction_GetScriptRealValue"
void NATIVE_DECL Task_Handler_44E000(void* context);   // 0x0x44E000: the handler registered for "AIFunction_GetRandomValue"
void NATIVE_DECL Task_Handler_44E930(void* context);   // 0x0x44E930: the handler registered for "AIFunction_GetEventDistance"
void NATIVE_DECL Task_Handler_44E9B0(void* context);   // 0x0x44E9B0: the handler registered for "AIFunction_GetAlarmTriggerDistance"
void NATIVE_DECL Task_Handler_44EAF0(void* context);   // 0x0x44EAF0: the handler registered for "AIFunction_SetAnimationInterval"
void NATIVE_DECL Task_Handler_44EBB0(void* context);   // 0x0x44EBB0: the handler registered for "AIFunction_AddAnimationEntry"
void NATIVE_DECL Task_Handler_44ECB0(void* context);   // 0x0x44ECB0: the handler registered for "AIFunction_GetAnimationToPlay"
void NATIVE_DECL Task_Handler_44EE40(void* context);   // 0x0x44EE40: the handler registered for "AIFunction_SendResponse"

void NATIVE_DECL Task_RegisterAIActions_44CCA0(void)
{
    NATIVE_LOG_ENTER("Task_RegisterAIActions_44CCA0", "0x0044CCA0");
    // 0x0044CCAE: push 3; push 0; push 0x44D0E0; push 0x53D9DC; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIAction_Patrol"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44D0E0)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x3U)));
    // 0x0044CCC1: push 1; push 0; push 0x44D160; push 0x53D9CC; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIAction_Combat"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44D160)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1U)));
    // 0x0044CCD4: push 1; push 0; push 0x44D260; push 0x53D9BC; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIAction_Dead"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44D260)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1U)));
    // 0x0044CCE7: push 2; push 0; push 0x44D2C0; push 0x53D9A8; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIAction_FallFlat"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44D2C0)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x2U)));
    // 0x0044CCFD: push 3; push 0; push 0x44D420; push 0x53D994; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIAction_Activate"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44D420)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x3U)));
    // 0x0044CD10: push 2; push 0; push 0x44D4A0; push 0x53D980; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIAction_WalkToNode"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44D4A0)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x2U)));
    // 0x0044CD23: push 2; push 0; push 0x44D510; push 0x53D96C; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIAction_RunToNode"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44D510)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x2U)));
    // 0x0044CD36: push 5; push 0; push 0x44D580; push 0x53D958; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIAction_FireAtNode"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44D580)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x5U)));
    // 0x0044CD4C: push 5; push 0; push 0x44D630; push 0x53D944; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIAction_FireAtTask"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44D630)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x5U)));
    // 0x0044CD5F: push 2; push 0; push 0x44D6E0; push 0x53D92C; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIAction_PlayAnimation"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44D6E0)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x2U)));
    // 0x0044CD72: push 3; push 0; push 0x44D750; push 0x53D918; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIAction_PlaySound"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44D750)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x3U)));
    // 0x0044CD85: push 2; push 0; push 0x44D7D0; push 0x53D900; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIAction_MoveToEvent"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44D7D0)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x2U)));
    // 0x0044CD9B: push 2; push 0; push 0x44D870; push 0x53D8E8; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIAction_LookAtEvent"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44D870)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x2U)));
    // 0x0044CDAE: push 2; push 0; push 0x44D8F0; push 0x53D8D4; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIAction_Stunned"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44D8F0)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x2U)));
    // 0x0044CDC1: push 1; push 0; push 0x44D960; push 0x53D8BC; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIAction_KickGrenade"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44D960)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1U)));
    // 0x0044CDD4: push 2; push 0; push 0x44DA00; push 0x53D8A4; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIAction_RunPanicking"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44DA00)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x2U)));
    // 0x0044CDEA: push 1; push 0; push 0x44DA80; push 0x53D894; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIAction_Idle"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44DA80)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1U)));
    // 0x0044CDFD: push 1; push 0; push 0x44DAE0; push 0x53D880; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIAction_SetCombat"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44DAE0)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1U)));
    // 0x0044CE10: push 0; push 0; push 0x44E060; push 0x53D864; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIFunction_DefaultHandler"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44E060)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x0U)));
    // 0x0044CE23: push 0; push 0; push 0x44DBC0; push 0x53D844; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIFunction_RemoveAlarmActions"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44DBC0)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x0U)));
    // 0x0044CE39: push 1; push 0; push 0x44DC40; push 0x53D828; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIFunction_SetViewLength"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44DC40)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1U)));
    // 0x0044CE4C: push 1; push 0; push 0x44DCD0; push 0x53D808; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIFunction_SetAlarmViewLength"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44DCD0)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1U)));
    // 0x0044CE5F: push 1; push 0; push 0x44DD30; push 0x53D7F0; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIFunction_SetViewAlpha"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44DD30)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1U)));
    // 0x0044CE72: push 1; push 0; push 0x44DD90; push 0x53D7D8; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIFunction_SetViewGamma"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44DD90)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1U)));
    // 0x0044CE88: push 1; push 0; push 0x44DDF0; push 0x53D7B4; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIFunction_SetSecondaryViewLength"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44DDF0)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1U)));
    // 0x0044CE9B: push 1; push 0; push 0x44DE80; push 0x53D78C; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIFunction_SetSecondaryAlarmViewLength"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44DE80)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1U)));
    // 0x0044CEAE: push 1; push 0; push 0x44DEE0; push 0x53D768; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIFunction_SetSecondaryViewAlpha"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44DEE0)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1U)));
    // 0x0044CEC1: push 1; push 0; push 0x44DF40; push 0x53D744; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIFunction_SetSecondaryViewGamma"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44DF40)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1U)));
    // 0x0044CED7: push 1; push 0; push 0x44E0C0; push 0x53D728; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIFunction_SetEventPriority"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44E0C0)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1U)));
    // 0x0044CEEA: push 1; push 0; push 0x44E150; push 0x53D708; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIFunction_SetInvulnerability"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44E150)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1U)));
    // 0x0044CEFD: push 1; push 0; push 0x44E1B0; push 0x53D6EC; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIFunction_SetInstantDeath"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44E1B0)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1U)));
    // 0x0044CF10: push 1; push 0; push 0x44E210; push 0x53D6CC; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIFunction_SetDeathAnimation"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44E210)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1U)));
    // 0x0044CF26: push 1; push 0; push 0x44E270; push 0x53D6AC; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIFunction_SetAlarmTriggerID"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44E270)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1U)));
    // 0x0044CF39: push 1; push 0; push 0x44E2D0; push 0x53D68C; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIFunction_SetAlarmControlID"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44E2D0)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1U)));
    // 0x0044CF4C: push 1; push 0; push 0x44E330; push 0x53D670; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIFunction_SetAlarmAccess"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44E330)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1U)));
    // 0x0044CF5F: push 1; push 0; push 0x44E390; push 0x53D658; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIFunction_SetGunnerID"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44E390)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1U)));
    // 0x0044CF75: push 2; push 0; push 0x44E6D0; push 0x53D634; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIFunction_SetScriptIntegerValue"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44E6D0)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x2U)));
    // 0x0044CF88: push 2; push 0; push 0x44E790; push 0x53D614; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIFunction_SetScriptRealValue"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44E790)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x2U)));
    // 0x0044CF9B: push 0; push 0; push 0x44E3F0; push 0x53D5F4; call 0x4B88C0.
    Script_QHashNodeRegisterMode1_4B88C0(const_cast<char*>("AIFunction_GetAlarmTriggerID"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44E3F0)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x0U)));
    // 0x0044CFAE: push 0; push 0; push 0x44E430; push 0x53D5D4; call 0x4B88C0.
    Script_QHashNodeRegisterMode1_4B88C0(const_cast<char*>("AIFunction_GetAlarmControlID"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44E430)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x0U)));
    // 0x0044CFC4: push 0; push 0; push 0x44E470; push 0x53D5B8; call 0x4B88C0.
    Script_QHashNodeRegisterMode1_4B88C0(const_cast<char*>("AIFunction_GetAlarmAccess"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44E470)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x0U)));
    // 0x0044CFD7: push 0; push 0; push 0x44E4B0; push 0x53D5A0; call 0x4B88C0.
    Script_QHashNodeRegisterMode1_4B88C0(const_cast<char*>("AIFunction_GetGunnerID"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44E4B0)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x0U)));
    // 0x0044CFEA: push 0; push 0; push 0x44E4F0; push 0x53D57C; call 0x4B88C0.
    Script_QHashNodeRegisterMode1_4B88C0(const_cast<char*>("AIFunction_GetAlarmControlStatus"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44E4F0)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x0U)));
    // 0x0044CFFD: push 0; push 0; push 0x44E5E0; push 0x53D560; call 0x4B88C0.
    Script_QHashNodeRegisterMode1_4B88C0(const_cast<char*>("AIFunction_GetGunnerStatus"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44E5E0)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x0U)));
    // 0x0044D013: push 1; push 0; push 0x44E740; push 0x53D53C; call 0x4B88C0.
    Script_QHashNodeRegisterMode1_4B88C0(const_cast<char*>("AIFunction_GetScriptIntegerValue"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44E740)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1U)));
    // 0x0044D026: push 0; push 0; push 0x44DFA0; push 0x53D51C; call 0x4B88C0.
    Script_QHashNodeRegisterMode1_4B88C0(const_cast<char*>("AIFunction_GetCurrentEventType"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44DFA0)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x0U)));
    // 0x0044D039: push 0; push 0; push 0x44E860; push 0x53D500; call 0x4B88C0.
    Script_QHashNodeRegisterMode1_4B88C0(const_cast<char*>("AIFunction_IsEventBehind"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44E860)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x0U)));
    // 0x0044D04C: push 1; push 0; push 0x44E800; push 0x53D4E0; call 0x4B88F0.
    Script_QHashNodeRegisterMode2_4B88F0(const_cast<char*>("AIFunction_GetScriptRealValue"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44E800)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1U)));
    // 0x0044D062: push 1; push 0; push 0x44E000; push 0x53D4C4; call 0x4B88F0.
    Script_QHashNodeRegisterMode2_4B88F0(const_cast<char*>("AIFunction_GetRandomValue"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44E000)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1U)));
    // 0x0044D075: push 0; push 0; push 0x44E930; push 0x53D4A8; call 0x4B88F0.
    Script_QHashNodeRegisterMode2_4B88F0(const_cast<char*>("AIFunction_GetEventDistance"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44E930)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x0U)));
    // 0x0044D088: push 0; push 0; push 0x44E9B0; push 0x53D484; call 0x4B88F0.
    Script_QHashNodeRegisterMode2_4B88F0(const_cast<char*>("AIFunction_GetAlarmTriggerDistance"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44E9B0)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x0U)));
    // 0x0044D09B: push 2; push 0; push 0x44EAF0; push 0x53D464; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIFunction_SetAnimationInterval"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44EAF0)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x2U)));
    // 0x0044D0B1: push 2; push 0; push 0x44EBB0; push 0x53D444; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIFunction_AddAnimationEntry"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44EBB0)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x2U)));
    // 0x0044D0C4: push 0; push 0; push 0x44ECB0; push 0x53D424; call 0x4B88C0.
    Script_QHashNodeRegisterMode1_4B88C0(const_cast<char*>("AIFunction_GetAnimationToPlay"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44ECB0)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x0U)));
    // 0x0044D0D7: push 0; push 0; push 0x44EE40; push 0x53D40C; call 0x4B8890.
    Script_QHashNodeRegisterEx(const_cast<char*>("AIFunction_SendResponse"),
        reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&Task_Handler_44EE40)),
        nullptr,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x0U)));
}

} // extern "C"

#undef NATIVE_DECL
