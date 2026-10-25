# The startup guard's dependency closure: what it is, what it gates, and what the MVP actually needs

Read-only analysis for the Lead's decision of 2026-10-25. Author: `flow-analyst`. No `native_*.cpp`, no
`CMakeLists.txt`, no `native-verification.json` was touched.

Artifact for every figure: retail `igi.exe` SHA-256
`dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`. Extents are bounded by the `ret` and the
alignment nops after it; where a decode window was used instead, the window is named.

---

## 0. Three instrument corrections, including one of mine

**0.1 — The guard's extent and call population.** Bounded by its `ret` at `0x00402FEB`:
`0x00402930`–`0x00402FEB` = **1724 bytes, 402 instructions, 315 call instructions**. Of those, **314 are
direct (`E8`) and one is indirect — `call ebp` at `0x00402F6F`**, inside the local helper described below.
Distinct direct targets: **308**. So the Lead's "314 call sites, 308 distinct targets" and my "315 call
instructions" are both right and count different things; the difference is one indirect call, and it is now
located rather than averaged away.

**0.2 — My own earlier figure for this region was a phase, not a routine.** I first reported "780 bytes,
156 instructions, 155 calls" for `0x00402930`. That is the registration phase, and it is bounded by a
**tail `jmp 0x004CEF30` at `0x00402C37`, not by a `ret`** — the phase contains no `ret` at all. Reporting a
chosen dump window as an extent is the same fault the Lead recorded against his own `pd 156`; the window was
mine, and the phase is what it actually is.

**0.3 — The extent contains three regions, and they are not one routine.**

| region | extent | instructions | calls | distinct targets | ends with |
|---|---|---|---|---|---|
| registration sweep | `0x00402930`–`0x00402C37` | 156 | **155** | **154** | `jmp 0x004CEF30` (tail) |
| teardown sweep | `0x00402C40`–`0x00402F42` | 155 | **154** | **150** | `jmp 0x00508F90` (tail) |
| local helper | `0x00402F50`–`0x00402FEB` | 78 | **6** | **6** | `ret` at `0x00402FEB` |

Two consequences that matter for any partition:

