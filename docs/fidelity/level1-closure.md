# P05 — Level 1 dependency, script and entry-to-spawn closure research

**Task:** `task-2` — P05 Level 1 dependency, script and entry-to-spawn closure research (`tasks/plan.md`).
**Date:** 2026-10-24 (UTC+05:30); research window 2026-10-23 21:52 UTC – 2026-10-24 03:40 UTC+05:30.
**Authority:** retail `/Users/haseeb-mir/Downloads/IGI/igi.exe`, SHA-256
`dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01` (PE32 x86, image base `0x00400000`, IGI 1.2-era file, 2002).
**Checkout:** `/Users/haseeb-mir/Documents/Code/project-open-igi`, branch `mvp/level1-integration`, HEAD
`a402692f56cc75e67e39658663252e8ab51b3f34` (working tree inspected read-only).
**Method:** read-only static analysis of the retail image with radare2 6.2.0 and igi1conv v1.11.0 (QVM
`info|disasm|decompile`) over the already-installed retail game data, plus the **existing captured runtime
artifacts**. No build was run, no game process was launched, no Frida/IGI-Probe session was opened (the Lead owns
the runtime lease), and the retail game directory was not modified. Only this file and
`build/audit/p05-20261023T215247Z/` were written.

**Evidence labels used throughout**

| Label | Meaning |
|---|---|
| `[observed]` | Recorded in an existing retail runtime capture under `docs/fidelity/evidence/` or `build/runtime-traces/` (no new runtime capture was made). |
| `[static]` | Disassembly / decompiled QVM / directory inventory produced by this research or an existing cited static note, with exact VA or file offset. |
| `[hypothesis]` | Inference not yet proven by observation or complete static data; must not be treated as a retail fact. |

**Raw artifacts** (all under `build/audit/p05-20261023T215247Z/`, digest list in Appendix A): QVM info/disasm/
decompiled text for `config.qvm`, `humanplayer.qvm`, `MISSION.QVM`, `OBJECTS.QVM`, `TERRAIN.QVM`, `lod.qvm`;
radare2 dumps for `Main` (0x48F530), CLI parser (0x48F360), `QTask_CreateTask` (0x401000),
`Level_Start` (0x415B30), `Level_Load` (0x4F0E10/0x4F0D40), `Qvm_Load` (0x4B80B0), `Task_New` (0x4F0780),
`HumanPlayer` type init (0x40FE30), `Human` type init (0x45E450), `DefineHumanPlayerGeneral` (0x410070);
`retail-observed-105.json`, `closure-groups.json`, `level1-inventory.json`, `level1-assets.json`,
`config-calls.json`, `humanplayer-general-args.json`.

---

## 1. Direct-launch branch: `igi.exe window level1`

### 1.1 Command-line token matcher (0x48F360) — `[static]`, verified this session

`Main` (0x48F530) first stores its command-line argument pointer: `push [esp+0x480]; call 0x48F0C0`
(0x48F5F3/0x48F5F4; `StoreCommandLine` is `mov eax,[esp+4]; mov [0x5C8BC8],eax; ret`). The parser then works
off that stored NUL-terminated string (`0x48F080 GetCommandLine` returns `[0x5C8BC8]`):

* `0x48F360 AppMain_ParseCmdLineArgs(name, handler)` — caller pushes `handler`, then `name`, so cdecl
  argument 0 is the case-insensitive switch literal (e.g. `"Window"`, `"Level"`) and argument 1 the handler.
* It reads the command line through `0x48F080 GetCommandLine` (returns `[0x5C8BC8]`), copies it to a local
  buffer, tokenizes with `0x4A5A0C` on space/TAB, folds `a`–`z` by `+0xE0` and compares each token as a
  case-insensitive **prefix** of `name`; on a match it calls `handler(token + strlen(name))`, i.e. the handler
  receives the remaining suffix of the matched token.
* Therefore `igi.exe window level1` registers: `"winter"`-style tokens only match registered literals; `window`
  matches `Window` with an *empty* suffix (handler ignores its argument), and `level1` matches `Level` with
  suffix `"1"`.

Registered startup switches (registration sites are inside `Main`, all `[static]`):

| Token | Registration site(s) | Handler | Handler behavior |
|---|---|---|---|
| `Window` | push `0x48F350` @0x48F680, push `"Window"` (`0x541EB8`) @0x48F687, call `0x48F6A6` | `0x48F350` | `push 0; call 0x48F120` → `SetScreenMode(0)` = **windowed** (`[static]`, dump `r2-cmdline-literals.txt`) |
| `WinInput` | 0x48F6AB / 0x48F6B0 / call 0x48F6B5 | `0x48F920` | `push 1; call 0x48F280` |
| `Germany` | 0x48F6BA / 0x48F6BF / call 0x48F6C4 | `0x48F930` | `0x48F300("Germany"); 0x48F2B0(1)` |
| `USA` | 0x48F6C9 / 0x48F6CE (literal 0x5382C8) / call 0x48F6D3 | `0x48F950` | `0x48F300("USA")` |
| `HardcoreGerman` | push `0x48F960` @0x48F7AF, literal `0x541E38` @0x48F7B4, call 0x48F7BE (after `0x401000` returns) | `0x48F960` | `push 0; call 0x48F2B0` |
| `Level` | `Game_Init` `0x415530`: `push 0x415B10; push "Level"; call 0x48F360` @0x41553A (`[static]`, `r2-cmdline-xrefs.txt`) | `0x415B10` | `atoi`-like `0x4A5490(suffix)`, stores result in mission-id global `0x539560`; also calls `0x48F210` which stores the value at `0x5C8C08` (startup-mode flag) |

Notes:
* The `Window` handler is the only thing that distinguishes windowed from fullscreen; with no `window` token the
  game keeps its default (fullscreen) screen mode. `docs/fidelity/runtime-test-workflow.md` requires `window` for
  the paired scene test, and the static branch confirms why.
* The `Main` code path after the four initial switches is: `0x498480` DirectX version check (`cmp eax, 0x700`,
  i.e. requires DirectX ≥ 7.0, message `"This application requires DirectX version %d.%d or greater to run."` at
  0x541E68) → `CreateWindowExA` (class/title from the `"Innerloop"` literal at `0x541D68`) → `ShowCursor`,
  `ShowWindow(hwnd, 10)` → `0x48F0E0 SetWindowHandle` → `0x498460` → `call 0x401000` → store loop-status byte
  `[0x5C8DE8]` → parse `HardcoreGerman` → per-frame loop (`0x498460`, `0x48F170 AppContext_GetIsActive`,
  `0x401050 QTask_OpenEvent`, else `Sleep(500)`; `PeekMessageA`/`TranslateMessage`/`DispatchMessageA`;
  `DestroyWindow` when `[0x5C8DE8]` clears; `ReleaseMutex` on the normal exit path at 0x48F8CA)
  (`[static]`, dumps `r2-main-window.txt`, `r2-0048F530-main.txt`).

### 1.2 `level1` means mission **id** 1, not a directory name — `[static]` + `[observed]`

* `0x415B10` = `mov eax,[esp+4]; push eax; call 0x4A5490; push 0; mov [0x539560],eax` — `0x4A5490` is the
  atoi-analogue, `0x539560` the mission-id global (`[static]`, `r2-cmdline-xrefs.txt`, `r2-setmission.txt`).
  Writers of `0x539560`: `0x4158A1` (menu mission selection path) and `0x415B1C` (this CLI handler); readers
  `0x415B45`, `0x415B59`, `0x416298`, `0x416300` (`[static]`, `r2-missionid-writers.txt`).
* `MISSIONS/location0/LEVEL1/MISSION.QVM` decompiles to
  `DefineMission(1, "Mission 1 Name", "Mission 1 Desc", "", 2, "missions/location0/level1", "missions/location0/common", "missions/location0/level1", "location0", "level1", "mission1.spr")`
  (27 instructions, `[static]`, `MISSION.QVM.decompiled.qsc`). Levels 2/4/7/10/14 define ids equal to their
  number with the same root pattern, so `level1` → id 1 → `MISSIONS/location0/LEVEL1`.
* `Level_Start` `0x415B30` reads `[0x539560]`, resolves it through the mission registry `0x485170`; a null
  lookup logs `"The mission ID %d does not exist."` and spins at `0x415B6D` (no graceful exit)
  (`[static]`, `r2-levelstart.txt`, `RETAIL_MAIN_FLOW.md` §Level_Start).
* **Observed retail acceptance of this exact command line:**
  * `build/runtime-traces/retail-suspended-level1-20261019T133003Z/capture.json` — `arguments: ["window","level1"]`,
    retail SHA-256 `dea34892…`, requested cwd `Z:\Users\haseeb-mir\Downloads\IGI`, 98 hooks installed / 0 failed,
    82 natives called, and `declared_level_script = MISSIONS/location0/LEVEL1/OBJECTS.QVM` SHA-256
    `90acf17a8af76ddc24d40f061e796fc5964feed9298f321cb02b160ceaa89971` (`[observed]`; that file itself records
    the limitation "File identity for requested level, not a decoded runtime Qvm_Load filename").
  * `docs/fidelity/evidence/20261023-level1-paired-reachability.md` — suspended-spawn paired run with the same
    `window level1` argv, 105/120 MVP addresses executed in retail, 3,652,554 hook hits (`[observed]`).

