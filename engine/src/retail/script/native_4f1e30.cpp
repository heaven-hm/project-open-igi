// Native: AIGraph_InitStep_4F1E30 (blessed address-suffixed placeholder; retail symbol not established); retail VA 0x004F1E30-0x004F1E9C.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 109 bytes (0x6D), 1 block, 0 edges,
//   27 instructions, counted over the exact image. Extent ends at the `ret` at 0x004F1E9C; the byte after it is `0x90` padding.
// Tools/evidence: radare2 6.2.0 `pd 31 @ 0x4f1e30`; scripts/native_fidelity_audit.py RetailImage.va_to_off() for the image bytes and call_index() for
//   called-vs-address-taken. Complete extent: five identical call/store pairs, each `call getter; push eax; push <string>; call 0x004F19C0; mov dword [<global>],eax`:
//   0x004F1E30 call 0x004F0FA0 / 0x004F1E36 push 0x0054E1A0 / 0x004F1E3B call 0x004F19C0 / 0x004F1E40 mov [0x0054E160],eax;
//   0x004F1E45 call 0x004F0F50 / 0x004F1E4B push 0x0054E194 / 0x004F1E50 call 0x004F19C0 / 0x004F1E55 mov [0x0054E150],eax;
//   0x004F1E5A call 0x004F0FD0 / 0x004F1E60 push 0x0054E184 / 0x004F1E65 call 0x004F19C0 / 0x004F1E6A mov [0x0054E154],eax;
//   0x004F1E6F call 0x004F0FE0 / 0x004F1E75 push 0x0054E174 / 0x004F1E7A call 0x004F19C0 / 0x004F1E7F mov [0x0054E158],eax;
//   0x004F1E84 call 0x004F0FF0 / 0x004F1E8A push 0x0054E164 / 0x004F1E8F call 0x004F19C0 / 0x004F1E94 add esp,0x28 / 0x004F1E97 mov [0x0054E15C],eax / 0x004F1E9C ret.
//   The single `add esp,0x28` releases TEN dwords, which is exactly the five two-push argument pairs, so every one of the five registration calls takes
//   two arguments and none is deferred. The strings were read from the image and are byte-identical: 0x0054E1A0 `EnumInt32`, 0x0054E194 `EnumReal32`,
//   0x0054E184 `EnumString16`, 0x0054E174 `EnumString32`, 0x0054E164 `EnumString256`.
//   NOTE THE TWO ORDERINGS DIFFER, and both are preserved: the GLOBALS are written in address order 0x0054E160, 0x0054E150, 0x0054E154, 0x0054E158, 0x0054E15C,
//   while the STRING each is registered under runs Int32, Real32, String16, String32, String256 - so the second store goes to the lowest global address and
//   the fifth to the highest. The `add esp,0x28` also sits between the last call and the last store; that is a scheduling artifact with no observable effect.
//   Callees, all transcribed: 0x004F0FA0 Core_GetRenderWordA_4F0FA0; 0x004F0F50 Sys_GetGlobal32_4F0F50; 0x004F0FD0 Core_GetRenderWordB_4F0FD0;
//   0x004F0FE0 Sys_GetGlobal32_4F0FE0; 0x004F0FF0 Sys_GetGlobal32_4F0FF0; 0x004F19C0 Script_ParameterTypeRegister.
//   call_index() over the image reports every one of the ten call sites as a real call; there is no address-taken-only symbol in this body, and no push here
//   is left on the stack for a later callee, so the deferred-push rule does not apply to this routine.
// Behavior/CFG: no loop, no conditional branch - one straight-line block of five repetitions. Each repetition reads one argument-less dword getter, registers
//   a script parameter type under a fixed name with the getter's value as the template index, and stores the returned type id into a dword global. Nothing is
//   tested, so the order of the five registrations is the entire contract of this routine.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the disassembly above; runtime UNOBSERVED. Recorded limits: (a) the name is the blessed
//   address-suffixed placeholder; the `Enum*` strings name the registered parameter types, not this routine. (b) 0x0054E154 and 0x0054E15C are defined here
//   because no other file defines them; each definition carries 0xFFFFFFFF, the value the image holds at that address (verified: bytes ff ff ff ff).
//   0x0054E160, 0x0054E150 and 0x0054E158 are already defined by sys/native_4f1e00.cpp, sys/native_4f1e10.cpp and sys/native_4f1e20.cpp respectively, so they
//   are declared extern here to keep one definition per symbol; those three definitions carry no initializer, i.e. 0, while the image holds 0xFFFFFFFF - a
//   discrepancy in those files, not in this one, and it is reported rather than silently mirrored. (c) The `name` argument is a pointer in retail and is
//   declared `std::uint32_t` by the tree's own transcription of 0x004F19C0, so the literal's address is passed as that integer; the string bytes are identical
//   but the address differs from retail's 0x0054E1Ax, which is the established convention for string literals whose retail address points into static data.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

