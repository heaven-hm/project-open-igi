// IGI native provenance: Weapon_AmmoPickup - 0x0045FF80 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0045FF80-Weapon_AmmoPickup.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0045FF80-Weapon_AmmoPickup.hpp"
int Weapon_AmmoPickup(void* context, int* arg2)

// Sig: void Weapon_AmmoPickup(void* player, void* pickupEntity)
// Note: Gives an AMMO pickup to the player (same struct conventions as
// Weapon_GunPickup: player carry-flags and pickup-entity fields drive the
// grant; plays the pickup sound). Params: player = player struct ptr,
// pickupEntity = ammo world entity. Use for custom ammo pickups.
{
    char result = *(context + 0xcf5);

    if (result)
        return result;

    Weapon_EjectShell_8(context + 0x340, *arg2, arg2[1]);
    return Sound_EventTrigger("weaponpickup_1", nullptr, 0);
}
