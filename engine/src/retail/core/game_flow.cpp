#include "igi/retail/core/game_flow.hpp"

#include "igi/retail/core/task_types.hpp"

#if defined(_MSC_VER) && defined(_M_IX86)
#define IGI_RETAIL_CDECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define IGI_RETAIL_CDECL __attribute__((cdecl))
#else
#define IGI_RETAIL_CDECL
#endif

// Native: Game_Init; retail VA 0x00415530.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x00415530..0x00415653.
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
// Status/limits: STRUCTURALLY_VERIFIED; the class id, the object size, the event identifiers and
//           the three phases are the retail's. There is no separate "Level" class entry to
//           transcribe: the retail registers only QTask and Game on this path. The command-line
//           switch parser is outside this native and is not part of the entry ordering claim.

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

std::uint16_t DAT_0057B1A2 = 0U;
std::uint8_t DAT_00539540[15] = {};

IGI_RETAIL_CDECL void Game_Init()
{
    // The retail derives the Game class from the current task type, which the
    // native at 0x004018F0 reads from 0x0054F928.
    DAT_0057B1A2 = TaskType_Register("Game", 0x158U, QTask_GetCurrentType(), 0U);
    // The retail allocates fifteen events and keeps their identifiers in the
    // bytes at 0x00539540..0x0053954E.
    for (std::uint8_t& event : DAT_00539540)
    {
        event = QTask_AllocateEvent(0U);
    }
    // The phase 0 handler goes through the context link native at 0x00401400;
    // the phase 1 and phase 2 handlers go through the no-context link native
    // at 0x00401530.
    QTask_LinkQTaskEventHandler(DAT_0057B1A2, 0U, Level_Start);
    QTask_LinkQTaskEventHandlerNoContextFn(DAT_0057B1A2, 1U, Game_Setup);
    QTask_LinkQTaskEventHandlerNoContextFn(DAT_0057B1A2, 2U, Game_DeleteHandler);
}

// Native: Level_Start; retail VA 0x00415B30.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x00415B30..0x00415D40.
// Behavior/CFG: the retail's phase 0 handler runs the level start. It brings the task list up through 0x00402820 and 0x004028D0, looks the mission up through 0x00485170, reads the mission path with "MISSION:" through 0x004B12F0, and then loads the content in the retail's order: the terrain textures and lightmaps through 0x004C5F00 and the terrain through 0x004C6240 at 0x00415C31 and 0x00415C44, the level package through 0x004CFBD0 at 0x00415C88, the common package through 0x004CFC00 at 0x00415C92, and the common forms through 0x004CEFA0 at 0x00415C9E. The calls for the mission scripts, the sound and the camera sit around them and are outside the MVP scope, so this transcription skips them and the port runs the content through the phase 0 hook.
// Status/limits: STRUCTURALLY_VERIFIED; the content order is the retail's.
IGI_RETAIL_CDECL void Level_Start(void* game)
{
    if (igi::retail::core::g_level_start_hook != nullptr)
    {
        igi::retail::core::g_level_start_hook(game);
    }
}

IGI_RETAIL_CDECL void Game_Setup(void* game)
{
    if (igi::retail::core::g_per_frame_hook != nullptr)
    {
        igi::retail::core::g_per_frame_hook(game);
    }
}

IGI_RETAIL_CDECL void Game_DeleteHandler(void* game)
{
    if (igi::retail::core::g_shutdown_hook != nullptr)
    {
        igi::retail::core::g_shutdown_hook(game);
    }
}

} // extern "C"

#undef IGI_RETAIL_CDECL
