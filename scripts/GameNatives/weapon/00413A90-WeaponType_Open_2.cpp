// IGI native provenance: WeaponType_Open_2 - 0x00413A90 - Radare2 CFG 6 blocks (candidate; no CFG score recorded); runtime behavior not verified.
// Evidence: 00413A90-WeaponType_Open.md; catalog provenance record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00413A90-WeaponType_Open_2 2.hpp"

/* Analysis reconstruction for the original 32-bit Windows image.
 * The absolute helper addresses are not valid in the macOS replica. */
typedef WeaponType_Open_2(*format_path_fn)(char *, const char *, ...);
typedef void *(*qvm_load_fn)(const char *);
typedef WeaponType_Open_2 (*qvm_action_fn)(void *);
typedef WeaponType_Open_2 (*error_show_fn)(const char *, ...);
#define GAME_FN(type, address) ((type)(uintptr_t)(address))

void WeaponType_Open_2(weapon_type_context_t *ctx)
{
    char script_path[256];
    if (!ctx->load_enabled)
        return;

    GAME_FN(format_path_fn, 0x004A53B3)(
        script_path, "%s/%s",
        (const char *)(uintptr_t)ctx->script_directory_ptr32, "weapon.qsc");
    void *vm = GAME_FN(qvm_load_fn, 0x004B80B0)(script_path);
    if (vm) {
        GAME_FN(qvm_action_fn, 0x004B85B0)(vm);
        GAME_FN(qvm_action_fn, 0x004B83D0)(vm);
        return;
    }

    GAME_FN(error_show_fn, 0x004AF7B0)(
        "WeaponType_Open_2(): Couldn't load script: %s", "weapon.qsc");
    for (;;) { }
}
