# Screenshot evidence status (level-1 MVP candidate)

**Current verdict: NO VALID LIVE-FRAME SCREENSHOT for any candidate in this lineage.**

Evidence: Tester's lifetime + capture study on frozen `c2c2ac1f`
(`docs/fidelity/evidence/LIFETIME-AND-SCREENSHOT.md`, `lifetime-study.json`).

## Facts

- Lifetimes are bimodal: cold 2.6-3.4 s vs warm 0.52-0.59 s, same binary, same
  entry, same exception, exit code 3 every run with the identical stderr tail.
  So the earlier 1.03 s exit was warm-state timing, not a startup change.
- On a 3.4 s run a Quartz window poll saw **no on-screen wine/igi window at any
  time**; only an offscreen 500x500 `wine` window at 2.1 s.
- 2.0 s and retimed 1.5/0.8 s captures are pixel-equivalent to the desktop
  (mean abs diff <= 0.007/255), even when the process was alive across the
  capture.

## Consequence

- `screenshots/internal/2026-10-24-igi1-prefix.png` and the retimed variants are
  **INVALIDATED** and must not be cited as a rendered frame.
- No release, no version, no changelog "runs" claim is permitted until a real
  visible frame is captured and proven (process alive before/after plus pixel
  difference from desktop, or an in-process render capture).
- The likely cause is that the visible window is created after
  `QTask_RegisterClasses` returns, which currently always throws; the fix is to
  transcribe the registrar chain, not to retime captures.
