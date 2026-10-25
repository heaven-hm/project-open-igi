# Changelog

## Release policy

A version is published **only** when the built `igi1.exe` starts, keeps running,
and a screenshot of the running game is captured and attached to that version.
A build that fails closed at a documented boundary (for example the registrar
chain throw at `QTask_RegisterClasses 0x402930`) is an **internal build**: it is
recorded under "Unreleased" with its evidence, and it never gets a version
number and never counts as a release.

## Unreleased

### 2026-10-25 - The named blocker set is cleared: five more leaves and one owner/extern pair

- `igi1.exe` 14,258,968 -> **14,260,288** bytes, green. Five natives linked, all `BUILD_VERIFIED`, runtime UNOBSERVED:
  `0x00460BE0` (`mov ax,[0x005BDAF8]`), `0x00460C40` (`[0x0053F317]`), `0x00460C50` (`mov eax,[0x005BDC18]`), `0x0047E990` (`[0x00540914]`) and
  `0x00460C10` (the fourth lazy event-id allocator with the retail `0xFF` sentinel).
- Every body was spot-verified against the image before integration, not accepted from the report.
- `0x0047E990` is the ownership rule demonstrated from the other side: it reads the byte that the FlashBang registrar `0x0047E700` allocates and
  writes, so it declares that global `extern` and defines nothing. `nm` shows `undefined _g_540914` and no data symbol in the object.
- Caveat recorded for the block: `0x0053F316` has no reader among the transcribed bodies while `0x0053F314`, `0x0053F315` and `0x0053F317` each have
  exactly one owner - so if anything consumes that byte it is still unowned, and the gap is named rather than assumed empty.
- With this, the named leverage blockers from the earlier analysis are all either transcribed or reclassified as CRT (`0x004A54A0` = `__chkstk`).

### 2026-10-25 - Two config leaf bytes, and a blocker that turned out to be the compiler's stack probe

- `igi1.exe` 14,258,430 -> **14,258,968** bytes, green. `0x00413B30` and `0x00413B40` linked (`mov al,[0x005391AF]; ret` and `[0x005391B0]`),
  `BUILD_VERIFIED`, runtime UNOBSERVED. Their AL-only store versus C++ EAX zero-extension is recorded as a codegen deviation, not papered over.
- **`0x004A54A0` is MSVC `__chkstk`, not a game native.** Verified against the image (`0x004A54A0..0x004A54CE`), sitting between the known CRT routines
  `0x004A53B3` (sprintf) and `0x004A5500` (strncpy). It is now classified CRT in `scripts/native_coverage.py` and must never be hand-transcribed.
  The 7 registrars counted behind it are therefore free, but each has a frame larger than 0x1000 and owns a real >=4 KB local array that must be
  reproduced from the retail frame size rather than invented.
- **Tool blind spot recorded**: `build/retail-dis.txt` does not render calls to `0x4A54A0` at all, while `r2 /r` lists callers at 0x004170A5,
  0x0041AAF5, 0x0041B8E5, 0x0042DCF5, 0x004310F5, 0x0044C8D5, 0x0046E375 and 0x00485DB5. Callee counters that trust the listing cannot see it.
- Global naming normalised: `g_413b20` (named after its function) became `g_5391ae` (named after its data address, the convention used everywhere
  else), so the three-byte block 0x005391AE/AF/B0 now has one owner per address and one naming rule.

### 2026-10-25 - The three highest-leverage startup blockers

- `igi1.exe` 14,257,059 -> **14,258,430** bytes, green. Three blockers linked, all `BUILD_VERIFIED`, runtime UNOBSERVED:
  `0x00401FA0 TaskType_SetInheritedFlags_401FA0`, `0x00460BF0 Task_GetCachedEventId_460BF0`, `0x004E6610 Task_GetCachedEventId_4E6610`.
- Two of the three are the same 28-byte lazy event-id shape as `0x00467D90` (retail `0xFF` sentinel, one `QTask_AllocateEvent` path), which is why
  they were cheap despite blocking 8 and 9 registrars respectively: the leverage table measured how many callers a body has, not how hard it is.
- That is now three cached-event-id bodies in the tree with identical retail shape; the fourth, if one appears, should be transcribed by comparison
  against these rather than from scratch - but still verified per instruction.
- Each closure was checked by compiling the object and looking at `nm -u` rather than by reading the source, and every undefined symbol resolves in
  the tree (`QTask_TypeTable`, `QTask_AllocateEvent`).

### 2026-10-25 - FlashBang and M203 registrars, and the first registrar that passes a non-zero fourth argument

- `igi1.exe` 14,255,796 -> **14,257,059** bytes, green. `0x0047E700 Task_DefineFlashBang` (129 B / 1 block / 34 instructions) and
  `0x00476110 Task_DefineM203` (150 B / 1 block / 39 instructions) linked, both `BUILD_VERIFIED`, runtime UNOBSERVED.
- **Finding worth carrying to every later registrar**: FlashBang passes **4** as the fourth `TaskType_Register` argument, not 0. Verified in the
  image at `0x0047E711` (`push 4`) with the stack layout `name, 0x48, parent, 4, 0`; `0x00401900` tests that word and calls `0x004B0B00` when it
  is non-zero, so transcribing 0 there would change behaviour. Every other registrar landed so far passes 0.
- **Second structural negative recorded**: FlashBang has no `Script_QHashNodeRegister` call, so it receives no `TASKTYPE_` script name - the same
  shape as Computer. Two negatives now, so "every registrar registers a script name" is demonstrably false and must not be assumed.
- Both cleanups close exactly: FlashBang `add esp,0x40` = 16 dwords (2 register slots + 3 register arguments + 3*3 link arguments + 2 registry
  lookup arguments); M203 `add esp,0x38` = 14 and `add esp,0x18` = 6.