**Conclusion (§1):** `igi.exe window level1` is a supported, observed direct-launch path: `window` selects
windowed mode through `0x48F350→0x48F120(0)`, `level1` sets mission id 1 through `0x415B30`'s registration of
`0x415B10`, which then loads `MISSIONS/location0/LEVEL1/OBJECTS.QVM`. There is no directory-name token and no
level-specific code path.

---

## 2. Ordered entry → startup → Level 1 load → world → player-spawn call map

Every row gives the retail VA (or import), the purpose, the evidence label, and — where the existing 2026-10-23
capture saw the address — its **first-hit sequence** in the 105/120 retail run (`obs#N`, see
`retail-observed-105.json`). The observed sequence is used only as supporting order evidence; the ordered map is
the static call graph.

### 2.1 Pre-main → `Main`

| # | Step | Evidence |
|---|---|---|
| 1 | CRT entry `0x4A511B` (not in `native-verification.json`; identified in `RETAIL_MAIN_FLOW.md`) | `[static]` prior r2 note |
| 2 | `Main` `0x48F530`, 994 bytes / 26 blocks | `[static]` + `obs#9` |

Pre-`Main` static-init native hits seen by the capture: `Mem_AllocChunk` `0x4AA670`, `Mem_AllocWithRetry`
`0x4AA682`, `Mem_AllocateRawBlock` `0x4AA6AE` (`obs#1–3`), `Camera_ApplyBandFilter_10` `0x4A7157`,
`Camera_ApplyLineFilter_24` `0x4AC655`, `Camera_SetFieldOfView_24` `0x4AC620`, `Camera_ApplyBandFilter_24`
`0x4AC66B`, `Camera_ApplyWideScreen_24` `0x4AC6FD` (`obs#4–8`) — i.e. the probe installed 98–120 hooks before
resume and therefore sees pre-main static-init calls (`[observed]`).

### 2.2 `Main` startup sequence (static order)

| # | Call / action | Evidence |
|---|---|---|
| 1 | `CreateMutexA(NULL, TRUE, "IGIMUTEX")` @0x48F544 with name pointer `0x00541ECC`; duplicate path MessageBox `"IGI is already running."` (0x48F8F0) | `[static]` + `[observed]` in 2026-10-19 matrix; candidate deliberately uses `IGI1MUTEX` (see §6) |
| 2 | `StoreCommandLine` `0x48F0C0` — pointer pushed @0x48F5F3 (`mov ecx,[esp+0x480]`), call @0x48F5F4, stores the command-line string pointer at `0x5C8BC8` | `[static]`, manifest status RUNTIME_TESTED (body `mov eax,[esp+4]; mov [0x5C8BC8],eax`) |
| 3 | `StoreInstanceHandle` `0x48F0D0` (call @0x48F5FA, stores its argument at `0x5C8BC0`) | `[static]`, manifest RUNTIME_TESTED |
| 4 | `SetConfigFileName` `0x48F0F0("filesys.cfg")`, `SetAppTitle` `0x48F100("Innerloop")` | `[static]` |
| 5 | app-context startup setters, in order: `0x48F140(1)`, `0x48F1A0(0)`, `0x48F1C0(1)`, `0x48F1E0(0)`, `0x48F240(1)`, `0x48F260(1)`, `0x48F210(1)`, `0x48F2B0(0)`, `0x48F300(0x00567C74)`, `0x48F200(0)` | `[static]` (`r2-0048F530-main.txt`); `0x48F140` `obs#10`, `0x48F260` `obs#11` |
| 6 | `ParseCmdLineArgs("Window",0x48F350)`, `("WinInput",0x48F920)`, `("Germany",0x48F930)`, `("USA",0x48F950)` | `[static]` (see §1.1) |
| 7 | `0x498480` DirectX version check (requires ≥ 7.0) | `[static]` |
| 8 | `CreateWindowExA` @0x48F770; `ShowCursor`, `ShowWindow(hwnd,10)`, `0x48F0E0 SetWindowHandle` | `[static]` |
| 9 | `0x498460` | `[static]` + `obs#12` |
| 10 | **`0x401000 QTask_CreateTask`** @0x48F7AA | `[static]` + `obs#13` (manifest BUILD_VERIFIED) |
| 11 | store return (loop status) in `0x5C8DE8`; `ParseCmdLineArgs("HardcoreGerman",0x48F960)` | `[static]` |
| 12 | frame loop: `0x498460`, `0x48F170 AppContext_GetIsActive` (`obs#53`), if active `0x401050 QTask_OpenEvent` (`obs#54`) else `Sleep(500)`; `PeekMessageA`/`TranslateMessage`/`DispatchMessageA`; `DestroyWindow` if `0x5C8DE8==0`; normal exit `ReleaseMutex` @0x48F8CA | `[static]` |

### 2.3 `QTask_CreateTask` 0x401000 — engine bootstrap (67 bytes, `[static]` fully disassembled this session)

| Call site | Action |
|---|---|
| 0x401000/0x401005 | `push "IGI"` (0x536144); `SetAppTitle 0x48F100` |
| 0x40100A/0x40100C | `push 1; call 0x48F330` → **force-precompiled flag `0x5C8BF0` = 1** (`0x48F330` is `mov [0x5C8BF0],eax`; its only caller is 0x40100C) |
| 0x401011 | `QTask_RegisterClasses 0x402930` (`obs#14`) |
| 0x401016/0x401018 | `Flow_CreateHandler(0)` `0x402800` (`obs#46`) |
| 0x401020 | `0x48F220 GetStartupMode` → reads `0x5C8C08` (written by `0x48F210`) |
| 0x401029/0x40102B | if startup mode == 0: `Flow_SetState(4)` `0x4028A0` (`obs#52`) |
| 0x401036/0x401038 | else: `Flow_SetState(2)`; both paths `mov al,1; ret` → returns 1 |

