# Lifetime study and screenshot invalidation - frozen `c2c2ac1f`

Frozen sha `c2c2ac1f10a3ed79633667af053ae114ca334ffe2d2e8562c12a711058a44922` re-verified before/after every run; pair untouched.

## 1. Lifetime study (5 sequential runs each)

Driver methodology matches the stored samples (spawn-suspended Frida, elapsed from resume to process-gone).

| run | driver elapsed s | direct Wine spawn->exit s | target exit |
| --- | --- | --- | --- |
| 1 | 1.041 | 3.049 | 3 |
| 2 | 0.520 | 0.550 | 3 |
| 3 | 0.518 | 0.547 | 3 |
| 4 | 0.520 | 0.586 | 3 |
| 5 | 0.520 | 0.541 | 3 |

Direct runs: exit 3, stderr tail every run:
`terminate called after throwing an instance of 'std::logic_error'` / `what(): QTask_RegisterClasses (0x00402930): the AI graph registrar is outside the MVP scope ...`
Driver runs: result observed, one entry/0 returns, target stderr is not forwarded by the Frida driver (only the direct runs carry the target stderr tail).

## 2. Comparison with stored lifetimes

| frozen pair | stored driver elapsed s |
| --- | --- |
| `a133b6ad` | 4.138 |
| `58c19edd` | 3.105 |
| `88be5b54` | 3.090 |
| `0e0b0d9c` | 2.559 |
| `c2c2ac1f` (initial capture) | 1.031 |

## 3. Interpretation

- The same frozen binary is **bimodal**: cold/direct runs last 2.6-3.4 s, warm/direct runs 0.54-0.59 s, and the driver saw 0.52-1.04 s. The stored last-four lifetimes (2.56-4.14 s) sit in the cold band.
- Same entry, same counts, same registrar logic_error on every run; no new error and no candidate-status change. This is **run-to-run startup/warm-cache variance**, not a deterministic behaviour change tied to the DirectInput globals/0x509CC0 edit. The high machine load (1-min load 3.0-3.6 with CrossOver, Binary Ninja, Electron and Harness active) is consistent with the timing spread, but a cold/warm bimodality is already enough to explain 1.03 s without a build regression.
- Wall-clock load: before `3.59 4.07 4.47`, after direct series `3.03 3.89 4.39`.

## 4. Retimed capture result - NO PASS

| attempt | process alive immediately before / after | pixels vs desktop (mean abs diff / frac >32) | pass |
| --- | --- | --- | --- |
| initial 2.0 s | yes / yes | 0.003 / 0.0 (vs contemporaneous desktop) | **INVALIDATED** |
| early 0.8 s | n/a | desktop-equivalent | **INVALIDATED** |
| retimed 1.5 s | yes / yes | 0.007 / 0.0 | **FAIL** |
| retimed 2.0 s | yes / yes | 0.005 / 0.0 | **FAIL** |

- The initial 2.0 s image was pixel-equivalent to its contemporaneous desktop capture (0.003 mean abs diff); both retimed attempts found the process alive immediately before and after `screencapture`, but the screen was pixel-identical to the desktop baseline (0.005-0.007): no visible Wine window existed at 1.5 s or 2.0 s.
- Quartz window poll during a 3.4 s run: **no on-screen wine/igi window**; only an offscreen 500x500 `wine` window appeared at 2.1 s, and `screencapture -l` failed with "could not create image from window". No visible 640x480 game window was observed.
- Therefore the current `2026-10-24-igi1-prefix.png` and the retimed files are **not screenshot passes** and must not be used as release evidence. Kept for the record; early 0.8 s attempt retained.

## 5. Action

No more capture attempts on this pair. Screenshot evidence needs either a longer-lived/visible window or a window-server capture path; the frozen pair and all native statuses are unchanged.