- Branch lists for the earlier branchy bodies were recorded with the branch condition behind each jump (`0x004169D0`'s 13 blocks per-block counts,
  `0x00467D90`'s single sentinel test), so those two are no longer "walked, probably fine" entries.

### 2026-10-25 - The 17-registrar unblocker, and two config tables that are not leaf getters

- `igi1.exe` 14,254,193 -> **14,255,796** bytes, green. Four natives linked, all `BUILD_VERIFIED`, runtime UNOBSERVED:
  `0x00467D90 Task_GetCachedEventId_467D90`, `0x00416930` / `0x004169D0 Sys_LookupOrAppendRecord_*`, `0x00416870 Sys_GetSettingByte_416870`.
- `0x00467D90` is 28 bytes / 3 blocks: load `0x0053F582`, and only if it is still retail's `0xFF` sentinel call `QTask_AllocateEvent(0x181)` and
  store the low byte back. Verified against the image directly, so the "blocks 17 registrars" entry is a cheap lazy initialiser, not a knot.
- `0x00416930` (table `0x0057B5E8`, stride `0x3C`, count `0x0057BAB4`) and `0x004169D0` (table `0x0057B1A8`, stride `0x30`, count `0x0057BAB8`) are
  13-block record lookups with an append path, not config leaves - both spot-verified against the image's own table/count references. The
  `0x00416xxx` range is therefore not uniform, and prior sizing assumptions there are not to be reused.
- **Declared deviation, recorded rather than hidden**: retail inlines `strlen` (`repne scasb`) and the record copy (`rep movsd` + `rep movsb`); the
  transcriptions call `std::strlen`/`std::memcpy` with byte count `strlen+1`. Behaviourally equivalent, not instruction-equivalent. The record is
  marked with that limitation, and neither table is given an invented bound because their extents are not statically established.
- Closure moved: `0x0047E700` and `0x00476110` are now closed (every callee transcribed), and the four registrars previously blocked only by the
  config range are unblocked.

### 2026-10-25 - GunShot and HumanCamera registrars, plus two self-corrections from the lane

- `igi1.exe` 14,252,981 -> **14,254,193** bytes, green. `0x0047B4B0 Task_DefineGunShot` ("GunShot", 0xC38, id 0x005BE3CE,
  script name `TASKTYPE_GUNSHOT`) and `0x004825C0 Task_DefineHumanCamera` ("HumanCamera", 0x128, id 0x00540990, plus a do-while
  clearing 128 eight-byte records at 0x005C1188). Both `BUILD_VERIFIED`, runtime UNOBSERVED.
- `g_5be3cc` (0x005BE3CC) was already defined in `sys/native_47b100.cpp`, so the new registrar declares it `extern` and adds no second
  definition - the single-writer rule held without a link error this time.
- **Tooling correction from the lane**: its batch closure scan parsed `pdfj` JSON, which emits call targets as `fcn.00416930` rather than
  `0x416930`, so the regex matched nothing and the scan reported "ABSENT: none" unconditionally. Re-checked with the `pd` text form,
  `0x47E700` and `0x476110` are **blocked**, not closed: they call `0x416930`/`0x4169D0`, which have no transcription anywhere.
- **Recount correction**: the doc's `0x4825C0` sizing (131 B / 3 blocks / 34 instructions) is right; the earlier 27-instruction count was the
  stop-at-first-`ret` artifact. Per-block counting (`af` then `afbj`) is now the only accepted method for multi-block bodies.
- Consequence for scheduling: `0x416930`/`0x4169D0` sit in the `0x00415xxx`-`0x00419xxx` range that has been reserved, so the two blocked
  registrars queue behind opening that range rather than behind the startup walk.

### 2026-10-25 - Two more task-type registrars, and a registrar that deliberately has no script name

- `igi1.exe` 14,251,830 -> **14,252,981** bytes, green. `0x0046DD00 Task_DefineGunx2` ("Gunx2", 0x138, id -> 0x0053FE68) and
  `0x0046BA60 Task_DefineComputer` ("Computer", 0x198, id -> 0x005BE2C4) are linked, both `BUILD_VERIFIED`, runtime UNOBSERVED.
- Both bodies are single-block and their `add esp` cleanups were used as an independent check on the call set: 0x38 = 14 pushes
  (2 shared + 3 register args + 3*3 link args), then 0xC three times (pre-pushed handler slot + 2 link args), then 0x14
  (handler slot + 2 link args + 2 script-registration args).
- **Structural finding, recorded so nobody "fixes" it later**: `0x0046BA60` has no `Script_QHashNodeRegister` call at all - "Computer" never
  receives a `TASKTYPE_` script name from this routine. `0x0046DD00` does register `"TASKTYPE_GUNX2"`. Symmetry is not evidence.
- The linkage lesson from `0x0046DF10` held on the next two files: their registrar declarations now copy the `extern "C"` linkage from the
  definitions, and `nm` shows `_Script_QHashNodeRegister`, not a mangled name.

### 2026-10-25 - Second startup wave: five registered natives and the knife-stab task type

- `igi1.exe` 14,249,775 -> **14,251,830** bytes, green. Six natives added to the link: the other lane's `0x00402820`, `0x00402870`,
  `0x00490370`, `0x004E6030`, `0x00495CC0` (stable across two checks before integration, one VA per file, provenance present, no duplicate VAs),
  and `0x0046DF10 Task_DefineKnifeStab` from the startup-chain worker.
- Integration fix worth recording: `0x0046DF10` declared `Script_QHashNodeRegister` without `extern "C"` while the transcription defines it
  `extern "C"`, so the link failed with `undefined reference to Script_QHashNodeRegister(void*, void*)`. Fixed at the declaration; the
  definition was correct. A companion-signature check at write time would have caught this before the shared build.
- `0x0046DF10` also shows why callee bodies beat call-site shapes: the `push 0x0046DF70` before a getter that ignores its stack is the
  pre-pushed third argument of the later `QTask_LinkQTaskEventHandler`, and both `add esp,0x14` cleanups independently measure five dwords.
- All six are `BUILD_VERIFIED` with `runtime UNOBSERVED` recorded; none of them is reachable from startup yet because `0x00402930` still aborts.

### 2026-10-25 - The DX gate is real now, and a paired runtime check caught a GUID bug static reading missed

- `0x00498480 Display_ProbeDirectXVersion` is wired into WinMain at its retail site (`0x0048F6E2`) with the retail `< 0x700` requirement path
  (`sprintf` 0x541E68, `MessageBoxA` "Error" `MB_ICONERROR`, `exit(1)`), and `0x00407D60 Material_DefineScriptNatives` is linked
  (`igi1.exe` 14,244,376 -> **14,249,775** bytes, green).
- **Paired runtime finding**: with the gate wired, the candidate showed the retail requirement dialog while retail `igi.exe`, run in the same CrossOver
  bottle with the same arguments, passed the gate and opened its window. Cause: our transcription of `IID_IDirectDraw2` (`0x00534A60`) and
  `IID_IDirectDrawSurface3` (`0x00534A40`) had `Data2`/`Data3` byte-swapped, so both `QueryInterface` calls failed and the tier stalled below 0x700.
  The image bytes are `... 43 2b cf 11 ..` and `... b2 69 d0 11 ..`, i.e. little-endian words `0x2B43`/`0x11CF` and `0x69B2`/`0x11D0`. Both corrected
  from the image, and the candidate now passes the gate and reaches `QTask_RegisterClasses` exactly like retail does.
- **This is the first divergence the runtime side found that the static side had already "verified"**: the probe's provider block claimed byte-exact
  GUIDs and 38/38 blocks, and it was still wrong on two GUIDs. Reading a GUID in a decompiler listing is not the same as decoding the image bytes in
  memory order; the evidence now says which of the two happened.
- `0x00498480` status is `RUNTIME_TESTED` with a documented scope (the tier outcome under CrossOver, paired with retail; COM internals not traced).
  `0x00407D60` is `BUILD_VERIFIED`/runtime UNOBSERVED - it is entry #3 of the `0x00402930` sequence.
- **Concurrent-writer incident, reported not hidden**: `CMakeLists.txt` gained five entries (`native_402870`, `native_402820`, `sys/native_490370`,
  `core/native_4e6030`, `render/native_495cc0`) from another agent's lane; two of those files do not compile (`Game_FrameDispatchRecord` and two
  `Native_*` declarations missing), which broke the shared build. The entries were removed from `CMakeLists.txt` (files left on disk, nothing deleted) so
  the build is green again; they belong to that lane's integration, not this one.

