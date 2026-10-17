# Level 1 weapon checkpoint: manual checks

> Historical record for a prior macOS gameplay-prototype branch. Its build paths,
> controls, and results do not describe the current Windows x86 game-engine-impl
> branch or the current MVP. The authoritative current scope is [MVP.md](../../MVP.md)
> and its sequence is [plan.md](../../plan.md). The historical test record below is
> retained as provenance only; it is not current acceptance evidence.

This was an experimental checkpoint, not completed Level 1 MVP acceptance.

## Build delivered

- Source: `/Users/haseeb-mir/Documents/Code/project-open-igi`
- Branch: `feature/gameplay-prototype`
- Gameplay revision: `9a3c71f` (preceded by range correction `18c821b`)
- Build: `/tmp/igi-openigi-qvm-review.N3YiRT`
- Bundle: `/Users/haseeb-mir/Downloads/IGI/igi-level1-9a3c71f.app`
- Executable SHA-256: `b922e67d5bab2e5aaaf9967349be52d7f123c72be03327d8a2776355f0292cab`
- Bundle signature: strict verification passed after installation and copying.

Launch the tested executable directly:

```sh
/Users/haseeb-mir/Downloads/IGI/igi-level1-9a3c71f.app/Contents/MacOS/igi \
  window level1 --graphics high \
  --game /Users/haseeb-mir/Downloads/IGI
```

For Finder/LaunchServices, pass application arguments after `--args`:

```sh
open -n /Users/haseeb-mir/Downloads/IGI/igi-level1-9a3c71f.app \
  --args window level1 --graphics high
```

The direct executable launch was checked from the final delivered path. That
check does not establish successful LaunchServices startup on every configuration.

## What to test

1. Switch weapons with right mouse (next), O (previous), or the mouse wheel.
   Check Glock, MP5 and knife equip without losing their separate magazines.
2. Glock: hold left mouse. One shot should fire per press; release and press
   again for the next shot. Automated Level 1 replay confirmed one shot during
   60 held ticks and two cumulative shots after a release and another hold.
3. MP5: hold left mouse and check magazine depletion. Release the trigger,
   press F to reload, wait for reload completion, then fire again. The original
   configured burst ceiling is 32; the original clip also holds 32, so this
   manual check alone cannot distinguish the burst gate from an empty clip.
   The dedicated automated three-round-burst test checks the separate gate.
4. Knife: test at close reach; the original effective range is 1 metre rather
   than the previous 1.8 metres. MP5/Glock use 80 metres; AK47 uses 2000 metres.
   Visual distance estimates are not proof of exact range parity.
5. Basic regressions: walk, jump with E, crouch with C, and try building
   openings. Report any stuck position, failed weapon change, missing shot,
   health loss without visible cause, or crash with the weapon and location.

If comparing with retail, use the same weapon, location, trigger sequence and
original assets. Bullet spread/recoil and overall graphics still differ; this
checkpoint does not claim those systems have been completed.

## Automated checks already run

- All 19 functional CTests passed after the gameplay changes. The required
  full-game retail parity gate is excluded from that count and remains open.
- Three focused error/gameplay tests pass after adding zero, negative and NaN
  range rejection checks; the fresh ASan/UBSan gameplay probe also passes.
  Leak detection is disabled on macOS.
- Original range native executed in CrossOver for eight ranges with shot-object
  memory checks; original blocked-fire/release native paths passed six cases.
- Staged and installed bundles each completed three windowed frames cleanly.
  A screenshot from the staged run was inspected; it demonstrates rendering,
  not pixel-level retail equivalence.

Evidence: [range](weapon_effective_range.md), [burst gate](weapon_burst_gate.md).
Runtime logs: `/tmp/igi-level-one-manual-gui.log`,
`/tmp/igi-level-one-installed-gui.log`, `/tmp/igi-level-one-burst.log`.
Exception/sanitizer log: `/tmp/igi-manual-error-tests-asan.log`.

## Next required work

Original cooldown rounding/tick order, spread/RNG/recoil, AI/archetype scripting,
mission objectives, collision/building coverage, gameplay animations and rendering
parity remain required. Continue Level 1 work after this incremental checkpoint;
manual checks here do not accept the whole MVP or authorize expanding scope.
