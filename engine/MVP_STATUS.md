# Experimental gameplay status

Updated 2026-10-17. Current game source: `/Users/haseeb-mir/Documents/Code/project-open-igi`,
branch `feature/retail-gameplay-parity`. Earlier measurements and delivery paths below
were imported from the native-MVP predecessor; they are historical evidence and
do not establish acceptance of the current checkout.

## Acceptance status

Original gameplay equivalence is **NOT VERIFIED**. The first acceptance milestone
is Level 1 only. Movement/building entry, weapons/health, AI/navigation, mission
logic, original graphics/assets and gameplay animations remain required.
The C++ sound engine is partial and remains in scope; movies/movie animation,
cutscenes, subtitles, smoke, optional effects and menu presentation are deferred.
After Level 1 verification, hand off the build
and wait for the owner's manual test before expanding to other levels.
Functional regressions passing does not establish matching original behavior.

## Implemented and checked

- Original converter parsers are reused with their notices and baseline SHA manifest; local formatting and compatibility adaptations exist. Proprietary assets stay external.
- Native macOS C++20 loop, original mission selection, config.qvm input profiles, timestamped logs, replay, state trace, screenshot, and bounded execution.
- Original terrain/models/textures, original movement parameters/root tracks, capsule world collision, jump/crouch, several original weapons, shared ammunition, pickups, simplified projectiles/health/combat.
- Original navigation graphs and authored patrol commands; basic perception and combat. AI scheduling and combat policy remain provisional.
- Recursive visual MEF ATTA transforms, a 32-level/cycle guard, invisible death-zone suppression, and firing-only GUNFLAME classification from original magicobj.qvm.
- Original body and first-person hand rigs, hierarchical skinning and quaternion/tangent sampling, script-selected idle/move/fire/reload view clips, bone attachments. Exact animation transitions, aiming, hit/death sequences and weapon-part motion remain incomplete.
- Partial retail audio path: ILSF decoding, sound-definition and bank loading, a bounded PCM mixer, SDL output, mission music, player jump/fall, weapon fire, and mapped first-person animation-event sounds. Animation-event timing and spatial placement remain C#-derived/unverified; player-body/AI animation sounds, material footsteps and broader QVM audio routes remain incomplete.
- Cinematic branches are suppressed during gameplay-only startup. Level 2 Jones actor 333 is inactive. This suppression is not a complete original conditional interpreter or intro/outro lifecycle.

## Latest bug corrections

1. Building openings: preserve TAMC portal metadata and skip non-solid portals for capsule movement and generic sight/projectile casts. The movement adapter now uses original collision mesh level 1, keeping sight/projectile geometry separate at level 0. A stronger integration replay walks through an original Level 2 opening in both directions for 15 simulation ticks and stays grounded; it uses the full translated collision scene with combat isolated. A bare capsule-plane crossing was insufficient and has been replaced. This does not verify every building, dynamic doors, the full native query dispatch, ladders, or elevators.
2. Muzzle effects: gun-flame attachments are hidden while idle, shown during a shot, then hidden again. They contribute no solid collision. Synthetic rendered checks and original AK47 attachment classification pass.
3. Enemy firing: shots and damage already occurred, but only idle/walk poses were displayed. Firing events now have counters, debug logs, muzzle visibility and soldier firing pose 56 from the original state table. Clear-sight combat holds position rather than applying the previous chase movement above 35 metres. Full original tactical/burst behavior remains unverified.
4. Graphics controls: accept both `--graphics high` and `--gfx high` after macOS `open --args`. Log actual terrain detail, triangle count and viewport. High/low original terrain density differs in regression tests. Retail lightmaps, material blending, sky, shadows and lighting remain incomplete; this setting does not fix all visual differences.