### 2026-10-25 - BackgroundFX wave linked (10 natives) and the 0x00A94E84 dispatch blocker closed

- `igi1.exe` 14,231,896 -> **14,244,376** bytes, build green with no undefined and no duplicate symbols. The wave adds `BackgroundFX_EmitAxisRuns_419330`
  (520 B/144 instrs), `EmitFan_419630` (290/90), `EmitSegments_419540` (230/77), `BuildAxes_419860`, `UpdateRecord_419820`, `EmitRecord_419950`,
  `Init_419760` (the class registrar itself), `ZeroField20_4197F0`, `AdvanceField20_419800` and `BuildRotation_4192E0`.
- The `0x00A94E84` render-backend dispatch that blocked this wave is now usable from transcribed code through the external-`extern "C"` table, with the retail
  form `g_a94e84[g_a84a54]`. The index word `0x00A84A54` is loaded as a defined zero dword rather than folded to a literal, so the memory access retail
  performs is still present.
- **Data increment transcribed**: `0x00539AF0`-`0x00539C17` as typed objects with real pointers - two 5-entry draw lists, the circle/axis-pair arrays, and
  the three draw records. A throwaway host check that *executed* the compiled initialisers against the image dump caught two wrong `g_539b50` entries
  (`{200,100,200,-100}`, `{0,-400,0,150}`) before they reached a commit. Reading the table was not enough; running it was.
- Recorded limits, not smoothed over: `0x00419630`'s x87 `fistpll` is modelled with `std::llrintl` and low-32-bit truncation, GCC folds `cos`/`sin` into one
  `_sincosl` import where retail has separate `fcos`/`fsin`, so **last-bit equality is not claimed**; and retail leaves one value on the x87 register stack on
  that function's `count > 0` path (unobservable through memory, return or ABI).
- Statuses are `BUILD_VERIFIED` with runtime UNOBSERVED: `0x00419760` is entry #153 of the `0x00402930` sequence, so the startup pass still cannot reach the
  wave even though it now executes in the binary.

### 2026-10-25 - 23 orphaned natives linked; two duplicate class-id owners fixed

- 23 engine sources that no build target contained are now compiled and linked (`igi1.exe` 14,222,853 -> **14,231,896** bytes, build green). They cover the
  `0x00491Dxx` render-handler set, the `0x00490xxx` and `0x004E7C50` render entries, the `0x004B6Cxx` model pair, `0x00418xx`-adjacent core entries, the
  `0x004EBC60` audio entry, `0x0051A1D0` dynobj and `0x005186C0` script. All 23 already had manifest records; status moves to `BUILD_VERIFIED` with runtime
  UNOBSERVED recorded per record.
- **Two duplicate owners found, because these files had never been linked together before:** `g_548638` (retail `0x00548638`, parent class id) was defined
  both by `core/native_4d9730.cpp` (`QObj_Init`, the writer) and by `render/native_004da1d0.cpp` (reader only); `g_54875c` (retail `0x0054875C`, DynCubeObj
  class id) was defined both by `dynobj/native_004e5370.cpp` (`DynCubeObj_Init`, the writer) and by `dynobj/native_0051a1d0.cpp` (reader only). The readers now
  declare `extern`, the writers keep the single definition, and the provenance text saying otherwise was corrected.
- This is exactly the failure mode "not in the build" hides: a duplicate definition is silent while nobody links the files, and it appears as a link error the
  moment they are integrated. It also means the earlier `0 of 52 duplicate-VA` result was about VAs, not about symbols.
- The remaining 29 orphans are `input/` (17 files, blocked on the missing half of `InputPort`) and the ten `native_419xxx` files the BackgroundFX wave is
  still writing.

### 2026-10-25 - Audit: 52 engine sources are not in the build

- `docs/fidelity/evidence/unbuilt-engine-sources.md`: 52 of 466 engine `.cpp` files are neither compiled nor linked (414 listed). An unbuilt native is not a
  transcription - it cannot be runtime-checked, it violates the no-dead-code rule, and it shrinks every "remaining work" count silently.
- **0 duplicate-VA files**, so no rule-13 violation: each unlisted file either carries a VA no linked file covers (49 files) or has no retail VA at all (3 port
  scaffolding files in `input/`).
- Integration test on 42 of them (all but the ten `native_419xxx` files the BackgroundFX wave is still writing): compiles clean, fails only at link with **10
  unresolved symbols** - `InputPort::Run`/`SetKeyDown`/`SetMouseButtonDown`/`AddMouseDelta`/`Remap`/`ActionPressed`/`ActionDown` plus `g_54875c`,
  `g_548638`. The backlog is close to linkable; the input subsystem is missing the other half of its own class, not an unknown dependency.
- Trigger for the audit: a worker reported `0x00498460` as "transcribed but unlinked". Checking the whole tree beat trusting one file's claim - the real number
  was 52, not one.

### 2026-10-25 - Startup sequence sized: 154 callees, 67,560 bytes in retail call order

