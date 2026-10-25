#include "dev_log.hpp"
#include "igi/retail/core/game_flow.hpp"

#include "igi/retail/core/app_main_commands.hpp"
#include "igi/retail/core/qtask_list.hpp"
#include "igi/retail/core/task_types.hpp"

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

// Native: Game_Init; retail VA 0x00415530.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x00415530..0x004157CD. The previous claim in this block read "VA 0x00415530..0x00415653", which is wrong: 0x00415653 is push 0x200, the first instruction of the first effect group this transcription omitted. The claim therefore truncated exactly where the implementation stopped, which is why it survived every check - a record wrong about its own size in the same direction it is incomplete contradicts nothing. The real extent is 168 instructions ending at the ret at 0x004157CD; 0x004157CE-0x004157CF are alignment nops and 0x004157D0 starts the next function with sub esp,0x18.
// Behavior/CFG: retail 0x00415530 begins with AppMain_ParseCmdLineArgs("Level", 0x00415B10).
//           0x0048F360 is the command-line token matcher: it copies the command line (0x0048F080),
//           tokenizes it on space/tab (0x004A5A0C), compares each token case-insensitively against
//           the supplied literal, and calls the callback with the remainder after the match. The
//           "Level" literal is therefore the ``Level <n>`` command-line switch, not a class name;
//           0x00415B10 parses n (0x004A5490) into 0x00539560 and calls 0x0048F210, which stores it
//           at 0x005C8C08. This corrects the earlier note that called 0x0048F360 a class
//           registration helper.
//           The actual registration follows: 0x004018F0 returns the current task type (the QTask
//           class id at 0x0054F928, registered by 0x00401900("QTask", 0x20, 0, 0x181) in
//           0x00401000), then 0x00415530 registers "Game" through 0x00401900 with size 0x158 and
//           keeps the returned class id at 0x0057B1A2. It allocates fifteen events through
//           0x004017C0 (ids at 0x00539540..0x0053954E), and links the phase 0 handler 0x00415B30
//           through 0x00401400, the phase 1 handler 0x00416260 and the phase 2 handler 0x00416550
//           through 0x00401530 into the handler table at 0x00A96AE0 at class_id + phase * 384.
// Status/limits: STRUCTURALLY_VERIFIED for the CFG (168 instructions, one block, no branches, `ret`
//           at 0x004157CD) and for the effect order; BUILD_VERIFIED (single-file
//           i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra, zero warning lines).
// Verified: 100% manually verified by HeavenHM (project-igi-natives/IGI_Natives/IGI-Natives.json).
// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
//           The class id, the object size, the fifteen event identifiers, the three phases and the
//           groups A and E..K below are the retail's. There is no separate "Level" class entry to
//           transcribe: the retail registers only QTask and Game on this path. An earlier revision of
//           this block said the command-line switch parser was outside this native; the parser call is
//           at 0x0041553A, inside it, and is transcribed below as group A.
//           NOT LINKABLE YET, and the reason is a CALLEE gap, not a handler gap: five of the fourteen
//           callees are not transcribed anywhere in the tree - 0x004B8870, 0x004F4870, 0x004A29E0,
//           0x004A29C0, 0x004A28B0 - so groups E, G, H, I, J and K cannot bind. They are declared as
//           NATIVE GAPs above, with signatures read from their own bodies; nothing is stubbed and no
//           argument value is invented. The other nine callees are defined in the tree: this file was
//           linkable with 5 undefined before the extension, and the extended file reports exactly
//           those five gaps and nothing else. Runtime UNOBSERVED in both binaries.
//           The step from this native to the level start is DATA, not code: 0x00415B30 has no call
//           site anywhere in the image, so the phase 0 handler is reached through the handler table
//           at 0x00A96AE0 (class_id + phase * 384), not by a call from here.

namespace igi::retail::core {
namespace {

GameLevelStartHook g_level_start_hook = nullptr;
GamePerFrameHook g_per_frame_hook = nullptr;
GameShutdownHook g_shutdown_hook = nullptr;

} // namespace

void SetGameLevelStartHook(GameLevelStartHook hook) noexcept { g_level_start_hook = hook; }
void SetGamePerFrameHook(GamePerFrameHook hook) noexcept { g_per_frame_hook = hook; }
void SetGameShutdownHook(GameShutdownHook hook) noexcept { g_shutdown_hook = hook; }

} // namespace igi::retail::core

