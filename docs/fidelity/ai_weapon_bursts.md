# Level 1 AI weapon burst and cadence port

Retail executable: `/Users/haseeb-mir/Downloads/IGI/igi.exe`  
SHA-256: `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`

## Retail evidence

- REA/Ghidra pseudocode for `0x48B9C0`, evidence
  `ev_06e5403057d4c8b07840e33a2f41c9b3efad501aa2de5a578fb6a379388d2ff8`,
  shows the routine reads the weapon clip/burst configuration, calculates the
  AI burst count from the configured minimum and random clip fractions when the
  weapon burst is not a separate fixed burst, caps that count, calculates the
  distance-scaled hit probability, and applies the first-fire half factor.
  Ghidra reports one contiguous 521-byte function body at `0x48B9C0..0x48BBC8`.
- REA/Ghidra pseudocode for `0x48D080`, evidence
  `ev_da225611ca73b5ac07e59d84a2f8d44de0e628ae33e0a035dbbb7b0880ef5b59`,
  shows the combat task starts the fire sequence, calls burst setup, and clears
  the first-fire flag after setup.
- REA/Ghidra pseudocode for `0x48D8E0`, evidence
  `ev_25533f5fd01f5afc197ead625880c2ecbb62fc2ac79c009848d2085a794c682e`,
  shows the fire task counts rounds when the weapon clip changes and updates
  the aim point. Vanilla C# `AiGuardCombat.Firing` mirrors this clip-delta
  accounting and applies the configured extra delay to single-round bursts.
- `0x48B9C0` multiplies the clip count by the generated float fraction, then
  converts with x87 `FISTP`. REA/Ghidra pseudocode for the game startup path
  `0x48F530`, evidence
  `ev_49b7b1cdd00663e55d99157b40d1ed7a7ee9eb7476a4cdc42cf4fa0e56a238b4`,
  shows startup calls `0x498460` before entering the game loop. That function's
  pseudocode, evidence
  `ev_f5e7c63918c2ad7a8dd1b9b29e3a7930d191693cee9310bfbcf559e5cad12b3f`,
  sets the x87 control word's rounding-control bits with `| 0xC00`, selecting
  truncation. The clip product must not be rounded to single precision before
  `FISTP` converts it.
- Vanilla C# `SoldierRenderer.FireIfAsked` decrements weapon cooldown each tick,
  reduces the loaded clip only when the body's fire state forwards its trigger,
  and sets the next cooldown with `GunTiming.TicksPerRound`:
  `apps/OpenIGI.Desktop/Rendering/SoldierRenderer.cs:2007-2046` in the
  `open-igi` reference project. Reload uses the selected weapon's configured
  reload seconds; the C# AI returns to its stand phase on the completion tick.
- Independent radare2 analysis of the same retail digest reports
  `fcn.0048b9c0` as 521 bytes, with entry `0x48B9C0` and end `0x48BBC8`:
  `r2 -q -A -c 's 0x48b9c0; pdf' /Users/haseeb-mir/Downloads/IGI/igi.exe`.
  This independently confirms the function extent and the burst/config field
  accesses. It does not establish source-level equivalence.

The vanilla C# burst calculation is in
`src/OpenIGI.Game/Ai/AiCombatBurst.cs:42-101`; its guard state is in
`src/OpenIGI.Game/Ai/AiGuardCombat.cs:267-375`. These files are used as a
porting reference only. They do not replace the retail binary as ground truth.

## C++ implementation

`mission_gameplay.cpp` reads the selected difficulty's `HumanAIConfigItem`
values for maximum rounds per sequence, clip fractions, hit probabilities, aim
precision, and single-shot delays. It also reads each weapon's category,
magazine size, fixed burst size, rounds per minute, reload time, and spread from
its original `weapon.qvm` definition. Invalid values fail scene loading with a
specific exception.

`GameSimulation::advance_actors` now tracks each actor's loaded magazine,
weapon cooldown, active burst, rounds left, single-shot delay, and reload timer.
It chooses the burst length and hit chance once at burst start, applies
first-fire accuracy to that burst, uses the weapon's 30 Hz cooldown and spread,
and refills the magazine after the weapon's reload time. The clip fraction is
multiplied in double precision before conversion to an integer, preserving the
retail x87 truncate behavior. The reload completion tick returns through the
stand phase before another burst begins. Logs report the loaded AI row, burst
count, fixed burst hit chance, shot count, remaining magazine, reload start,
and reload completion.

## Verification

- `mvp_movement_weapon_ai` checks a configured three-round burst, 700 RPM
  two-tick shot cadence, 2-second reload, magazine restoration, and a
  near-integer clip fraction that distinguishes retail truncate semantics from
  an intermediate single-precision rounding step.
- `mvp_original_level_gameplay` loads original Level 1 assets, verifies a real
  guard's combat row and weapon definition, and requires timestamped burst and
  shot events.
- Both focused probes pass in the current build.
- Runtime capture: `/tmp/igi-level-one-ai-burst-runtime.log` and
  `/tmp/igi-level-one-ai-burst-runtime.trace`. Guard 2017 loaded `GD_2`, burst
  cap 30, clip fractions `0.2 + random * 0.3`, and aim precision `0.8`. The
  captured bursts were 13 and 11 rounds, with hit chances `0.432357` and
  `0.864714`; the trace includes both hit and miss outcomes.

These checks validate the C++ port against synthetic timing assertions and
original Level 1 data. They do not compare paired per-tick retail and C++ AI
state. The complete AI task tree is not ported: the C++ actor still turns
instantly, does not gate trigger forwarding on body animation state, and pauses
firing while the player is not visible. It also does not yet handle grenade
actions, evasive states, or NPC-to-NPC hit-probability rolls. Exact retail
behavior remains unverified.