- Added `docs/fidelity/evidence/startup-402930-callee-sizes.md`: every distinct callee of `0x00402930` in retail call order with its radare2 extent,
  block/instruction counts and whether an engine implementation exists. 154 callees, sizes recovered for all, **67,560 bytes total**; 36 are <= 200 B,
  101 are 201-800 B, 17 are > 800 B, and 8 have an engine implementation.
- The order matters more than the total: the sequence's **first** callee is the AI graph registrar `0x00508F80` (787 B / 198 instrs) and its first
  callee `0x005183D0` reports 4985 B over 90 instructions - a jump-table/data-bearing extent, so the AI graph is a subsystem wave, not a one-pass job.
  Because the guard stops at the first untranscribed registrar, nothing after `0x00508F80` becomes observable until that cluster lands.
- This replaces guessing about "277 small registrars" with measured buckets. Sizes are r2 extents and are planning data: a function whose tail r2 merges
  with a neighbour can read large.

### 2026-10-25 - Startup guard's own gap figure corrected: 146 absent callees, not 134

- Re-measured `0x00402930`'s callee closure against the tree rather than against the manifest's candidate paths: of **154 distinct callees, 8 have
  an engine implementation** (`0x00401FC0`, `0x00415530`, `0x00419760`, `0x00424B90`, `0x0042DAC0`, `0x0042FE40`, `0x00489560`, `0x004C1780`) and
  **146 do not**. The fail-closed guard text and the provenance block said 134, which understated the remaining work by a dozen natives; both now
  carry the measured figure and the old one is marked retracted.
- The same provenance line gave the function extent as `0x00402930..0x00402A40`. The real extent is `0x00402930..0x00402C3C` (780 bytes, 156
  five-byte instructions: 155 calls plus the tail `jmp 0x004CEF30` at `0x00402C37`), matching the ordered inventory in
  `docs/fidelity/evidence/startup-402930-call-sequence.md`.
- Why the numbers drifted: earlier counts used the manifest's `implementation.path`, which for many records points at an external `scripts/GameNatives`
  candidate rather than engine code. A candidate file is not a transcription. Measured here as "does an engine source implement this VA".

### 2026-10-25 - Render-backend table 0x00A94E84 gains external C linkage

- `engine/src/retail/dynobj/local_models_mef.cpp` defined the 16-entry backend init table as internal-linkage storage in an anonymous namespace, so a
  transcribed core native could not index it the way retail does (`call *0x00A94E84(,%reg,4)`). The definition keeps its single object and now has
  `extern "C"` linkage; `engine/include/igi/retail/dynobj/local_models_mef.hpp` declares it. No getter and no wrapper were added - the table is data
  retail itself exposes, and a helper would have been a non-retail abstraction.
- Build green; `igi1.exe` 14,222,891 -> 14,222,853 bytes (linkage/COMDAT difference only, no code change), and `nm` shows `B _g_a94e84` external.
  A same-named internal object in `texture/` remains internal and unaffected.

### 2026-10-25 - QVM evaluator cluster and its transcription set link into the candidate (27 natives, internal build)

- **27 natives listed in one batch** and linked: `igi1.exe` grows 14,206,798 -> 14,222,891 bytes, build green. This closes the QVM closure that had
  blocked the script layer since the accessor work started: the last three undefined symbols (`0x004B9120`, `0x004B8CE0`, `0x004B9660`) now have
  in-tree definitions, and the accessor trio (`0x004B8A20`, `0x004B8A50`, `0x004B8A80`) links with them. Piecemeal integration was never possible -
  tracked TUs already reference all three accessors, which a link test proved before this batch.
- **Two CRT routines identified instead of transcribed**: `0x005169B0` is statically linked MSVC `strncat` (291 B, leaf, zero callees and data
  references, one call site in the whole `.text`) and `0x004A5500` is MSVC `strncpy` (254 B, 35 blocks); both join `0x004A53B3` sprintf as CRT
  closure reductions with no implementation unit.
- **ABI correction from bytes**: `0x004B8CE0` takes three arguments, not four. `0x004B9040` pushes four words but the first is a retained stack word
  consumed by the following `strncpy` after retail's deliberately short `add $0xc`; the declaration, the call and the comment were corrected rather
  than silently changed.
- **Cross-file name mismatch fixed**: `native_4b8a80.cpp` still declared `0x004B8CE0` under its previous placeholder name and passed an untyped
  destination; the declaration now matches the definition, so the batch links instead of failing with one undefined symbol.
- Verification status for every native in the batch is `BUILD_VERIFIED` with `runtime UNOBSERVED` recorded in its limitations. The changed-native gate
  still blocks them, and correctly so: its behavioral runner reports `unsupported native address` for these VAs because no paired runtime observation
  exists - these functions execute from script and QTask dispatch that the candidate does not reach yet. A successful link is not behavioral evidence.

### 2026-10-25 - QTask_DeleteQTaskType (0x00401A20) transcribed and linked (internal build, no release)

- `engine/src/retail/core/native_401a20.cpp` (50 B, 15 instructions, 3 blocks, 3 edges) is listed in `CMakeLists.txt` and links into the candidate;
  `igi1.exe` grows 14,206,047 -> 14,206,798 bytes with the build green. It is `TaskType_Register`'s counterpart: masks the id, tests slot+0x24,
  frees through `0x004B0B70` when set, then clears slot+0x25 on both paths without clearing +0x24.
- Its two dependencies were already in the build - `QFile_ReleaseBlock_4B0B70` (`qfile/native_4b0b70.cpp`) and `QTask_TypeTable` - so the only new
  file in this chain is this one. No duplicate was created for the two callees that already existed.
- The manifest record `0x00401a20` previously pointed at the external `scripts/GameNatives` candidate and carried a `VERIFIED` status inherited from
  that catalog's "100% human-verified" claim. The record now names the engine transcription with its SHA-256 and status `BUILD_VERIFIED`; the old
  claim is preserved as a limitation that explicitly applies to that candidate, not to this transcription. Runtime remains UNOBSERVED.

### 2026-10-25 - automatic native intake and verification, repository cleanup (internal tooling, no release)

