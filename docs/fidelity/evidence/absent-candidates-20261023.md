# Absent-candidate manifest audit (2026-10-23)

Scope: the 84 records in `docs/fidelity/native-verification.json` whose `implementation.path` does not
exist in this branch at HEAD `efc3b24b0ede80c5561d15ece7e1f190d30a619a` (worktree
`codex-smart-workflow`). All 84 claimed `STRUCTURALLY_VERIFIED` when the audit started.

Machine-readable enumeration: `absent-candidates-20261023.json`. Patch:
`build/manifest-patches/absent-candidates.json` (84 entries; the manifest itself was not edited).

## Headline finding: stale paths, not missing code

The 84 recorded `implementation.path` values are all absent, but the implementation content is not.
A same-address `<address>-<Name>.cpp` exists in this branch for all 84, and its sha256 equals the
record's `implementation.sha256` byte for byte (verified for every record). The deleted names are the
pre-cleanup duplicates: the same file with ` 2` appended, an underscore variant, or a different name
stem. The manifest still points at the duplicate names and therefore skips the content that is present.

- 84/84 recorded implementation paths absent.
- 84/84 recorded implementation contents present in this branch at a canonical same-address `.cpp`.
- 60/84 records already name that source in `game_natives.path`; for the other 24 `game_natives.path`
  names a header (`.hpp`), while the implementation `.cpp` sits beside it unbound.
- The recorded sha256 is therefore still valid for a file in this branch; only the path is stale.

Because the records are not bound to those files and no fresh per-native verification was performed,
the labels are still unsupported: the content existing does not verify it. This audit lowers the status;
it deliberately does not rebind the records to the byte-identical files.

## Root cause

The legacy duplicate `scripts/GameNatives/` filenames were deleted from this branch in commit
`cfa826232b97` (2026-10-22, "Adopt the authoritative 120-native MVP set").
The canonical files were kept, but the manifest was not repointed, so the gate looks for paths that no
longer exist while the sources sit beside them. The default gate only checks the changed native set,
which is why the stale labels were never contradicted by a run.

## Historical copies

- 66/84 recorded revisions are also byte-identical in sibling worktrees (58 only on `retail-clean-flow`;
  5 on `codex-f0f0-runtime`, `game-engine-impl` and `retail-clean-flow`; 3 on `codex-f0f0-runtime` and
  `retail-clean-flow`).
- 18/84 exist in no sibling worktree at HEAD; the exact revision survives in git history.
- For all 84, `cfa826232b97^:<recorded path>` is byte-identical to the recorded sha256,
  independently of the canonical source in this branch.

## Same-address source provenance

The canonical same-address sources carry their own provenance comments: 46 PASS, 35 FAIL, 3 with
no CFG score. These are unverified historical comments in the files themselves, not manifest verification;
34 of the 60 files named directly by `game_natives.path` record FAIL (`CFG score ... (FAIL; structural only)`),
which is a further reason not to rebind without fresh checks. The distribution over all 84 is in the JSON.

## Retail-side evidence audit

- Binary Ninja: 84/84 usable. The active view was identified as the retail image, not the candidate:
  its entry point `0x004A50F5`, image range
  `0x00400000-0x00C35058` and sampled code bytes match retail `igi.exe`
  (sha256 `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`); `build-clean-win32/igi1.exe` has entry
  `0x00401450`. Every recorded entry exists in that view, and every stored node set matches.
  For 82 the stored direct-edge set matches too; for 2 the artifact records the 5 nodes but no edges,
  while Binary Ninja reports four indirect-branch successors (noted below).
- radare2: 82/84 usable. The artifacts agree with Binary Ninja on nodes and direct edges. The two
  jump-table entries have an incomplete radare2 graph (one block, no edges), so only Binary Ninja is
  relied on for those.