// 0x0054E160/0x0054E150/0x0054E158: already defined by the sibling files, so declared here and not redefined.
extern std::uint32_t g_54e160;
extern std::uint32_t g_54e150;
extern std::uint32_t g_54e158;

// 0x0054E154 and 0x0054E15C: written by this routine and defined by no other file. Each carries the image's value 0xFFFFFFFF.
std::uint32_t g_54e154 = 0xFFFFFFFFU;
std::uint32_t g_54e15c = 0xFFFFFFFFU;

extern "C" {

// The five getters are argument-less dword getters defined inside `extern "C"` in their own translation units; they have no headers.
std::uint32_t NATIVE_DECL Core_GetRenderWordA_4F0FA0(void);   // 0x004F0FA0 (core/native_4f0fa0.cpp)
std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F0F50(void);       // 0x004F0F50 (sys/native_4f0f50.cpp)
std::uint32_t NATIVE_DECL Core_GetRenderWordB_4F0FD0(void);   // 0x004F0FD0 (core/native_4f0fd0.cpp)
std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F0FE0(void);       // 0x004F0FE0 (sys/native_4f0fe0.cpp)
std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F0FF0(void);       // 0x004F0FF0 (sys/native_4f0ff0.cpp)

// 0x004F19C0: registers a script parameter type. Declared exactly as the tree's transcription of it defines it (script/native_4f19c0.cpp, inside `extern "C"`).
std::int32_t Script_ParameterTypeRegister(std::uint32_t const name, std::int32_t const template_index);

void NATIVE_DECL AIGraph_InitStep_4F1E30()
{
    // 0x004F1E30-0x004F1E40: register "EnumInt32" with the value of 0x004F0FA0 as its template index, store the id at 0x0054E160.
    const std::uint32_t int32_word = Core_GetRenderWordA_4F0FA0();
    ::g_54e160 = static_cast<std::uint32_t>(
        Script_ParameterTypeRegister(reinterpret_cast<std::uint32_t>("EnumInt32"), static_cast<std::int32_t>(int32_word)));

    // 0x004F1E45-0x004F1E55: register "EnumReal32" with the value of 0x004F0F50, store the id at 0x0054E150.
    const std::uint32_t real32_word = Sys_GetGlobal32_4F0F50();
    ::g_54e150 = static_cast<std::uint32_t>(
        Script_ParameterTypeRegister(reinterpret_cast<std::uint32_t>("EnumReal32"), static_cast<std::int32_t>(real32_word)));

    // 0x004F1E5A-0x004F1E6A: register "EnumString16" with the value of 0x004F0FD0, store the id at 0x0054E154.
    const std::uint32_t string16_word = Core_GetRenderWordB_4F0FD0();
    ::g_54e154 = static_cast<std::uint32_t>(
        Script_ParameterTypeRegister(reinterpret_cast<std::uint32_t>("EnumString16"), static_cast<std::int32_t>(string16_word)));

    // 0x004F1E6F-0x004F1E7F: register "EnumString32" with the value of 0x004F0FE0, store the id at 0x0054E158.
    const std::uint32_t string32_word = Sys_GetGlobal32_4F0FE0();
    ::g_54e158 = static_cast<std::uint32_t>(
        Script_ParameterTypeRegister(reinterpret_cast<std::uint32_t>("EnumString32"), static_cast<std::int32_t>(string32_word)));

    // 0x004F1E84-0x004F1E97: register "EnumString256" with the value of 0x004F0FF0, store the id at 0x0054E15C.
    const std::uint32_t string256_word = Sys_GetGlobal32_4F0FF0();
    ::g_54e15c = static_cast<std::uint32_t>(
        Script_ParameterTypeRegister(reinterpret_cast<std::uint32_t>("EnumString256"), static_cast<std::int32_t>(string256_word)));
}

} // extern "C"

#undef NATIVE_DECL
