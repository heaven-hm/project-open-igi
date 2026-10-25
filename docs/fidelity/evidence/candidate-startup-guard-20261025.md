# Candidate runtime observation: startup stops at the 0x00402930 fail-closed guard

Recorded 2026-10-25. First run of the freshly built candidate after the recent integration wave, under CrossOver/Wine with the retail command line.

## Conditions

- Candidate: `build-win32-no-verify-gate/igi1.exe`, 14,269,804 bytes, SHA-256 `f253028eb35db4ea…` (copied into the game directory as `igi1.exe`).
- Runner: `/Users/haseeb-mir/Applications/CrossOver.app/Contents/SharedSupport/CrossOver/bin/wine`, `CX_BOTTLE=open-igi`, `IGI1MUTEX=1`, working
  directory `/Users/haseeb-mir/Downloads/IGI` (retail startup checks `YMBE.AFP` by relative path).
- Command line: `igi1.exe window level1`, waited 30 s, then enumerated on-screen windows with Quartz.

## Observed

```
2026-10-25 07:04:44.171 igi1.exe[15228:8000486] +[IMKClient subclass]: chose IMKClient_Modern
terminate called after throwing an instance of 'std::logic_error'
  what():  QTask_RegisterClasses (0x00402930): the retail function at this address is the startup init
  sequence, 155 ordered init/open/load calls ending in a tail jump to 0x004CEF30, not a class registrar;
  it is not transcribed, 146 of its 154 distinct callees have no engine implementation, and it stops at AI graph
```

No window owned by the candidate or by Wine appeared in the Quartz enumeration: the process terminates before creating one, so there is nothing to
screenshot. An earlier attempt with a relative executable path failed differently (`winewrapper.exe:error: cannot execute L"igi1.exe window level1"`)
and is recorded only as a runner detail: the candidate must be launched by absolute path under this CrossOver wrapper.

## What this does and does not establish

- It establishes that the candidate reaches the transcribed WinMain path and the DirectX probe, gets as far as the startup init sequence, and then
  takes the fail-closed branch. That is a real runtime observation of the guard, not of the natives behind it.
- It does **not** establish anything about the 154 callees of `0x00402930`: none of them was reached, so every status for them stays
  `BUILD_VERIFIED` with runtime unobserved.
- The message text carries hardcoded counts ("155 ordered … calls", "146 of its 154 distinct callees"). They were accurate when written and will drift
  as registrars land; the guard should either derive them or be updated whenever the count changes. This is a maintenance hazard in the message, not a
  behavioural difference, but a stale number in a diagnostic string is still a claim that becomes false.

## Consequence for the objective

The playable path is blocked exactly here. Level 1 cannot render until `0x00402930`'s call sequence is transcribed (or the guard is removed, which
would be a rule-2 violation because the guard is what prevents a partial startup from silently shifting task-class ids). The ordered call inventory
lives in `docs/fidelity/evidence/startup-402930-call-sequence.md`; the lane is working down the 109-callee unblocked pool smallest-first.