- Semantic review: 0/84 usable. All 84 reports are bound to the implementation sha256 (which exists)
  but are not native-specific: 81 apply the same IFF-chunk-parser
  boilerplate ("chunk tag constants", "payload offsets", "sibling and child walk") to GOPlayer, camera
  setters, Define_Mission, Sys_ReadFileBlock and others; the other 3 repeat one one-line retail summary for
  all seven checks. They are preserved as historical evidence but support no native-specific claim.
- Aggregate: 84/84 have usable retail evidence (Binary Ninja); 0 have none. 82/84 have a usable second
  retail source, and 0/84 have usable candidate-side verification evidence.

## Recorded CFG numbers were not what the labels implied

- 80/84 `cfg_similarity: 1.0` values are the retail Binary Ninja-vs-radare2 graph score
  (0.35*node_jaccard + 0.65*edge_jaccard), not candidate equivalence.
- 4 records carry a historical candidate-vs-retail score below the 0.85 structural threshold, so their old
  `STRUCTURALLY_VERIFIED` label was unsupported even for the recorded revision:
  - `0x004A23D0` Camera_ApplyHandiCam: `cfg_similarity` 0.04375 (`scripts/GameNatives/camera/004A23D0-Camera_ApplyHandiCam 2.cpp`).
  - `0x004AAA44` MathSys_Invoke21: `cfg_similarity` 0.07 (`scripts/GameNatives/mathsys/004AAA44-MathSys_Invoke21 2.cpp`).
  - `0x004AD004` MathSys_Invoke3: `cfg_similarity` 0.07 (`scripts/GameNatives/mathsys/004AD004-MathSys_Invoke3 2.cpp`).
  - `0x004B0B90` Sys_ReadFileBlock_2: `cfg_similarity` 0.175 (`scripts/GameNatives/sys/004B0B90-Sys_ReadFileBlock_2 2.cpp`).

## Entries with incomplete radare2 evidence

- `0x004AAA44` MathSys_Invoke21: Binary Ninja has 5 nodes with 4 indirect-dispatch successors; the radare2
  artifact has 1 block and no edges.
- `0x004AD004` MathSys_Invoke3: Binary Ninja has 5 nodes with 4 indirect-dispatch successors; the radare2
  artifact has 1 block and no edges.

## Build/behavioral claims bound to the recorded revision

10 of the 84 also carried `verification.build` and `verification.behavioral` artifacts (SetAppTitle,
SetScreenMode, SetDebugCompileState, Config_SetContentControlPassword, HumanPlayer_WireSlide and
HumanPlayer_LoadParameters 7-11). The behavioral artifacts are a single "static-structure" case, not paired
runtime captures. Both fields are moved to `historical_evidence` and removed from the active verification
block; the artifacts were not re-run.

## Representation chosen

The repository convention for superseded artifacts is `verification.historical_evidence`, a list of
`{evidence_path, note}` entries, plus `verification.limitations` (for example `0x004C5810`, `0x004D14E0`,
`0x004AF7B0`). No new field was invented. For each record the patch:

- sets `verification.status` to `ANALYZED`; the content exists but is unbound and unverified, so no
  candidate structural, build or runtime claim is supportable for the record as written;
- keeps `implementation.path` and `implementation.sha256` unchanged as the historical reference, and records
  both the byte-identical source in this branch and the exact git recovery ref;
- appends limitations naming the absent path, the deletion commit, the byte-identical same-address source,
  the worktree/branch history, the retail-evidence re-check, the unusable semantic review, the true meaning
  of `cfg_similarity`, and the unbound status of the same-address source;
- moves `semantic_review` (and any `build`/`behavioral`/`behavioral_runner`) into `historical_evidence` and
  removes the active fields; the artifacts themselves are untouched;
- records `cfg_similarity_basis` (an existing field) instead of deleting the inherited score;
- sets `selected_decompiler` to `binary_ninja`, matching the tool actually used for the re-check.

## Validation and aggregate statuses

- Patch entries: 84; unique addresses: 84; every target record
  initially had an absent implementation path; every `path` matches the record.

