# Jump motion and separate impact velocity

Reference: retail `igi.exe`, SHA-256
`dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`.
Workflow: `/Users/haseeb-mir/Documents/Code/open-igi/RE.md`.

## Existing implementations reused

The launch calculation is transcribed from the existing native export
`project-igi-natives/assets/BinaryNinja/natives/humanplayer/0040BA70-HumanPlayer_HandleInput_4.c`.
The related exports at `0x408460`, `0x4614B0`, `0x4632D0` and `0x40BE40`
establish gravity, separate motion/impact fields, consumption of inherited
movement, and clearing both fields on landing. Vanilla C# `PlayerBody.cs`
contains the same previous-animation-step launch and takeoff tick ordering.

`Human+0x664..0x66C` stores movement velocity. `Human+0x670..0x678` stores
the separate impact impulse. The movement integrator adds both, but applies
its `0.9` damping only to the impact impulse and slope acceleration. The C++
player incorrectly stored airborne motion in the damped field. Descriptive
field names now make the distinction explicit throughout callers and tests.

The jump entry reads the previous world-space animation delta at `0x520` and
`0x524`, normalizes it, scales it by the configured horizontal launch speed,
adds inherited movement, and installs the airborne callback without calling
it. Gravity therefore starts the following tick. Its explicit float stores on
the horizontal X normalization path are retained. ARM64 `long double` is not
x87 extended precision; general diagonal bit equivalence remains unverified.

## Static evidence

- REA/Ghidra airborne `0x408460`:
  `ev_63600d5cab4a5c2a4978fbb351941a3e746a0a3ae2811039431b93fb1fdca044`.
- REA/Ghidra integrator `0x4614B0`:
  `ev_d263f1949e2f0e462d427be609dd5daadc83058dabb4d31b6d8584c3b652a1f1`.
- REA/Ghidra landing `0x40BE40`:
  `ev_daad5c897d9921e3271621d221c80c91ad5ead3765264323afae7778770843c2`.
- REA/Ghidra inherited-movement consumption `0x4632D0`:
  `ev_6e20147806eb9e47a5df6501f110883ddfb35d7658711635be6b3d69d362c2bc`.
- The full-image Ghidra database lacks the jump entry. Its exact 347 original
  bytes were analyzed separately at original load address `0x40BA70`:
  `ev_6a09a7b73380634bb4f12488444f774ea4b44087324af036fb1fb28eda0eb0f0`.
  Window SHA-256: `067a8acf5e1fd8d519b093198530dcc6463624ed4677bdef165c9df43539159a`.
  This is an extracted instruction-window analysis, not a different retail build.
  Ghidra, live Binary Ninja MCP and radare2 each recover eight basic blocks.
  Block-count agreement alone is not a behavioral-equivalence score.

## Regression and execution

The new gameplay regression failed before correction at the launch-height
assertion. It checks full takeoff velocity, next-tick gravity, undamped
horizontal motion after releasing movement before jump, and no post-landing
drift. The movement composition test still verifies damping of impact impulses.

A 90-tick Level 1 headless replay on original assets, stationary jump on tick
11, gives these port observations:

| Measurement | Previous release | Corrected port |
| --- | ---: | ---: |
| Takeoff displacement | 0.206379890 m | 0.250000000 m |
| Maximum rise | 0.746271490 m | 1.636266474 m |
| Ticks observed airborne | 17 | 25 |
| Returned to ground | Yes | Yes |

Captures: `/tmp/igi-level-one-jump-before.csv`,
`/tmp/igi-level-one-jump-after.csv`, and
`/tmp/igi-level-one-jump-comparison.json`. These are port-before/port-after
captures, not a retail-versus-port gameplay replay.

All 20 functional CTests pass. ASan/UBSan gameplay, movement-integrator and
original-Level-1-asset tests pass with macOS leak detection disabled. Evidence
logs are retained in the respective build trees' `Testing/Temporary` folders.
Full natural retail jump capture, collision parity, airborne animation-root
motion, edge walk-off entry and general numerical equivalence remain open.
