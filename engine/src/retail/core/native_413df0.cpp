// Native: Task_DefineWeaponType_413DF0 (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00413DF0-0x00413F87 (408 bytes, 1 block, 0 edges, 99 instructions).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: build/retail-dis.txt (GNU objdump) instruction listing 0x00413DF0-0x00413F87 (6a00 e8c9d9feff 6a00 a2ac915300 e8bdd9feff 6a00 a2ad915300 ... 83c418 c3); radare2 6.2.0 af/afij (408 B, 1 block, 99 instructions, 0 edges - the function has no branch of its own); the 21 literals read byte-for-byte at 0x005393BC, 0x005393AC, 0x00539398, 0x00539384, 0x00539370, 0x0053935C, 0x00539344, 0x00539330, 0x00539314, 0x005392FC, 0x005392E0, 0x005392C8, 0x005392B0, 0x00539298, 0x00539280, 0x00539268, 0x00539250, 0x00539240, 0x0053922C, 0x00539218 and 0x00539208; image values read with px: the seven event bytes at 0x005391AC are FF FF FF FF FF FF 00 and the four name-space slots at 0x0056E218-0x0056E257 are zero; the guard's call site 0x004029AD is this function's only caller (one `call 0x413df0` in the listing) and pushes nothing.
// Behavior/CFG: one basic block, no branch of its own. Allocates seven events through 0x004017C0 and stores each result's AL at 0x005391AC..0x005391B2, registers the "DefineWeaponType" script command (handler 0x00413F90, null slot, type word 0xFFFFFFFF) through 0x004B8890, registers the sixteen weapon/sight/ammo display type names through 0x004B8810 in the id order retail pushes (WEAPONTYPE_GUN..PROXIMITYMINE 0..7, SIGHTDISPLAYTYPE_NONE/CROSSHAIR 0..1, AMMODISPLAYTYPE_NONE..GRENADE 0..5), then for each of the four name spaces at 0x0056E248, 0x0056E238, 0x0056E218 and 0x0056E228 initialises the list head through 0x004F1CD0 and creates that space's pattern record through 0x004F1DB0 ("WEAPONTYPE_.*", "SIGHTDISPLAYTYPE_.*", "AMMODISPLAYTYPE_.*", "WEAPON_ID_.*").
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 99-instruction listing and afij (408 B / 1 block / 0 edges); runtime UNOBSERVED. Neither list callee is a gap: 0x004F1CD0 is the already transcribed Sys_InitGlobalListHead_4F1CD0 (engine/src/retail/sys/native_4f1cd0.cpp) and 0x004F1DB0 is Sys_NewListRecord_4F1DB0 (engine/src/retail/sys/native_4f1db0.cpp); both are referenced by name and neither is duplicated here (one implementation per retail VA). 0x004F1DB0's own callee 0x004AF9D0 is still untranscribed and is declared as a gap in that file. The handler 0x00413F90 is a raw code immediate in retail and is not transcribed yet. The seven event bytes at 0x005391AC..0x005391B2 are also read one at a time by the native at 0x00413B00-0x00413B60, so they are declared here as one seven-byte object with the image values and must be reused by name when that native is transcribed. Retail schedules each event-id store after the following push (for example `push $0x0` at 0x00413DF7 precedes `mov %al,0x5391AC` at 0x00413DF9); the port stores each result next to its own call, which changes no value and no order any other code can observe. Retail never cleans the individual calls: its four `add $...,%esp` instructions (0x00413E7B 0x44, 0x00413EDE 0x40, 0x00413F4F 0x40, 0x00413F84 0x18) cover exactly the dwords pushed, and the compiler's own argument cleanups replace them.
#include <cstdint>

#include "igi/retail/core/task_types.hpp"

// task_types.hpp defines NATIVE_DECL for its own declarations and #undefs it at its end (line 209), so this native restates the
// same guarded definition before its own use.
#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" void Script_QHashNodeRegister(void* arg1, void* value); // 0x004B8810 (script/native_4b8810.cpp)
extern "C" void Script_QHashNodeRegisterEx(void* arg0, void* arg1, char const* arg2, void* arg3); // 0x004B8890 (script/native_4b8890.cpp)

// 0x004F1CD0: already transcribed as Sys_InitGlobalListHead_4F1CD0 in engine/src/retail/sys/native_4f1cd0.cpp. Referenced by
// name here, never duplicated (one implementation per retail VA). Its body pushes the 0x004F1CF0 context and tail-calls 0x004F1D60.
extern "C" void Sys_InitGlobalListHead_4F1CD0(void* head);