| Status | Before | After |
| --- | --- | --- |
| VERIFIED | 80 | 80 |
| STRUCTURALLY_VERIFIED | 801 | 717 |
| RUNTIME_TESTED | 1 | 1 |
| ANALYZED | 0 | 84 |
| INCONCLUSIVE | 1 | 1 |
| **Total** | **883** | **883** |

The patch was applied to a deep copy of the manifest with RFC 7386 merge-patch semantics (null deletes a
key, arrays replace, objects merge). Validation checked all 84 records after application, confirmed the
other 799 records were byte-unchanged, and confirmed the on-disk manifest hash stayed
`57eaea6d5132b8556fc2e6b1d1be37e113d81e2043a05e33cd4deb4e7e3b5b4d`.

## Not resolved

- The records were not rebound to the byte-identical same-address sources. Rebinding is mechanically easy
  and would let the gate see the files, but it would need a fresh static/semantic/runtime pass and is
  outside this label-only task; the 4 sub-0.85 records and the templated semantic reviews would still block.
- No status above ANALYZED was assigned.
- The two jump-table entries still lack a complete second retail source; a fresh radare2 run would be
  needed before any structural comparison.
- 18 records have no sibling-worktree copy at the recorded revision; recovery there requires git history.
- The semantic-review artifacts need per-native regeneration if these candidates are reintroduced; the
  current ones must not be reused.

## Enumerated records by directory

| Directory | Records |
| --- | --- |
| `scripts/GameNatives/camera` | 36 |
| `scripts/GameNatives/go` | 15 |
| `scripts/GameNatives/define` | 11 |
| `scripts/GameNatives/humanplayer` | 6 |
| `scripts/GameNatives/mathsys` | 4 |
| `scripts/GameNatives/fx` | 3 |
| `scripts/GameNatives/break` | 1 |
| `scripts/GameNatives/defineq` | 1 |
| `scripts/GameNatives/dynobj` | 1 |
| `scripts/GameNatives/misc` | 1 |
| `scripts/GameNatives/script` | 1 |
| `scripts/GameNatives/sys` | 1 |
| `scripts/GameNatives/terrain` | 1 |
| `scripts/GameNatives/weapongun` | 1 |
| `scripts/GameNatives/weatherwatersky` | 1 |

## Full enumeration

