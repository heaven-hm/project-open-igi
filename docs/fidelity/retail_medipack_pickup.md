# Retail Medipack Pickup Health Behavior

## Retail evidence

- Image SHA-256: `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`
- Native: `Weapon_GunPickup` at `0x0045FFC0`
- REA/Ghidra evidence: `ev_969c64534bfb476404db23fcf72a6689a539cfbcd9cc0d902adb437647577249`
- Ghidra body: 168 bytes, 14 basic blocks, five calls
- radare2: 168 bytes, 14 basic blocks, 65 instructions, five calls
- Conditional branch counts: Ghidra 10, radare2 11; CFG score `0.970`

For a normally enabled human, the native scans the weapon task list. A duplicate weapon routes its payload through `0x0047CE00` when it has an ammunition record; a new weapon routes through `0x00477C50`. Both paths then trigger `weaponpickup_1` through `0x004E7200`. The native body contains no direct human health update or health-helper call.

## C++ correction

The C++ pickup path no longer restores `0.5` health when the pickup identifier is `WEAPON_ID_MEDIPACK`. It follows the existing weapon-add path and adds the medipack weapon to inventory. A regression first damages the player through a controlled 16-metre platform fall, then confirms that collecting a medipack increases inventory size without changing the remaining health.

## Remaining fidelity work

This verifies that pickup is not a healing action and that the weapon task is added. It does not yet establish the medipack round payload's exact storage field or use/depletion behavior. The native `0x0047CE00` ammo record's third-field cap is also not mapped, so the generic C++ reserve cap remains a separate fidelity issue.

## Verification

- `mvp_navigation_inventory_probe`: passed with the fall-damaged medipack case.
- `igi_game` and the probe rebuilt successfully with the repository's warning-as-error compiler options.
