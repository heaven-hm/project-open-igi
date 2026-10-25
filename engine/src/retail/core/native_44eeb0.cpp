// Native: Task_RegisterAIEvents_44EEB0 (behavior-derived placeholder name: it registers the AI event, action-flag, detection-range and AI-type names into the script registry; retail symbol unknown); retail VA 0x0044EEB0-0x0044F050.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 417 bytes (0x1A1), 3 blocks, 3 edges, 111 instructions, 31 call sites, counted per block with
//   af + afbj over the exact image; the twelve 0x90 bytes at 0x0044F051-0x0044F05C are alignment padding before the next function at 0x0044F060.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf and the objdump listing build/retail-dis.txt over the same extent. `psz` at all 59 name addresses and at
//   every dword of the 29-entry table at 0x0053C9C0 gives the byte-identical names below. A push-by-push simulation closes EXACTLY with no frame:
//   256 bytes of pushes minus 8 bytes of saved-register pops equals the 0xF8 total of the four `add esp` cleanups (0x40 x3, 0x20, the 0x8 inside the
//   loop x29, and 0x10). The single callee 0x004B8810 is Script_QHashNodeRegister, already transcribed in script/native_4b8810.cpp, and the argument
//   order here is (name, value) because retail pushes the value first and the name second.
// Behavior/CFG: one straight-line block of 92 instructions that performs 28 registrations with explicit ids (0x0044EEB0-0x0044F011), then a
//   do-while loop over the 29-entry name table at 0x0053C9C0 whose body registers each entry with the loop counter as its value and whose test
//   `cmp esi,0x53CA34; jl` is at the BOTTOM (0x0044F014-0x0044F031, self-loop back edge to 0x0044F01B), then two more registrations and the
//   epilogue (0x0044F033-0x0044F050). No argument, no frame, no stack probe, no return value (eax is the callee's and is discarded).
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The table at 0x0053C9C0 holds image pointers to `.data` strings; the port holds its own pointers to
//   byte-identical literals, which is the same substitution core/native_45e450.cpp makes for 0x0053F348. The meaning of each id is NOT established -
//   the ids are transcribed as the literals retail pushes (including 0x1E000 and 0x1000 for the two detection ranges).
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

// 0x004B8810: Script_QHashNodeRegister, defined by script/native_4b8810.cpp. Retail pushes the value first and the name second, so the name is
// argument 1 and the value is argument 2.
void Script_QHashNodeRegister(void* arg1, void* value);

// 0x0053C9C0: the 29-entry name table the loop at 0x0044F01B walks, stride 4, ending at 0x0053CA34. The image holds pointers to `.data` strings; the
// port holds pointers to byte-identical literals. Nothing else in the tree owns this object yet.
char const* g_53c9c0[29] = {
    "AITYPE_RPG",
    "AITYPE_GUNNER",
    "AITYPE_SNIPER",
    "AITYPE_ANYA",
    "AITYPE_EKK",
    "AITYPE_PRIBOI",
    "AITYPE_CIVILIAN",
    "AITYPE_PATROL_UZI",
    "AITYPE_PATROL_AK",
    "AITYPE_PATROL_SPAS",
    "AITYPE_PATROL_PISTOL",
    "AITYPE_GUARD_UZI",
    "AITYPE_GUARD_AK",
    "AITYPE_GUARD_SPAS",
    "AITYPE_GUARD_PISTOL",
    "AITYPE_SECURITY_PATROL_UZI",
    "AITYPE_SECURITY_PATROL_SPAS",
    "AITYPE_MAFIA_PATROL_UZI",
    "AITYPE_MAFIA_PATROL_AK",
    "AITYPE_MAFIA_PATROL_SPAS",
    "AITYPE_MAFIA_GUARD_UZI",
    "AITYPE_MAFIA_GUARD_AK",
    "AITYPE_MAFIA_GUARD_SPAS",
    "AITYPE_SPETNAZ_PATROL_UZI",
    "AITYPE_SPETNAZ_PATROL_AK",
    "AITYPE_SPETNAZ_PATROL_SPAS",
    "AITYPE_SPETNAZ_GUARD_UZI",
    "AITYPE_SPETNAZ_GUARD_AK",
    "AITYPE_SPETNAZ_GUARD_SPAS",
};

