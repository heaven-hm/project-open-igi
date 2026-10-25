// Native: Config_RegisterGameOptions_402FF0 (behavior-derived placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00402FF0-0x00403657 (1639 bytes, 15 blocks, 21 edges, 449 instructions); called from the startup guard 0x00402930 at 0x00402944 (position 5 of its 155 ordered calls).
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x00402FF0-0x00403657; radare2 6.2.0 af/afbj per block (block 0x00402FF0 1375B/367i, then 0x0040354F 24B/8i, 0x00403567 5B/1i, 0x0040356C 24B/8i, 0x00403584 5B/1i, 0x00403589 24B/8i, 0x004035A1 7B/2i, 0x004035A8 22B/8i, 0x004035BE 7B/2i, 0x004035C5 22B/8i, 0x004035DB 35B/11i, 0x004035FE 10B/3i, 0x00403608 36B/9i, 0x0040362C 1B/1i, 0x0040362D 42B/12i); all 69 name literals and the five table images read as raw bytes from the image at their retail VAs; esp simulated instruction by instruction to the ret, which is how the shared 8 push at 0x00403632 and the late 0x004035F2 cleanup were resolved.
// Behavior/CFG: registers 69 script entry points through 0x004B8890 (and 0x004B88F0 for one of them) with the tuple (name, handler, 0, type) - 68 name/handler pairs plus "DefineGameOptions" with a null handler - in 18 cleanup groups (17 x 0x40 and one 0x10), then walks five retail tables of {name, id} records and registers each through 0x004B8810: 27 records at 0x00536558, 3 at 0x00536928, 127 at 0x00536998, then 6 records at 0x0053331C and 3 at 0x0053337C whose second argument is the loop counter rather than a table field. It then leaves the render mode with 0x004950E0/0x004B0FD0, dispatches 0x00491EA0(frame, 5) and stores its result at 0x00567C90, copies that many 0x100-byte records from the frame into the 0x00567C98 table (stride 0x404, zeroing the dword at +0x400 of each), calls 0x00491CF0 then 0x00491980, enters the render mode with 0x004B0F90 and clears the first byte of the 0x00BC79C8 password buffer.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 449-instruction listing and the per-block afbj counts; runtime UNOBSERVED. Four callees are native gaps outside this lane's range and are declared here, not transcribed: 0x004B88F0 (used for exactly one registration, "Config_GraphicOptionsSetGamma"), 0x00491EA0, 0x00491CF0 and 0x00491980, so this file cannot link until they land. Retail never cleans the single argument it pushes for 0x004B0FD0 and reuses it as the second argument of 0x00491980; the port passes 8 to both calls, which is the same stack, and the compiler's own cleanup replaces the late 0x004035F2 add. The destination table at 0x00567C98 is declared with five 0x404-byte records because the copy source is this routine's 0x500-byte frame and 0x00491EA0 is handed 5 as its second argument, so no run can copy more than five records without reading past the frame; retail itself has no bound check here.
#include <cstdint>
#include <cstring>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

// One retail script-command record: a NUL-terminated name in the first 0x20 bytes and the command id at +0x20, stride 0x24.
struct ScriptCommandRecord
{
    char name[0x20];
    std::uint32_t id;
};

// One retail language record: a NUL-terminated name in 0x10 bytes, stride 0x10. The id these loops register is the loop counter.
struct ScriptLanguageRecord
{
    char name[0x10];
};

// Retail 0x00536558-0x00536924: the 27 script-command records of the first loop, read byte for byte from the image.
// Defined with internal linkage because no other transcribed native reads this region yet; if one does, this definition
// must become the single owner for the VA.
ScriptCommandRecord const g_536558[27] = {
    {"MoveLeft", 1U},
    {"MoveRight", 2U},
    {"MoveUp", 4U},
    {"MoveDown", 8U},
    {"Fire", 256U},
    {"AlternateFire", 16384U},
    {"Reload", 512U},
    {"Activate", 1024U},
    {"Crouch", 2048U},
    {"Jump", 4096U},
    {"NextWeapon", 8192U},
    {"PrevWeapon", 524288U},
    {"Peek", 32768U},
    {"Binoculars", 65536U},
    {"MapComputer", 131072U},
    {"ToggleWalkRun", 262144U},
    {"ZoomIn", 1048576U},
    {"ZoomOut", 2097152U},
    {"WeaponCategory1", 4194304U},
    {"WeaponCategory2", 8388608U},
    {"WeaponCategory3", 16777216U},
    {"WeaponCategory4", 33554432U},
    {"WeaponCategory5", 67108864U},
    {"WeaponCategory6", 134217728U},
    {"WeaponCategory7", 268435456U},
    {"WeaponCategory8", 536870912U},
    {"WeaponCategory9", 1073741824U},
};