- Added one entry point, `scripts/native_autoverify.py`: changed/all/staged/commit-range discovery, a persistent sequential watch queue, source/build/probe/evidence invalidation, static BN/r2/angr capture, and the existing strict semantic/behavioral/paired-runtime checker. It never assigns native verification statuses or invents probes.
- Enabled staged native checks in the configured Git hook; added CMake changed/all verification targets and a self-hosted runtime CI workflow. Unknown addresses, incomplete staging, stale images/receipts, missing custom probes, unknown ABI or mismatching outputs/side effects block approval. CI is configured, not claimed to have run remotely.
- Validation: eight infrastructure tests, real SetConfigFileName static capture (runtime correctly blocked for missing reviewed probe registration), CMake target discovery, compatibility CLI checks and the gate-integrity check. Evidence: `docs/fidelity/evidence/native-auto-20261025/validation.json`.
- Audited the preserved demo image `9e7cdced...` against `game-engine-flow`: 50 engine-native records have matching historical source hashes; zero fresh paired runtime passes are available for that image. The reuse allowlist is empty. This is not a gameplay comparison or a zero-percent-correctness claim; no demo code was copied.
- Removed five byte-identical duplicate root documents, retaining canonical copies under `docs/`; consolidated two duplicated runner implementations into compatibility entry points. Updated references without deleting historical evidence. Removed 118 tracked generated Python cache files; source scripts and ignore rules remain.
- Fixed chronological discovery: game QTask addresses are no longer excluded as CRT; default walk begins at WinMain; the full 3,073-row direct-call order is retained. Function/disassembly inputs are now private and image/hash-bound instead of relying on shared `/tmp/afl.json`. Static direct-call order is not universal runtime coverage.


### 2026-10-25 - Startup init sequence measured (entry corrected after review; the guard was right)

- **Evidence added**: `docs/fidelity/evidence/startup-402930-call-sequence.md` - the ordered call inventory of `0x00402930..0x00402F50`.
- **Measured twice, agreeing**: 309 `call` instructions through `build/retail-dis.txt` and an independent
  `objdump -d --start-address=0x402930 --stop-address=0x402F50`; the region is a straight-line call sequence plus `nop` padding.
- **Correction (this entry was initially wrong)**: an earlier version of this entry claimed the candidate's guard ("155 ordered calls ending in a
  tail jump to 0x004CEF30") was inaccurate, based on 309 calls over 303 distinct callees. That measurement ran from `0x00402930` to the next
  *manifest* native `0x00402F50`, which spans **two functions**. `0x00402930` really does end at `0x00402C3B`, with the last call at `0x00402C32`
  to `0x00402F50` (`Config_YmbeAfp`) and the tail `jmp 0x004CEF30` at `0x00402C37` - exactly what the guard says. Independently
  `objdump --start-address=0x402930 --stop-address=0x402C3B` reports **155 calls**. The worker who flagged this was right and this entry is
  corrected rather than left standing.
- **Real closure for `0x00402930`**: 155 call sites, 154 distinct callees, 22 with an implementation file, **132 absent** - consistent with the
  guard's 134 within counting-method noise. The neighbour at `0x00402C40` is a separate, unrecorded 154-call sequence (150 distinct, 145 absent)
  with no manifest record, no work-order entry and no direct caller found by `axt`; its first calls are the small `0x004192B0`/`0x004199B0`
  routines that call `0x00401A20`, so it is plausibly a companion register/unregister pass - a hypothesis, not a finding.
- Counted strictly: a callee whose source exists but has no manifest record yet (the pending `0x42D*`/`0x42F*` handler files) counts as absent here.

### 2026-10-25 - TaskType_Register (0x00401900) moved to its own native and linked (internal build, no release)

- **Atomic move, as the no-duplication rule requires**: the `TaskType_Register` definition was removed from
  `engine/src/retail/core/task_types.cpp` (82 lines) and the existing transcription listed in `CMakeLists.txt` as
  `engine/src/retail/core/native_401900.cpp`. The build is green and `igi1.exe` links with the native in the target (14,206,047 bytes).
- **Linkage settled by experiment, not assumption**: the header declares the native inside its `extern "C"` block (line 74), so the
  definition keeps C linkage. Defining it in `namespace igi::retail::core` produced `undefined reference to TaskType_Register` from six
  call sites (cam_task, native_424b90, native_4d9730, native_004e5370, flow, game_flow) - evidence that the 279 callers bind the unmangled
  symbol. The first attempt was wrong for exactly that reason and is recorded here rather than hidden.
- **Manifest**: the `0x00401900` record now names `native_401900.cpp` with its SHA-256 and status `BUILD_VERIFIED`, limitations recorded
  (runtime UNOBSERVED; tail consumers `0x00401A20`/`0x004012F0` and the `0x004B0B70` free path still untranscribed).
- **Worker correction accepted**: my claim that `Debug_Error` (0x004AF7B0) and `Mem_AllocIndexedBlock` (0x004B0B00) had no engine
  implementation was wrong - they are `retail_error.cpp:29` and `mem/task_list.cpp:13`, both already in the igi source list. The worker was
  right not to duplicate them (rule 10.15). Stale "not transcribed yet" texts in `task_types.hpp` and `native_00530a80.cpp` were corrected.

### 2026-10-25 - QVM core dependency table; shared state words given external linkage (internal build, no release)

- **Evidence added**: `docs/fidelity/evidence/qvm-core-dependency-table.md` - per-VA sizes, r2 CFG, ABI, callers, callees and the wave ordering
  for the QVM closure, including the finding that the three evaluators (`0x004B9120`, `0x004B9660`, `0x004B8CE0`) are one mutually
  recursive cluster through `0x004B9040` with no acyclic order inside it.
- **Linkage defect fixed in `engine/src/retail/script/qvm_native_loader.cpp`**: `g_a965a0`, `g_a96598` and `g_a965a8` were defined inside the
  file's anonymous namespace, so the `extern` declarations in `qvm_execute.cpp` could never resolve - the CMake note calling these words
  "not ported" was wrong; they were ported but with internal linkage. Moved to namespace scope with the retail evidence for each word
  (writers/readers by VA) recorded at the definition. A duplicate definition in `qvm_execute.cpp` was never an option: it would have split
  the state that `Qvm_ReportError` and `Qvm_SetGlobalState` update.
- **Verified, not assumed**: `nm` on separately compiled objects shows the three words defined `D/D/B` in `qvm_native_loader.o` and referenced
  `U` in `qvm_execute.o`, so the pair now resolves; the full build stays green at 14,206,388 bytes. This fixes the linkage, **not** the
  evaluator transcription - `0x004B9120`/`0x004B9660`/`0x004B8CE0` remain unimplemented.

### 2026-10-25 - QVM argument accessors transcribed and measured; the script-layer wall mapped (internal build, no release)

