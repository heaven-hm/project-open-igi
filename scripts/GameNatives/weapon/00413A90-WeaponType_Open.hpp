#pragma once
#include <stddef.h>
#include <stdint.h>

/* Candidate only: Ghidra xref + Radare2 forced function analysis. */
typedef struct weapon_type_context {
    unsigned int script_directory_ptr32; /* original PE pointer value at +0x00 */
    unsigned int unknown_04;
    unsigned char load_enabled;          /* observed byte read at +0x08 */
    unsigned char _padding_09[3];
} weapon_type_context_t;
_Static_assert(offsetof(weapon_type_context_t, load_enabled) == 8, "PE field offset");
_Static_assert(sizeof(weapon_type_context_t) == 12, "PE x86 context layout");

void WeaponType_Open(weapon_type_context_t *ctx);
