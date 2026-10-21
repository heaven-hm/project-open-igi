#pragma once

// The retail Game class and its phase handlers.
//
// igi.exe registers the Game class during the 0x00402930 sequence and links
// three phase handlers into the handler table at 0x00A96AE0:
//
//   phase 0  0x00415B30 Level_Start          the level start, which loads the
//                                            terrain, the sounds, sprites.res,
//                                            textures.res and objects.qsc
//   phase 1  0x00416260 Game_Setup           the per-frame update
//   phase 2  0x00416550 Game_DeleteHandler   the teardown
//
// The per-frame dispatch at 0x00401050 reads the phase 1 base of that table,
// so the engine only follows the retail flow when the class is registered and
// the handlers are linked the way the retail links them.

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define IGI_RETAIL_CDECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define IGI_RETAIL_CDECL __attribute__((cdecl))
#else
#define IGI_RETAIL_CDECL
#endif

namespace igi::retail::core {

// The platform hooks the phase handlers drive. The retail's handlers work on
// the game object and its task list; the port's scene is the equivalent, so the
// handlers call through these.
using GameLevelStartHook = void (*)(void*);
using GamePerFrameHook = void (*)(void*);
using GameShutdownHook = void (*)(void*);

void SetGameLevelStartHook(GameLevelStartHook hook) noexcept;
void SetGamePerFrameHook(GamePerFrameHook hook) noexcept;
void SetGameShutdownHook(GameShutdownHook hook) noexcept;

} // namespace igi::retail::core

extern "C" {

// The retail Game class id, kept at 0x0057B1A2.
extern std::uint16_t DAT_0057B1A2;

// The retail class init event identifiers, 0x00539540..0x0053954E. The retail
// keeps one byte per allocated event.
extern std::uint8_t DAT_00539540[15];

// Native: Game_Init; retail VA 0x00415530.
// Registers the Game class with a 0x158 byte object, clears the fifteen class
// init flags and links the three phase handlers.
void IGI_RETAIL_CDECL Game_Init();

// Native: Level_Start; retail VA 0x00415B30; the phase 0 handler.
// The retail sets the frame count, starts the game timing, looks the mission up
// and loads the level. The port runs the same phase through the platform hook.
void IGI_RETAIL_CDECL Level_Start(void* game);

// Native: Game_Setup; retail VA 0x00416260; the phase 1 handler.
// The retail redraws, checks the debug key, honours the pause flag and then
// walks the task list. The port runs the same phase through the platform hook.
void IGI_RETAIL_CDECL Game_Setup(void* game);

// Native: Game_DeleteHandler; retail VA 0x00416550; the phase 2 handler.
// The retail releases the entities and the session state. The port runs the same
// phase through the platform hook.
void IGI_RETAIL_CDECL Game_DeleteHandler(void* game);

} // extern "C"

#undef IGI_RETAIL_CDECL