`qvm_native_loader.cpp` already records the same force-precompiled relation
(`Script_GetForcePrecompiledState` reads the port's flag), and
`engine/src/retail/core/qtask_create_task.cpp` is a BUILD_VERIFIED transcription of this body. The candidate
`Main` does **not** call it (see §6).

### 2.4 `QTask_RegisterClasses` 0x402930 and the two registration arms

`0x402930` is a flat, prologue-less list of class-registration calls (791 bytes). Its first calls (`[static]`,
`r2-registerclasses.txt`):

`0x508F80`, `0x489560`, `0x407D60` (catalog `HumanPlayer_Setactivematerial`), `0x488BB0`, `0x402FF0`,
**`0x415530 Game_Init`** @0x402949, `0x4C1780`, **`0x401FC0 Flow_Init`** @0x402953, then the long remainder
(`0x488800`, `0x488620`, `0x487BE0`, `0x4872C0`, `0x415070`, `0x486900`, `0x485A40`, `0x485330`, `0x4851B0`,
`0x484F30`, `0x4825C0`, `0x4813E0`, `0x4801B0`, `0x47E700`, `0x47CF70`, `0x47CF20`, `0x47CAB0`, `0x413DF0`,
`0x407290`, `0x47B4B0`, `0x477E30`, `0x477220`, `0x476730`, `0x476110`, `0x475AC0`, `0x475380`, `0x473ED0`,
`0x4739D0`, `0x4733F0`, `0x472D00`, `0x472A70`, `0x472670`, … — full list in `r2-registerclasses.txt`; 30 of
these registration targets are **not** in `native-verification.json`).

* **`Game_Init` 0x415530** — registers the `"Game"` task type (`TaskType_Register("Game", 0x158, …)`), allocates
  15 event ids, and links phase handlers through `0x401400/0x401530`: phase 0 = `Level_Start 0x415B30`, phase 1 =
  `0x416260`, phase 2 = `0x416550`. It also registers the `Level` command-line switch (`0x415B10`) — see §1.1.
  (`[static]`; the retail body may not call `0x48F360` here for any other switch — `hardcore` handling is the
  later `HardcoreGerman` call in `Main`.)
* **`Flow_Init` 0x401FC0** — registers the `"Flow"` task type (`TaskType_Register("Flow", 0x54, …)`,
  `[static]`: `str "Flow"` @0x536274) and installs three event handlers: event 0 → `0x402070`, event 1 →
  `0x402260 Flow_RunHandler`, event 2 → `0x402740` (`[static]`, `r2-flowcreatehandler.txt`). It also registers the
  `"FLOW_EVENT_QUIT"` symbol through `0x4B8810`.

### 2.5 Flow event 0 (`0x402070`) — config + human-player parameter load (`[static]`, exact sequence)

`Flow_CreateHandler(0)` `0x402800` resolves the `"Flow"` class id and calls `0x4012A0`, which dispatches flow
event 0 to handler `0x402070`. `0x402070` then runs, in order:

| Offset | Action |
|---|---|
| 0x402079 | store Flow context at `0x567C8C` |
| 0x4020A3/0x4020A5 | `push 0x1e; call 0x402820` |
| **0x4020B0/0x4020C9** | `push "LOCAL:config.qsc"` (0x5362F4) → **`Config_Read 0x405850`** (`obs#47`, 1 hit) |
| 0x4020CE | `0x413B70` — **not in manifest** (identity unknown) |
| **0x4020D3** | **`HumanPlayer_LoadParameters 0x4137A0`** (`obs#49`, 1 hit) → `"LOCAL:humanplayer/humanplayer.qsc"` → `Qvm_Load 0x4B80B0` → `Qvm_Execute 0x4B85B0` → `Qvm_Cleanup 0x4B83D0` (`[static]`, `r2-humanplayer-loadparameters.txt`) |
| 0x4020D8 | `0x408350` — not in manifest |
| 0x4020DD | `"LOCAL:magicobj"` → `0x4C4930` |
| 0x4020E7 | `"LOCAL:animtrigger"` → `0x4EC0C0` |
| 0x4020F1 | `"LOCAL:physicsobj"` → `0x4EE030` |
| 0x4020FB | `"LOCAL:weapons/weaponconfig.qsc"` → `0x4071E0` (weapons = later stage) |
| 0x402109… | `0x4EDCB0`, `0x4012A0`, `0x406DF0`, stores Flow context fields +0x28/+0x2C/... |

`Config_Read 0x405850` itself (`[static]`, `r2-configread.txt`): `Qvm_Load(path)` → if non-null `Qvm_Execute` →
`Qvm_Cleanup` → `0x4058A0 Config_SaveSoundOptions`, `0x405980 Config_NoD3DDriversWithHard`,
`0x405B30 Config_Language_2`; if the load fails it calls `0x405300 Config_SaveGraphicOptions` and the same three
apply functions. Because `Qvm_Execute` runs the config QVM, its `GOInRemap` calls are the config's input-binding
writes: `obs#48` = `GOInRemap 0x4037C0`, **81 hits**, exactly 3 × 27 actions in the current (3-block) config file
(see §4.3); `HumanPlayer_LoadParameters`'s QVM execution is what reaches `DefineHumanPlayerGeneral 0x410070`
(`obs#50`, 1 hit).

**Resulting ordering (`[static]` order and `[observed]` first hits agree):** `QTask_RegisterClasses` finishes →
`Flow_CreateHandler(0)` (obs#46) → flow event 0 → `Config_Read` (47) → config `GOInRemap` (48) →
`HumanPlayer_LoadParameters` (49) → `DefineHumanPlayerGeneral` (50) → `InputPort_Process` (51) →
`Flow_SetState(4)` (52) → first frame `0x498460`/`0x48F170`/`0x401050` (53–54) → `Game_GetContext 0x402880`
(55) → `Flow_RunHandler 0x402260` (56) → Game phase 0 = `Level_Start`.

### 2.6 `Level_Start` 0x415B30 — Level 1 content load order

Mission id → mission record: `push [0x539560]; call 0x485170` (mission lookup, not in manifest), null → log
`"The mission ID %d does not exist."` + spin `jmp 0x415B6D` (`[static]`). Then (`[static]` `r2-levelstart.txt`,
order cross-checked with `docs/fidelity/flow/RETAIL_MAIN_FLOW.md` §Level_Start):

| Step | Call | Observed anchor |
|---|---|---|
| task-list bring-up | `0x402820(0x1e)`, `0x4028D0` | — |
| mission paths | `0x4B12F0` for `"MISSION:"`; `sprintf 0x4A53B3` `"LOCAL:%s/"` → buffer `0x57B568`; mission ptr → `0x57BABC` | — |
| level boilerplate | `0x453270`, `0x4D9950`, `0x416010`, `0x406390`, `0x48A440(9)`, `0x4C5B20(3500)`, `0x408390`, `0x4C1780` | — |
| **terrain** | `sprintf("%sterrain/terrain")` → **`0x4C5F00 Terrain_LoadTexturesAndLightmaps`** then **`0x4C6240 Terrain_Load`** (call sites 0x415C31/0x415C44 per `game_flow.cpp` comment) | 0x4C5F00 `obs#65`, 0x5175A0 Terrain_LoadTexturePackage `obs#66`, 0x4C6240 `obs#70`, 0x4C6670 Terrain_CreateMaterial `obs#71` (6 hits), 0x4C6760 Terrain_CreateTileMap `obs#72` (8 hits) |
| terrain params | `0x4B2610`, `0x4B25B0(1.0f, 0xDB840)`, `0x4CEEA0`, `0x4B25F0`, `0x4CEA60` | — |
| **level + common packages** | `"LOCAL:%s/"` → `0x4CFBD0`; `"LOCAL:common/"` → `0x4CFC00`; `0x4CEFA0`; `0x4F0CE0` | model/MEF package loads `obs#73–89` (see 2.7) |
| common resources | `"LOCAL:common/sounds"`, `"MISSION:sounds"` `0x4E68D0` (sound = later stage); `"LOCAL:common/sprites/sprites.res"` and `"LOCAL:common/textures/textures.res"` via `0x4B5F00 Resource_Load` + `0x4161D0`/`0x416130` | 0x4B5F00 `obs#30` |
| remaining setup | `0x46B070`, `0x4868A0`, `0x402890`, `0x485300`, `0x416D40`, `0x4F0CB0`, `0x4E6060`, `0x4E6550`, `0x416920`, `0x4E7D40`, `0x401AE0`, `0x4E7D70`, `0x4D2D30`, `0x4061C0`, `0x4061E0`, `0x4ED5E0`, `0x4F80E0`, `0x437500`, `0x4EDCC0`, `0x4F65B0`, `0x4F65D0(30)`, `0x485320`, `0x4F65C0` | — |
| **level script** | **`0x4F0E10 Level_Load(path buffer 0x57B568, 30)` @0x415F1A** | script-load chain `obs#39`/`43–45` (first hits earlier for config/humanplayer), level-script-dependent anchors `obs#90–105` |
| post-script | `0x416920`, `0x4CE830` | — |

### 2.7 Terrain / world asset load (`[static]` + `obs`)

* `Terrain_LoadTexturesAndLightmaps 0x4C5F00(context, base_path)` → observed `Terrain_LoadTexturePackage
  0x5175A0` (`obs#66`), `Texture_UploadMipmapPacket 0x4B23B0` (`obs#64`), `Texture_UploadPalettePacket
  0x4B7CF0` (`obs#68`). `Terrain_Load 0x4C6240` → `Terrain_CreateMaterial 0x4C6670` ×6 (`obs#71`) and
  `Terrain_CreateTileMap 0x4C6760` ×8 (`obs#72`) — matching the 6 `CreateTerrainMaterial` and 8
  `CreateTerrainTileMap` calls in `TERRAIN.QVM` (§5.3). `HeightMap_Enter 0x506F00` (`obs#91`),
  `TerrainLightMap_Init_2 0x4F82B0` (`obs#92`, 151,302 hits), `TextureModifier_Push 0x505E40` (`obs#90`,
  12,314 hits), `Terrain_TraverseContourQuadtree 0x4D14E0` (`obs#102`), `Terrain_DecodeChunkCommands
  0x510010` (`obs#103`), `Terrain_Draw 0x512C30` (`obs#104`).
* Level package → model/MEF path: `DynObj_LoadModelTexturePackage 0x4CF080`, `DynObj_LoadPackageAnimations
  0x4CF910`, `DynObj_LoadModelTextures 0x4CF790`, `DynObj_LoadPackagePalettes 0x4CF870`, `DynObj_LoadPackageMeshes
  0x4CF330`, `DynObj_LoadModelMesh 0x4CF390` (604 hits), `DynObj_LoadPackageLovs 0x4CF630`,
  `DynObj_LookupHashItem 0x4CF300`, `AnimIff_LoadAnimationFile 0x4D67A0`, `LODSettings_LoadParameters 0x4EDB60`
  (462 hits), `Model_GetAttachmentCount 0x4D0D10`, `Model_GetAttachment 0x4D0CF0` (`obs#73–95`).
* ILFF/IFF container readers `0x4B7470`, `0x4B7490`, `0x4B74D0`, `0x4B7540`, `0x4B7570`, `0x4B7580`, `0x4B7590`,
  `0x4B60F0`, `0x4CEAA0`, `0x517890/0x5178E0/0x517940/0x5179C0` (`obs#58–63, 75–80, 86`).
* Resource layer `0x4B59F0 Resource_CreateNode`, `0x4B5AF0 Resource_CanonicalizePath`, `0x4B5B30
  Resource_CalculatePathHash`, `0x4B5B60 Resource_FileExists`, `0x4B5C90 Resource_FreeNode`, `0x4B5F80
  Resource_RegisterBuffer`, `0x4B6380 Resource_Unload`, `0x4AAD10 Resource_MatchPrefix`, `0x4AD73D
  String_ToUpperChar` (1,443,550 hits — hottest native in the retail run) (`obs#22–30, 86`).

### 2.8 Level script load: `0x4F0E10` → `0x4F0D40` → precompiled `OBJECTS.QVM` (`[static]`, verified this session)

1. `0x4F0E10` formats **`"%s/objects.qsc"`** (`sprintf 0x4A53B3`, format string `0x54E090`) from the level path
   buffer and calls `0x4F0D40`.
2. `0x4F0D40` frees the 4096-entry task-slot array `0xA71890..0xA75890` through `0x401B20`, then calls
   `0x4B8930 Script_SetSymbolContext(level_context [0xA758A4], "Task_New")` — it binds the script symbol
   `Task_New` to the **level script's** symbol context (so the level QVM's `Task_New` calls resolve to
   `0x4F0780`), then `Qvm_Load 0x4B80B0(path)`, `0x4F0E50(path,0,0,1)`, `Qvm_Execute 0x4B85B0`,
   `Qvm_Cleanup 0x4B83D0`, `0x4F1A70`, `0x401D80`; on failure logs
   `"Level_Load(): Couldn't load script: %s"` (0x54E068) and spins. (`r2-levelload-body.txt`)
3. `Qvm_Load 0x4B80B0` (`[static]`, `r2-qvmload.txt`) copies the path, calls `0x4B1E90 String_FindExtension`,
   then **writes the literal `".qvm"` (bytes at `0x547290`) into the extension slot** — i.e. the logical
   `"/objects.qsc"` name is rewritten to `"/objects.qvm"` before any file access.
4. `Qvm_Load` then tests `0x48F340 Script_GetForcePrecompiledState` (`mov eax,[0x5C8BF0]`). Because
   `QTask_CreateTask` executed `0x48F330(1)` at 0x40100C, the flag is 1 for the whole run, so the
   `Resource_FileExists` (`0x4B5B60`) branch and `0x4BD220` QVM-file loader run: **retail executes the shipped
   compiled `OBJECTS.QVM`, not a runtime-compiled `objects.qsc`** (`[static]`, high confidence).
   `Script_GetDebugCompileState 0x48F1D0` is consulted earlier for the debug/compile-info path (observed 121
   hits, `obs#40`).

### 2.9 World-object creation and player spawn

* The level QVM's `Task_New(...)` opcodes dispatch to the registered native **`Task_New` = `0x4F0780`**
  (`[static]`: registration at `0x4EF79F` — `push -1; push 0; push 0x4F0780; push "Task_New"; call 0x4B8890`;
  `r2-tasknew.txt`). `Task_New` reads the class-name string, the class id and the parameter list through
  `0x4B8AD0/0x4B8A20/0x4B8A80`, then creates the task instance. **Its creation path into
  `QTask_CreateTask`/class constructors is not yet transcribed** (`[hypothesis]`: it goes through the task-type
  table populated by `TaskType_Register` and therefore through `0x401900`'s class registry; the exact call site
  inside 0x4F0780 was not traced in this pass).
* Registered classes relevant to spawn (`[static]`): `"Human"` type at `0x45E450` —
  `TaskType_Register("Human", 0xCF8, 0x4D9610, …)`; `"HumanPlayer"` type at `0x40FE30` —
  `TaskType_Register("HumanPlayer", 0xDF0, 0x460BE0, …)` plus event handlers `0x410370` (event 0),
  `0x454EE0` (event 1), `0x410840` (event 2), with the class id kept at `0x5385B0`. These two registrations are
  only 0x60 and 0x140 bytes of visible setup, but they are the hooks the script's
  `Task_New(0, "HumanPlayer", "TheGoodGuy", …)` record needs (§5.2).
* First per-frame player evidence `[observed]`: `InputPort_Process 0x4ED5C0` (`obs#51`, 1 hit at startup),
  `Human_CameraEyePosition 0x465820` (`obs#97`, 243,155), `Human_IsCrouched 0x463180` (`obs#98`, 243,245),
  `Transcontext_BuildViewBlock 0x497D70` (`obs#99`), `Display_ComputeScreenAspect 0x491D40` (`obs#100`),
  `TransContext_SetActiveTransContext 0x497E70` (`obs#101`), `Human_TaskViewReset 0x4659E0` (`obs#105`, **1
  hit** — consistent with a one-time player-view reset at spawn).
* The camera/evaluation side is otherwise still retail-unobserved in this set: `Human_RunUpdate 0x4109A0`,
  `Human_Init 0x45E450`'s descendants, `0x460BE0` ctor, and the `Task_New`→ctor call site are static-only.
  `docs/fidelity/mvp-candidate-execution-gap.md` lists 70 retail-observed-only natives with 29 having direct
  candidate call sites — the spawn/update group is part of that gap.

### 2.10 Observed first-hit phase bands (2026-10-23 capture, 105/120)

| Band | Sequence range | Content |
|---|---|---|
| pre-main static init | 1–8 | memory allocators, camera helpers |
| `Main` startup | 9–13 | `Main`, app-context setters, `0x498460`, `QTask_CreateTask` |
| class registration | 14–45 | `0x402930`, memory/hash/texture/qvm/resource/ILFF init, first `Qvm_Load` (39), `Qvm_ExecuteInstructions` (43), `Qvm_Cleanup` (45) |
| flow bootstrap | 46–52 | `Flow_CreateHandler`, `Config_Read`, `GOInRemap`, `HumanPlayer_LoadParameters`, `DefineHumanPlayerGeneral`, `InputPort_Process`, `Flow_SetState` |
| frame loop | 53–56 | `0x498460`, `AppContext_GetIsActive`, `QTask_OpenEvent`, `Game_GetContext`, `Flow_RunHandler` |
| terrain + resources | 57–72 | texture/ILFF/terrain package, `Terrain_Load*` |
| world models | 73–89 | MEF/MTP/LOD/DynObj package loads |
| lightmaps + script | 90–96 | `TextureModifier_Push`, `HeightMap_Enter`, `TerrainLightMap_Init_2`, hot flag predicate 0x4F1160, model attachments |
| player + render | 97–105 | human camera/crouch, transcontext, terrain draw, `Human_TaskViewReset` |

---

## 3. M1 required-native closure split

Grouped from the 120-address MVP set (`scripts/mvp_native_targets.json`), the 885-record manifest
(`docs/fidelity/native-verification.json`), and the control addresses this research added.
Machine-readable: `build/audit/p05-20261023T215247Z/closure-groups.json` (126 addresses: 84 seen in the
retail 105 run, 85 in the 120-target list, 103 in the manifest, **23 absent from the manifest**, **41 absent
from the 120 list**).

### 3.1 P05/P13 entry & startup control — 35 addresses

Present in manifest: `Main 0x48F530` (STRUCTURALLY_VERIFIED), `GetCommandLine 0x48F080`, `StoreCommandLine
0x48F0C0` (RUNTIME_TESTED), `StoreInstanceHandle 0x48F0D0` (RUNTIME_TESTED), `SetScreenMode 0x48F120`,
`SetAppTitle 0x48F100` (manifest name `SetAppTitle`, source path under `scripts/GameNatives/fx`),
`SetWindowHandle 0x48F0E0` (STRUCTURALLY_VERIFIED), `AppContext_SetIsActive 0x48F140` (RUNTIME_TESTED),
`AppContext_SetTerrainLightmapsUsed 0x48F260`, `QTask_CreateTask 0x401000` (BUILD_VERIFIED),
`QTask_RegisterClasses 0x402930`, `Flow_CreateHandler 0x402800`, `Flow_SetState 0x4028A0`,
`Game_Init 0x415530`, `Level_Start 0x415B30`, `sprintf 0x4A53B3`, `Debug_Error 0x4AF7B0`,
`Script_GetForcePrecompiledState 0x48F340`, `Script_GetDebugCompileState 0x48F1D0`.

**Missing from the manifest (14) — required to close the direct-launch path:**
`0x48F360` (CLI token matcher; only candidate path is `scripts/GameNatives/fx/0048F360-AppMain_ParseCmdLineArgs.cpp`),
`0x48F350` (`Window` handler), `0x415B10` (`Level` handler = mission id setter), `0x4A5490` (atoi),
`0x485170` (mission lookup), `0x401FC0` (Flow type/event registration), `0x48F210`/`0x48F2B0`/`0x48F200`/
`0x48F280`/`0x48F290`/`0x48F2A0`/`0x48F300` (thin app-context setters), `0x498480` (DirectX version check),
`0x4A511B` (CRT entry). `0x4A5490`, `0x48F360`, `0x485170` also have **no candidate source file**.

### 3.2 P13 task/tick/player-spawn — 25 addresses

`QTask_OpenEvent 0x401050`, `Game_GetContext 0x402880`, `Flow_RunHandler 0x402260`, `0x498460`,
`AppContext_GetIsActive 0x48F170`, `Config_Read 0x405850`, `GOInRemap 0x4037C0`,
`HumanPlayer_LoadParameters 0x4137A0`, `DefineHumanPlayerGeneral 0x410070`, `InputPort_Process 0x4ED5C0`,
`Human_TaskViewReset 0x4659E0` (RUNTIME_TESTED), `Human_IsCrouched 0x463180`,
`Human_CameraEyePosition 0x465820`, `Human_RunUpdate 0x4109A0`, `FallLanding 0x408460` (TRANSCRIBED),
`Transcontext_BuildViewBlock 0x497D70`, `Display_ComputeScreenAspect 0x491D40`,
`TransContext_SetActiveTransContext 0x497E70` (VERIFIED), plus `0x4F0E10 Level_Load`, `0x4F0D40`
(task-array reset + symbol bind + script exec), `0x4B8930 Script_SetSymbolContext`, `0x4F0780 Task_New`,
`0x460BE0 HumanPlayer ctor`, `0x4B8890` (native registration) — **the last five are absent from the manifest**
and are the largest catalog gap for P13, together with the class constructors registered at `0x40FE30`/`0x45E450`.
`0x401FC0 Flow_Init` is absent from the 120 list; `0x402800 Flow_CreateHandler` is present but observed only once
(its only substantive work is dispatching flow event 0).

### 3.3 P11 models / resources / placements — 50 addresses

All 50 are in the 120-target list and all have manifest records; 49 were retail-observed. Grouped:
MEF/DynObj loaders (`0x4CF080/0x4CF330/0x4CF390/0x4CF630/0x4CF790/0x4CF870/0x4CF910/0x4CF300`,
`0x4D0CF0/0x4D0D10`, `0x4D67A0`, `0x4EDB60`), ILFF/IFF readers
(`0x4B7470/0x4B7490/0x4B74D0/0x4B7540/0x4B7570/0x4B7580/0x4B7590`, `0x4CEAA0`, `0x4B60F0`),
resource manager (`0x4B59F0/0x4B5AF0/0x4B5B30/0x4B5B60/0x4B5C90/0x4B5F00/0x4B5F80/0x4B6380`, `0x4AAD10`,
`0x4AD73D`, `0x4B0F80`, `0x4B1E90`), memory/stream (`0x4AA670/0x4AA682/0x4AA6AE`, `0x4B0C60`, `0x4B0BA0`,
`0x4B0D40`, `0x4AF8F0`, `0x4AF960`, `0x4B1690/0x4B1780/0x4B17F0`), texture (`0x4B23B0/0x4B24C0/0x4B26A0/
0x4B26B0/0x4B7CF0`), formatting/error (`0x4A53B3`, `0x4AF7B0`).
**Status:** 47 STRUCTURALLY_VERIFIED, 1 RUNTIME_TESTED (`Resource_Load 0x4B5F00` semantic caveat), 1
BUILD_VERIFIED, 1 VERIFIED. No P11 address is missing from the manifest, but **semantics** are not uniform:
`Resource_Load` is still a candidate-cdecl guess, and the `DynObj_*` group is structurally verified only — no
paired typed runtime observation exists for package field layout (§5.5 assets are the input side).
Placement transforms are the open P11 item: `Task_DeclareParameters("Building"/"EditRigidObj", …,
"Orientation","Real32x9", …)` declares a 9-float orientation, while `OBJECTS.QVM` rows print only 3 orientation
components before the model string (e.g. `Task_New(1100, "Building", "LargeFuelContainer", 24316832.0,
-55962088.0, 174413136.0, 0, 0, 0, "438_01_1")`). Whether the script passes 3 Euler angles that the parameter
packer expands to a 9-float matrix, or the decompiler collapses a 9-value argument list, is **unresolved**
(`[hypothesis]`: 3-angle `Angle`-typed values; the `SplineObj` declaration uses `Real32x3` + separate `Angle`).

### 3.4 P12 terrain / textures / collision — 16 addresses

`Terrain_LoadTexturesAndLightmaps 0x4C5F00`, `Terrain_Load 0x4C6240`, `Terrain_CreateMaterial 0x4C6670`,
`Terrain_CreateTileMap 0x4C6760`, `Terrain_LoadTexturePackage 0x5175A0`, `TerrainLightMap_Init_2 0x4F82B0`,
`TextureModifier_Push 0x505E40`, `HeightMap_Enter 0x506F00`, `Terrain_TraverseContourQuadtree 0x4D14E0`,
`Terrain_Draw 0x512C30`, `Terrain_DecodeChunkCommands 0x510010`, `Render_GetActiveBackendId 0x4CEA80` —
all in the 120 list and manifest, 12 observed. **Missing from the manifest (4):** `0x4CFBD0` (level package
loader), `0x4CFC00` (common package loader), `0x4CEFA0` (common forms loader), `0x4F0CE0` — these sit between
`Level_Start` and the terrain package work and are required for terrain materials/lightmaps; `0x4C5B20`
(terrain budget 3500), `0x4B5F00`-driven `sprites.res`/`textures.res` loads, `0x4CEA60`, `0x4B2610/0x4B25B0`
are also outside the 120 set. Also relevant and **misidentified in the catalog**: `0x4F1160` is labeled
`Script_ParseEnumReal32_5` but the live function is an 11-byte predicate
(`mov al,[0xA758BE]; test; jne; mov al,[0xA758BD]; je; xor eax,eax; ret`) with **154,289 observed hits** at the
lightmap/attachment stage — it cannot be a script-enum parser (`[static]` + `[observed]` contradiction, §6 U6).

### 3.5 Later-stage (explicitly out of M1) — inventory-backed

From `OBJECTS.QVM`'s 1,360 `Task_New` records, the classes that can be deferred: soldiers/AI
(`HumanSoldier` 29, `HumanAI` 29, `AIGraph` 11, `AnimTask` 8, `AreaActivate` 8), weapons/ammo
(`Gun` 40, `GunPickup` 7, `AmmoPickup` 5, `GunMP5SD` 4, `GunDRAGUNOV` 1, `AddAmmo` 1), patrol data
(`PatrolPath` 37, `PatrolPathCommand` 141), mission UI/objectives (`StatusMessage` 23, `ComputerHilight` 17,
`CutScene` 5, `SCamera` 5, `SCameraControl` 2, `DefineComputerObjective` 1, `AlarmControl` 1), sound
(`ConditionalSound` 21, `AmbientArea` 21), scripted FX (`Heli` 2, `Binocular` 1, `ExplodeObject` 6) — 423 task
instances in total. Native-side exclusions: sound (`0x4E68D0` sound-dir init, `0x4E6550`, `0x4E7D40`,
`0x4E7D70`, `0x4EDCC0`, `0x4F65B0…`), weapons (`0x4071E0` weaponconfig), menus (`0x417xxx`/`0x4181xx`
Flow_SetState(2) menu path), AI (`HumanSoldier`/`HumanAI` constructors), objectives/HUD.

### 3.6 Candidate implementation state for the closure (read-only source inspection)

| Source | State | Closure impact |
|---|---|---|
| `engine/src/retail/core/main.cpp` (142 lines) | `Main` implemented but **never calls `0x401000 QTask_CreateTask`**; Win32 class `"IGI"`, `CreateWindowExW`, `IGI1MUTEX` | **Top blocker:** without `QTask_CreateTask` the registration/flow/level path cannot start; this alone explains most of the 70 retail-only natives |
| `engine/src/retail/core/qtask_create_task.cpp` | Faithful `QTask_CreateTask` transcription, BUILD_VERIFIED; comments already note "the candidate main does not call 0x00401000" | Needs the `Main` call wired |
| `engine/src/retail/core/class_registration.cpp` (65 lines) | `QTask_RegisterClasses` deliberately truncated: "still reports its untranscribed registrars and throws before the flow calls" (qtask_create_task.cpp comment) | Blocks `Game_Init`/`Flow_Init`/`HumanPlayer`/`Human` registration and every script class |
| `engine/src/retail/core/game_flow.cpp` (106 lines) | `Game_Init` transcribed (class + 15 events + 3 phases); **`Level_Start` is a hook dispatch only**; its own comment says the terrain/package/script content calls are "outside the MVP scope, so this transcription skips them" — this contradicts M1, which requires Level 1 terrain/models/player | Must not be treated as closed; the content order in §2.6–2.8 is the transcript target |
| `engine/src/retail/script/humanplayer_defines.cpp` | `DefineHumanPlayerGeneral` transcribed with exact scaling (4096000/108000, 4096, 30, 17+17 table walks), plus `Config_Read`/`HumanPlayer_LoadParameters` in related units | Good base for P14–P16; argument *meanings* beyond scaling are unset |

**Counts for the M1 closure decision:** of the 126 control/world addresses catalogued here, 84 were executed by
retail in the 2026-10-23 run, 23 have no manifest record at all, and 41 are absent from the 120-address MVP set.
The 120-address set is therefore **not** a sufficient native-closure target for M1: it omits the entry control
natives, `Level_Load`/`Level_Start` body, the level/common package loaders, `Task_New`, and the HumanPlayer
constructor chain.

---

## 4. Decoded QVM parameters for P14–P16

### 4.1 `humanplayer.qvm` — `DefineHumanPlayerGeneral` 43 arguments

File: `/Users/haseeb-mir/Downloads/IGI/humanplayer/humanplayer.qvm`, 1,457 B, SHA-256 prefix `6749062f42121bf6…`,
LOOP 8.5, 213 instructions, 27 identifiers, ctable size 0x291; decompiled text in
`humanplayer.qvm.decompiled.qsc` (6 lines), argument file offsets in `humanplayer-general-args.json`.

Retail consumer `DefineHumanPlayerGeneral 0x410070` (`[static]`; 326 bytes, 5 blocks; argument frame is stack
argument 3, return block pointer stack argument 1 — the script VM calls natives as
`native(value_block_slot, symbol_ctx_+0x18, arg_frame)` at `0x004BD8F3`-`0x004BD8FE`) stores:
arg0 unscaled; args1–3 × `4096000.0f` (`0x5334A4`) × `1/108000` (`0x5334A0`) (x87 80-bit); args4–5 ×
`4096.0` double (`0x533498`); arg6 × `30.0` double (`0x533488`) then `fistp qword`, low dword stored; args7–8
unscaled; args9–25 → 17 × 16-byte records table A `0x5389F8`; args26–42 → 17 records table B
`0x538B08..0x538C18`; all table values unscaled. Script value block: +0x00=1, +0x08=float64 1.0, +0x10=type
descriptor `0x00567C74`, six dwords copied to the context; +0x04/+0x14 unwritten (port zeros them — documented
difference in `engine/src/retail/script/humanplayer_defines.cpp`).

Argument table (index, QVM file offset, raw value, post-scale meaning):

| Idx | File off | Raw | Retail store |
|---:|---|---|---|
| 0 | 0x349 | 1.75 | unscaled float 1.75 (hypothesis: eye height/camera offset) |
| 1 | 0x34F | 17.5 | ×(4096000/108000)=37.9259 → 663.70 units (hypothesis: walk speed) |
| 2 | 0x355 | 27 (`PUSHB`) | → 1024.00 units (hypothesis: run speed 27 km/h → 1024 units/tick) |
| 3 | 0x358 | 0.5 | → 18.96 units |
| 4 | 0x35E | 0.85 | ×4096 = 3481.6 |
| 5 | 0x364 | 0.85 | ×4096 = 3481.6 |
| 6 | 0x36A | 0.25 | ×30 = 7.5 → `fistp` (7 or 8 depending on x87 control word; chop-mode setter `0x498460` runs before the loop) |
| 7 | 0x370 | 3.0 | unscaled |
| 8 | 0x376 | 0.5 | unscaled |
| 9–25 | 0x37C–0x3D3 | 0.5, 0.75, 1, 0.5, 0.75, 1, 0.5, 0.75, 1, 0.5, 0.75, 1, 1, 1, 1, 1, 1 | table A, unscaled (17 records) |
| 26–42 | 0x3D9–0x402 | 0.5, 0.75, 1, 0.5, 0.75, 1, 0.5, 0.75, 1, 0.5, 0.75, 1, 1, 1, 1, 100000, 100000 | table B, unscaled (17 records) |

Unknowns: the per-argument *meaning* (speed/jump/step/scale/state multipliers) is not established; the 17+17
tables are unnamed state parameter sets; arg6's rounding depends on the x87 control word at that moment; and
`human_player_movement.md`'s 43-arg note ("only first four meanings established") remains the extent of
semantic knowledge.

### 4.2 `lod.qvm`

`/Users/haseeb-mir/Downloads/IGI/lod.qvm`, SHA-256 prefix `8dc4ffebeec643dc…`, 48,510 B, 10,657 instructions,
459 strings, ctable 0x17E5/42,393 B. Consumed by `LODSettings_LoadParameters 0x4EDB60` (462 observed hits).
Per-object LOD assignment: `OBJECTS.QVM` creates one `LODSettings` task
`(50, 65, 65, 2450, 2450, 1600, 0.25, 15)` and `LODSettings_LoadParameters` reads this file. Field semantics
remain `[hypothesis]` (8 scalars; likely distances/ratios) — not decoded further here.

### 4.3 `config.qvm` — current shipped/installed state is **mutated**, and shipped defaults are not recoverable

Current file `/Users/haseeb-mir/Downloads/IGI/config.qvm`: 5,055 B, SHA-256
`4743dd505e4bfce8aa50b0f5d281974daf24d21fe2f771a3faaa86e68ca60e0d`, mtime 2026-10-24 01:26; LOOP 8.5,
1,717 instructions. Decompiled: `config.qvm.decompiled.qsc`. Structure: `GOStart` once, then **three appended
GO blocks** of 27 `GOInRemap` + 14 scalar calls each (125 calls total; per-call and per-argument file offsets in
`config-calls.json`).

| Field | Block 1 off/value | Block 2 off/value | Block 3 (effective) off/value |
|---|---|---|---|
| `GOStart` | 0x53F → 2 | — | — |
| `GOContentControlPW` | 0x553 → 0 | — | — |
| `GOPlayer` | 0x567 → `"Heaven"` | 0xA34 → `"HeavenMir"` | 0xF01 → `"Haseeb"` |
| `GOActiveMission` | 0x57B → 13 | 0xA48 → 13 | 0xF15 → 13 |
| `GOInMouInv` | 0x58F → FALSE | 0xA5C → FALSE | 0xF29 → FALSE |
| `GOInMouSens` | 0x5A2 → 0.4 | 0xA6F → 0.3 | 0xF3C → 0.3 |
| `GOGfxDisp` | 0x957/0x95B/0x95F → 960,600,16 | 0xE24/… → 1024,640,16 | 0xF… → 1024,640,16 |
| `GOGfxDevice` | 0x973 → 2 | 0xE40 → 4 | → 4 |
| `GOGameLang` | 0x9AD → ENGLISH | 0xE7A → ENGLISH | → ENGLISH |
| `GOGameDiff` | 0x9C1 → GD_2 | 0xE8E → GD_1 | → GD_2 |
| `GOIsBlood` | 0x9D5 → TRUE | 0xEA2 → TRUE | → TRUE |
| `GOSoundSpeech/Music/FX` | → (1,1) | → (1,1) | → (1,1) |

Block 3 (the last block, i.e. the values a fresh `Config_Read` execution would leave in effect) bindings
(`GOInRemap(action, device, key)`; argument offsets are `arg0` file offsets in the current file):

| Action | Off | Device | Key | Action | Off | Device | Key |
|---|---|---|---|---|---|---|---|
| MoveLeft | 0xF5B | KEYBOARD | KEY_A | Peek | 0x10F3 | KEYBOARD | KEY_NUMPAD1 |
| MoveRight | 0xF7D | KEYBOARD | KEY_D | Binoculars | 0x1115 | KEYBOARD | KEY_SPACE |
| MoveUp | 0xF9F | KEYBOARD | KEY_W | MapComputer | 0x1137 | KEYBOARD | KEY_K |
| MoveDown | 0xFC1 | KEYBOARD | KEY_S | ToggleWalkRun | 0x1159 | KEYBOARD | KEY_UP |
| Fire | 0xFE3 | MOUSE | MOUSE_BUTTON_1 | ZoomIn | 0x117B | KEYBOARD | KEY_PAGEUP |
| Reload | 0x1005 | KEYBOARD | KEY_R | ZoomOut | 0x119D | KEYBOARD | KEY_PAGEDOWN |
| Activate | 0x1027 | KEYBOARD | KEY_Q | WeaponCategory1–9 | 0x11BF–0x12CF (step 0x22) | KEYBOARD | KEY_1–KEY_9 |
| Crouch | 0x1049 | KEYBOARD | KEY_C | | | | |
| Jump | 0x106B | KEYBOARD | KEY_E | | | | |
| NextWeapon | 0x108D | MOUSE | MOUSE_BUTTON_2 | | | | |
| PrevWeapon | 0x10AF | KEYBOARD | KEY_O | | | | |
| AlternateFire | 0x10D1 | KEYBOARD | KEY_F | | | | |

**Contradiction with the existing fidelity note.** `docs/fidelity/human_player_movement.md` records the shipped
`config.qvm` as SHA-256 `9fc8cc579f86db2e456972d0678161a303e84a40857a3f3f1a98e115042dd3b0` with arrow-key
bindings, player `Haseeb`, sensitivity 0.3, mission 13 and 1024×640×16. **No file with that digest exists** in
the retail game directory or in `/Users/haseeb-mir/Downloads` (`[static]` search this session). The closest
files are all mutated variants: `config copy.qvm` SHA prefix `a63382d4…` (5,055 B, same 3-block content),
`config.qvm.bak-20260828-030547` `dd94b974…` (3,770 B, single block, `GOStart(1)`, player `"Joe"`, mission 32,
arrow keys, `RETURN`), and `/Users/haseeb-mir/Downloads/config.qvm` `8060316d…` (5,065 B, same mutated content).
Retail rewrites this file per session (each run appends a block; `Config_Save*` writers exist at
`0x4058A0`/`0x405300` and the observed 81 `GOInRemap` hits equal exactly 3×27). **Conclusion:** the current
`config.qvm` is *player-state output*, not an authored default; shipped defaults are unrecoverable from this
machine and must come from a pristine installer/CD copy, or be reconstructed from retail code defaults
(`Config_CreateNewPlayerProfile 0x4050A0`, `Config_GetActivePlayerProfile 0x406220`, graphics defaults in
`0x404560/0x404590/0x404910`). Candidate `config.qvm` handling must be generic (read + apply whatever the file
says), never hard-coded to these bindings.

Load/apply chain for P14 (`[static]`): flow event 0 → `Config_Read 0x405850("LOCAL:config.qsc")` → Qvm_Load →
`.qvm` → `Qvm_Execute` → `GO*` natives (`GOInRemap 0x4037C0` writes input mappings; `GOInMouSens 0x4038E0`;
`GOGfxDisp 0x405EC0`; `GOGfxDevice`, `GOGameDiff`, `GOIsBlood 0x403DF0`, `GOSound* 0x403E60/0x403EF0/0x403F80`,
`GOPlayer 0x4036B0`, `GOActiveMission`, `GOStart`) → then `Config_SaveSoundOptions 0x4058A0`,
`Config_NoD3DDriversWithHard 0x405980`, `Config_Language_2 0x405B30` apply the derived state.

**QVM native ABI warning:** manifest signatures for the `GO*` family are structural guesses like
`int32_t GOInRemap(int32_t param_1, float param_2)`, while the verified convention for script natives is a
3-argument VM frame `(value_block_slot, symbol_ctx_+0x18, arg_frame)` (see `0x410070`). P14 must fix the ABI from
`0x004BD8F3`-`0x004BD8FE` before writing the `GO*` implementations.

---

## 5. Level 1 entity and placement inventory

Source: `MISSIONS/location0/LEVEL1/OBJECTS.QVM` (183,749 B, SHA-256 prefix `90acf17a8af76ddc…`; 44,244
instructions, 1,456 decompiled lines) plus the level directory. Machine-readable:
`level1-inventory.json`, `level1-assets.json`.

### 5.1 Task creation by class — 1,360 `Task_New` records across 56 declared parameter classes

Top classes: `EditRigidObj` 380, `TerrainLightMap` 268, `PatrolPathCommand` 141, `EditCamera` 48, `Gun` 40,
`PatrolPath` 37, `Container` 36, `HumanSoldier` 29, `HumanAI` 29, `Building` 26, `SplineObjWaypoint` 25,
`StatusMessage` 23, `Door` 23, `Train` 23, `LightmapInfo` 22, `ConditionalSound` 21, `AmbientArea` 21,
`Static` 18, `ComputerHilight` 17, `AIGraph` 11, `TextureModifier` 9, `AreaActivate` 8, `AnimTask` 8,
`ConditionalContainer` 8, `GunPickup` 7, `SplineObj` 6, `Switch` 6, `ExplodeObject` 6, `SCamera` 5,
`AmmoPickup` 5, `CutScene` 5, `EditVariable` 4, `GunMP5SD` 4, `HeightMap` 3, `LevelTimer` 3, `Terminal` 3,
`Wire` 3, `Smoke` 3, `GlobalLightKeyframe` 2, `DirlightKeyframe` 2, `FlatSkyLayer` 2, `SCameraControl` 2,
`Heli` 2, `HumanPlayer`/`HumanPlayerInput`/`MapComputer`/`LODSettings`/`MipMapControl`/`GlobalLight`/
`Dirlight`/`FlatSky`/`Dynamic`/`AlarmControl`/`DefineComputerObjective`/`Fence`/`Car`/`Binocular`/`AddAmmo`/
`GunDRAGUNOV`/`LevelFlow` 1 each (`EditRigidObj` includes unnamed records with id −1).

### 5.2 World hierarchy and the player record (`[static]`)

Root rows in order: `Container "EnvironmentalEffects"` holding
`MipMapControl (TRUE,1,-0.4,-0.4)`,
`LODSettings (50, 65, 65, 2450, 2450, 1600, 0.25, 15)`,
`GlobalLight` id 4 (36.5, 4 keyframes), `Dirlight "TheSun"` id 5 (TRUE,TRUE,18.0, 2 keyframes),
`FlatSky "TheSky"` (2 `FlatSkyLayer`s referencing `missions/location0/common/textures/sky_layer1.tex` and
`sky_layer2.tex`); a `Static→Container "TerrainModifiers"` holding `Container "Heightmaps"` with 3 `HeightMap`
records and `Container "TerrainTexturing"` with 9 `TextureModifier` records (Grass/Rock/Dirt families; bitmap
ids 2,3,23,4,5,7,6,8,24; sizes 128/64; level 11; `HeightMap`: positions `(22128590,-57264816,174634432)`,
`(22735164,-56375428,174696704)`, `(23335550,-56157360,174594512)`, size 128, smooth 8/7, bitmap ids 0,1,2);
then the player and `LevelFlow` (id 10, final row, waits on `CutScene_1204.isFinished` /
`StatusMessage_1391.isSendt`).

**Player task 0:** `Task_New(0, "HumanPlayer", "TheGoodGuy", 24578514.0, -55852816.0, 174421680.0,
3.09598708152771, "000_01_1", 0, …)` — position triple, gamma/heading 3.095987…, model `000_01_1`, team 0; its
child `HumanPlayerInput(FALSE,FALSE,"","")`.

**Placement encoding (hypothesis, flagged):** raw position components are ~1e7–1e8 integers. Dividing by 4096
gives human-scale world coordinates: player `(6000.6, -13635.9, 42583.4)`; building 1100
`(5936.7, -13662.6, 42581.3)` → player is 63.9 m east / 26.7 m north / 2.1 m above that fuel container;
heightmap origins land 5700–6000 m apart in x, consistent with level size. This supports **4096 units per
metre** but the scale is *not proven*: no transform native (object placement → matrix) was traced this session,
and orientation is printed as 3 components while the declared parameter type is `Real32x9` (see §3.3). P11 must
not guess: next step is to decode the placement/transform consumer (the class constructors registered for
`Building`/`EditRigidObj`/`SplineObj`, reached from `0x4F0780 Task_New`) and read the 9-float orientation packing
from `Task_DeclareParameters`.

### 5.3 Terrain data (`[static]` + `[observed]`)

`TERRAIN/TERRAIN.QVM` (12,733 B, SHA-256 prefix `353608aa6b254f8c…`) decompiles to exactly
**6 × `CreateTerrainMaterial`** and **8 × `CreateTerrainTileMap`**, each tile map passing 64 material indices
(8×8 tiles): ids 0..7, index sets such as `(1,2,…)`, `(3,4,5,6,…)`, `(7,8,9,10,…)`, `(11…)`, `(12,13,…)`,
`(14,15,…)`, `(16,17,…)`. These match the observed `Terrain_CreateMaterial` ×6 / `Terrain_CreateTileMap` ×8
hits exactly — a clean P12 closure check.

Files: `TERRAIN.BIT` 71,552 B `6bbedfe0b3226051…`; `TERRAIN.CMD` 508,696 B `554e7dcd6508c748…`;
`TERRAIN.CTR` 352,704 B `1204b4b46e05dc04…`; `TERRAIN.HMP` 52,995 B `fcf493aa2afd4464…`;
`TERRAIN.LMP` 461,360 B `b30bfbe757b1c761…`; `TERRAIN.QVM` 12,733 B `353608aa6b254f8c…`;
`TERRAIN.TEX` 590,764 B `456522b3e6c540ee…`; `lightmaps/lightmaps.res` 1,668,412 B
`e29da734fc2f6ddf…`.

### 5.4 Level assets

* `MISSION.QVM` 313 B `d9ef8e943cbdf522…`; `OBJECTS.QVM` 183,749 B `90acf17a8af76ddc…`;
  `objects.qsc` 181,221 B `bbc23ed8ff2348f9…` (both source and compiled present; retail uses the `.QVM`, §2.8).
* `MODELS/LEVEL1.RES` 5,029,944 B `7202d1ea2efa48f1…`; `MODELS/models/` **415 `.mef` files**;
  `MODELS/LEVEL1_unpacked/` present.
* `TEXTURES/LEVEL1.RES` 6,662,688 B `ffa06e1cad6a963e…`; `TEXTURES/textures/` **393 `.tex` files**
  (including `_argb8888` variants).
* `../COMMON/MODELS/location0.res` + `models/`, `../COMMON/TEXTURES/location0.res`, `sky_layer1.tex`,
  `sky_layer2.tex`, `textures/`; `../COMMON/location0.dat` 12,133 B, `location0.mtp` 13,256 B.
* `LEVEL1.MTP` 26,528 B `cd9f32c89fe29910…`; `LEVEL1.DAT` 28,495 B `012337be0a7fb92f…`;
  `AI/` 38 QVMs; `GRAPHS/` 12 `.dat` (largest `graph4019.dat` 776,805 B); `SOUNDS/` 30 files.
* Model references in `OBJECTS.QVM`: **84 distinct model names** (`905_01_1` ×78, `303_02_1` ×43,
  `306_01_1` ×43, `342_01_1` ×23, `219_01_1` ×21, `227_01_1` ×20, `301_01_1` ×18, `338_01_1` ×17,
  `303_01_1` ×17, `234_01_1` ×15, `202_01_1` ×15, `319_05_1` ×12, …), all under `MODELS/LEVEL1.RES` or
  `MODELS/models/`.
* Material/texture inputs for P12: the 6 terrain materials + 8 tile maps + 9 `TextureModifier` records + 2 sky
  layers above; `TEXTURES/textures/*.tex` and `lightmaps/lightmaps.res` are the packaged forms.

---

## 6. Unknowns, contradictions and exact next research actions

| ID | Unknown / contradiction | Evidence | Exact next action (owner) |
|---|---|---|---|
| U1 | Candidate `Main` does not call `QTask_CreateTask 0x401000`; `QTask_RegisterClasses` throws before the flow calls | `engine/src/retail/core/main.cpp` (no `0x401000` call), `class_registration.cpp` 65 lines, `qtask_create_task.cpp` comment | **Blocker.** Wire `Main → QTask_CreateTask`, then close the registration list (`0x402930` 190+ call sites; 30 absent from manifest) so `Game_Init`/`Flow_Init` run (implementer) |
| U2 | `Level_Start` body not transcribed: terrain 0x4C5F00/0x4C6240, level/common package 0x4CFBD0/0x4CFC00, common forms 0x4CEFA0, script 0x4F0E10 are hook-only | `game_flow.cpp` lines 75–86 + comment | Transcribe the §2.6 order; add the 5 missing manifest records first (docs/native-verification) |
| U3 | `Level_Load`/`Task_New`/`Script_SetSymbolContext` absent from manifest and 120 list; `Task_New 0x4F0780` body not traced (creation path into class ctors only `[hypothesis]`) | §2.8–2.9 | Static + typed runtime capture of `Task_New` class-id/arg-block (Lead runtime lease), then manifest entries |
| U4 | `.qsc → .QVM` rewrite and force-precompiled=1 are static-only for the level script; the 2026-10-19 capture explicitly did not decode the runtime `Qvm_Load` filename | `0x4B8107`/`0x48F330`/`0x40100C` dumps; capture limitation text | One typed probe of `Qvm_Load`'s resolved path (retail), then record as `RUNTIME_TESTED` evidence |
| U5 | Placement position encoding (4096/m) and orientation packing (3 vs 9 floats) unresolved | §3.3, §5.2 | Decode the placement consumer reached from `Task_New`; read `Task_DeclareParameters` type sizes |
| U6 | `0x4F1160` catalog name `Script_ParseEnumReal32_5` is contradicted by the live 11-byte flag predicate and 154,289 observed hits | `r2-levelload-body.txt`; `obs#93` | Re-identify and correct the catalog before P12 lightmap work; do not cite that name as fact |
| U7 | Shipped `config.qvm` unrecoverable; documented hash `9fc8cc…` absent; live file is 3-block mutated state | §4.3 | Obtain pristine config from installer/CD or reconstruct defaults from `Config_CreateNewPlayerProfile`/graphics option getters; keep current file as observed player state only |
| U8 | 81 `GOInRemap` hits = 3 blocks × 27 actions assumed "execute all, last wins"; `GOStart(2)` semantics unknown | `obs#48`, `config.qvm.decompiled.qsc` | Typed probe or static trace of `Config_Read`'s QVM execution order; confirm block-append semantics |
| U9 | `0x413B70`, `0x408350`, `0x508F80`, `0x489560`, `0x488BB0`, `0x484F30..0x47CF20` (registration initialisers) absent from manifest; identities unknown | `r2-flowinit-configload.txt`, `r2-registerclasses.txt` | Identify each from its string/import references and add to catalog; `0x413B70`/`0x408350` sit in the config/humanplayer load path |
| U10 | QVM script-native ABI: manifest `GO*` signatures are `(int,float)`-style guesses vs verified 3-arg VM frame | §4.3, `humanplayer_defines.cpp` comment | Fix ABI from `0x004BD8F3`–`0x004BD8FE` before writing P14 `GO*` natives |
| U11 | Entry `0x4A511B → Main 0x48F530` and CRT/static-init context only from prior r2 note; pre-main observed hits (mem/camera) not explained | `RETAIL_MAIN_FLOW.md`, `obs#1–8` | angr/R2 CFG recovery of entry and CRT init; explain pre-main camera calls |
| U12 | Candidate startup divergences vs docs: `main.cpp` uses `IGI1MUTEX` (docs `runtime-test-workflow.md`/`RETAIL_MAIN_FLOW.md` say retail `IGIMUTEX`), `CreateWindowExW`/class `IGI` vs retail `CreateWindowExA`/`Innerloop`, no `HardcoreGerman` parse, no `0x48F330(1)` in `Main` (it is in `QTask_CreateTask`) | `engine/src/retail/core/main.cpp`; AGENTS 10.8 | Lead decision: align or document the divergence before paired runtime claims; mutex-name contradiction must be resolved because both games cannot run simultaneously otherwise |
| U13 | `Main`'s window class/title and D3D version gate not implemented in candidate (`0x498480`, DirectX ≥ 7.0) | §1.1/§2.2 | Add to Main transcription scope (P13) |

**Recommended closure order for P06–P13 (from §3):**
1. U1 (Main → `QTask_CreateTask`; registration completeness) — unblocks everything.
2. U2/U3 (Flow event 0 config/humanplayer path → `Level_Start` body → `Level_Load` → `Task_New`).
3. P12 then P11 (terrain packages first because `Terrain_CreateMaterial`×6/`TileMap`×8 is a verifiable closure
   check; then MEF/DynObj package loads), resolving U4/U5/U6 while doing so.
4. P13 spawn/tick (`HumanPlayer`/`Human` type registration, ctor chain, `Human_RunUpdate`, camera, input) with a
   paired typed runtime capture of the §2.10 player band.
5. P14–P16 use §4 tables (fix U10 ABI, U7 config provenance, U8 block semantics first).

---

## Appendix A — raw artifacts (this research)

Directory: `build/audit/p05-20261023T215247Z/` (sha256 of each file recorded in `artifact-manifest.json`).

| Artifact | Content |
|---|---|
| `retail-observed-105.json` | Parsed 2026-10-23 table: seq, VA, name, category, retail hits, candidate column, verdict, joined manifest name/status/signature/path |
| `closure-groups.json` | 126 control/world addresses grouped P05/P13-entry, P13-spawn, P11, P12 with observed/in-120/manifest flags |
| `level1-inventory.json` | 56 declared classes, 1,360 `Task_New` rows by class, representative records (world root, player, heightmaps, buildings, later-stage classes) |
| `level1-assets.json` | 84 model references (by class + top counts), 415 `.mef`, 393 `.tex`, 9 texture modifiers, 2 sky layers |
| `config-calls.json` | 125 config calls with call/argument file offsets; block-3 binding offsets |
| `humanplayer-general-args.json` | 43 `DefineHumanPlayerGeneral` args with QVM file offsets and raw values |
| `MISSION.QVM`, `OBJECTS.QVM`, `TERRAIN.QVM`, `humanplayer.qvm`, `config.qvm`, `lod.qvm` `.decompiled.qsc` / `.info.txt` / `.disasm.txt` | Decompiled/decoded QVM sources |
| `r2-*.txt` (28 dumps) | `Main`, CLI parser/literals, `QTask_CreateTask`, `QTask_RegisterClasses`, `Level_Start`, `Level_Load`, `Qvm_Load`, `Task_New`, `Flow_Init`/event-0, `Config_Read`, HumanPlayer/Human type init, `Script_GetForcePrecompiledState`, force-precompiled writers, refs |

## Appendix B — reference checkouts inspected for discovery only (no code copied)

| Checkout | Revision | License | Use |
|---|---|---|---|
| `project-igi-converter` | git unusable: `.git/HEAD` missing; `HEAD.lock` says `ref: refs/heads/feature/unified-igi-desktop-qt`; latest reflog `22d5d18500f5edc262d2cae41251cd9fc881e44d`; branch file content `22d5d185…`, earlier `0ad7cef11fc5e2c91f42db9f7f8cc731428b41b2` | "Heaven-HM Community Modding License v1.0" + `LICENSE-PRIOR` | QVM format/decompiler behaviour only |
| `project-igi-editor` | HEAD `c506cd83…`, branch `logging-3.6.8` (dirty) | see repo LICENSE | level/task cross-reference discovery only |
| `open-igi` | HEAD `dade2b1a…`, branch `macos-build-pre-develop` (dirty) | GPLv3 | layout/path cross-check only |
| `re-agent` `/Users/haseeb-mir/Documents/Code/re-agent` | **unavailable (missing)** | — | not used; recorded as unavailable |

## Appendix C — tool and method limits

* Binary Ninja, Ghidra and angr were **not** used in this pass (read-only static pass with radare2 6.2.0 +
  igi1conv v1.11.0 over existing artifacts); no CFG/angr recovery is claimed here.
* No runtime capture was made: "observed" facts come from the existing 2026-10-19 suspended level-1 capture and
  the 2026-10-23 paired reachability capture. The 2026-10-23 raw JSON is not present in this checkout; its
  report table (105 rows) is the observed-order source, and candidate-side reachability in that report is known
  invalid (RVA probe), so no candidate column is used as evidence here.
* `igi.exe window level1` was not launched by this research; the Lead's runtime lease is unaffected.
* QVM decompiler output is tool formatting, not source: `PUSHSIB` string indices, compressed equal arguments and
  the `Real32x9` parameter packing appear in the decompiled text and are marked accordingly.