- **Evidence added**: `docs/fidelity/evidence/qvm-accessors-4b8a20-4b8a50-4b8a80.md`. Three accessors measured on radare2 6.2.0
  `af`/`pd` against the retail image (all three 3 blocks / 2 branch edges, no prologue, plain `ret`):

  | VA | bytes | instructions | r2 nargs/frame | retail call sites |
  | --- | --- | --- | --- | --- |
  | `0x004B8A20` `Script_EvalArgInt` | 46 | 17 | 2 / 8 | 231 |
  | `0x004B8A50` `Qvm_GetFloatArg` | 44 | 17 | 2 / 8 | 201 |
  | `0x004B8A80` `Sys_ResolveSymbol_23` | 66 | 25 | 4 / 16 | 75 |

- **Correction recorded, not inherited**: for `0x004B8A20` and `0x004B8A50` the historical Binary Ninja/angr candidate disagrees with
  the retail CFG (5 blocks, splitting at the calls) while Binary Ninja and radare2 on the retail bytes agree on 3 blocks / 2 edges; the
  document states which parts agree (entry, flag at `+0x04`, scale-4 table at `+0x08`, the two x87 returns) and which do not.
- **The wall is now explicit**: each accessor is a two-branch wrapper whose work sits in untranscribed callees, so linking the three
  replaces three undefined symbols with six - `0x004BCAB0` (41 B, 14 instrs), `0x004B9120` (1188 B, 65 blocks, 484 instrs),
  `0x004BD010`, `0x004B9660`, `0x004BD040`, `0x004B8CE0`. `0x004BD010`/`0x004B9660` are already transcribed inside `qvm_execute.cpp`, but
  that file is the **unlinked** `igi_qvm_execute_object` target, gated on eight further callees plus three QVM state words.
- **Not committed, still untracked by design**: the three accessor `.cpp` files and the six handler `.cpp` files. None of the nine can join
  the `igi` link until that closure exists; the build stays green and the nine files are backed up under `build/pending/`.

### 2026-10-25 - QTask handler ABI settled and independently re-verified (internal build, no release)

- **Evidence added**: `docs/fidelity/evidence/qtask-handler-abi.md` (635 lines) settles the handler ABI from retail instructions: the
  second handler argument is a **caller-supplied pointer to caller-owned storage**, not a record field and not the QVM argument frame -
  size callbacks take `std::uint32_t*` out-params, parse callbacks take `void*` to a three-word `{block, argument frame, next index}`
  record that loader `0x004EDD20` builds at `E-0x18`. `FrameHandler` is valid as the table's **storage** type only; as a call signature
  it is proven incomplete (two-word dispatch sites carry integer second words).
- **Two structural claims re-checked by the Lead against `build/retail-dis.txt`, both exact**: the class registrator `0x00401900` copies
  the parent row with `0xFE` iterations and a `0x600`-byte stride (`lea 0xa96ae0(,%ebp,4)` destination, `lea 0xa96ae0(,%edi,4)` source,
  `add $0x600` on both, `dec %edx / jne`), which proves `entry(class, phase) = 0xA96AE0 + class*4 + phase*0x600`, i.e. the 0x180-entry
  phase stride this tree already uses; and the image references that base with exactly **791 refs across 13 folded phase bases**
  (588/119/15/1/21/2/13/5/7/5/4/7/4) - the census in the document is accurate, and the 725 dispatch sites split 457 two-word / 268 one-word.
- **Six handler natives written but deliberately NOT committed**: `0x0042DAC0`, `0x0042DB00`, `0x0042DCE0`, `0x0042FE40`, `0x0042FE80`,
  `0x0042FEF0`. They are syntax-clean and stable, but adding them to the build fails at link with three undefined callees
  (`0x004B8A20` 8x, `0x004B8A50` 6x, `0x004B8A80` 3x). Per the no-dead-code rule they stay out of the tree and out of CMake until those
  three natives land; a safety copy is at `build/pending/handler-set-20261025.tar.gz`. The three accessors are now the assigned unblock.

### 2026-10-25 - Wave A of the startup chain: Cursor registrar and its handlers (internal build, no release)

- **Integrated 4 new natives**, each with the five-line provenance block, one file per retail VA, CFG measured on both engines:
  | VA | native | size / CFG (Binary Ninja = radare2) |
  | --- | --- | --- |
  | `0x00424280` | `Task_WalkAndDispatchEvent_424280` | 117 B, 10 blocks / 14 edges |
  | `0x00424B90` | `Cursor_Init_424B90` (registrar called from `0x00402BC9`) | 121 B, 1 block / 0 edges |
  | `0x00424C10` | `Cursor_InitState_424C10` (phase-0 handler) | 47 B, 1 block / 0 edges |
  | `0x00424CA0` | `Cursor_WinProcMouseMove_424CA0` (WM_MOUSEMOVE handler) | 50 B, 4 blocks / 5 edges |
  Score: **1.000** on all four (node/edge Jaccard, Binary Ninja view vs radare2 `afbj`), status `STRUCTURALLY_VERIFIED`, build green.
- **ABI facts recorded, not papered over**: `0x00424C10` reads a second stack argument, so it belongs to the two-word phase-0 dispatch at
  `0x004012DE` (push context, push record, clean 8), not the one-word `0x00402481` site; `0x00424CA0` is a four-word window-procedure
  frame; `0x00424280` is one-word and `FrameHandler`-compatible. `FrameHandler` itself is unchanged.
- **Globals** `0x0057BC58`/`0x0057BC5C` (cursor x/y) and `0x0057BC60` (Cursor class id) had no owner in the tree; each is now defined
  exactly once, in the file of the native that owns it, as image zero-fill with the retail signedness.
- **Header correction**: `task_types.hpp` still called the link-native arguments `event_id, context`; the corrected bodies take
  `class_id` first and the event word second (`0x00401400` masks it to its low byte, `0x00401530` uses the full word), so the parameters,
  their documentation and the two natives now agree.
- **Still blocked, with the reason recorded**: `0x00424C40` needs `0x004B4E00` (absent from the manifest; it is a 317 B single-block leaf,
  the cheapest unblock), `0x00424CE0` needs `0x00491CF0`, and the Wave B registrars need `0x00416920`/`0x004528F0`/`0x00460BE0`/
  `0x004B3790`/`0x004CC2A0`/`0x00491610`/`0x004905A0`/`0x00490D10`/`0x004903B0`. `docs/fidelity/evidence/startup-chain-next-callees.md` §9
  carries the per-address blocker table.