void NATIVE_DECL Task_RegisterAIEvents_44EEB0(void)
{
    NATIVE_LOG_ENTER("Task_RegisterAIEvents_44EEB0", "0x0044EEB0");
    // 0x0044EEB2-0x0044EEB9: ids 0x00-0x07, one registration per (value, name) push pair; 0x0044EF12: add esp,0x40.
    Script_QHashNodeRegister(const_cast<char*>("AIEVENT_CREATE"),                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x0U)));
    Script_QHashNodeRegister(const_cast<char*>("AIEVENT_DELETE"),                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1U)));
    Script_QHashNodeRegister(const_cast<char*>("AIEVENT_DEAD"),                               reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x2U)));
    Script_QHashNodeRegister(const_cast<char*>("AIEVENT_ANIMATION"),                          reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x3U)));
    Script_QHashNodeRegister(const_cast<char*>("AIEVENT_IDLE"),                               reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x4U)));
    Script_QHashNodeRegister(const_cast<char*>("AIEVENT_ALERT"),                              reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x5U)));
    Script_QHashNodeRegister(const_cast<char*>("AIEVENT_ALERT_RESPONSE"),                     reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x6U)));
    Script_QHashNodeRegister(const_cast<char*>("AIEVENT_COMBAT"),                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x7U)));
    // 0x0044EF15-0x0044EF1C: ids 0x08-0x0F; 0x0044EF75: add esp,0x40.
    Script_QHashNodeRegister(const_cast<char*>("AIEVENT_ALARMON"),                            reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x8U)));
    Script_QHashNodeRegister(const_cast<char*>("AIEVENT_ALARMOFF"),                           reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x9U)));
    Script_QHashNodeRegister(const_cast<char*>("AIEVENT_WALK"),                               reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xAU)));
    Script_QHashNodeRegister(const_cast<char*>("AIEVENT_GROUNDIMPACT"),                       reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xBU)));
    Script_QHashNodeRegister(const_cast<char*>("AIEVENT_DOOR"),                               reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xCU)));
    Script_QHashNodeRegister(const_cast<char*>("AIEVENT_FENCE"),                              reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xDU)));
    Script_QHashNodeRegister(const_cast<char*>("AIEVENT_LADDER"),                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xEU)));
    Script_QHashNodeRegister(const_cast<char*>("AIEVENT_TAKINGDAMAGE"),                       reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xFU)));
    // 0x0044EF78-0x0044EF7F: ids 0x10-0x17; 0x0044EFD8: add esp,0x40.
    Script_QHashNodeRegister(const_cast<char*>("AIEVENT_GUNSHOT"),                            reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x10U)));
    Script_QHashNodeRegister(const_cast<char*>("AIEVENT_GRENADETHROWN"),                      reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x11U)));
    Script_QHashNodeRegister(const_cast<char*>("AIEVENT_GRENADELAND"),                        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x12U)));
    Script_QHashNodeRegister(const_cast<char*>("AIEVENT_FLASHBANG"),                          reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x13U)));
    Script_QHashNodeRegister(const_cast<char*>("AIEVENT_GUNSHOTMISS"),                        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x14U)));
    Script_QHashNodeRegister(const_cast<char*>("AIEVENT_EXPLOSION"),                          reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x15U)));
    Script_QHashNodeRegister(const_cast<char*>("AIEVENT_ENEMYDETECTION"),                     reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x16U)));
    Script_QHashNodeRegister(const_cast<char*>("AIEVENT_FRIENDLYDETECTION"),                  reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x17U)));
    // 0x0044EFDB-0x0044EFFD: two action flags and the two detection ranges; 0x0044F011: add esp,0x20.
    Script_QHashNodeRegister(const_cast<char*>("AIACTIONFLAG_NONE"),                          reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x0U)));
    Script_QHashNodeRegister(const_cast<char*>("AIACTIONFLAG_PUSHABLE"),                      reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1U)));
    Script_QHashNodeRegister(const_cast<char*>("HUMANAI_DETECTIONEVENT_GUNSHOT_RANGE"),       reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1E000U)));
    Script_QHashNodeRegister(const_cast<char*>("HUMANAI_DETECTIONEVENT_GUNSHOT_SILENCED_RANGE"),  reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1000U)));

    // 0x0044F014: xor edi,edi; 0x0044F016: mov esi,0x0053C9C0 (loop counter and table cursor).
    std::uint32_t index = 0;
    char const* const* entry = &::g_53c9c0[0];
    // 0x0044F01B: the loop head; 0x0044F024: add esp,8 (the two pushes of this call); 0x0044F027/0x0044F02A: cursor and counter advance;
    // 0x0044F02B/0x0044F031: cmp esi,0x0053CA34; jl - the test is at the bottom, so the body runs at least once.
    do
    {
        Script_QHashNodeRegister(const_cast<char*>(*entry), reinterpret_cast<void*>(static_cast<std::uintptr_t>(index))); // 0x0044F01F: call 0x4B8810
        entry += 1;                                                              // 0x0044F027: add $0x4,%esi
        index += 1;                                                              // 0x0044F02A: inc %edi
    } while (entry < &::g_53c9c0[29]);                                           // 0x0044F031: jl 0x0044F01B
    // 0x0044F033-0x0044F046: the two AIALARMACCESS_ names; 0x0044F04B: add esp,0x10; 0x0044F04E/0x0044F04F: pop edi/pop esi.
    Script_QHashNodeRegister(const_cast<char*>("AIALARMACCESS_BEFORECOMBAT"),                 reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x0U)));
    Script_QHashNodeRegister(const_cast<char*>("AIALARMACCESS_AFTERCOMBAT"),                  reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1U)));
}

} // extern "C"

#undef NATIVE_DECL