// Retail 0x00536928-0x00536994: the 3 device records of the second loop.
ScriptCommandRecord const g_536928[3] = {
    {"INPUTPORT_DEVICE_KEYBOARD", 1U},
    {"INPUTPORT_DEVICE_JOYPAD", 2U},
    {"INPUTPORT_DEVICE_MOUSE", 3U},
};

// Retail 0x00536998-0x00537B74: the 127 key records of the third loop.
ScriptCommandRecord const g_536998[127] = {
    {"KEY_ANGLE", 1073741910U},
    {"KEY_ESC", 1073741825U},
    {"KEY_A", 1073741854U},
    {"KEY_B", 1073741872U},
    {"KEY_C", 1073741870U},
    {"KEY_D", 1073741856U},
    {"KEY_E", 1073741842U},
    {"KEY_F", 1073741857U},
    {"KEY_G", 1073741858U},
    {"KEY_H", 1073741859U},
    {"KEY_I", 1073741847U},
    {"KEY_J", 1073741860U},
    {"KEY_K", 1073741861U},
    {"KEY_L", 1073741862U},
    {"KEY_M", 1073741874U},
    {"KEY_N", 1073741873U},
    {"KEY_O", 1073741848U},
    {"KEY_P", 1073741849U},
    {"KEY_Q", 1073741840U},
    {"KEY_R", 1073741843U},
    {"KEY_S", 1073741855U},
    {"KEY_T", 1073741844U},
    {"KEY_U", 1073741846U},
    {"KEY_V", 1073741871U},
    {"KEY_W", 1073741841U},
    {"KEY_X", 1073741869U},
    {"KEY_Y", 1073741845U},
    {"KEY_Z", 1073741868U},
    {"KEY_0", 1073741835U},
    {"KEY_1", 1073741826U},
    {"KEY_2", 1073741827U},
    {"KEY_3", 1073741828U},
    {"KEY_4", 1073741829U},
    {"KEY_5", 1073741830U},
    {"KEY_6", 1073741831U},
    {"KEY_7", 1073741832U},
    {"KEY_8", 1073741833U},
    {"KEY_9", 1073741834U},
    {"KEY_SEPARATOR", 1073741876U},
    {"KEY_DECIMAL", 1073741907U},
    {"KEY_COMMA", 1073741875U},
    {"KEY_COLON", 1073741970U},
    {"KEY_SLASH", 1073741877U},
    {"KEY_CAPITAL", 1073741882U},
    {"KEY_BACKSLASH", 1073741867U},
    {"KEY_BACKSPACE", 1073741838U},
    {"KEY_UNDERLINE", 1073741971U},
    {"KEY_CAPSLOCK", 1073741882U},
    {"KEY_NUMLOCK", 1073741893U},
    {"KEY_SCROLL", 1073741894U},
    {"KEY_TAB", 1073741839U},
    {"KEY_RETURN", 1073741852U},
    {"KEY_LEFT_SHIFT", 1073741866U},
    {"KEY_RIGHT_SHIFT", 1073741878U},
    {"KEY_LEFT_CTRL", 1073741853U},
    {"KEY_RIGHT_CTRL", 1073741981U},
    {"KEY_LEFT_ALT", 1073741880U},
    {"KEY_RIGHT_ALT", 1073742008U},
    {"KEY_DIVIDE", 1073742005U},
    {"KEY_MULTIPLY", 1073741879U},
    {"KEY_SUBTRACT", 1073741898U},
    {"KEY_ADD", 1073741902U},
    {"KEY_MINUS", 1073741836U},
    {"KEY_EQUALS", 1073741837U},
    {"KEY_LEFT_BRACKET", 1073741850U},
    {"KEY_RIGHT_BRACKET", 1073741851U},
    {"KEY_SEMICOLON", 1073741863U},
    {"KEY_APOSTROPHE", 1073741864U},
    {"KEY_GRAVE", 1073741865U},
    {"KEY_LEFT_MENU", 1073741880U},
    {"KEY_RIGHT_MENU", 1073742008U},
    {"KEY_LEFT_WIN", 1073742043U},
    {"KEY_RIGHT_WIN", 1073742044U},
    {"KEY_SPACE", 1073741881U},
    {"KEY_END", 1073742031U},
    {"KEY_HOME", 1073742023U},
    {"KEY_LEFT", 1073742027U},
    {"KEY_UP", 1073742024U},
    {"KEY_RIGHT", 1073742029U},
    {"KEY_DOWN", 1073742032U},
    {"KEY_INSERT", 1073742034U},
    {"KEY_DELETE", 1073742035U},
    {"KEY_PAGEUP", 1073742025U},
    {"KEY_PAGEDOWN", 1073742033U},
    {"KEY_F1", 1073741883U},
    {"KEY_F2", 1073741884U},
    {"KEY_F3", 1073741885U},
    {"KEY_F4", 1073741886U},
    {"KEY_F5", 1073741887U},
    {"KEY_F6", 1073741888U},
    {"KEY_F7", 1073741889U},
    {"KEY_F8", 1073741890U},
    {"KEY_F9", 1073741891U},
    {"KEY_F10", 1073741892U},
    {"KEY_F11", 1073741911U},
    {"KEY_F12", 1073741912U},
    {"KEY_F13", 1073741924U},
    {"KEY_F14", 1073741925U},
    {"KEY_F15", 1073741926U},
    {"KEY_NUMPAD0", 1073741906U},
    {"KEY_NUMPAD1", 1073741903U},
    {"KEY_NUMPAD2", 1073741904U},
    {"KEY_NUMPAD3", 1073741905U},
    {"KEY_NUMPAD4", 1073741899U},
    {"KEY_NUMPAD5", 1073741900U},
    {"KEY_NUMPAD6", 1073741901U},
    {"KEY_NUMPAD7", 1073741895U},
    {"KEY_NUMPAD8", 1073741896U},
    {"KEY_NUMPAD9", 1073741897U},
    {"KEY_NUMPADENTER", 1073741980U},
    {"KEY_NUMPADCOMMA", 1073742003U},
    {"KEY_NUMPADEQUALS", 1073741965U},
    {"KEY_NUMPADSTAR", 1073741879U},
    {"KEY_NUMPADMINUS", 1073741898U},
    {"KEY_NUMPADPLUS", 1073741902U},
    {"KEY_NUMPADPERIOD", 1073741907U},
    {"KEY_NUMPADSLASH", 1073742005U},
    {"MOUSE_BUTTON_1", 3221225473U},
    {"MOUSE_BUTTON_2", 3221225474U},
    {"MOUSE_BUTTON_3", 3221225476U},
    {"MOUSE_BUTTON_4", 3221225480U},
    {"MOUSE_BUTTON_5", 3221225488U},
    {"MOUSE_BUTTON_6", 3221225504U},
    {"MOUSE_BUTTON_7", 3221225536U},
    {"MOUSE_BUTTON_8", 3221225600U},
    {"MOUSE_WHEEL_UP", 3221225728U},
    {"MOUSE_WHEEL_DOWN", 3221225984U},
};