5. Weapon effective range: load the original DefineWeaponType argument 25 for player and enemy projectiles instead of fixed 400/100-metre ranges (knife was 1.8 metres). Original script values include MP5/Glock 80m, AK47 2000m and knife 1m. Two regressions failed before correction and passed afterward. All 19 functional tests pass; controlled CrossOver execution of the retail shot-slot routine verifies eight range conversions and complete shot-object writes. See [range evidence and limits](../docs/fidelity/weapon_effective_range.md). This does not verify spread/recoil or full combat parity.

6. Object placement orientation: compare Level 1 emitted static mesh vertices against an independent matrix evaluation of the retail `Z(gamma) * Y(beta) * X(alpha)` row-vector transform at `0x4B38E0`. The 413-object Level 1 audit, rotated-mesh requirement, and per-object finite/tolerance checks pass. Actor and door transforms are checked from their runtime scene records, while static geometry is checked from emitted vertices. This is a port-side transform check, not a paired retail runtime capture; see [verification evidence](VERIFICATION.md).

7. AI sight target height: use the retail human descriptor at `0x465820` to aim visibility at the target's feet plus `0.69 m` standing or `0.35 m` crouched. REA instruction immediates differ from the current vanilla C# reference's `0.7 m` and `0.319 m`; the port follows the retail bytes. Standing and crouched low-cover probes and the original Level 1 scene probe pass. This change does not establish parity for AI cones, scan cadence, hearing, or combat policy; see [verification evidence](VERIFICATION.md).

8. Terrain quality controls: separate `--graphics ultra` from `enhanced`; Level 2 proves strictly increasing original terrain triangle counts for low, medium, high, enhanced, and ultra. Level 1 headless single-frame runs show the software renderer remains far too slow for dense presets: High 261,144 triangles / 0.345 s, Enhanced 2,087,084 / 1.047 s, Ultra 5,655,922 / 2.453 s. These are one-run application timings, not a stable benchmark. Ultra is opt-in; hardware rendering is still required for a playable high-detail mode.

9. Soldier projectile damage zones: skinned collision triangles retain their MEF bone indices, and player projectiles trace the posed Level 1 soldier mesh before applying the original soldier zone multiplier. The retail mapping and damage tables were read from the executable; the latest real Level 1 probe hit gameplay guard 2017 on bone 14 / zone 5 and defeated it in four logged hits after initializing its configured 0.5 maximum health. This does not yet implement per-body-part damage reactions, enemy damage zones against the player, or prove a retail-versus-port shot trace. See [the verification record](VERIFICATION.md).

10. macOS developer-mode toggle: replace the F11 shortcut with the physical `G` key, while preserving F9 as an alternate, because macOS may consume F11 as a system media/display key before SDL receives it. The SDL event regression verifies `G` regardless of translated case, F9, key-up/repeat rejection, and F11 rejection. Startup and HUD help show the replacement. Manual keyboard delivery still needs an in-app check.

11. Texture filtering: retain authored TEX v11 mip levels, generate additional levels for enhanced filtering, and expose `--texture-filter nearest|trilinear` plus `--texture-anisotropy 1|2|4|8|16`. Default ratios follow the C# presets; the focused renderer probe verifies authored/generated selection, anisotropic detail retention, and serial/parallel consistency. Level 1 runtime captures verify 1x and 16x produce different output. Retail `GR_MIPMAP_NEAREST_DITHER` is not yet reproduced, and this does not establish Vanilla or Enhanced image parity. The CPU 16x path is too expensive for high-resolution gameplay; a GPU renderer remains outstanding.

12. Player weapon view recoil: the retail `Muzzle_Eye` routine at `0x462E50` adds callback pitch/yaw outputs to retained values at `+0x128` and `+0x134`, then multiplies by `0.899999976`. The C++ simulation now accumulates and decays the sampled values each live tick and uses them in rendered aim and subsequent shot direction without turning the movement heading. The state trace records sampled recoil, accumulated recoil, and effective view angles. The isolated weapon regression and Level 1 gameplay, renderer, navigation, and Ultra tests pass. The accumulator operation is statically verified by Ghidra and radare2; the retained-field-to-view mapping has not yet been compared to a retail runtime capture.

