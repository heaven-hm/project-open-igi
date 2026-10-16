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
and computes signed fixed/random recoil terms. Those samples are stored and
traced. Camera application and recovery are not completed by this change.

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
implemented, but view recoil, integer cooldown/tick order, reload state-machine
timing, body hit zones and AI firing policy remain open. The full retail gate
must continue to reject missing comparison captures.