- **Reachability unchanged**: the candidate still stops at the `0x00402930` guard, so these four natives are built and structurally
  verified but not yet reached at runtime; no runtime claim is made.

### 2026-10-25 - correction: the candidate "scene" capture was the desktop, not candidate output (internal build, no release)

- **Correction of the 2026-10-25 paired scene entry above**: the candidate image was not the candidate's output. Captured region
  `523,286 634x476` is **pixel-identical** to the same rectangle captured after the candidate process died (identical fraction 1.0,
  mean absolute difference 0.0), so the window contributed no pixels at all and the image shows the desktop behind it.
- **Why**: the candidate creates its window (`owner igi1.exe`, title `Innerloop`, `634x476` at `523,286`) and then terminates at the
  `0x00402930` guard within milliseconds - before DirectDraw/Direct3D initialisation and before any frame is painted. `screencapture -l`
  additionally reports `could not create image from window`, which is the same fact from the window server side: no composited surface
  exists because nothing was drawn.
- **Tooling gap and fix**: `matches_window` only compares the window's bounds with the image size - it proves the capture rectangle is
  the window, **not** that the window drew anything. `scripts/tester/game_scene_capture.py` now has `--verify-painted`: it re-captures the
  same rectangle after terminating the process and compares. Measured on this run: retail `painted: true` (`painted_fraction` 1.0),
  candidate `painted: false` (`painted_fraction` 0.0).
- **Artifact renamed** to `screenshots/pair-2026-10-25/candidate-level1-window-unpainted.png` so it cannot be mistaken for game evidence.
  It records window geometry only. No candidate render exists for Level 1 until the startup sequence runs.

### 2026-10-25 - paired level 1 scene capture, both games, window-scoped (internal build, no release)

- **Tool fix**: `scripts/tester/game_scene_capture.py` now supports an open title (`--title ''`), a minimum window area, and a
  configurable poll interval. The candidate's window appears for well under a second before the `0x00402930` guard terminates the
  process, so the default 200 ms poll missed it entirely; at a 10 ms poll the window is found and captured.
- **Retail capture**: `igi.exe window level1`, Escape sent to skip the intro, window `igi.exe` title `IGI`, id `11744`, pid `45132`,
  bounds `1024x668`, captured by CGWindowID at `2048x1336` (scale 2, `matches_window: true`) ->
  `screenshots/pair-2026-10-25/retail-level1-scene.png`.
- **Candidate capture**: `igi1.exe window level1`, window owner `igi1.exe`, **title `Innerloop`** (the retail `WNDCLASSA` class name the
  candidate transcribes), id `11763`, pid `46311`, bounds `634x476` at `523,286`, captured at `1268x952` (scale 2,
  `matches_window: true`) -> `screenshots/pair-2026-10-25/candidate-level1-scene.png`. The window capture fell back to a region capture
  of the window rectangle because `screencapture -l` cannot build an image for a wine Direct3D-backed window; the region is exactly the
  window's bounds, which is why `matches_window` is true.
- **Comparison**: the images are not comparable as renders - the candidate dies at the guard before it draws a frame, so its capture is a
  window shell, not Level 1. Different pixel content is expected and no equivalence is claimed. The capture tooling itself is verified on
  both programs: pid, window id, bounds, image size and scale agree for each.

### 2026-10-25 - reverse-engineering skills applied: retail registrar names mined (internal build, no release)

- **Selected skills** (recorded in `docs/agents/re-skills-applied.md`): `igi-auto-re-agent` for registrar push-pair mining,
  `igi-validation` for the dual-engine Gate/Ghidra+radare2 rule, `reverse-engineering` as the technique catalogue. Checked and
  recorded: radare2 present; **Ghidra headless MCP on `127.0.0.1:8771` is not answering**, so no dual-engine score can be claimed yet;
  `igi-validation`'s documented image path does not exist on this machine (this repo's authority image is the Downloads copy).
- **Added**: `scripts/registrar_pairs.py` - for every `call` into the four retail registrars (`0x4B8890`, `0x4BAAC0`, `0x4B88C0`,
  `0x4B88F0`) it back-decodes the preceding pushes (cdecl: the name string is pushed last, so the push nearest the call is the name),
  reads the C string from the image and cross-references the function with the manifest.
- **Result**: **167 call sites, 166 unique registered natives, 88 already implemented here, 78 missing** (19 call sites undecoded).
  Table: `docs/fidelity/evidence/registrar-pairs.md`. This is the only source of authoritative retail names for these routines:
  `Flow_RequestEvent` (`0x00402780`), `GOStart` (`0x00403660`), `Game_SetMission` (`0x00415890`), the `MenuManager_*` family,
  `MovieSubtitle_RegisterFont/RegisterText`, `Game_SetMusicVolume/SetSFXVolume`, and 70 more.
- **Structural finding**: none of the missing named natives is a direct callee of `0x00402930` and none appears in the call-order walk -
  they are script natives invoked indirectly by the QVM through the registry, so static call-graph work can never reach them; they have to
  be landed from the script/mission requirements. This is a gap the call-order list alone cannot show.

### 2026-10-25 - corrected the two QTask link natives from retail operands (internal build, no release)

- **Corrected (fidelity defect, found by `chain-worker`, fixed by Lead)**: `QTask_LinkQTaskEventHandler` (`0x00401400`) had its two
  argument roles swapped. Retail `0x00401408` loads the SECOND argument into `ebp` and takes the phase from its low byte, while
  `0x00401429` loads the FIRST argument and masks it to get the index base; `0x0040145F` compares the type record's first word against
  the FIRST argument and `0x00401474`-`0x0040147A` recurse with the SECOND. The tree body used the first argument for phase/recursion and
  the second for the comparison, which with the registrar call shape `(class id, event id, handler)` made every phase link collide on
  `class + (class & 0xff) * 0x180`. Both bodies now match retail; `0x00401530` also had its record comparison against the wrong argument,
  and retail does not low-byte-mask its stride argument - both corrected.
- **Re-measured**: `0x00401400` and `0x00401530` both rebuild green and score **CFG 1.000** (7 blocks / 10 edges each) against retail.
- **Why it matters**: the class registrars pushed by the startup sequence call these two natives; the wrong operand made the handler
  table land in the wrong phase rows, so the startup-chain transcription could not be validated against retail registration state.
