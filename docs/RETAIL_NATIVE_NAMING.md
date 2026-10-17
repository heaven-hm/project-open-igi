# Retail Native Naming and Provenance

C++ methods that port or directly model a retail `igi.exe` game native use a readable subsystem/action name:

- `Game_SetThing()`
- `Level_LoadParams()`
- `HumanPlayer_UpdateMotion()`
- `Weapon_Fire()`
- `SoundSystem_DefineSound()`

Use full words and stable names. Do not use address suffixes, numbered guesses, or shortened variable names as substitutes for recovered meaning.

Put a nearby source comment on each native port that identifies it as an original IGI.EXE native and records the retail virtual address. Include the available REA evidence ID and static CFG result when known. State clearly when the C++ function is only a helper or partial port; a related native address does not prove that the whole retail routine has been reproduced. Runtime parity must be described separately from static identification.

Keep QVM symbol strings exactly as shipped in the retail scripts because they are part of the script ABI. The C++ wrapper or implementation name may use the subsystem/action convention, but the QVM symbol itself must not be renamed.

## Current implementation notes

- `Weapon_ComputeAccuracyCone()`, `Weapon_SampleAccuracyCone()`, and `Weapon_SampleShotDirection()` port selected behavior from original `Gun_Fire` at `0x00478BA0`; they are helpers, not separate retail entry points.
- `Weapon_SampleRecoil()` ports selected recoil behavior from original `Gun_FireMessage` at `0x00479780`; it is a helper, not a separate retail entry point.
- `HumanPlayer_CanActivateLadder()` ports the activation-eligibility check associated with original `Ladder_Update` at `0x004404C0`; it is not the complete native routine.
- The ladder mount path ports selected placement and heading effects associated with original `Ladder_Mount` at `0x004113E0`; runtime parity remains unverified.
- Retail-derived sound parsing, definition, bank, and mixing routines use the `SoundSystem_` prefix; comments identify the corresponding retail routine or callback and available evidence. Portable SDL and file-system adapters are not retail Windows natives.
