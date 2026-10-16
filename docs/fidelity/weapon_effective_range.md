# Original weapon effective range

Verified correction, 2026-10-16. Scope: original range loading and projectile
expiration in the C++ game. Sound and cutscenes are unaffected.

## Sources reused

The implementation mapping came from the existing Binary Ninja exports under
`/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/`:

- `natives/misc/00413F90-DefineWeaponType.c`
- `natives/gunshot/0047B190-GunShot_TakeSlot.c`
- `cproj/WEAPON_FIRING_RESEARCH.md`

The C# vanilla candidate agrees at
`open-igi/src/OpenIGI.Game/Weapons/WeaponTypeRegistry.cs`: argument 25 is
EffectiveRange. These exports were inspected as original-logic candidates,
then cross-checked against machine instructions, not accepted from names alone.

## Retail evidence

Original executable: `/Users/haseeb-mir/Downloads/IGI/igi.exe`.
SHA-256: `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`.

- `DefineWeaponType`, `0x00413F90`: reads real argument 25 through
  `0x004B8A50` and stores it at table base `0x0056E258` plus field offset
  `0x650`, stride `0x6E8`. At `0x00414378`, the store is FSTP to
  `0x0056E8A8 + record offset`.
- `GunShot_TakeSlot`, `0x0047B190`: FLD at `0x0047B1AC` loads field
  `0x650`; FMUL at `0x0047B1B2` multiplies by original constant 4096.0
  (`0x005333B8`); FSTP at `0x0047B223` stores remaining distance at slot
  offset `0x34`. This is a float store, despite a misleading integer cast
  in the raw Ghidra pseudocode.
- Local REA with Ghidra 12.1.2 analyzed exact original instruction windows:
  `weapon-definition.bin`, file offset `0x13F90`, 2165 bytes, load address
  `0x00413F90`; evidence
  `ev_c9388fd7464134ae0b32a09f2f3c7b535bc7d939caa3d6116a2f21c77d9bc362`.
  Ghidra/radare2 block counts: 15/15, block-count score 1.0.
- `shot-slot.bin`, file offset `0x7B190`, 219 bytes, load address
  `0x0047B190`; evidence
  `ev_aa3ec18692d42eb9c2677e0318a13076fcada11b10f2101cdddc798a920c57dd`.
  Ghidra/radare2 block counts: 6/6, block-count score 1.0.
  These are raw-window analyses with explicit parent offsets, not claims
  that the complete binary database defines every function.

Original weapon `.qvm` files were independently decompiled with the existing
`project-igi-converter/build-macos-cli/bin/igi1conv` into `.qsc` under `/tmp`:

| Weapon | Argument 25, metres |
| --- | ---: |
| MP5 SD3 | 80 |
| Glock 17 | 80 |
| AK47 | 2000 |
| Knife | 1 |

## Game correction

The loader now takes argument 25 instead of assigning every firearm 400 metres
and every knife 1.8 metres. It rejects non-finite/non-positive ranges and logs
loaded values at debug level. Player projectiles already consumed the parameter;
enemy projectiles now consume the same value instead of a fixed 100 metres.
The native game uses metres, so it stores the original metre value directly.

## Execution and memory verification

`engine/tools/retail_weapon_range_probe.cpp` is an isolated Windows x86
analysis tool, not a Windows dependency of the game. It reads the original
219-byte shot-slot routine from the supplied executable and executes it under
CrossOver. Only the four-byte FMUL constant address is relocated to a host
4096.0f value; every other instruction byte is compared with the input before
execution. No game entrypoint or imports are executed.

Eight ranges (1, 1.8, 20, 80, 100, 400, 1000, 2000) pass bit-level range checks.
The complete 1600-byte shot object matches its expected post-call memory,
including allocation counters, active-slot pointer, direction, weapon pointer,
owner and preserved zero fields. This is controlled isolated retail-code
execution with an address relocation, not a live full-game capture.

Reproduce on this Mac:

```sh
i686-w64-mingw32-g++ -std=c++20 -Wall -Wextra -Wpedantic -Werror -static \
  -Wl,--image-base,0x10000000,--disable-dynamicbase \
  engine/tools/retail_weapon_range_probe.cpp -o /tmp/retail-weapon-range.exe
LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8 \
  /Users/haseeb-mir/Applications/CrossOver.app/Contents/SharedSupport/CrossOver/bin/wine \
  --bottle project-igi 'Z:\tmp\retail-weapon-range.exe' \
  'Z:\Users\haseeb-mir\Downloads\IGI\igi.exe'
```

The original Level 1 weapon-data regression and the enemy expiration regression
both failed before correction and passed afterward. All 19 functional CTests
pass (`ctest -LE retail`); the separate full retail parity gate is not included
and remains incomplete. A 300-frame Level 1 headless run using high graphics
and original assets exits cleanly after 300 simulation ticks.

Logs and raw evidence: `/tmp/igi-qvm-retail-evidence/`,
`/tmp/igi-range-functional-suite.log`, `/tmp/igi-level-one-range.log`.

## Remaining fidelity work

This correction does not establish exact weapon behavior overall. Spread/RNG,
recoil, burst scheduling, penetration, hit zones, animation timing, AI behavior
and matching full-game event traces remain required for Level 1 acceptance.
