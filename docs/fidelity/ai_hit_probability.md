# Retail AI aim point and hit probability

Reference executable: `/Users/haseeb-mir/Downloads/IGI/igi.exe`
SHA-256: `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`

## Retail evidence

- `0x406F70`, REA/Ghidra evidence `ev_4052e7f313b4f539848da6eadcc3669d0c286226eaa525b247b50c2c4566515a`, copies the configured minimum and maximum hit probabilities from the selected `HumanAIConfigItem` to the live AI record and copies aim precision separately.
- The same `0x406F70` body copies `HumanAIConfigItem+0x2A0` into `Human+600`; the vanilla C# `AiCombatParameters.MaximumHealth` uses that value as the health threshold. The weapon firing path instead supplies the held weapon's damage factor to the human-hit resolver.
- `0x48B9C0`, evidence `ev_da3b5974598376d1fffb3b3310ca68b9bf7d19f657de5ee67161a0f95ce29a68`, calculates hit probability by interpolating the row's minimum and maximum values over the primary view length. It clamps distance to that length and halves the probability when the first-fire flag is set.
- `0x48D080`, evidence `ev_5bf96696dc94d59c43c6c9b6927fb907af7e4ad8f3a43551a76bee22653242a4`, calls the burst setup routine and then clears the first-fire flag.
- `0x48D8E0`, evidence `ev_997e84950651a7bb9534534d9eb3fb75cf520f0c6b37e5b3b51d16f85fd21fd0`, moves the retained aim point toward the shared target record using the configured aim-precision scalar.
- `0x451380`, evidence `ev_479ceb416f232b43eda51bf92e535cc7c90585ef1646a9b5198e857f3e4b2bc4`, copies the target's position and raises Z by 2048 units standing or 1024 units crouched (0.5 m or 0.25 m).

The vanilla C# reference implements these behaviors in `AiCombatBurst`,
`AiGuardCombat`, and `AiSoldier.AimPointOf`. Its `HumanAiConfigItemTask`
parameter order was used to read the corresponding values from the C++
initializer's `Task_New` arguments. Level 1 runtime logs confirm that guard 2017
resolves `GD_2` values `hit probability 1.0..0.3` and aim precision `0.8`.

## C++ changes

Mission loading now resolves the selected difficulty's AI accuracy row and
validates probabilities and aim precision as finite values in `[0, 1]`. AI
projectiles aim at the authored standing/crouching target mark and update their
retained aim with that row's aim-precision value. The hit chance is interpolated
from the row using the current distance and a 26 m primary-view default. The
first shot after acquiring the player has its probability halved. Projectile
collision rolls that probability only after intersecting the player; a failed
roll logs a miss and the projectile continues.

The same `HumanAIConfigItem`'s field named `Damage scale` initializes the
soldier's maximum health. It does not multiply the damage of rounds the soldier
fires. Enemy shot damage now comes from the loaded weapon definition, matching
the vanilla C# `FireIfAsked`/`TryShootAiRound` path.

The old tick/actor-index trigonometric aim perturbation was removed. It had no
support in the retail routine or the vanilla C# implementation.

## Verification

- `mvp_movement_weapon_ai` verifies that a zero-probability AI round does not
  damage the player while the guard still fires, and that the AI maximum-health
  setting changes the soldier's health without scaling its weapon damage.
- `mvp_original_level_gameplay` loads Level 1, checks the real guard's resolved
  settings, finds a clear in-cone guard/player placement, and verifies timestamped
  fire events.
- A real Level 1 runtime capture recorded guard 2017's first shot at probability
  `0.432692`, a probability miss, then subsequent shots at `0.865385`. Five hits
  used the loaded AK damage of `0.3` and reduced player health from 3.0 to 1.5.
  The same run defeated guard 2017 in four `0.15` hits against its configured
  `0.5` maximum health.
- Clang static analysis reported no diagnostics for the changed gameplay and
  probe translation units.

## Remaining differences

This is not full AI combat parity. The C++ actor scheduler still uses a fixed
cooldown instead of the retail burst, trigger, weapon cadence, and reload task
state machine. The first-fire probability is therefore halved on the first shot
rather than once per burst. The primary view length uses the constructor's 26 m
default; the C++ mission path does not yet execute AI QVM view-length overrides.
NPC-to-NPC hit-probability rolls are also not implemented. No paired retail
runtime trace has established full behavioral equivalence.