- **The registration sweep never falls through into the teardown sweep** — it tail-jumps to `0x004CEF30`
  (a three-instruction routine: `mov eax,[0xA4EDEC]` / `mov [0xA4EDF0],eax` / `ret`, i.e. a tail-call idiom
  that returns to the guard's caller).
- **The teardown sweep has exactly one entry in the whole image: `jmp 0x00402C40` at `0x004010C7`**, inside
  the routine that begins at `0x004010A0` (`push 1` / `push 0x54F8E0` / `call 0x004A507C` / `call 0x00401A60`
  / `test eax,eax` / `je 0x004010C7` / `call 0x00402880` / `call 0x004015F0` / `jmp 0x00402C40`). The teardown
  sweep is therefore a **shared tail block of a different routine**, and the gate at `0x00401A60` decides
  whether two calls run before it. Nothing calls `0x00402C40`; nothing falls into it.

**0.4 — The two AI-graph sequences, sized correctly.** `0x005183D0` is bounded by the `jmp 0x00519710` at
`0x0051853D` and the nops after it: **`0x005183D0`–`0x0051853D` = 365 bytes, 74 instructions, 73 calls, 68
distinct targets**, the first being `call 0x00530270` with no prologue. The Lead's "164 instructions, 163
calls, 148 distinct" is reproducible exactly — as a **byte scan of `0x005183D0`–`0x00518869`**:

```
python3 - <<'EOF'   # E8 byte scan, target = v+5+rel32
0x005183D0-0x0051853D -> 73 hits, 68 distinct      (the bounded sequence)
0x005183D0-0x00518704 -> 146 hits, 135 distinct
0x005183D0-0x00518869 -> 163 hits, 148 distinct    (reproduces the reported figure exactly)
0x005183D0-0x00518900 -> 167 hits, 152 distinct
EOF
```

So the reported figure is the scan's window — it walks past the tail `jmp`, past the nops, and absorbs the
**sibling sequence `0x00518550`–`0x005186B7` (72 calls, 67 distinct)** plus about twenty single-call bodies
up to `0x00518869`'s `ret`. The bound the Lead warned me to apply is the bound that was missing here.

**0.5 — The AI graph is two sibling sequences, each reached from its own entry:**

```
0x00508F80  call 0x005183D0 ; jmp 0x00508FA0      <- guard call #1 (AIGraph_Init)  -> 73-call sequence
0x00508F90  call 0x00509650 ; jmp 0x00518550      <- tail of the guard's teardown sweep -> 72-call sequence
```

`0x00508F80` and `0x00508F90` are adjacent addresses with different tails; the guard's first call enters the
first sequence and the guard's teardown sweep exits into the second. "One real hole at `0x005183D0`" is
therefore one hole **plus** its sibling, its two entry routines (`0x00508FA0` = `AIGraph_Register_508FA0`,
`0x00509650`) and their continuation.

---

## 1. The partition of the guard's 155 call sites

Rules applied to the **retail registered class name** first (the string the registrar hands to `0x00401900`),
falling back to the manifest name only when there is no registrar. Families were then read row by row and
overridden where the name alone misleads — every override is listed in §1.1 with its reason. Nothing was
bucketed by guess: anything the name does not decide is in the unclassified table.

| bucket | count |
|---|---|
| MVP first increment | 11 |
| Scene objects | 18 |
| UI / menus | 25 |
| Out of scope (§10.10) | 100 |
| Unclassified | 1 |
| **total call sites** | **155** |

### 1.1 MVP first increment — every inclusion and its evidence

These are the guard calls whose own name, registered class or manifest name puts them in the first increment
(window, 3D scene, original Level 1 models/textures, player movement and controls).

| call # | VA | registered class | manifest name |
|---|---|---|---|
| 3 | `0x00407D60` | — | HumanPlayer_Setactivematerial |
| 5 | `0x00402FF0` | — | Config_FillPlayerProfileListBox_2 |
| 19 | `0x004825C0` | HumanCamera | HumanCamera_Init |
| 65 | `0x00465A10` | HumanAttach | HumanAttach_Init |
| 66 | `0x00464260` | HumanView | HumanView_Init |
| 67 | `0x0045E450` | Human | Human_Init |
| 68 | `0x0040FE30` | HumanPlayer | HumanPlayer_Init |
| 69 | `0x0045DB70` | HumanPlayerInput | HumanPlayerInput_Init |
| 119 | `0x0042FE40` | — | QTask_LinkClassHandlersB_42FE40 |
| 121 | `0x0042DAC0` | — | QTask_LinkClassHandlersA_42DAC0 |
| 155 | `0x00402F50` | — | Core_CheckCdPresent_402F50 |

Evidence for the group as a whole, not just the names:

- The player chain is four separate registrations plus three init calls: `Human_Init 0x0045E450`,
  `HumanPlayer_Init 0x0040FE30`, `HumanPlayerInput_Init 0x0045DB70`, `HumanView_Init 0x00464260`,
  `HumanAttach_Init 0x00465A10`, `HumanCamera_Init 0x004825C0` and the non-registrar
  `HumanPlayer_Setactivematerial 0x00407D60`. Movement and controls cannot be reconstructed from the guard
  alone — these register the classes; the behaviour is in their handlers.
- `0x00402FF0` and `0x00402F50` are the two non-registrar infrastructure calls (config profile fill, CD
  check). `0x0042DAC0` / `0x0042FE40` are the two handler linkers, which is why they are here rather than in
  a domain bucket: they link handlers onto ids registered earlier in the same sweep (see §3).
- `0x00422B40 ModelViewBox_Init` is **not** here: it is a menu widget that displays a model. It is in the UI
  table with that reason.

### 1.2 Scene objects (needed for a full level, not for the first increment)

| call # | VA | registered class | manifest name |
|---|---|---|---|
| 79 | `0x00449D30` | GlassShatter | Task_DefineGlassShatter_449D30 |
| 80 | `0x00449970` | Glass | Task_DefineGlass |
| 83 | `0x004485E0` | Building | Task_DefineBuilding |
| 85 | `0x004464C0` | Door | Task_DefineDoor |
| 86 | `0x00444BF0` | Switch | Task_DefineSwitch |
| 87 | `0x00443ED0` | Drawer | Task_DefineCabinetAndDrawer |
| 88 | `0x00443680` | Terminal | Task_DefineTerminal |
| 89 | `0x00440FD0` | SCameraLens | Task_DefineSCameraTypes |
| 90 | `0x00440C20` | SCameraControl | Task_DefineSCameraControl |
| 92 | `0x004403E0` | Ladder | Task_DefineLadder |
| 93 | `0x0043F900` | Fence | Task_DefineFence |
| 94 | `0x0043EFF0` | Wire | Task_DefineWire |
| 95 | `0x0043E8D0` | GenericPhysicsObj | Task_DefineGenericPhysicsObj |
| 96 | `0x0043E4F0` | GenericPhysicsMagicObj | Task_DefineGenericPhysicsMagicObj (behavior-derived placeholder name |
| 97 | `0x0043CC70` | Elevator | Task_DefineElevator |
| 98 | `0x0043C6D0` | ElevatorFloorIndicator | Task_DefineElevatorFloorIndicator |
| 105 | `0x00438DF0` | RotatingObject | Task_DefineRotatingObject |
| 110 | `0x00436780` | RainEffect | Task_DefineRainEffect |

### 1.3 UI / menus (retail boots through these; a directly loaded scene does not)

| call # | VA | registered class | manifest name |
|---|---|---|---|
| 13 | `0x00415070` | IntroScreen | Task_DefineIntroScreen_415070 |
| 14 | `0x00486900` | StatusScreen | Task_DefineStatusScreen |
| 17 | `0x004851B0` | GameScreen | Task_DefineGameScreen (behavior-derived placeholder name |
| 133 | `0x004170A0` | MenuManager | Task_DefineMenuManager_4170A0 |
| 134 | `0x00424B90` | Cursor | Cursor_Init_424B90 |
| 135 | `0x00423FC0` | MenuWindow | MenuWindow_Register_423FC0 |
| 136 | `0x004230A0` | ControlsMenu | ControlsMenu_Register_4230A0 |
| 137 | `0x00422B40` | ModelViewBox | ModelViewBox_Init |
| 138 | `0x004223D0` | MenuFrame | MenuFrame_Register_4223D0 |
| 139 | `0x00421750` | MenuScreen | MenuScreen_Register_421750 |
| 140 | `0x00421430` | MenuButton | MenuButton_Register_421430 |
| 141 | `0x00420CC0` | MenuText | MenuText_Register_420CC0 |
| 142 | `0x00420300` | MenuTextConditional | MenuTextConditional_Register_420300 |
| 143 | `0x0041F340` | SlideBar | Task_DefineSlideBar_41F340 |
| 144 | `0x0041E690` | ListBox | Task_DefineListBox_41E690 |
| 145 | `0x0041DC50` | ToggleBox | Task_DefineToggleBox_41DC50 |
| 146 | `0x0041D080` | InputBox | Task_DefineInputBox_41D080 |
| 147 | `0x0041C700` | AsciiBox | Task_DefineAsciiBox_41C700 |
| 148 | `0x0041B8E0` | ScrollListBox | Task_DefineScrollListBox_41B8E0 |
| 149 | `0x0041B310` | PictureBox | Task_DefinePictureBox_41B310 |
| 150 | `0x0041AAF0` | TextBox | Task_DefineTextBox_41AAF0 |
| 151 | `0x0041A1E0` | TypeWriterBox | Task_DefineTypeWriterBox_41A1E0 |
| 152 | `0x004199F0` | DialogWindow | Task_DefineDialogWindow_4199F0 |
| 153 | `0x00419760` | BackgroundFX | BackgroundFX_Init_419760 |
| 154 | `0x004190E0` | MenuLoader | MenuLoader_Register_4190E0 |

### 1.4 Out of scope per §10.10 (AI, weapons, damage, sound, mission logic, vehicles)

**100 of the 155 call sites.** Grouped by family rather than listed row by row, because the families are the
useful fact: AI and detection (`0x00508F80`, `0x00489560`, `0x0045E450`'s soldier subclasses
`0x0045CAB0`/`0x00459880`/`0x00456AD0`/`0x00454970`, `0x00406450`); weapons and ammunition
(`0x00407290`, `0x00413DF0` and the gun/ammo/mine/explosive/pickup families `0x0047xxxx`–`0x0046xxxx`);
vehicles and aircraft (`0x004310F0`, `0x004330F0`, `0x0042DCF0`, `0x0042FF00`, `0x0042D2B0`, `0x0042BEB0`,
`0x0042ACC0`, `0x0042BC00`'s owner, plus the door/wheel/gear/hatch/rudder/cockpit families); missiles,
rockets and hit zones (`0x00427FF0`, `0x00427830`, `0x00427D90`, `0x00426D90`, `0x004272A0`); sound and
animation (`0x004C1780`, `0x00436B10`, `0x00424E80`); mission logic (`0x00488BB0`, `0x00488620`,
`0x00488800`, `0x004872C0`, `0x00487BE0`, `0x00401FC0`, `0x00486900`, `0x00485A40`, `0x00485330`,
`0x00484F30`).

Full VA list: `0x00401FC0`, `0x00406450`, `0x00407290`, `0x00413DF0`, `0x00424E80`, `0x00425C80`, `0x00426630`, `0x00426D90`, `0x004272A0`, `0x00427830`, `0x00427D90`, `0x00427FF0`, `0x0042ACC0`, `0x0042BEB0`, `0x0042D2B0`, `0x0042DCF0`, `0x0042FF00`, `0x004310F0`, `0x004330F0`, `0x004351E0`, `0x00435870`, `0x00435D90`, `0x004361D0`, `0x00436B10`, `0x00437510`, `0x00438390`, `0x004388B0`, `0x004397C0`, `0x0043A840`, `0x0043AD80`, `0x0043B2F0`, `0x0043B850`, `0x0043BFD0`, `0x00440710`, `0x00447DB0`, `0x00448D30`, `0x00449360`, `0x0044C130`, `0x0044C8D0`, `0x004533B0`, `0x00453550`, `0x00454970`, `0x00456AD0`, `0x00459880`, `0x0045CAB0`, `0x00465BC0`, `0x004663D0`, `0x00466E40`, `0x00466F70`, `0x00467700`, `0x004682F0`, `0x004688B0`, `0x00468E20`, `0x004696B0`, `0x0046AB60`, `0x0046B510`, `0x0046B7F0`, `0x0046BA60`, `0x0046C0C0`, `0x0046CB70`, `0x0046D420`, `0x0046DB40`, `0x0046DD00`, `0x0046DF10`, `0x0046E370`, `0x0046F2B0`, `0x00470410`, `0x00470610`, `0x00470FA0`, `0x00472670`, `0x00472A70`, `0x00472D00`, `0x004733F0`, `0x004739D0`, `0x00473ED0`, `0x00475380`, `0x00475AC0`, `0x00476110`, `0x00476730`, `0x00477220`, `0x00477E30`, `0x0047B4B0`, `0x0047CAB0`, `0x0047CF20`, `0x0047CF70`, `0x0047E700`, `0x004801B0`, `0x004813E0`, `0x00484F30`, `0x00485330`, `0x00485A40`, `0x004872C0`, `0x00487BE0`, `0x00488620`, `0x00488800`, `0x00488BB0`, `0x00489560`, `0x004C1780`, `0x004C1780`, `0x00508F80`

### 1.5 Unclassified — named as unclassified

| call # | VA | registered class | manifest name |
|---|---|---|---|
| 6 | `0x00415530` | Game | Game_Init |

`0x00415530 Game_Init` registers the class `"Game"`. Whether the top-level game-state class is "mission
logic" (§10.10, out of scope) or the object a scene is created inside (MVP) is **not decided by its name or
its callee set**, and I am not going to decide it by guessing. Its callee set is 14 registration-infrastructure
calls, which is consistent with either reading.

### 1.6 The number the Lead asked for

Of the guard's **154 distinct direct targets**: **11** are first-increment relevant, **18** are scene objects,
**25** are UI/menus, **100** are out of scope, and **1** is unclassified. But the more important number is
this one: **the guard contains no terrain, model, texture or render loader at all.** Its registrars' only
render-adjacent callees, at depth 2, are eight config-word getters and one model call —
`Camera_Get_4F0F80`, `Camera_Get_4F1000`, `Core_GetRenderWordA_4F0FA0`, `Core_GetRenderWordB_4F0FD0`,
`MTP_AddLODVirModel_4CEBA0`, `Render_GetByte_4D14D0`, `Sys_EnumRenderModesPair_4978F0` and
`Terrain_CalculateNormals_8` (the last called by `0x00508F80`). **The MVP's scene natives are not in this
population**, so "how many of the 308 does the MVP need" cannot be answered by shrinking the guard — the
scene natives have to be reached through the level-load path, which is a different closure.

---

## 2. `0x005183D0`'s targets: how many have a file

**[M]** Command and result (the manifest is the VA→file index; `implementation.path` is the transcribed file):

```
python3 - <<'EOF'
import json,re,subprocess,capstone
man=json.load(open('docs/fidelity/native-verification.json'))
impl={int(e['address'],16):(e.get('implementation') or {}).get('path') for e in man['natives']}
# decode 0x005183D0 to the tail jmp at 0x0051853D, collect direct call targets
B=open('/Users/haseeb-mir/Downloads/IGI/igi.exe','rb').read(); TR,TV=0x1000,0x401000
md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32); md.detail=True
t=set()
for i in md.disasm(B[TR+(0x5183D0-TV):TR+(0x5183D0-TV)+0x16E],0x5183D0):
    if i.mnemonic=='call': t.add(i.operands[0].imm)
print(len(t), sum(1 for a in t if impl.get(a)), sum(1 for a in t if not impl.get(a)))
EOF
68 11 57
```

**68 distinct targets: 11 have a transcribed file, 57 do not.** (The 68 is the bounded sequence of §0.4, not
the 148 of the byte scan.) The sibling sequence `0x00518550`–`0x005186B7` has 72 calls / 67 distinct targets
and its own file count; it is a separate measurement and is not included above.

---

## 3. The shortest path from the guard to a window — the answer is negative, and here is the evidence

**The window is not downstream of the guard. It is created before the guard runs, in `WinMain`.**

- **[M]** No guard target calls a window or display native. Searching all 154 targets' direct callees for
  `0x0048F0E0` (StoreWindowHandle), `0x0048F120` (Display_SetScreenMode), `0x0048F100` (SetAppTitle) and
  `0x0048F0C0` (StoreCommandLine) returns **NONE**. The guard's own extent calls none of them either.
- **[M from the tree's own record]** `engine/src/retail/core/main.cpp` (retail `0x0048F530`) creates the
  window in its startup block: `CreateWindowExA` at retail `0x0048F770` (class `"Innerloop"`, 640×480,
  style from `GetScreenMode`), `ShowCursor(0)`/`ShowWindow(10)`/`UpdateWindow` at `0x0048F799`, then
  `StoreWindowHandle 0x0048F0E0` at `0x0048F7A0`. That is `WinMain`'s body, and the guard is a separate
  call in the startup flow.
- **[M]** The registration sweep's only exit is `jmp 0x004CEF30` — a three-instruction getter that returns to
  the caller. There is no path from the sweep to a window, and the sweep cannot be "shortened" toward one.

**Consequence for the MVP decision:** a subset of the guard's registrars cannot produce a window, because the
window does not come from the guard. What the guard gates is **class registration** — the classes that a
level's objects instantiate. The port already has the window (the diagnostic shell); what it does not have is
the scene, and the scene's natives live outside this population (§1.6).

**And partial execution of the guard is unsafe for three measured reasons, so "run only the registrars we
need" is a restructuring, not a subset:**

1. **The registration sweep is one straight-line block with no branches** — 155 consecutive `call`s, no test,
   no frame, no local state. There is no point at which a subset could be entered or exited.
2. **The sweep's order carries state.** The getter `0x00424850` (`mov ax,[0x0053B90C]` / `ret`) supplies the
   parent class for ten registrars in the Lead's band and others; `0x0053B90C` is written **only inside
   `0x00423FC0 MenuWindow_Register`** — 17 references in that body, first write at `0x00423FD9` — and
   `MenuWindow` is **guard call #135**. Every registrar called after #135 reads the value `MenuWindow` left
   behind. Dropping or moving `MenuWindow` changes the parent of every later registration, and no build or
   link check can see it.
3. **The teardown sweep depends on the registration sweep having run.** Each of the eleven teardowns in
   `0x00419000`–`0x0041FFFF` reads its own registrar's container handle and class id (measured per file in
   the band summary). Running the teardown block without the registrations deletes unallocated handles; running
   it after only some registrations deletes a partial set. The block is entered from `0x004010C7`, not from the
   registration sweep, so the two are already separable in the binary — which is exactly why the dependency
   has to be stated rather than assumed.

---

## 4. Unresolved, and the queue

- **`0x004B8890` chain — not started.** The Lead's new assignment (seven-argument forwarder at
  `0x004B8890`–`0x004B88B2`, blocked on `0x004BAAC0`) is queued behind this note and is **not** transcribed;
  nothing in this document should be read as covering it.
- **`0x00407D60` double-ownership — not checked yet.** `0x00407D60` is guard call #3, `HumanPlayer_Setactivematerial`,
  in the MVP table above, and it is reported as blocked on five files including `0x004B8890`. The Lead's
  warning that `0x569474` may already be owned by `render/native_00407d60.cpp` needs the file read before any
  write; a second definition of one VA is the defect no check here catches.
- **`0x004017C0`'s fatal path.** The `jmp 0x004017E8` self-jump (infinite hang after pushing
  `"Unable to allocate event"` at `0x005361A8` and calling `0x004AF7B0`) is a behaviour to preserve, and it is
  also reached from the teardown sweep's gate routine; it is recorded here so that the partition cannot be
  read as licence to "fix" it.
- **`0x004010A0` and `0x004010C7`'s containing routine have no manifest entry**, so the gate that decides
  whether the teardown sweep runs is itself untranscribed. It is the first thing to transcribe if the teardown
  path is ever exercised.
- **The 4 guard targets with no manifest entry** are `0x0042ACC0` (Train, 24 callees), `0x0044C130`
  (AmbientArea, 14), `0x004533B0` (PatrolPathCommand, 8) and `0x00453550` (PatrolPath, 7). All four are
  out-of-scope families, and `0x0042ACC0` is the framed registrar measured in the band summary.