13. AI shot aim, hit probability and health scale: remove the invented trigonometric aim offset; use the retail target mark (feet +0.5 m standing / +0.25 m crouched), difficulty-specific hit-probability rows, row aim precision, distance interpolation, and first-fire half-probability. Interpret the row's “Damage scale” as soldier maximum health; calculate fired-round damage from the weapon definition. A real Level 1 runtime scenario verifies guard 2017's logged `GD_2` row, probability misses, subsequent hits and health loss. The default-only view length and absent NPC-to-NPC shot checks remain incomplete. See [AI aim and hit-probability evidence](../docs/fidelity/ai_hit_probability.md).

14. AI weapon burst and cadence: read maximum rounds per sequence, clip fractions, single-shot delay, weapon category, magazine size, fixed burst size, rounds per minute and reload time from the original QVM definitions. REA/Ghidra and radare2 both resolve retail `0x48B9C0` to the same 521-byte body; REA also confirms startup selects x87 truncate mode before gameplay. Vanilla C# supplies the actor fire/cooldown/reload integration reference. C++ now tracks burst, magazine, weapon cooldown and reload state and logs each transition. Synthetic burst/cadence/reload/rounding assertions and the real Level 1 runtime probe pass; guard 2017 logged bursts of 13 and 11 rounds from its `GD_2` settings. Body animation trigger gating, continuous fire after sight loss and paired retail runtime parity remain unverified. See [AI burst and cadence evidence](../docs/fidelity/ai_weapon_bursts.md).

15. Level fog: replace the invented linear gray fade with the Level 1 `GlobalLightKeyframe` exponential fog values, use separate terrain/object colors, and preserve the first-person weapon's unfogged draw. REA/Ghidra evidence verifies the retail `0x527520`, `0x50FD20`, and `0x512C30` calculation. The real Level 1 loader probe and synthetic per-band renderer checks pass. This slice does not implement keyframe animation/night-vision transitions or compare a retail Direct3D 7 capture. See [retail distance fog evidence](../docs/fidelity/retail_distance_fog.md).
16. Object lightmap data: parse the retail OLM archive, map all 22 Level 1 `LightmapInfo` task owners to their named sets, and verify all 1,308 OLM entries. REA confirms the retail `LightmapInfo` task initializer and `DrawLightmapMesh` registration. Page indices through recursive MEF attachment/LOD trees, geometry binding and actual rendering remain incomplete. See [retail object lightmap evidence](../docs/fidelity/retail_object_lightmaps.md).

## Measured original Level 2

| Item | Measurement |
| --- | ---: |
| Soldier declarations / active gameplay guards | 25 / 22 |
| Navigation graphs / nodes / patrol paths | 14 / 465 / 22 |
| Initial supported weapons | 3 |
| Body rig bones / clips | 33 / 95 |
| First-person hand rig bones / clips | 56 / 78 |
| Visual recursive ATTA instances / maximum depth | 98 / 2 |
| High-detail terrain triangles | 351,509 |
| Total static render triangles in current probe | 375,034 |
| Non-solid portal faces / grounded traversal directions checked | 148 / 2 |
| Guards moving more than 0.5 metres in 300 ticks | 6 |
| Enemy shots in that 300-tick scenario | 21 |
| Ending player health in that scenario | 2.85 / 3 |

The count and health results describe this deterministic prototype scenario, not an original-game trace. No passive damage occurs in a 600-tick enemy-free regression. Actual enemy fire can still reduce health.

## Current verification