- **Process**: `docs/agents/subagent-roles.md` records the owner's subagent split (Lead integrator, implementer, verifier/tester, research)
  with the mandatory "no self-verification" rule and the parallel-vs-sequential decision rule.

### 2026-10-25 - window-only scene capture: launch, load, Escape, capture (internal build, no release)

- **Fixed**: the earlier captures were full-screen grabs of the desktop (they showed the editor, not the game). New
  `scripts/tester/capture_game_window.py` enumerates windows by `CGWindowID`, raises the game window, captures it with
  `screencapture -l <window id>` only, and reports pid/window id/bounds/image size/scale so the image is provably that window
  (`matches_window: true`), with a region fallback when a window has no image backing.
- **Added**: `scripts/tester/game_scene_capture.py` - the owner's scene-check sequence in one command: launch
  `<exe> window level<N>` from the game directory through the CrossOver wine launcher, poll until the game window exists, wait the
  load time, bring it to the front, send Escape to skip the intro/cutscene, capture only the window, then terminate the process.
- **Verified (retail)**: `igi.exe window level1`, Escape sent (`CGEventPostToPid` + `osascript`), window `igi.exe` title `IGI`
  id `11715` pid `41085` bounds `1024x668` at `328,177`, captured `2048x1336` (scale 2, `matches_window: true`) ->
  `screenshots/pair-2026-10-25/retail-level1-scene.png`.
- **Candidate**: `igi1.exe window level1` through the same sequence produced **no window**: it still fails closed at
  `0x00402930` before a capturable game window exists. No candidate scene screenshot is claimed; T1 remains blocked on T0.

### 2026-10-25 - diagnostic logging + paired same-level capture (internal build, no release)

- **Added**: `engine/include/igi/diagnostic_log.hpp` - diagnostic-only logging infrastructure (owner rule 3(a)): never changes control
  flow, values or ordering, writes `$TMPDIR/igi1-diagnostic.log`, one line per call, flushed. First call site: the fail-closed guard in
  `class_registration.cpp`, which now logs `fail-closed: startup init sequence not transcribed, stopping at AI graph` before throwing.
  Verified live: the candidate ran under CrossOver wine and the log line was produced at
  `[0x00402930] fail-closed: startup init sequence not transcribed, stopping at AI graph`.
- **Captured**: same-level runs of retail `igi.exe` and candidate `igi1.exe` with `window level1`, screenshots by the macOS
  `screencapture` tool: `screenshots/pair-2026-10-25/retail-level1.png` and `screenshots/pair-2026-10-25/candidate-level1.png`.
- **Result**: retail ran (process alive at capture); the candidate reached the guard and terminated, so the candidate capture is not a
  level render. No release; T1 still blocked on T0.

### 2026-10-25 - T0/T1 start: flow order tool, dead-code and port-constant fixes (internal build, no release)

- **Task state**: T1 (3D objects + terrain visible) is **not** started because T0 blocks it: the candidate still fails closed at
  `QTask_RegisterClasses (0x00402930)`, which is the retail startup init sequence, not a registrar. Nothing in this entry is a release.
- **Added**: `scripts/flow_order.py` and `docs/fidelity/evidence/chronological-native-order.md` - the retail call flow from the PE entry
  point in call order: 3022 functions, 678 implemented, 2344 missing. This is the work order for not missing any method in the flow.
- **Added**: `docs/fidelity/MVP-task-breakdown.md` - the owner's small stacked tasks T0-T7 (startup chain, 3D objects, terrain,
  human player, weapons, AI, sound, menu) with the static + runtime + screenshot + changelog acceptance rule for each.
- **Fixed**: `flow.cpp` per-frame dispatch called a hardcoded `phase = 1U`; retail `0x00402462` calls `0x004E7530`, which is already
  transcribed as `Core_GetEventWord_4E7530`, so the dispatch now calls it. One port-only constant removed from the dispatch path.
- **Fixed**: removed the unused `link_handler` helper from `task_types.cpp` (dead code, rule 16); no native called it.
- **Launch attempt**: the candidate was copied to the game directory and started through the CrossOver wine launcher; no running game
  window and no 3D output were verified, and the capture `screenshots/internal/2026-10-25-igi1-t0-launch-attempt.png` is recorded as a
  launch attempt only, not as game evidence. No screenshot claim is made for T1.


### 2026-10-24 — startup-prefix internal build (no release)
- **State**: builds and starts, then terminates by design at `QTask_RegisterClasses`
  (`0x00402930`): the AI-graph registrar is not transcribed, so class ids after it
  would not match retail. Not runnable as a game, therefore **not a release**.
- **Achieved internally**: `WinMain` at retail `0x0048F530` as the only entry;
  startup prefix reproduced (mutex/duplicate path, `CoInitialize`, WNDCLASSA
  `"Innerloop"` style 3, 16 startup setters in retail order, four parser
  registrations, `CreateWindowExA` style `0x37C80000`, 640x480 at X=520/Y=285,
  `ShowCursor(0)`, `ShowWindow(10)`, `StoreWindowHandle`); B3 WinProc
  handler-chain container init (`0x48F674-0x48F6A6`) with the retail 12-byte
  `{next,+prev_link,+handler}` entry layout.
- **How**: matching decompilation of retail `dea34892…` with Binary Ninja (single
  decompiler), Radare2 CFG, angr exact entry; paired Frida prefix probes
  (retail `PREFIX_RETAIL_MATCH 178/179`; the chain side effect was the only real
  divergence and B3 now implements it).
- **Internal candidate**: `igi1.exe` sha256 `30c39039…` (deployed by
  `build_no_verify.sh`).
- **Evidence screenshot (internal, not a release)**: `screenshots/internal/2026-10-24-igi1-startup-prefix.png`.
- **Blocking next step**: transcribe the registrar chain starting at
  `0x00508F80` so `QTask_CreateTask` returns and the window stays alive.


### Planned

- Follow the Level 1 playable-demo sequence in MVP.md and plan.md: Windows x86 build, OBJECTS.QVM model placement, textured terrain, complete visual verification, then playable simulation and gameplay systems including sound.

### Current implementation state

- The Windows x86 target now enters at the retail WinMain (`0x0048F530`) and reproduces the retail startup prefix; it still terminates at the documented registrar boundary (`QTask_RegisterClasses 0x402930`) and is therefore not a playable build.
- Resource, MTP, MEF container, and LOOP texture parsing have focused tests.
- Level 1 QVM object execution, 3D/terrain rendering, player and AI simulation, combat integration, and sound playback are not yet connected into a playable mission.
