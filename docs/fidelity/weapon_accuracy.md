# Weapon cone, projectile count and recoil sampling

Retail executable SHA-256:
`dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`.
Workflow: `open-igi/RE.md`. The application remains experimental.

## Sources reused

The source bodies are the existing exports in
`project-igi-natives/assets/BinaryNinja/natives/gun/00478BA0-Gun_Fire.c`
and `00479780-Gun_FireMessage.c`. Vanilla C# `GunFiring.cs`, `HeldGun.cs`
and `WeaponTypeRegistry.cs` provide typed candidates for the same routines.
The current live Binary Ninja database agrees on their instruction ranges.

`Gun_Fire` selects minimum spread while the pre-shot burst count is <= 3,
maximum afterwards, converts degrees to radians and takes the absolute value.
For every configured projectile, a nonzero cone consumes three uniform draws
whose product gives radius, then a fourth for bearing. A zero cone skips those
draws. One round consumes one magazine entry regardless of projectile count.

Even while sound output is deferred, the semiautomatic path's sound-variant
word draw is preserved before cone sampling. `Gun_FireMessage` then consumes
random pairs until their doubled centered values lie within the unit disc,
and computes signed fixed/random recoil terms. `Muzzle_Eye` (`0x462E50`)
initializes two per-update kick outputs, dispatches the active weapon callback,
adds those outputs to retained values at `+0x128` and `+0x134`, then multiplies
each sum by `0.899999976`. The C++ port now retains the sampled pitch/yaw,
applies that accumulation and decay on each live simulation tick, and uses the
resulting view angles for rendered aim and subsequent shot direction. Player
movement heading remains separate.

The retail accumulator operation is directly present in the executable. The
mapping from its retained fields into the port's rendered pitch/yaw basis is
inferred from the callback data and the C# vanilla reference; a paired retail
runtime capture is still required before this can be called behavior-verified.

The typed implementation is `WeaponAccuracy`; the runtime uses the verified
`RetailRandomGenerator`. Mission loading retains original `DefineWeaponType`
arguments 14 and 18..23 instead of discarding them. Non-integer projectile
counts, out-of-range integer conversions and non-finite spread/recoil values
raise controlled exceptions. Debug shot logs include cone and projectile count.

## Static and script evidence

The retained REA/Ghidra original instruction-window analysis of `Gun_Fire` is
`ev_b067ac83a54f37bd0c6e3a05c7be3da2918bf7db4c48f3f6fc18138542c4d0f2`.
Its 3,035 bytes were rechecked against the current original image at file offset
`0x78BA0`. Ghidra, Binary Ninja and radare2 each recover 78 blocks.

The original 316-byte fire-message window at `0x479780` was analyzed through
REA/Ghidra: `ev_46829eb95cce685f8e9d52701d5da1cfce09db209e6a12d0dfa849f4a014a004`.
Ghidra and radare2 each recover nine blocks. This is instruction-window
evidence with unresolved external callees, not a complete original game replay.
Block-count agreement alone is not an equivalence proof.

REA/Ghidra analyzed `Muzzle_Eye` at `0x462E50` in evidence
`ev_be8eb36c821e85df2d118478691f88b534a9eabde5c51900b7b46cf2eba15d78`; its
instruction listing is `ev_8887279b4a4043274d99617d99bce21e32f71caf379a1bfa3dd46d250c8f982e`.
The Ghidra body and radare2 6.2.0 both identify a 550-byte function with nine
basic blocks. The final instructions add callback outputs to the retained
values at `+0x128` and `+0x134`, multiply each by binary constant `0x3f666666`
(`0.899999976`), and store them. The Ghidra decompile shows both outputs are
initialized to zero before dispatch. This statically verifies accumulator
arithmetic and ordering, but not how the original renderer consumes those
fields.

The existing `igi1conv` decompiler produced readable views of the shipped
`WEAPONS/MP5SD/WEAPON.QVM` and `WEAPONS/GLOCK/WEAPON.QVM`, retained as
`/tmp/igi-mp5-weapon.qsc` and `/tmp/igi-glock-weapon.qsc`. Both declare one
projectile per round. MP5 has spread -2/+2 degrees, fixed/random pitch 0.8/0.8;
Glock has -1.5/+1.5 degrees and fixed/random pitch 1.5/0. Original-asset
integration assertions check these values after mission loading.

## Regression and runtime checks

Before implementation, the three-projectile regression failed because the
runtime emitted only one straight projectile. It now passes alongside checks
for normalized nonzero-cone directions, absolute signed spread, burst boundary,
the three-sample product, zero-cone draw suppression, fixed recoil draw count,
zero-projectile bookkeeping, invalid values and deterministic restart.

The 21 functional CTests pass. Focused accuracy, gameplay and original-Level-1
tests also pass under ASan/UBSan, with macOS leak detection disabled.
After the view-recoil update, the focused weapon, renderer, movement/weapon/AI,
navigation/inventory, real Level 1, and Ultra preset checks passed 6/6.

A 120-tick Level 1 port replay fired 18 MP5 rounds, sampled a positive pitch
kick on each shot, advanced RNG state only on fired ticks, then completed its
reload to 32 rounds. Trace: `/tmp/igi-level-one-accuracy-trace.csv`.
Log: `/tmp/igi-level-one-accuracy.log`. The trace now exposes projectile count,
sampled recoil and all three RNG words for future aligned comparisons.

## Remaining parity limits

This does not establish natural retail-versus-port shot equivalence. The
prototype initializes its stream from the verified constructor state on
restart; mission setup and missing shared consumers can advance the retail
stream differently. Captured state and consumer order must be aligned.
The C# polar-direction candidate uses host trigonometry and normalization;
its correspondence to the original basis/matrix path still requires direct
runtime comparison, including x87 versus ARM64 rounding. Recoil sampling is
sampling and accumulation are implemented, but the retained-field-to-camera
mapping, integer cooldown/tick order, reload state-machine timing, body hit
zones and AI firing policy remain open. The full retail gate must continue to
reject missing comparison captures.