| Address | Name | Claimed | Recorded implementation path (absent) | Byte-identical source in this branch |
| --- | --- | --- | --- | --- |
| `0x004036B0` | GOPlayer | STRUCTURALLY_VERIFIED | `scripts/GameNatives/go/004036B0-GO_Player.cpp` | `scripts/GameNatives/go/004036B0-GOPlayer.cpp` |
| `0x00403750` | GOActiveMission | STRUCTURALLY_VERIFIED | `scripts/GameNatives/go/00403750-GO_ActiveMission 2.cpp` | `scripts/GameNatives/go/00403750-GOActiveMission.cpp` |
| `0x00403870` | GOInMouInv | STRUCTURALLY_VERIFIED | `scripts/GameNatives/go/00403870-GO_InMouInv 2.cpp` | `scripts/GameNatives/go/00403870-GOInMouInv.cpp` |
| `0x004038E0` | GOInMouSens | STRUCTURALLY_VERIFIED | `scripts/GameNatives/go/004038E0-GO_InMouSens 2.cpp` | `scripts/GameNatives/go/004038E0-GOInMouSens.cpp` |
| `0x00403950` | GOGfxDisp | STRUCTURALLY_VERIFIED | `scripts/GameNatives/go/00403950-GO_GfxDisp.cpp` | `scripts/GameNatives/go/00403950-GOGfxDisp.cpp` |
| `0x00403A00` | GOGameLang | STRUCTURALLY_VERIFIED | `scripts/GameNatives/go/00403A00-GO_GameLang.cpp` | `scripts/GameNatives/go/00403A00-GOGameLang.cpp` |
| `0x00403AC0` | GOGameDiff | STRUCTURALLY_VERIFIED | `scripts/GameNatives/go/00403AC0-GO_GameDiff 2.cpp` | `scripts/GameNatives/go/00403AC0-GOGameDiff.cpp` |
| `0x00403C10` | Config_SetContentControlPassword | STRUCTURALLY_VERIFIED | `scripts/GameNatives/go/00403C10-GO_ContentControlPW.cpp` | `scripts/GameNatives/go/00403C10-Config_SetContentControlPassword.cpp` |
| `0x00403DF0` | GOIsBlood | STRUCTURALLY_VERIFIED | `scripts/GameNatives/go/00403DF0-GO_IsBlood 2.cpp` | `scripts/GameNatives/go/00403DF0-GOIsBlood.cpp` |
| `0x00403E60` | GOSoundSpeech | STRUCTURALLY_VERIFIED | `scripts/GameNatives/go/00403E60-GO_SoundSpeech.cpp` | `scripts/GameNatives/go/00403E60-GOSoundSpeech.cpp` |
| `0x00403EF0` | GOSoundMusic | STRUCTURALLY_VERIFIED | `scripts/GameNatives/go/00403EF0-GO_SoundMusic.cpp` | `scripts/GameNatives/go/00403EF0-GOSoundMusic.cpp` |
| `0x00403F80` | GOSoundFX | STRUCTURALLY_VERIFIED | `scripts/GameNatives/go/00403F80-GO_SoundFX.cpp` | `scripts/GameNatives/go/00403F80-GOSoundFX.cpp` |
| `0x00404010` | GOGfxDevice | STRUCTURALLY_VERIFIED | `scripts/GameNatives/go/00404010-GO_GfxDevice 2.cpp` | `scripts/GameNatives/go/00404010-GOGfxDevice.cpp` |
| `0x00404080` | GOGfxGamma | STRUCTURALLY_VERIFIED | `scripts/GameNatives/go/00404080-GO_GfxGamma 2.cpp` | `scripts/GameNatives/go/00404080-GOGfxGamma.cpp` |
| `0x004040F0` | GOGfxPerformance | STRUCTURALLY_VERIFIED | `scripts/GameNatives/go/004040F0-GO_GfxPerformance 2.cpp` | `scripts/GameNatives/go/004040F0-GOGfxPerformance.cpp` |
| `0x00407E30` | DefineQMaterial | STRUCTURALLY_VERIFIED | `scripts/GameNatives/defineq/00407E30-DefineQ_Material.cpp` | `scripts/GameNatives/defineq/00407E30-DefineQMaterial.cpp` |
| `0x00407F50` | DefineGameMaterial | STRUCTURALLY_VERIFIED | `scripts/GameNatives/define/00407F50-Define_GameMaterial.cpp` | `scripts/GameNatives/define/00407F50-DefineGameMaterial.cpp` |
| `0x00408460` | FallLanding | STRUCTURALLY_VERIFIED | `scripts/GameNatives/misc/00408460-FallLanding 2.cpp` | `scripts/GameNatives/misc/00408460-FallLanding.cpp` |
| `0x0040DA40` | HumanPlayer_LoadParameters_7 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/humanplayer/0040DA40-HumanPlayer_LoadParameters_7 2.cpp` | `scripts/GameNatives/humanplayer/0040DA40-HumanPlayer_LoadParameters_7.cpp` |
| `0x0040E620` | HumanPlayer_LoadParameters_8 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/humanplayer/0040E620-HumanPlayer_LoadParameters_8 2.cpp` | `scripts/GameNatives/humanplayer/0040E620-HumanPlayer_LoadParameters_8.cpp` |
| `0x0040F370` | HumanPlayer_LoadParameters_9 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/humanplayer/0040F370-HumanPlayer_LoadParameters_9 2.cpp` | `scripts/GameNatives/humanplayer/0040F370-HumanPlayer_LoadParameters_9.cpp` |
| `0x0040FDA0` | HumanPlayer_LoadParameters_10 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/humanplayer/0040FDA0-HumanPlayer_LoadParameters_10 2.cpp` | `scripts/GameNatives/humanplayer/0040FDA0-HumanPlayer_LoadParameters_10.cpp` |
| `0x004101C0` | DefineHumanPlayerWeaponCycle | STRUCTURALLY_VERIFIED | `scripts/GameNatives/define/004101C0-Define_HumanPlayerWeaponCycle 2.cpp` | `scripts/GameNatives/define/004101C0-DefineHumanPlayerWeaponCycle.cpp` |
| `0x00410240` | DefineHumanPlayerWeaponCategory | STRUCTURALLY_VERIFIED | `scripts/GameNatives/define/00410240-Define_HumanPlayerWeaponCategory.cpp` | `scripts/GameNatives/define/00410240-DefineHumanPlayerWeaponCategory.cpp` |
| `0x004102D0` | DefineHumanPlayerAmmoLimit | STRUCTURALLY_VERIFIED | `scripts/GameNatives/define/004102D0-Define_HumanPlayerAmmoLimit.cpp` | `scripts/GameNatives/define/004102D0-DefineHumanPlayerAmmoLimit.cpp` |
| `0x00410370` | HumanPlayer_LoadParameters_11 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/humanplayer/00410370-HumanPlayer_LoadParameters_11 2.cpp` | `scripts/GameNatives/humanplayer/00410370-HumanPlayer_LoadParameters_11.cpp` |
| `0x00411F20` | HumanPlayer_WireSlide | STRUCTURALLY_VERIFIED | `scripts/GameNatives/humanplayer/00411F20-HumanPlayer_WireSlide1_2.cpp` | `scripts/GameNatives/humanplayer/00411F20-HumanPlayer_WireSlide.cpp` |
| `0x00471120` | WeaponGun_ProcessWNES | STRUCTURALLY_VERIFIED | `scripts/GameNatives/weapongun/00471120-WeaponGun_ProcessWNES 2.cpp` | `scripts/GameNatives/weapongun/00471120-WeaponGun_ProcessWNES.cpp` |
| `0x0047CAD0` | DefineAmmoType | STRUCTURALLY_VERIFIED | `scripts/GameNatives/define/0047CAD0-Define_AmmoType.cpp` | `scripts/GameNatives/define/0047CAD0-DefineAmmoType.cpp` |
| `0x00484F60` | DefineMission | STRUCTURALLY_VERIFIED | `scripts/GameNatives/define/00484F60-Define_Mission.cpp` | `scripts/GameNatives/define/00484F60-DefineMission.cpp` |
| `0x0048F100` | SetAppTitle | STRUCTURALLY_VERIFIED | `scripts/GameNatives/fx/0048F100-FX_ExplosionSpawn_23.cpp` | `scripts/GameNatives/fx/0048F100-SetAppTitle.cpp` |
| `0x0048F120` | SetScreenMode | STRUCTURALLY_VERIFIED | `scripts/GameNatives/fx/0048F120-FX_FireBallUpdate_23.cpp` | `scripts/GameNatives/fx/0048F120-SetScreenMode.cpp` |
| `0x0048F1C0` | SetDebugCompileState | STRUCTURALLY_VERIFIED | `scripts/GameNatives/fx/0048F1C0-FX_FireBallUpdate_24.cpp` | `scripts/GameNatives/fx/0048F1C0-SetDebugCompileState.cpp` |
| `0x004A23D0` | Camera_ApplyHandiCam | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004A23D0-Camera_ApplyHandiCam 2.cpp` | `scripts/GameNatives/camera/004A23D0-Camera_ApplyHandiCam.cpp` |
| `0x004A5955` | Camera_ResetMatrix_5 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004A5955-Camera_ResetMatrix_5 2.cpp` | `scripts/GameNatives/camera/004A5955-Camera_ResetMatrix_5.cpp` |
| `0x004A67E4` | MathSys_Invoke7_4A67E4 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/mathsys/004A67E4-MathSys_Invoke7_4A67E4 2.cpp` | `scripts/GameNatives/mathsys/004A67E4-MathSys_Invoke7_4A67E4.cpp` |
| `0x004A68A9` | Camera_UpdateViewCone_8 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004A68A9-Camera_UpdateViewCone_8 2.cpp` | `scripts/GameNatives/camera/004A68A9-Camera_UpdateViewCone_8.cpp` |
| `0x004A8A91` | Camera_ResetMatrix_14 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004A8A91-Camera_ResetMatrix_14 2.cpp` | `scripts/GameNatives/camera/004A8A91-Camera_ResetMatrix_14.cpp` |
| `0x004A8AD0` | Camera_UpdateTransform_15 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004A8AD0-Camera_UpdateTransform_15 2.cpp` | `scripts/GameNatives/camera/004A8AD0-Camera_UpdateTransform_15.cpp` |
| `0x004A8B37` | Camera_SetFieldOfView_15 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004A8B37-Camera_SetFieldOfView_15 2.cpp` | `scripts/GameNatives/camera/004A8B37-Camera_SetFieldOfView_15.cpp` |
| `0x004A9BD1` | Camera_ApplyWideScreen_17 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004A9BD1-Camera_ApplyWideScreen_17 2.cpp` | `scripts/GameNatives/camera/004A9BD1-Camera_ApplyWideScreen_17.cpp` |
| `0x004A9C40` | Camera_ApplyPostProcess_17 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004A9C40-Camera_ApplyPostProcess_17 2.cpp` | `scripts/GameNatives/camera/004A9C40-Camera_ApplyPostProcess_17.cpp` |
| `0x004A9C57` | Camera_UpdateViewCone_17 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004A9C57-Camera_UpdateViewCone_17 2.cpp` | `scripts/GameNatives/camera/004A9C57-Camera_UpdateViewCone_17.cpp` |
| `0x004A9CA0` | Camera_ResetMatrix_17 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004A9CA0-Camera_ResetMatrix_17 2.cpp` | `scripts/GameNatives/camera/004A9CA0-Camera_ResetMatrix_17.cpp` |
| `0x004A9CB5` | Camera_UpdateTransform_18 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004A9CB5-Camera_UpdateTransform_18 2.cpp` | `scripts/GameNatives/camera/004A9CB5-Camera_UpdateTransform_18.cpp` |
| `0x004A9CCC` | Camera_SetFieldOfView_18 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004A9CCC-Camera_SetFieldOfView_18 2.cpp` | `scripts/GameNatives/camera/004A9CCC-Camera_SetFieldOfView_18.cpp` |
| `0x004A9CE5` | Camera_ApplyLineFilter_18 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004A9CE5-Camera_ApplyLineFilter_18 2.cpp` | `scripts/GameNatives/camera/004A9CE5-Camera_ApplyLineFilter_18.cpp` |
| `0x004A9D28` | Camera_ApplyBandFilter_18 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004A9D28-Camera_ApplyBandFilter_18 2.cpp` | `scripts/GameNatives/camera/004A9D28-Camera_ApplyBandFilter_18.cpp` |
| `0x004AA0A6` | MathSys_Invoke19 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/mathsys/004AA0A6-MathSys_Invoke19 2.cpp` | `scripts/GameNatives/mathsys/004AA0A6-MathSys_Invoke19.cpp` |
| `0x004AAA44` | MathSys_Invoke21 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/mathsys/004AAA44-MathSys_Invoke21 2.cpp` | `scripts/GameNatives/mathsys/004AAA44-MathSys_Invoke21.cpp` |
| `0x004AD004` | MathSys_Invoke3 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/mathsys/004AD004-MathSys_Invoke3 2.cpp` | `scripts/GameNatives/mathsys/004AD004-MathSys_Invoke3.cpp` |
| `0x004AD140` | Camera_ResetMatrix_26 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004AD140-Camera_ResetMatrix_26 2.cpp` | `scripts/GameNatives/camera/004AD140-Camera_ResetMatrix_26.cpp` |
| `0x004AD1B0` | Camera_UpdateTransform_27 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004AD1B0-Camera_UpdateTransform_27 2.cpp` | `scripts/GameNatives/camera/004AD1B0-Camera_UpdateTransform_27.cpp` |
| `0x004AE670` | Camera_ResetMatrix_29 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004AE670-Camera_ResetMatrix_29 2.cpp` | `scripts/GameNatives/camera/004AE670-Camera_ResetMatrix_29.cpp` |
| `0x004AE730` | Camera_SetFieldOfView_30 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004AE730-Camera_SetFieldOfView_30 2.cpp` | `scripts/GameNatives/camera/004AE730-Camera_SetFieldOfView_30.cpp` |
| `0x004AF740` | Camera_ApplyBandFilter_30 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004AF740-Camera_ApplyBandFilter_30 2.cpp` | `scripts/GameNatives/camera/004AF740-Camera_ApplyBandFilter_30.cpp` |
| `0x004AF746` | Camera_ApplyHandiCam_30 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004AF746-Camera_ApplyHandiCam_30 2.cpp` | `scripts/GameNatives/camera/004AF746-Camera_ApplyHandiCam_30.cpp` |
| `0x004AF74C` | Camera_ApplyWideScreen_30 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004AF74C-Camera_ApplyWideScreen_30 2.cpp` | `scripts/GameNatives/camera/004AF74C-Camera_ApplyWideScreen_30.cpp` |
| `0x004AF752` | Camera_ApplyPostProcess_30 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004AF752-Camera_ApplyPostProcess_30 2.cpp` | `scripts/GameNatives/camera/004AF752-Camera_ApplyPostProcess_30.cpp` |
| `0x004AF758` | Camera_UpdateViewCone_30 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004AF758-Camera_UpdateViewCone_30 2.cpp` | `scripts/GameNatives/camera/004AF758-Camera_UpdateViewCone_30.cpp` |
| `0x004AF75E` | Camera_ResetMatrix_30 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004AF75E-Camera_ResetMatrix_30 2.cpp` | `scripts/GameNatives/camera/004AF75E-Camera_ResetMatrix_30.cpp` |
| `0x004AF764` | Camera_UpdateTransform_31 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004AF764-Camera_UpdateTransform_31 2.cpp` | `scripts/GameNatives/camera/004AF764-Camera_UpdateTransform_31.cpp` |
| `0x004AF76A` | Camera_SetFieldOfView_31 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004AF76A-Camera_SetFieldOfView_31 2.cpp` | `scripts/GameNatives/camera/004AF76A-Camera_SetFieldOfView_31.cpp` |
| `0x004AF770` | Camera_ApplyLineFilter_31 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004AF770-Camera_ApplyLineFilter_31 2.cpp` | `scripts/GameNatives/camera/004AF770-Camera_ApplyLineFilter_31.cpp` |
| `0x004AF776` | Camera_ApplyBandFilter_31 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004AF776-Camera_ApplyBandFilter_31 2.cpp` | `scripts/GameNatives/camera/004AF776-Camera_ApplyBandFilter_31.cpp` |
| `0x004AF77C` | Camera_ApplyHandiCam_31 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004AF77C-Camera_ApplyHandiCam_31 2.cpp` | `scripts/GameNatives/camera/004AF77C-Camera_ApplyHandiCam_31.cpp` |
| `0x004AF782` | Camera_ApplyWideScreen_31 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004AF782-Camera_ApplyWideScreen_31 2.cpp` | `scripts/GameNatives/camera/004AF782-Camera_ApplyWideScreen_31.cpp` |
| `0x004AF788` | Camera_ApplyPostProcess_31 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004AF788-Camera_ApplyPostProcess_31 2.cpp` | `scripts/GameNatives/camera/004AF788-Camera_ApplyPostProcess_31.cpp` |
| `0x004AF78E` | Camera_UpdateViewCone_31 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004AF78E-Camera_UpdateViewCone_31 2.cpp` | `scripts/GameNatives/camera/004AF78E-Camera_UpdateViewCone_31.cpp` |
| `0x004AF794` | Camera_ResetMatrix_31 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004AF794-Camera_ResetMatrix_31 2.cpp` | `scripts/GameNatives/camera/004AF794-Camera_ResetMatrix_31.cpp` |
| `0x004AF79A` | Camera_UpdateTransform_32 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004AF79A-Camera_UpdateTransform_32 2.cpp` | `scripts/GameNatives/camera/004AF79A-Camera_UpdateTransform_32.cpp` |
| `0x004AF7A0` | Camera_SetFieldOfView_32 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004AF7A0-Camera_SetFieldOfView_32 2.cpp` | `scripts/GameNatives/camera/004AF7A0-Camera_SetFieldOfView_32.cpp` |
| `0x004AF7A6` | Camera_ApplyLineFilter_32 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/camera/004AF7A6-Camera_ApplyLineFilter_32 2.cpp` | `scripts/GameNatives/camera/004AF7A6-Camera_ApplyLineFilter_32.cpp` |
| `0x004B0B90` | Sys_ReadFileBlock_2 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/sys/004B0B90-Sys_ReadFileBlock_2 2.cpp` | `scripts/GameNatives/sys/004B0B90-Sys_ReadFileBlock_2.cpp` |
| `0x004B8C10` | BreakScript | STRUCTURALLY_VERIFIED | `scripts/GameNatives/break/004B8C10-Break_Script 2.cpp` | `scripts/GameNatives/break/004B8C10-BreakScript.cpp` |
| `0x004B8C60` | _Script_SetWarningLevel | STRUCTURALLY_VERIFIED | `scripts/GameNatives/script/004B8C60-Script_SetWarningLevel.cpp` | `scripts/GameNatives/script/004B8C60-_Script_SetWarningLevel.cpp` |
| `0x004C0250` | DynObj_ParseConfigFile | STRUCTURALLY_VERIFIED | `scripts/GameNatives/dynobj/004C0250-DynObj_ParseConfigFile 2.cpp` | `scripts/GameNatives/dynobj/004C0250-DynObj_ParseConfigFile.cpp` |
| `0x004C4520` | DefineMagicObj | STRUCTURALLY_VERIFIED | `scripts/GameNatives/define/004C4520-Define_MagicObj.cpp` | `scripts/GameNatives/define/004C4520-DefineMagicObj.cpp` |
| `0x004E8C60` | DefineSound | STRUCTURALLY_VERIFIED | `scripts/GameNatives/define/004E8C60-Define_Sound.cpp` | `scripts/GameNatives/define/004E8C60-DefineSound.cpp` |
| `0x004E9EC0` | DefineTriggerOnce | STRUCTURALLY_VERIFIED | `scripts/GameNatives/define/004E9EC0-Define_TriggerOnce.cpp` | `scripts/GameNatives/define/004E9EC0-DefineTriggerOnce.cpp` |
| `0x004EAD20` | DefineGraph | STRUCTURALLY_VERIFIED | `scripts/GameNatives/define/004EAD20-Define_Graph.cpp` | `scripts/GameNatives/define/004EAD20-DefineGraph.cpp` |
| `0x004EBA90` | DefineGroup | STRUCTURALLY_VERIFIED | `scripts/GameNatives/define/004EBA90-Define_Group.cpp` | `scripts/GameNatives/define/004EBA90-DefineGroup.cpp` |
| `0x005127D0` | Terrain_SelectTerrainLOD_2 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/terrain/005127D0-Terrain_SelectTerrainLOD_2 2.cpp` | `scripts/GameNatives/terrain/005127D0-Terrain_SelectTerrainLOD_2.cpp` |
| `0x005236C0` | WeatherWaterSky_Invoke7_5236C0 | STRUCTURALLY_VERIFIED | `scripts/GameNatives/weatherwatersky/005236C0-WeatherWaterSky_Invoke7_5236C0 2.cpp` | `scripts/GameNatives/weatherwatersky/005236C0-WeatherWaterSky_Invoke7_5236C0.cpp` |

The JSON also records each recorded sha256, the historical locations, the retail evidence audit and the
same-address provenance for every row.