extern "C" {

std::uint16_t QTask_GameClassId = 0U;
std::uint8_t QTask_ClassEventIds[15] = {};

// 0x00539550 and 0x00539558: the two 8-byte constants Game_Init registers with
// 0x004B8870. They are stored as the image's exact dwords rather than as C++ double
// literals, because the image value is not the value anyone would write:
//   0x00539550 = bytes 1b 2d 44 54 fb 21 09 40 = 0x400921FB54442D1B as an IEEE-754
//     double, which is 3.1415926535897944 - two ULP ABOVE the nearest-pi double
//     (0x400921FB54442D18). Retail's "MATH_PI" is therefore NOT pi. Do not "correct"
//     this to 3.141592653589793.
//   0x00539558 = bytes 00 00 00 00 00 00 b0 40 = 0x40B0000000000000 = 4096.0.
// Both are below the file-backed end of .data (0x00550000), so these initialisers are
// the image's own bytes.
std::uint64_t g_539550 = 0x400921FB54442D1BULL; // 0x00539550: "MATH_PI" - see above
std::uint64_t g_539558 = 0x40B0000000000000ULL; // 0x00539558: "WORLD_METER" = 4096.0

// 0x0057BA9C..0x0057BAB0: the six task lists Game_Init creates. These are past the
// file-backed end of .data, so the image value is zero. Each is read back by a
// not-yet-transcribed one-line getter - 0x004168A0, 0x004168B0, 0x004168C0,
// 0x004168D0, 0x004168E0 and 0x004168F0 in that order - which must declare these
// extern when they are transcribed, exactly as this file declares g_539540.
std::uint32_t g_57ba9c = 0U; // 0x0057BA9C
std::uint32_t g_57baa0 = 0U; // 0x0057BAA0
std::uint32_t g_57baa4 = 0U; // 0x0057BAA4
std::uint32_t g_57baa8 = 0U; // 0x0057BAA8
std::uint32_t g_57baac = 0U; // 0x0057BAAC
std::uint32_t g_57bab0 = 0U; // 0x0057BAB0

// The two script registrars Game_Init calls are transcribed but have no header; the
// declarations below match their definitions in script/native_4b8810.cpp and
// script/native_4b8890.cpp (the same local-declaration form native_402ff0.cpp uses).
void Script_QHashNodeRegister(void* arg1, void* value); // 0x004B8810
void Script_QHashNodeRegisterEx(void* arg0, void* arg1, char const* arg2,
                                void* arg3); // 0x004B8890

// NATIVE GAPS. The five callees below are not transcribed anywhere in the tree, so this
// file cannot link until they land. They are declared, not stubbed: no body and no
// argument value is invented. Each signature is read from its own retail body, bounded by
// its own `ret`, and each name is an explicit placeholder, not a retail symbol. Candidate
// names from scripts/GameNatives are not used where the body contradicts them.
//   0x004B8870, 29 bytes (0x004B8870-0x004B888F): `mov eax,[esp+8]; mov ecx,[esp+4];
//     push 0; push 2; push 0; push 8; push eax; push 0x004B7FE0; push ecx; call 0x004BAAC0;
//     add esp,0x1C; ret` - two arguments, a name and a value, forwarded to 0x004BAAC0 with
//     the fixed callback 0x004B7FE0 and size 8. Same name+value shape as the transcribed
//     sibling 0x004B8810.
//   0x004F4870, 10 bytes (0x004F4870-0x004F4879): `mov eax,[esp+4]; mov [0x00A76C40],eax;
//     ret` - one argument stored to the global 0x00A76C40. This is a setter, not a script
//     handler call; the GameNatives candidate name Script_CallNativeHandler_17 is
//     contradicted by the body.
//   0x004A29E0, 13 bytes: stores arg1/arg2/arg3 to 0x00936024/0x00936030/0x00936034.
//   0x004A29C0, 13 bytes: stores arg1/arg2/arg3 to 0x00936020/0x00936028/0x0093602C.
//   0x004A28B0, 17 bytes: stores arg1..arg5 to 0x00935C04/0x0093603C/0x00936040/
//     0x00936044/0x00936048. The three sit in the camera module - 0x004A2A00 registers
//     "CAMERAFILTER_TYPE_NONE" through 0x004B8810 - but the values Game_Init passes them
//     are a font path and a sprite path, so the GameNatives Camera_* candidate names are
//     not used and the names below describe only the call shape.
extern void NATIVE_DECL Script_QHashNodeRegisterData_4B8870(void* arg1, void* arg2);
extern void NATIVE_DECL Sys_SetGlobalA76C40_4F4870(void* arg1);
extern void NATIVE_DECL Render_StoreArgs936024_4A29E0(void* arg1, void* arg2, void* arg3);
extern void NATIVE_DECL Render_StoreArgs936020_4A29C0(void* arg1, void* arg2, void* arg3);
extern void NATIVE_DECL Render_StoreArgs935C04_4A28B0(void* arg1, void* arg2, void* arg3,
                                                      void* arg4, void* arg5);

NATIVE_DECL void Game_Init()
{
    NATIVE_LOG_ENTER("Game_Init", "0x00415530");
    // Group A, retail 0x00415530-0x0041553F: the "Level <n>" command-line switch. The
    // literal is the option name; 0x00415B10 is the switch handler, which retail passes as
    // a raw code immediate and which is not transcribed, so it is a cast integer and
    // creates no link dependency. Retail: push 0x00415B10; push 0x00539688; call
    // 0x0048F360; add esp,8.
    AppMain_ParseCmdLineArgs( // 0x0041553A: call 0x0048F360
        const_cast<char*>("Level"), // 0x00415535: push 0x00539688
        reinterpret_cast<std::int32_t(NATIVE_DECL*)(char*)>(
            static_cast<std::uintptr_t>(0x00415B10U))); // 0x00415530: push 0x00415B10

    // The retail derives the Game class from the current task type, which the
    // native at 0x004018F0 reads from 0x0054F928.
    QTask_GameClassId = TaskType_Register("Game", 0x158U, QTask_GetCurrentType(), 0U);
    // The retail allocates fifteen events and keeps their identifiers in the
    // bytes at 0x00539540..0x0053954E.
    for (std::uint8_t& event : QTask_ClassEventIds)
    {
        event = QTask_AllocateEvent(0U);
    }
    // The phase 0 handler goes through the context link native at 0x00401400;
    // the phase 1 and phase 2 handlers go through the no-context link native
    // at 0x00401530.
    QTask_LinkQTaskEventHandler(QTask_GameClassId, 0U, Level_Start);
    QTask_LinkQTaskEventHandlerNoContextFn(QTask_GameClassId, 1U, Game_Setup);
    QTask_LinkQTaskEventHandlerNoContextFn(QTask_GameClassId, 2U, Game_DeleteHandler);

    // Group E, retail 0x00415653-0x004156A5: six task lists. Retail pushes the capacity,
    // calls 0x004C1800 and stores the returned pointer in the six dwords
    // 0x0057BA9C..0x0057BAB0, in this order. The compiler interleaves each store with the
    // following push; the order written here is the order of the calls.
    g_57ba9c = static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(QTaskList_New(0x200))); // 0x00415653/0x0041565F
    g_57baa0 = static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(QTaskList_New(0x40))); // 0x0041565D/0x0041566E
    g_57baa4 = static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(QTaskList_New(0x100))); // 0x00415669/0x0041567B
    g_57baa8 = static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(QTaskList_New(0x100))); // 0x00415680/0x0041568C
    g_57baac = static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(QTaskList_New(0x10))); // 0x0041568A/0x0041569B
    g_57bab0 = static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(QTaskList_New(0x80))); // 0x00415696/0x004156A5

    // Group F, retail 0x004156AA-0x004156B0: the 0x10-capacity list is handed to
    // 0x004F4870, which stores it in the global 0x00A76C40.
    Sys_SetGlobalA76C40_4F4870(reinterpret_cast<void*>(
        static_cast<std::uintptr_t>(g_57baac))); // 0x004156AA/0x004156B0

    // Group G, retail 0x004156B5-0x004156BC.
    Script_QHashNodeRegister( // 0x004156BC: call 0x004B8810
        const_cast<char*>("GAME_FREQUENCY"), // 0x004156B7: push 0x00539670
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1EU))); // 0x004156B5: push 0x1E

    // Group H, retail 0x004156C1-0x004156DA: the two named constants. Retail passes the
    // address of each 8-byte slot, not its value.
    Script_QHashNodeRegisterData_4B8870( // 0x004156CB: call 0x004B8870
        const_cast<char*>("WORLD_METER"), // 0x004156C6: push 0x00539664
        &g_539558); // 0x004156C1: push 0x00539558
    Script_QHashNodeRegisterData_4B8870( // 0x004156DA: call 0x004B8870
        const_cast<char*>("MATH_PI"), // 0x004156D5: push 0x0053965C
        &g_539550); // 0x004156D0: push 0x00539550

    // Group I, retail 0x004156DF-0x0041578B: nine script registrations. The second
    // argument is a raw code immediate in every case - the handler address - so it is a
    // cast integer and creates no link dependency.
    Script_QHashNodeRegisterEx( // 0x004156ED: call 0x004B8890
        const_cast<char*>("Game_SetMission"), // 0x004156E8: push 0x0053964C
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00415890U)), // 0x004156E3
        nullptr, // 0x004156E1: push 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // 0x004156DF: push 1
    Script_QHashNodeRegisterEx( // 0x00415700: call 0x004B8890
        const_cast<char*>("Game_UpdateVolume"), // 0x004156FB: push 0x00539638
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x004158E0U)), // 0x004156F6
        nullptr, // 0x004156F4: push 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // 0x004156F2: push 0
    Script_QHashNodeRegisterEx( // 0x00415716: call 0x004B8890
        const_cast<char*>("Game_DisablePlayerInput"), // 0x00415711: push 0x00539620
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x004157D0U)), // 0x0041570C
        nullptr, // 0x0041570A: push 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // 0x00415708: push 0
    Script_QHashNodeRegisterEx( // 0x00415729: call 0x004B8890
        const_cast<char*>("Game_EnablePlayerInput"), // 0x00415724: push 0x00539608
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00415830U)), // 0x0041571F
        nullptr, // 0x0041571D: push 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // 0x0041571B: push 0
    Script_QHashNodeRegisterEx( // 0x0041573C: call 0x004B8890
        const_cast<char*>("Game_SetMusicVolume"), // 0x00415737: push 0x005395F4
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00415950U)), // 0x00415732
        nullptr, // 0x00415730: push 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // 0x0041572E: push 1
    Script_QHashNodeRegisterEx( // 0x0041574F: call 0x004B8890
        const_cast<char*>("Game_SetSFXVolume"), // 0x0041574A: push 0x005395E0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x004159B0U)), // 0x00415745
        nullptr, // 0x00415743: push 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // 0x00415741: push 1
    Script_QHashNodeRegisterEx( // 0x00415765: call 0x004B8890
        const_cast<char*>("Game_EnableMusic"), // 0x00415760: push 0x005395CC
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00415A10U)), // 0x0041575B
        nullptr, // 0x00415759: push 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // 0x00415757: push 0
    Script_QHashNodeRegisterEx( // 0x00415778: call 0x004B8890
        const_cast<char*>("Game_DisableMusic"), // 0x00415773: push 0x005395B8
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00415A60U)), // 0x0041576E
        nullptr, // 0x0041576C: push 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // 0x0041576A: push 0
    Script_QHashNodeRegisterEx( // 0x0041578B: call 0x004B8890
        const_cast<char*>("Game_CutsceneDelete"), // 0x00415786: push 0x005395A4
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00415AB0U)), // 0x00415781
        nullptr, // 0x0041577F: push 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U))); // 0x0041577D: push 1

    // Group J, retail 0x00415790-0x004157A7: the font and sprite paths handed to the two
    // three-argument setters. Retail pushes the integers first and the path last, so the
    // path is argument 1.
    Render_StoreArgs936024_4A29E0( // 0x00415799: call 0x004A29E0
        const_cast<char*>("LOCAL:computer/font1.fnt"), // 0x00415794: push 0x00539588
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x75U)), // 0x00415792: push 0x75
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x14U))); // 0x00415790: push 0x14
    Render_StoreArgs936020_4A29C0( // 0x004157A7: call 0x004A29C0
        const_cast<char*>("LOCAL:common/sprites/battery.spr"), // 0x004157A2: push 0x00539564
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x4U)), // 0x004157A0: push 4
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x2U))); // 0x0041579E: push 2

    // Group K, retail 0x004157AF-0x004157C5: five arguments, pushed as raw dwords. The
    // three float bit patterns are 0x3F666666 = 0.9f, 0x3E4CCCCD = 0.2f and 0xC2480000 =
    // -50.0f; they are passed as the image's dwords, not as float literals.
    Render_StoreArgs935C04_4A28B0( // 0x004157C5: call 0x004A28B0
        const_cast<char*>("LOCAL:computer/font1.fnt"), // 0x004157C0: push 0x00539588
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x3F666666U)), // 0.9f
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x3E4CCCCDU)), // 0.2f
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xC2480000U)), // -50.0f
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U))); // 0x004157AF: push 0
}

