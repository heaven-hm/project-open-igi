# Original player weapon burst gate

Date: 2026-10-16. Retail executable SHA-256:
`dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`.

## Existing source reused and verified

The behavior is transcribed from the existing export
`project-igi-natives/assets/BinaryNinja/natives/gun/00478BA0-Gun_Fire.c`,
checked against radare2 and local REA/Ghidra. The C# vanilla candidate agrees
in `OpenIGI.Game/Weapons/GunFiring.cs` and `GunState.cs`.
`DefineWeaponType` argument 17 is the signed maximum-rounds-per-burst value,
stored at weapon record offset `0x64C` (base `0x0056E258`, stride `0x6E8`).

Original `Gun_Fire` instructions:

- `0x00478C6E` loads current burst count from gun offset `0x140`.
- `0x00478C74` loads weapon limit from `0x64C`.
- `0x00478C7A` compares equality. Equal counts block firing; this is not a
  greater-than-or-equal comparison.
- `0x00478C8A` clears burst count when the incoming fire request is false.
- `0x00479659` increments count after an actual round fires.

REA/Ghidra 12.1.2 evidence:
`ev_b067ac83a54f37bd0c6e3a05c7be3da2918bf7db4c48f3f6fc18138542c4d0f2`.
This is an exact 3035-byte instruction-window analysis, original file offset
`0x78BA0`, load address `0x00478BA0`. Ghidra and radare2 each recover 78
blocks (block-count score 1.0). It is not full-game behavioral equivalence.

## C++ game integration

The loader retains argument 17 instead of reducing it to an automatic/semi flag.
`WeaponState::rounds_in_current_burst` tracks actual rounds. The player firing
gate blocks equality with the configured limit, increments on firing, and resets
on trigger release. Existing per-weapon inventory state carries the counter.
Debug firing logs include the current burst round. Invalid non-integer/out-of-range
configuration values raise an explicit error before integer conversion.

The new synthetic three-round-burst regression failed before correction. It now
checks that holding fire yields exactly three shots, release resets the count,
and another pull allows three more shots. Original asset checks cover MP5=32,
AK47=30, Glock=1 and knife=1 directly from their weapon scripts.

## Original-code execution

`engine/tools/retail_firing_gates_probe.cpp` reads the complete 3035-byte
original `Gun_Fire` body into private executable memory and checks every byte
against the supplied executable. No instruction/address modifications are made.
Supplied transform/orientation buffers avoid the initial helper calls; only
blocked-fire and release paths are executed, avoiding firing/audio dependencies.

Six CrossOver scenarios cover burst limits 1, 3 and 30, each with held trigger
at its limit and with released trigger. They check the cleared firing-result
byte, exact burst-counter result, and unchanged ammunition. These paths pass.
This is isolated original-code execution with synthetic object state, not a
full-game capture or proof that allowed-shot paths and scheduling match.

Build the Windows x86 analysis tool with:

```sh
i686-w64-mingw32-g++ -std=c++20 -Wall -Wextra -Wpedantic -Werror -static \
  -Wl,--image-base,0x10000000,--disable-dynamicbase \
  engine/tools/retail_firing_gates_probe.cpp -o /tmp/retail-firing-gates.exe
```

Run in the existing CrossOver `project-igi` bottle with the original executable
as its sole argument. Native source bytes remain external to this repository.

## Checks and limits

- All 19 functional CTests pass; `-LE retail` excludes the unfulfilled full-game
  retail parity gate.
- A fresh fully instrumented ASan/UBSan build of `mvp_gameplay_probe` passes,
  including the burst, enemy range, movement and combat checks. Leak detection
  is disabled on macOS.
- Controlled original Level 1 Glock replay: hold for 60 ticks, release one tick,
  then hold through tick 120; expected cumulative shots are 1 then 2.

Logs: `/tmp/igi-burst-functional-suite.log`,
`/tmp/igi-burst-sanitized-run.log`, `/tmp/igi-level-one-burst.log`,
`/tmp/igi-level-one-burst-trace.csv`, and
`/tmp/igi-qvm-retail-evidence/retail-firing-gates-checked.log`.

Remaining: exact integer cooldown conversion/tick order, spread/RNG, recoil,
allowed-shot native paths, reload/holster state-machine transitions and AI
firing policy. The existing research report calls the cooldown a float, whereas
retail reads/decrements an integer and uses FISTP for conversion. Its rounding
control context needs verification before changing cadence. Do not treat the
C# truncation claim as evidence of the original x87 control word.