// Retail 0x0053331C-0x0053337C: the 6 language records of the fourth loop.
ScriptLanguageRecord const g_53331c[6] = {
    {"ENGLISH"},
    {"GERMAN"},
    {"FRENCH"},
    {"ITALIAN"},
    {"SPANISH"},
    {"USA"},
};

// Retail 0x0053337C-0x005333AC: the 3 records of the fifth loop.
ScriptLanguageRecord const g_53337c[3] = {
    {"GD_1"},
    {"GD_2"},
    {"GD_3"},
};

// Retail 0x00567C98: the display-mode table. Records are 0x404 bytes; other natives index the same base with the same
// stride (0x004043BA, 0x004046A0, 0x004059A5) and read the dword at record+0x400.
std::uint8_t g_567c98[5 * 0x404] = {};

// Retail 0x00567C90: the record count 0x00491EA0 reports and every reader of the table above uses as its bound.
std::int32_t g_567c90 = 0;

// Retail 0x00BC79C8: the 32-byte content-control password buffer, defined in engine/src/retail/config/config_profile.cpp.
extern "C" char Config_ContentControlPassword[0x20];

extern "C" {

// 0x004B8890: script hash-node registration, transcribed in engine/src/retail/script/native_4b8890.cpp (four arguments).
void Script_QHashNodeRegisterEx(void* arg0, void* arg1, char const* arg2, void* arg3);

// 0x004B88F0: NATIVE GAP - not transcribed. Its body is 0x004B8890's with the tail constants (1, 2, 0) instead of
// (1, 0, 0) when it forwards to 0x004BAAC0, so it takes the same four arguments. Declared from the retail push shape.
void Script_QHashNodeRegisterMode2_4B88F0(void* arg0, void* arg1, char const* arg2, void* arg3);

// 0x004B8810: script hash-node registration, transcribed in engine/src/retail/script/native_4b8810.cpp (two arguments).
void Script_QHashNodeRegister(void* arg1, void* value);

// 0x004950E0: transcribed in engine/src/retail/sys/native_4950e0.cpp (no arguments).
std::uint32_t Sys_GetGlobal32_4950E0(void);

// 0x004B0FD0: transcribed in engine/src/retail/sys/native_4b0fd0.cpp (one argument).
void Sys_LeaveRenderMode_4B0FD0(std::int32_t previous_mode);

// 0x004B0F90: transcribed in engine/src/retail/sys/native_4b0f90.cpp (no arguments).
std::int32_t Sys_EnterRenderMode_4B0F90(void);

// 0x00491EA0: transcribed in engine/src/retail/render/native_491ea0.cpp (two arguments). It builds a three-dword descriptor
// {arg1, &local, arg2} on its own stack, calls the driver handler at 0x00A94E84 indexed by the dword at 0x00542334, and
// returns the descriptor's first dword.
std::int32_t Display_DriverDispatch_491EA0(void* buffer, std::int32_t argument2);

// 0x00491CF0: transcribed in engine/src/retail/render/native_491cf0.cpp; the tree's terrain/view code records it as
// "Display_GetActiveMode" returning 0x00C28B40 and never reading its two arguments (engine/src/retail/terrain/terrain_draw.cpp:56).
void* Display_GetActiveMode_491CF0(void* argument1, std::int32_t argument2);

// 0x00491980: transcribed in engine/src/retail/render/native_491980.cpp, and it takes THREE arguments - the third is read at
// 0x004919EC as `mov 0x20(%esp),%eax` (the third argument word, [E+0x0C]) and invoked per matching record. This call site
// pushes only two arguments, so retail's third argument here is a stale stack word - the saved EDI slot of 0x00402FF7 - which
// the callee's own null check at 0x004919F2 skips; the port passes nullptr for it, recorded at the call site below.
std::int32_t Display_ApplyFlags_491980(void* argument1, std::int32_t argument2, void* argument3);

void NATIVE_DECL Config_RegisterGameOptions_402FF0(void)
{
    // 0x00402FF0: sub $0x500,%esp - retail's 1280-byte frame, which 0x00491EA0 fills and the tail loop reads from its
    // base. Kept as twenty 64-byte chunks of scalars rather than a byte array: an address-taken array makes the host
    // compiler emit a __stack_chk_guard/__stack_chk_fail pair that retail never calls.
    struct Chunk
    {
        std::uint32_t w0;
        std::uint32_t w1;
        std::uint32_t w2;
        std::uint32_t w3;
        std::uint32_t w4;
        std::uint32_t w5;
        std::uint32_t w6;
        std::uint32_t w7;
        std::uint32_t w8;
        std::uint32_t w9;
        std::uint32_t w10;
        std::uint32_t w11;
        std::uint32_t w12;
        std::uint32_t w13;
        std::uint32_t w14;
        std::uint32_t w15;
    };
    struct
    {
        Chunk c0;
        Chunk c1;
        Chunk c2;
        Chunk c3;
        Chunk c4;
        Chunk c5;
        Chunk c6;
        Chunk c7;
        Chunk c8;
        Chunk c9;
        Chunk c10;
        Chunk c11;
        Chunk c12;
        Chunk c13;
        Chunk c14;
        Chunk c15;
        Chunk c16;
        Chunk c17;
        Chunk c18;
        Chunk c19;                                                              // 20 * 64 = 0x500
    } frame = {};

    // 0x00402FF8..0x00403547: 69 registrations, four pushes each, with 0x40 bytes of cdecl cleanup after every fourth
    // call (0x00403044 .. 0x00403534) and 0x10 after the last one (0x00403547) - 276 dwords in all, which the compiler's
    // own cleanups replace.
    Script_QHashNodeRegisterEx( // 0x00403006: call 0x4b8890
        const_cast<char*>("GOStart"), // 0x00402FF9: push 0x00533238
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00403660U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x00403019: call 0x4b8890
        const_cast<char*>("GOPlayer"), // 0x0040300C: push 0x00533240
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x004036B0U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x0040302C: call 0x4b8890
        const_cast<char*>("GOInRemap"), // 0x0040301F: push 0x0053324C
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x004037C0U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(3U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x0040303F: call 0x4b8890
        const_cast<char*>("GOInMouInv"), // 0x00403032: push 0x00533258
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00403870U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x00403055: call 0x4b8890
        const_cast<char*>("GOInMouSens"), // 0x00403048: push 0x00533264
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x004038E0U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x00403068: call 0x4b8890
        const_cast<char*>("GOGfxDisp"), // 0x0040305B: push 0x00533270
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00403950U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(3U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x0040307B: call 0x4b8890
        const_cast<char*>("GOGfxDevice"), // 0x0040306E: push 0x0053327C
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00404010U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x0040308E: call 0x4b8890
        const_cast<char*>("GOGfxGamma"), // 0x00403081: push 0x00533288
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00404080U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x004030A4: call 0x4b8890
        const_cast<char*>("GOGameLang"), // 0x00403097: push 0x005332A8
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00403A00U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x004030B7: call 0x4b8890
        const_cast<char*>("GOGameDiff"), // 0x004030AA: push 0x005332B4
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00403AC0U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x004030CA: call 0x4b8890
        const_cast<char*>("GOIsBlood"), // 0x004030BD: push 0x005332C0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00403DF0U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x004030DD: call 0x4b8890
        const_cast<char*>("GOSoundSpeech"), // 0x004030D0: push 0x005332CC
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00403E60U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(2U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x004030F3: call 0x4b8890
        const_cast<char*>("GOSoundMusic"), // 0x004030E6: push 0x005332DC
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00403EF0U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(2U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x00403106: call 0x4b8890
        const_cast<char*>("GOSoundFX"), // 0x004030F9: push 0x005332EC
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00403F80U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(2U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x00403119: call 0x4b8890
        const_cast<char*>("GOGfxPerformance"), // 0x0040310C: push 0x00533294
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x004040F0U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x0040312C: call 0x4b8890
        const_cast<char*>("GOActiveMission"), // 0x0040311F: push 0x005332F8
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00403750U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x00403142: call 0x4b8890
        const_cast<char*>("GOContentControlPW"), // 0x00403135: push 0x00533308
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00403C10U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x00403155: call 0x4b8890
        const_cast<char*>("Config_FillPlayerProfileListBox"), // 0x00403148: push 0x005382A8
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00404FA0U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x00403168: call 0x4b8890
        const_cast<char*>("Config_GetActivePlayerProfileIndex"), // 0x0040315B: push 0x00538284
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00405010U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x0040317B: call 0x4b8890
        const_cast<char*>("Config_SetActivePlayerProfileIndex"), // 0x0040316E: push 0x00538260
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00405050U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x00403191: call 0x4b8890
        const_cast<char*>("Config_CreateNewPlayerProfile"), // 0x00403184: push 0x00538240
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x004050A0U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(2U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x004031A4: call 0x4b8890
        const_cast<char*>("Config_DeletePlayerProfile"), // 0x00403197: push 0x00538224
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00405400U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x004031B7: call 0x4b8890
        const_cast<char*>("Config_PlayerGetActiveMission"), // 0x004031AA: push 0x00538204
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x004057D0U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x004031CA: call 0x4b8890
        const_cast<char*>("Config_GetNumberOfPlayerProfiles"), // 0x004031BD: push 0x005381E0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x004054F0U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x004031E0: call 0x4b8890
        const_cast<char*>("Config_FillMissionSelectionBox"), // 0x004031D3: push 0x005381C0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00405660U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x004031F3: call 0x4b8890
        const_cast<char*>("Config_FillMissionPictureBox"), // 0x004031E6: push 0x005381A0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x004056F0U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x00403206: call 0x4b8890
        const_cast<char*>("Config_FillMissionTextBox"), // 0x004031F9: push 0x00538184
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00405760U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x00403219: call 0x4b8890
        const_cast<char*>("Config_FillScreenResolutionListBox"), // 0x0040320C: push 0x00538160
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00404450U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x0040322F: call 0x4b8890
        const_cast<char*>("Config_FillRenderDeviceListBox"), // 0x00403222: push 0x00538140
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x004046A0U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x00403242: call 0x4b8890
        const_cast<char*>("Config_GameOptionsInputGetInvertMouse"), // 0x00403235: push 0x00538118
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x004041D0U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x00403255: call 0x4b8890
        const_cast<char*>("Config_GameOptionsInputGetMouseSensitivity"), // 0x00403248: push 0x005380EC
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00404290U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x00403268: call 0x4b8890
        const_cast<char*>("Config_GameOptionsInputGetBloodEnabled"), // 0x0040325B: push 0x005380C4
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00404340U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x0040327E: call 0x4b8890
        const_cast<char*>("Config_GameOptionsInputSetInvertMouse"), // 0x00403271: push 0x0053809C
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00404160U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x00403291: call 0x4b8890
        const_cast<char*>("Config_GameOptionsInputSetMouseSensitivity"), // 0x00403284: push 0x00538070
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00404230U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x004032A4: call 0x4b8890
        const_cast<char*>("Config_GameOptionsInputSetBloodEnabled"), // 0x00403297: push 0x00538048
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x004042E0U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x004032B7: call 0x4b8890
        const_cast<char*>("Config_GraphicOptionsSetResolution"), // 0x004032AA: push 0x00538024
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00404510U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x004032CD: call 0x4b8890
        const_cast<char*>("Config_GraphicOptionsSetTransparency"), // 0x004032C0: push 0x00537FFC
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00404960U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x004032E0: call 0x4b8890
        const_cast<char*>("Config_GraphicOptionsSetDevice"), // 0x004032D3: push 0x00537FDC
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x004047F0U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterMode2_4B88F0( // 0x004032F3: call 0x4b88f0
        const_cast<char*>("Config_GraphicOptionsSetGamma"), // 0x004032E6: push 0x00537FBC
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x004048B0U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x00403306: call 0x4b8890
        const_cast<char*>("Config_GraphicOptionsSetPerformanceFlags"), // 0x004032F9: push 0x00537F90
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00404F00U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x0040331C: call 0x4b8890
        const_cast<char*>("Config_GraphicOptionsGetResolution"), // 0x0040330F: push 0x00537F6C
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x004045B0U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x0040332F: call 0x4b8890
        const_cast<char*>("Config_GraphicOptionsGetTransparency"), // 0x00403322: push 0x00537F44
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x004049C0U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x00403342: call 0x4b8890
        const_cast<char*>("Config_GraphicOptionsGetDevice"), // 0x00403335: push 0x00537F24
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00404710U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x00403355: call 0x4b8890
        const_cast<char*>("Config_GraphicOptionsGetGamma"), // 0x00403348: push 0x00537F04
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00404910U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x0040336B: call 0x4b8890
        const_cast<char*>("Config_GraphicOptionsGetPerformanceFlags"), // 0x0040335E: push 0x00537ED8
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00404F60U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x0040337E: call 0x4b8890
        const_cast<char*>("Config_GraphicOptionsGetPerfFlagsFromLevel"), // 0x00403371: push 0x00537EAC
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x004055C0U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x00403391: call 0x4b8890
        const_cast<char*>("Config_GraphicOptionsGetPerfLevelFromFlags"), // 0x00403384: push 0x00537E80
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00405530U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x004033A4: call 0x4b8890
        const_cast<char*>("Config_GraphicOptionsNumPerfLevels"), // 0x00403397: push 0x00537E5C
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00405620U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x004033BA: call 0x4b8890
        const_cast<char*>("Config_SoundOptionsSetSoundsEffects"), // 0x004033AD: push 0x00537E38
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00404A10U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x004033CD: call 0x4b8890
        const_cast<char*>("Config_SoundOptionsSetSoundsEffectsVolume"), // 0x004033C0: push 0x00537E0C
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00404AE0U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x004033E0: call 0x4b8890
        const_cast<char*>("Config_SoundOptionsSetMusic"), // 0x004033D3: push 0x00537DF0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00404B90U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x004033F3: call 0x4b8890
        const_cast<char*>("Config_SoundOptionsSetMusicVolume"), // 0x004033E6: push 0x00537DCC
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00404C40U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x00403409: call 0x4b8890
        const_cast<char*>("Config_SoundOptionsSetSpeech"), // 0x004033FC: push 0x00537DAC
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00404CF0U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x0040341C: call 0x4b8890
        const_cast<char*>("Config_SoundOptionsSetSpeechVolume"), // 0x0040340F: push 0x00537D88
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00404DA0U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x0040342F: call 0x4b8890
        const_cast<char*>("Config_SoundOptionsSetReverseStereo"), // 0x00403422: push 0x00537D64
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00404E50U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x00403442: call 0x4b8890
        const_cast<char*>("Config_SoundOptionsGetSoundsEffects"), // 0x00403435: push 0x00537D40
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00404A90U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x00403458: call 0x4b8890
        const_cast<char*>("Config_SoundOptionsGetSoundsEffectsVolume"), // 0x0040344B: push 0x00537D14
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00404B40U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x0040346B: call 0x4b8890
        const_cast<char*>("Config_SoundOptionsGetMusic"), // 0x0040345E: push 0x00537CF8
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00404BF0U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x0040347E: call 0x4b8890
        const_cast<char*>("Config_SoundOptionsGetMusicVolume"), // 0x00403471: push 0x00537CD4
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00404CA0U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x00403491: call 0x4b8890
        const_cast<char*>("Config_SoundOptionsGetSpeech"), // 0x00403484: push 0x00537CB4
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00404D50U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x004034A7: call 0x4b8890
        const_cast<char*>("Config_SoundOptionsGetSpeechVolume"), // 0x0040349A: push 0x00537C90
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00404E00U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x004034BA: call 0x4b8890
        const_cast<char*>("Config_SoundOptionsGetReverseStereo"), // 0x004034AD: push 0x00537C6C
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00404EB0U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x004034CD: call 0x4b8890
        const_cast<char*>("Config_SetContentControlPassword"), // 0x004034C0: push 0x00537C48
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00403C10U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x004034E0: call 0x4b8890
        const_cast<char*>("Config_GetContentControlPassword"), // 0x004034D3: push 0x00537C24
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00403C70U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x004034F6: call 0x4b8890
        const_cast<char*>("Config_VerifyContentControlPassword"), // 0x004034E9: push 0x00537C00
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00403CC0U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x00403509: call 0x4b8890
        const_cast<char*>("Config_IsContentControlPasswordEnabled"), // 0x004034FC: push 0x00537BD8
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00403D90U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x0040351C: call 0x4b8890
        const_cast<char*>("Config_IsGermany"), // 0x0040350F: push 0x00537BC4
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00403B30U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x0040352F: call 0x4b8890
        const_cast<char*>("Config_ResetGraphicOptions"), // 0x00403522: push 0x00537BA8
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00403B70U)), // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // pushed type
    Script_QHashNodeRegisterEx( // 0x00403542: call 0x4b8890
        const_cast<char*>("DefineGameOptions"), // 0x00403535: push 0x00537B94
        nullptr, // handler
        nullptr, // pushed 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // pushed type

    // 0x0040354A: mov $0x00536558,%esi; 0x0040354F: mov 0x20(%esi),%eax; push %eax; push %esi; call 0x004B8810;
    // 0x00403559: add $0x24,%esi; 0x0040355F: cmp $0x00536924,%esi; 0x00403565: jb.
    {
        std::uint8_t const* record = reinterpret_cast<std::uint8_t const*>(g_536558);
        do
        {
            std::uint32_t const id = *reinterpret_cast<std::uint32_t const*>(record + 0x20U); // 0x0040354F
            Script_QHashNodeRegister(const_cast<std::uint8_t*>(record),                  // 0x00403553: push %esi
                                     reinterpret_cast<void*>(static_cast<std::uintptr_t>(id))); // 0x00403552: push %eax
            record += 0x24U;                                                            // 0x00403559
        } while (record < reinterpret_cast<std::uint8_t const*>(g_536558) + 0x3CCU);      // 0x0040355F/0x00403565
    }

    // 0x00403567: mov $0x00536928,%esi; 0x0040356C: mov 0x20(%esi),%ecx; push %ecx; push %esi; call 0x004B8810;
    // 0x00403576: add $0x24,%esi; 0x0040357C: cmp $0x00536994,%esi; 0x00403582: jb.
    {
        std::uint8_t const* record = reinterpret_cast<std::uint8_t const*>(g_536928);
        do
        {
            std::uint32_t const id = *reinterpret_cast<std::uint32_t const*>(record + 0x20U); // 0x0040356C
            Script_QHashNodeRegister(const_cast<std::uint8_t*>(record),
                                     reinterpret_cast<void*>(static_cast<std::uintptr_t>(id)));
            record += 0x24U;                                                            // 0x00403576
        } while (record < reinterpret_cast<std::uint8_t const*>(g_536928) + 0x6CU);       // 0x0040357C/0x00403582
    }

    // 0x00403584: mov $0x00536998,%esi; 0x00403589: mov 0x20(%esi),%edx; push %edx; push %esi; call 0x004B8810;
    // 0x00403593: add $0x24,%esi; 0x00403599: cmp $0x00537B74,%esi; 0x0040359F: jb.
    {
        std::uint8_t const* record = reinterpret_cast<std::uint8_t const*>(g_536998);
        do
        {
            std::uint32_t const id = *reinterpret_cast<std::uint32_t const*>(record + 0x20U); // 0x00403589
            Script_QHashNodeRegister(const_cast<std::uint8_t*>(record),
                                     reinterpret_cast<void*>(static_cast<std::uintptr_t>(id)));
            record += 0x24U;                                                            // 0x00403593
        } while (record < reinterpret_cast<std::uint8_t const*>(g_536998) + 0x11DCU);     // 0x00403599/0x0040359F
    }

    // 0x004035A1: xor %edi,%edi; 0x004035A3: mov $0x0053331C,%esi; 0x004035A8: push %edi; push %esi; call 0x004B8810;
    // 0x004035AF: add $0x8,%esp; 0x004035B2: add $0x10,%esi; 0x004035B5: inc %edi; 0x004035B6: cmp $0x0053337C,%esi;
    // 0x004035BC: jl. The second argument is the counter, not a field of the record.
    {
        std::uint32_t counter = 0U;                                                     // 0x004035A1: xor %edi,%edi
        std::uint8_t const* record = reinterpret_cast<std::uint8_t const*>(g_53331c);
        do
        {
            Script_QHashNodeRegister(const_cast<std::uint8_t*>(record),                  // 0x004035A9: push %esi
                                     reinterpret_cast<void*>(static_cast<std::uintptr_t>(counter))); // 0x004035A8: push %edi
            record += 0x10U;                                                            // 0x004035B2
            ++counter;                                                                  // 0x004035B5
        } while (record < reinterpret_cast<std::uint8_t const*>(g_53331c) + 0x60U);       // 0x004035B6/0x004035BC
    }

    // 0x004035BE: xor %edi,%edi; 0x004035C0: mov $0x0053337C,%esi; 0x004035C5: push %edi; push %esi; call 0x004B8810;
    // 0x004035CC: add $0x8,%esp; 0x004035CF: add $0x10,%esi; 0x004035D2: inc %edi; 0x004035D3: cmp $0x005333AC,%esi;
    // 0x004035D9: jl.
    {
        std::uint32_t counter = 0U;                                                     // 0x004035BE: xor %edi,%edi
        std::uint8_t const* record = reinterpret_cast<std::uint8_t const*>(g_53337c);
        do
        {
            Script_QHashNodeRegister(const_cast<std::uint8_t*>(record),
                                     reinterpret_cast<void*>(static_cast<std::uintptr_t>(counter)));
            record += 0x10U;                                                            // 0x004035CF
            ++counter;                                                                  // 0x004035D2
        } while (record < reinterpret_cast<std::uint8_t const*>(g_53337c) + 0x30U);       // 0x004035D3/0x004035D9
    }

    // 0x004035DB: call 0x004950E0; 0x004035E0: push %eax; 0x004035E1: call 0x004B0FD0. Retail cleans that one argument
    // only later, at 0x004035F2, together with the two 0x00491EA0 pushes; the port lets the compiler clean at the call.
    Sys_LeaveRenderMode_4B0FD0(static_cast<std::int32_t>(Sys_GetGlobal32_4950E0()));

    // 0x004035E6: lea 0xc(%esp),%eax (the frame base); 0x004035EA: push $0x5; 0x004035EC: push %eax; 0x004035ED: call
    // 0x00491EA0; 0x004035F2: add $0xc,%esp; 0x004035F5: mov %eax,0x00567C90.
    g_567c90 = Display_DriverDispatch_491EA0(&frame, 5);

    // 0x004035FA: test %eax,%eax; 0x004035FC: jle 0x0040362D.
    if (g_567c90 > 0)
    {
        // 0x004035FE: push %ebx; 0x004035FF: mov $0x00568098,%edx; 0x00403604: lea 0xc(%esp),%ebx;
        // 0x00403608: lea -0x400(%edx),%edi; 0x0040360E: mov $0x40,%ecx; 0x00403613: mov %ebx,%esi;
        // 0x00403615: add $0x100,%ebx; 0x0040361B: rep movsd; 0x0040361D: movl $0x0,(%edx);
        // 0x00403623: add $0x404,%edx; 0x00403629: dec %eax; 0x0040362A: jne.
        std::uint32_t remaining = static_cast<std::uint32_t>(g_567c90);                  // %eax
        std::uint8_t const* source = reinterpret_cast<std::uint8_t const*>(&frame);       // %ebx (saved into %esi first)
        std::uint8_t* record = g_567c98;                                                 // %edi = %edx - 0x400
        do
        {
            std::memcpy(record, source, 0x100U);                                         // 0x0040361B: rep movsd, 0x40 dwords
            *reinterpret_cast<std::uint32_t*>(record + 0x400U) = 0U;                      // 0x0040361D: movl $0x0,(%edx)
            record += 0x404U;                                                            // 0x00403623
            source += 0x100U;                                                            // 0x00403615
            --remaining;                                                                 // 0x00403629
        } while (remaining != 0U);                                                       // 0x0040362A
    }

    // 0x0040362D: push $0x404390; 0x00403632: push $0x8; 0x00403634: call 0x00491CF0; 0x00403639: push %eax;
    // 0x0040363A: call 0x00491980; 0x0040363F: add $0xc,%esp. The 8 pushed at 0x00403632 is never cleaned, so it is the
    // second argument of BOTH calls: the same word serves as 0x00491CF0's argument2 and 0x00491980's argument2.
    void* const active_mode = Display_GetActiveMode_491CF0(
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00404390U)), 8);           // 0x0040362D..0x00403634
    Display_ApplyFlags_491980(active_mode, 8, nullptr);                                  // 0x00403639/0x0040363A. Retail pushes only these two
                                                                                         // arguments: the callee's third argument is the
                                                                                         // stale word at [E+0x0C] (this frame's saved EDI
                                                                                         // slot from 0x00402FF7), which the callee's own
                                                                                         // null check at 0x004919F2 skips. The port passes
                                                                                         // nullptr rather than an unknowable word, and the
                                                                                         // count at 0x00C10384 is 0 in the image, so the
                                                                                         // loop body does not run at this call site at all.

    // 0x00403642: call 0x004B0F90.
    Sys_EnterRenderMode_4B0F90();

    // 0x00403647: pop %edi; 0x00403648: movb $0x0,0x00BC79C8; 0x0040364F: pop %esi; 0x00403650: add $0x500,%esp;
    // 0x00403656: ret. Only the first byte of the 32-byte password buffer is cleared, and the store sits between the two
    // register restores in retail; the port keeps the store last because the pops have no observable equivalent.
    Config_ContentControlPassword[0] = 0;                                                // 0x00403648
}

} // extern "C"

#undef NATIVE_DECL