// Native: Level_Start; retail VA 0x00415B30.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x00415B30-0x0041600B (1244 bytes, 326 instructions, 90 call
//   sites - 89 direct and 1 indirect - over 65 distinct direct callees; no branches; `ret` at 0x0041600B; 0x0041600C-0x0041600F are
//   alignment nops and 0x00416010 starts the next function with `sub esp,0x11c`). The previous claim in this block read
//   "VA 0x00415B30..0x00415D40": 0x00415D40 is not an instruction boundary at all and 52 of the 90 call sites lie past it, so the claim
//   described 528 of 1244 bytes and stopped inside the body. Only 15 of the 65 distinct callees are transcribed in the tree; the other
//   50 are gaps, and the first step this transcription loses is the second call of the body, 0x00415B40 -> 0x004028D0 (7 instructions
//   at 0x004028D0-0x004028E9: `mov eax,[0x00567C80]; test eax,eax; jne +0xC; call 0x00490370; mov [0x00567C78],eax; inc dword
//   [0x00567C80]; ret` - a refcounted get-or-create over the object at 0x00567C78 and the count at 0x00567C80).
// Behavior/CFG: the retail's phase 0 handler runs the level start. It brings the task list up through 0x00402820 and 0x004028D0, looks the
//   mission up through 0x00485170, reads the mission path with "MISSION:" through 0x004B12F0, and then loads the content in the retail's
//   order: the terrain textures and lightmaps through 0x004C5F00 and the terrain through 0x004C6240 at 0x00415C31 and 0x00415C44.
//   0x004CFBD0 at 0x00415C88 and 0x004CFC00 at 0x00415C92 are NOT package loads, although this block previously described them as "the
//   level package" and "the common package": both are path setters that copy a string into a global - 0x004CFBD0 writes the mission
//   prefix at 0x00A4ED6C and 0x004CFC00 writes the common prefix at 0x00A4EDF8 - and the tree's own loader records use those two globals
//   as format prefixes: DynObj_LoadPackagePalettes formats "%spalettes/%s.pal" from 0x00A4ED6C, and DynObj_LoadPackageAnimations formats
//   "%sanims/%s.iff" from 0x00A4EDF8 when it is non-empty and from 0x00A4ED6C otherwise. 0x004CEFA0 at 0x00415C9E is the package opener
//   rather than "the common forms": it allocates 0x4C bytes, inserts the object into the list at 0x00A4EC60, formats "%s%s.mtp" from
//   0x00A4ED6C and its argument, loads that file into the object, and calls DynObj_LoadModelTexturePackage (0x004CF080). It is untranscribed.
// Status/limits: NOT A TRANSCRIPTION. The body below is a hook, so none of the retail instructions of this function are reproduced and
//   this file cannot claim STRUCTURALLY_VERIFIED for it. The calls the transcription omits are recorded here as gaps and not as skips:
//   the retail body has 90 call sites and this file issues none of them. The earlier wording - "outside the MVP scope, so this
//   transcription skips them" - recorded a scope decision as a reason to omit, which is what §10.13 forbids: a scope decision may order
//   the work, it may not stand in for the missing instructions. The two identical "Verified: 100% manually verified" lines that were here
//   are folded into this one, reworded to say what they cover: the catalog record for this VA
//   (project-igi-natives/IGI_Natives/IGI-Natives.json) claims 100% manual verification, and that claim is about the catalog's record,
//   not evidence for the body below.
NATIVE_DECL void Level_Start(void* game)
{
    NATIVE_LOG_ENTER("Level_Start", "0x00415B30");
    if (igi::retail::core::g_level_start_hook != nullptr)
    {
        igi::retail::core::g_level_start_hook(game);
    }
}

NATIVE_DECL void Game_Setup(void* game)
{
    NATIVE_LOG_ENTER("Game_Setup", "");
    if (igi::retail::core::g_per_frame_hook != nullptr)
    {
        igi::retail::core::g_per_frame_hook(game);
    }
}

NATIVE_DECL void Game_DeleteHandler(void* game)
{
    NATIVE_LOG_ENTER("Game_DeleteHandler", "");
    if (igi::retail::core::g_shutdown_hook != nullptr)
    {
        igi::retail::core::g_shutdown_hook(game);
    }
}

} // extern "C"

#undef NATIVE_DECL