- Clean committed-source archive and working-tree Release functional suite: **16/16 passed** after portal, muzzle, firing-presentation and graphics-option changes.
- Current orientation slice: Debug build of the affected C++ targets succeeded; **5/5 focused CTests passed**, including the Level 1 original-asset probe. This validates loaded scene geometry against authored `objects.qvm` transforms, not against a recorded retail scene.
- Soldier hit-zone slice: the updated C++ targets built; `mvp_gameplay_probe` and the real Level 1 combat probe passed; the five focused CTests passed. The latest real-asset run hit active guard 2017 on bone 14 / zone 5 and defeated it after four logged hits using its configured 0.5 maximum health. This is a port-side integration result, not a paired retail combat trace.
- AI firing slice: the complete non-retail functional suite passed **23/23**; `mvp_movement_weapon_ai` and the strengthened `mvp_original_level_gameplay` cover difficulty-row parsing, burst-size limits, retail truncate rounding, weapon cadence, reload and reload-completion transition. Clang static analyzer reported no diagnostics for the five changed C++ translation units. The real-asset log confirms Level 1 row resolution, 13- and 11-round bursts, distance-scaled probabilities, and probability-gated hits/misses. This is not a paired retail-vs-port trace.
- AddressSanitizer + UndefinedBehaviorSanitizer: **10/10 MVP probes passed**, including original Levels 1 and 2. Leak detection disabled; this is not a leak audit.
- Clang static analyzer: **13 authored translation units checked**, no diagnostics reported.
- Focused doorway, idle/fire/flame expiry and clear-line guard tests failed before their fixes and passed afterward.
- Original executable runtime capture: eight IGIProbe hooks armed with no SKIP records; 63 animation-state and 46 first-call movement/ground observations recorded. A separate controlled-input attempt armed nine hooks and logged ten accepted input edges, but yielded only one player-update and one player-integrator snapshot. No controlled trajectory, combat event, phase alignment, or matching port trace was established. Details and limitations are in [VERIFICATION.md](VERIFICATION.md).
- Fresh Level 1 headless runtime logs contain 413 `objects.qvm placement` records; all 413 report matching authored/runtime positions and orientations, and 362 static records verify emitted geometry. A separate 240-tick real-asset demo trace recorded recoil samples and view accumulation, while player health stayed at 3.0. These are port-side runtime observations, not paired retail comparisons. Captures: `/tmp/igi-level-one-placement-runtime.log` and `/tmp/igi-level-one-recoil-state.csv`.
- Required retail CTest gate: **FAILED**, because original/port matching capture evidence is missing. Full CTest is not green; functional-only `-LE retail` deliberately excludes this gate.
- Strict signature verification passed for the clean installed bundle and the isolated latest candidate. The corrected candidate completed a 300-tick headless run and a 60-frame direct-executable GUI run (90 simulation ticks) cleanly. LaunchServices startup of the newly signed candidate stalled in an OS file-open request; it was sampled and cleaned up, not counted as a passing launch. macOS TCC logged an access request for that candidate. The direct launch succeeded without changing OS permissions.

Earlier Level 1 documentation used actor **3031** as a target in a 21-shot kill. That actor is cinematic. The earlier result is invalid as evidence of an ordinary gameplay encounter; the current integration test requires 3031 to remain disabled and selects only active gameplay guards.

## Delivery and remaining work

At the owner's request, source revision `3b64826` was rebuilt without source changes and installed at `/Users/haseeb-mir/Downloads/IGI/igi.app`. Staging directory: `/tmp/igi-current-delivery/igi.app`. The installed executable SHA-256 is `030e921990c3b65bf43822831fae290e5e10c4c17407ac54382a5bccd79d03c4`; strict bundle signature verification passed. The previous app was preserved as `igi-backup-20261016-163748-935908.app` beside it. This is a working experimental gameplay checkpoint; exact retail parity and stable-release acceptance remain open.

Highest-priority gaps: original runtime/memory capture; full collision/query semantics and building/door coverage; complete AI task/state/body-trigger and sight-loss behavior; weapon recoil/spread/ballistics; full skeletal transitions and hit/death animation; general QVM execution and mission objectives; original lighting/materials and rendering. Windows/Linux remain untested targets. See [MVP_PLAN.md](MVP_PLAN.md) and [FIDELITY_REQUIREMENTS.md](FIDELITY_REQUIREMENTS.md).