// 0x004F1DB0: transcribed in engine/src/retail/sys/native_4f1db0.cpp as Sys_NewListRecord_4F1DB0 - the same name and signature
// engine/src/retail/core/native_47cf20.cpp already declares for it. It allocates 0x480C bytes through 0x004B0C60, stores the
// pattern at +0x4808, clears +0x04/+0x00, builds +0x08 and +0x2808 through 0x004AF9D0 and links the record into its first argument
// through 0x004AF8F0.
extern "C" void Sys_NewListRecord_4F1DB0(void* head, char const* pattern);

extern "C" {

// Retail 0x005391AC-0x005391B2: the seven event ids this registrar allocates, one byte each (image values FF FF FF FF FF FF 00).
std::uint8_t g_5391ac[7] = {0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0x00U};

// Retail 0x0056E248 / 0x0056E238 / 0x0056E218 / 0x0056E228: four name-space objects of 0x10 bytes each, all zero in the image.
// 0x0056E258, the next address any code references, is 0x10 past 0x0056E248, which fixes the stride. Declared as four separate
// objects so that each address keeps its own owner.
std::uint8_t g_56e248[0x10] = {};
std::uint8_t g_56e238[0x10] = {};
std::uint8_t g_56e218[0x10] = {};
std::uint8_t g_56e228[0x10] = {};

void NATIVE_DECL Task_DefineWeaponType_413DF0()
{
    // 0x00413DF0-0x00413E3A: seven `push $0x0; call 0x004017C0` pairs. Each result is stored as a byte at 0x005391AC+i; retail
    // places each store after the next push, and the trailing comment on each line gives that store's address.
    g_5391ac[0] = static_cast<std::uint8_t>(QTask_AllocateEvent(0U)); // 0x00413DF9: mov %al,0x005391AC
    g_5391ac[1] = static_cast<std::uint8_t>(QTask_AllocateEvent(0U)); // 0x00413E05: mov %al,0x005391AD
    g_5391ac[2] = static_cast<std::uint8_t>(QTask_AllocateEvent(0U)); // 0x00413E11: mov %al,0x005391AE
    g_5391ac[3] = static_cast<std::uint8_t>(QTask_AllocateEvent(0U)); // 0x00413E1D: mov %al,0x005391AF
    g_5391ac[4] = static_cast<std::uint8_t>(QTask_AllocateEvent(0U)); // 0x00413E29: mov %al,0x005391B0
    g_5391ac[5] = static_cast<std::uint8_t>(QTask_AllocateEvent(0U)); // 0x00413E35: mov %al,0x005391B1
    g_5391ac[6] = static_cast<std::uint8_t>(QTask_AllocateEvent(0U)); // 0x00413E4D: mov %al,0x005391B2 (retail stores this one after the four pushes below)

    // 0x00413E3F: push $0xffffffff; 0x00413E41: push $0x0; 0x00413E43: push 0x00413F90; 0x00413E48: push 0x005393BC;
    // 0x00413E52: call 0x004B8890.
    Script_QHashNodeRegisterEx(                                          // 0x00413E52: call 0x004B8890
        const_cast<char*>("DefineWeaponType"),                            // 0x00413E48: push 0x005393BC (the command name)
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00413F90U)), // 0x00413E43: push 0x00413F90 (handler)
        nullptr,                                                          // 0x00413E41: push 0x0 (the name slot)
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xFFFFFFFFU))); // 0x00413E3F: push 0xFFFFFFFF (the type word)

    // 0x00413E57-0x00413ED9: sixteen `push <id>; push <name>; call 0x004B8810` triples; 0x00413E7B: add $0x44,%esp is the late
    // cleanup of the seven event pushes, the four pushes above and the first three triples (17 dwords), and 0x00413EDE: add $0x40,%esp
    // cleans the eight triples that follow it.
    Script_QHashNodeRegister(const_cast<char*>("WEAPONTYPE_GUN"),                        // 0x00413E59: push 0x005393AC
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // 0x00413E57: push 0x0 (id)
    Script_QHashNodeRegister(const_cast<char*>("WEAPONTYPE_PISTOL"),                     // 0x00413E65: push 0x00539398
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // 0x00413E63: push 0x1
    Script_QHashNodeRegister(const_cast<char*>("WEAPONTYPE_SHOTGUN"),                    // 0x00413E71: push 0x00539384
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(2U))); // 0x00413E6F: push 0x2
    Script_QHashNodeRegister(const_cast<char*>("WEAPONTYPE_KNIFE"),                      // 0x00413E80: push 0x00539370
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(3U))); // 0x00413E7E: push 0x3
    Script_QHashNodeRegister(const_cast<char*>("WEAPONTYPE_GRENADE"),                    // 0x00413E8C: push 0x0053935C
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(4U))); // 0x00413E8A: push 0x4
    Script_QHashNodeRegister(const_cast<char*>("WEAPONTYPE_BINOCULAR"),                  // 0x00413E98: push 0x00539344
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(5U))); // 0x00413E96: push 0x5
    Script_QHashNodeRegister(const_cast<char*>("WEAPONTYPE_MEDIPACK"),                   // 0x00413EA4: push 0x00539330
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(6U))); // 0x00413EA2: push 0x6
    Script_QHashNodeRegister(const_cast<char*>("WEAPONTYPE_PROXIMITYMINE"),              // 0x00413EB0: push 0x00539314
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(7U))); // 0x00413EAE: push 0x7
    Script_QHashNodeRegister(const_cast<char*>("SIGHTDISPLAYTYPE_NONE"),                 // 0x00413EBC: push 0x005392FC
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // 0x00413EBA: push 0x0
    Script_QHashNodeRegister(const_cast<char*>("SIGHTDISPLAYTYPE_CROSSHAIR"),            // 0x00413EC8: push 0x005392E0
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // 0x00413EC6: push 0x1
    Script_QHashNodeRegister(const_cast<char*>("AMMODISPLAYTYPE_NONE"),                  // 0x00413ED4: push 0x005392C8
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // 0x00413ED2: push 0x0
    Script_QHashNodeRegister(const_cast<char*>("AMMODISPLAYTYPE_NORMAL"),                // 0x00413EE3: push 0x005392B0
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // 0x00413EE1: push 0x1
    Script_QHashNodeRegister(const_cast<char*>("AMMODISPLAYTYPE_CLIP"),                  // 0x00413EEF: push 0x00539298
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(2U))); // 0x00413EED: push 0x2
    Script_QHashNodeRegister(const_cast<char*>("AMMODISPLAYTYPE_BARREL"),                // 0x00413EFB: push 0x00539280
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(3U))); // 0x00413EF9: push 0x3
    Script_QHashNodeRegister(const_cast<char*>("AMMODISPLAYTYPE_SHELL"),                 // 0x00413F07: push 0x00539268
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(4U))); // 0x00413F05: push 0x4
    Script_QHashNodeRegister(const_cast<char*>("AMMODISPLAYTYPE_GRENADE"),               // 0x00413F13: push 0x00539250
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(5U))); // 0x00413F11: push 0x5

    // 0x00413F1D-0x00413F4A: the first two name spaces. 0x00413F4F: add $0x40,%esp cleans the five triples above, this pair's
    // two calls and the second pair's two calls (16 dwords).
    Sys_InitGlobalListHead_4F1CD0(g_56e248); // 0x00413F1D: push $0x0056E248; 0x00413F22: call 0x004F1CD0
    Sys_NewListRecord_4F1DB0(g_56e248,      // 0x00413F2C: push $0x0056E248
                                      "WEAPONTYPE_.*"); // 0x00413F27: push 0x00539240; 0x00413F31: call 0x004F1DB0
    Sys_InitGlobalListHead_4F1CD0(g_56e238); // 0x00413F36: push $0x0056E238; 0x00413F3B: call 0x004F1CD0
    Sys_NewListRecord_4F1DB0(g_56e238,      // 0x00413F45: push $0x0056E238
                                      "SIGHTDISPLAYTYPE_.*"); // 0x00413F40: push 0x0053922C; 0x00413F4A: call 0x004F1DB0

    // 0x00413F52-0x00413F7F: the last two name spaces. 0x00413F84: add $0x18,%esp cleans these two pairs' six dwords.
    Sys_InitGlobalListHead_4F1CD0(g_56e218); // 0x00413F52: push $0x0056E218; 0x00413F57: call 0x004F1CD0
    Sys_NewListRecord_4F1DB0(g_56e218,      // 0x00413F61: push $0x0056E218
                                      "AMMODISPLAYTYPE_.*"); // 0x00413F5C: push 0x00539218; 0x00413F66: call 0x004F1DB0
    Sys_InitGlobalListHead_4F1CD0(g_56e228); // 0x00413F6B: push $0x0056E228; 0x00413F70: call 0x004F1CD0
    Sys_NewListRecord_4F1DB0(g_56e228,      // 0x00413F7A: push $0x0056E228
                                      "WEAPON_ID_.*"); // 0x00413F75: push 0x00539208; 0x00413F7F: call 0x004F1DB0

    // 0x00413F87: ret - the last call's return value is left in eax and no caller reads it (0x004029AD is the only call site).
}

} // extern "C"
