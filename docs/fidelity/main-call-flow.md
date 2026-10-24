# Retail WinMain call flow (0x0048F530) - ordered game-native walk

The PE process entry is `0x004A50F5`, historically mislabeled
`Camera_ApplyHandiCam_3`. It performs CRT startup and calls `WinMain` at
`0x004A51BE`. Per the owner's clarification, compiler startup, CRT internals,
and Windows SDK implementations are supplied by the toolchain and are outside
the game-native transcription scope. The candidate's compiler-generated
`WinMainCRTStartup` is therefore not a missing game-native implementation.
The ordered game-code reconstruction starts at `WinMain` (`0x0048F530`), then
`StoreCommandLine` (`0x0048F0C0`). This boundary does not waive any game-native
ABI, call-order, state, CFG, or paired-runtime checks.

Captured from the retail image with Binary Ninja disassembly/Pseudo C (selected decompiler),
Radare2 `af; pdf` call order and angr CFGFast exact entries. Image: `/Users/haseeb-mir/Downloads/IGI/igi.exe`,
SHA-256 `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`.

Raw evidence: `build/main-0048f530.disasm.txt` (BN), `build/main-0048f530.r2.txt` (r2 pdf).

Method numbering follows the user mandate: method 1 = `main`; method 2 = the first method `main` calls;
then method 3 ... in the exact static call order of the retail body (prologue setters first, then the
CreateWindowExA path, then the per-frame loop body). Imported Win32/CRT calls are not methods.

| # | call site | target VA | name | manifest status | implementation | linked in candidate |
| - | --------- | --------- | ---- | --------------- | -------------- | ------------------- |
| 2 | 0x0048F5F4 | 0x0048F0C0 | StoreCommandLine | RUNTIME_TESTED | engine/src/retail/core/app_startup_natives.cpp | yes |
| 3 | 0x0048F5FA | 0x0048F0D0 | StoreInstanceHandle | RUNTIME_TESTED | engine/src/retail/core/app_startup_natives.cpp | yes |
| 4 | 0x0048F604 | 0x0048F0F0 | SetConfigFileName | STRUCTURALLY_VERIFIED | engine/src/retail/core/native_0048f0f0.cpp | yes |
| 5 | 0x0048F60E | 0x0048F100 | SetAppTitle | STRUCTURALLY_VERIFIED | scripts/GameNatives/fx/0048F100-SetAppTitle.cpp | GameNatives only |
| 6 | 0x0048F615 | 0x0048F140 | AppContext_SetIsActive | RUNTIME_TESTED | engine/src/retail/core/frame_natives.cpp | yes |
| 7 | 0x0048F61C | 0x0048F120 | SetScreenMode | STRUCTURALLY_VERIFIED | scripts/GameNatives/fx/0048F120-SetScreenMode.cpp | GameNatives only |
| 8 | 0x0048F622 | 0x0048F1A0 | AppContext_SetDebugged | STRUCTURALLY_VERIFIED | engine/src/retail/core/app_context.cpp | yes |
| 9 | 0x0048F629 | 0x0048F1C0 | SetDebugCompileState | STRUCTURALLY_VERIFIED | scripts/GameNatives/fx/0048F1C0-SetDebugCompileState.cpp | GameNatives only |
| 10 | 0x0048F62F | 0x0048F1E0 | AppContext_SetDebugtextState | STRUCTURALLY_VERIFIED | scripts/GameNatives/fx/0048F1E0-AppContext_SetDebugtextState.cpp | GameNatives only |
| 11 | 0x0048F636 | 0x0048F240 | App_ContextSetLightmapsUsed | STRUCTURALLY_VERIFIED | engine/src/retail/core/app_context.cpp | yes |
| 12 | 0x0048F63D | 0x0048F260 | AppContext_SetTerrainLightmapsUsed | STRUCTURALLY_VERIFIED | engine/src/retail/core/app_context.cpp | yes |
| 13 | 0x0048F643 | 0x0048F280 | unknown_setter_48f280 | NO RECORD | (none) | no |
| 14 | 0x0048F649 | 0x0048F290 | unrecorded | NO RECORD | (none) | no |
| 15 | 0x0048F64F | 0x0048F2A0 | unrecorded | NO RECORD | (none) | no |
| 16 | 0x0048F656 | 0x0048F210 | unrecorded | NO RECORD | (none) | no |
| 17 | 0x0048F65C | 0x0048F2B0 | unrecorded | NO RECORD | (none) | no |
| 18 | 0x0048F669 | 0x0048F300 | unrecorded | NO RECORD | (none) | no |
| 19 | 0x0048F66F | 0x0048F200 | unrecorded | NO RECORD | (none) | no |
| 20 | 0x0048F6A6 | 0x0048F360 | unrecorded | NO RECORD | (none) | no |
| 21 | 0x0048F6B5 | 0x0048F360 | unrecorded | NO RECORD | (none) | no |
| 22 | 0x0048F6C4 | 0x0048F360 | unrecorded | NO RECORD | (none) | no |
| 23 | 0x0048F6D3 | 0x0048F360 | unrecorded | NO RECORD | (none) | no |
| 24 | 0x0048F6E2 | 0x00498480 | unrecorded | NO RECORD | (none) | no |
| 25 | 0x0048F754 | 0x0048F130 | unrecorded | NO RECORD | (none) | no |
| 26 | 0x0048F7A0 | 0x0048F0E0 | SetWindowHandle | STRUCTURALLY_VERIFIED | engine/src/retail/core/native_0048f0e0.cpp | yes |
| 27 | 0x0048F7A5 | 0x00498460 | FP_InitExtendedPrecision | STRUCTURALLY_VERIFIED | engine/src/retail/core/fpu_state.cpp | yes |
| 28 | 0x0048F7AA | 0x00401000 | QTask_CreateTask | BUILD_VERIFIED | engine/src/retail/core/qtask_create_task.cpp | yes |
| 29 | 0x0048F7BE | 0x0048F360 | unrecorded | NO RECORD | (none) | no |
| 30 | 0x0048F80D | 0x00498460 | FP_InitExtendedPrecision | STRUCTURALLY_VERIFIED | engine/src/retail/core/fpu_state.cpp | yes |
| 31 | 0x0048F812 | 0x0048F170 | AppContext_GetIsActive | RUNTIME_TESTED | engine/src/retail/core/frame_natives.cpp | yes |
| 32 | 0x0048F81B | 0x00401050 | QTask_OpenEvent | STRUCTURALLY_VERIFIED | engine/src/retail/core/frame_natives.cpp | yes |

## Per-method acceptance protocol (user mandate, current)

Hard rule: **no game native is added to the linked codebase without prior static + runtime verification.** The already
added natives are not grandfathered: every one of them gets its own individual static and runtime test before it is
treated as accepted. Tests are static verification and runtime verification only; there are no standard unit tests and
no contract tests for this reconstructed game - the game running is the test.

Per native, in the retail call order above:

1. Static capture (Lead): one decompiler (Binary Ninja; Ghidra only as fallback), Radare2 CFG, angr exact entry.
   Record exact bytes/blocks/branches, strings and error messages (exact spelling), argument registers/locations,
   return value, memory writes, callees, call sites.
2. Static verification (Tester): independent check that the captured retail structure and the candidate source agree
   (blocks, edges, ordered call sites, constants, strings); evidence file per native under `build/tester/<run>/`.
3. Runtime verification, individual (Tester): a **custom Frida probe script for that single native**, run against the
   **full retail game**: `igi.exe window level1`, cwd `/Users/haseeb-mir/Downloads/IGI`, probe records ordered
   entry/return, arguments, return value and bounded memory snapshots where the probe is typed; then the same probe
   file against the **full candidate game**: freshly built `igi1.exe window level1`, same cwd/level/scenario,
   sequential (retail first, then candidate; never both at once - IGIMUTEX). Compare call order, arguments, return
   values, state deltas. Late-attach limitations are recorded, never hidden.
4. Only a native whose individual static + retail runtime + candidate runtime comparison passes moves to status
   RUNTIME_TESTED (VERIFIED only when the semantic/behavioural evidence gates also pass).
5. New native (not yet in the candidate): retail runtime probe and static capture happen **before** integration; the
   Lead then adds the exact transcription to `engine/src` + `CMakeLists.txt`, rebuilds with `./build_no_verify.sh`,
   deploys to `/Users/haseeb-mir/Downloads/IGI/igi1.exe`, and the Tester runs the candidate-runtime half of the same
   probe before the next method starts.
6. Batch verification comes **after** the individual tests: one combined probe/scenario over all accepted natives in
   the flow, on the full game, to confirm overall behaviour still matches; a batch mismatch reopens the individual
   native that diverged.

## Current position in the walk

- Methods 2-12 (0x0048F0C0 StoreCommandLine ... 0x0048F260) already have implementations in the tree from earlier work;
  their historical statuses are older-branch evidence and must be re-verified individually under this protocol.
- The first method in the order with **no implementation at all** is method 13 = `0x0048F280` (retail body 10 bytes,
  one block, caller passes an immediate argument at 0x0048F643). It is the next transcription target, and only after
  its static capture and retail-runtime probe are recorded.
- Methods 0x0048F200/0x0048F210/0x0048F280-0x0048F300 are the startup config setters (each one block, expected
  `mov eax,[esp+4]; mov [g_XXXX],eax; retn` shape per BN; exact global addresses to be recorded per method from
  disassembly). 0x0048F360 AppMain_ParseCmdLineArgs (22 blocks) and 0x00498480 Display_DinputDll (38 blocks) follow.
- The QTaskList slice (0x004C1790/0x004C17C0/0x004C1800) was implemented before this rule was stated; it is provisional
  and must pass its own individual static + retail/candidate runtime tests before any status above STRUCTURALLY_VERIFIED.


## Static closure facts from P05b (build/audit/p05b-20261023T222309Z)

Evidence: `summary.md`, `registration-order.md`, `per-native/*.md`, raw BN/r2 captures, retail
SHA-256 recorded in every file. These are ANALYZED facts, not runtime acceptance.

- **U1 closed statically.** `Game_Init` 0x415530 itself parses the `Level` switch:
  `push 0x415B10; push "Level"; call 0x48F360` @0x41553A, so the Level handler runs inside
  `QTask_CreateTask` -> 0x402930. It sets mission id `[0x539560]` and calls `0x48F210(0)`, leaving
  `[0x5C8C08]=0`; `0x401020` then takes `Flow_SetState(4)`, matching the single observed
  `Flow_SetState` call. No pre-CreateTask Level parse is missing from main.
- **Setter semantics (evidence-backed):** 0x48F280 -> `[0x5C8BE0]` (main 0; WinInput handler 1;
  proposed `Input_SetUseWin32Input`), 0x48F210 -> `[0x5C8C08]` (main 1; Level handler 0; proposed
  `Game_SetStartupMode`), 0x48F2B0 -> `[0x5C8BDC]` (main 0; Germany 1; HardcoreGerman 0; proposed
  `AppContext_SetGermanVersion`), 0x48F300 = unbounded strlen+1 copy of the argument string into
  `[0x5C8C20]` (main empty 0x567C74; Germany "Germany"; USA "USA"; proposed
  `AppContext_SetLanguage`), 0x48F130 = 6-byte getter of `[0x5C8C00]` (`Display_GetScreenMode`).
  0x48F290 -> `[0x5C8C0C]`, 0x48F2A0 -> `[0x5C8C10]`, 0x48F200 -> `[0x5C8C04]` stay UNKNOWN: no
  readers were found by raw-reference search, so no semantic name is claimed.
- **Call-site expectations for the `window level1` retail scenario:** 0x48F280/290/2A0/2B0/200 one
  call each with 0; 0x48F210 two calls (1 from main, then 0 from the Level handler); 0x48F300 one
  call with the empty-string pointer and return 1; 0x48F120 one call with 1 before the window is
  created; 0x48F130 one call after the CreateWindowExA argument build.
- **0x48F360 `AppMain_ParseCmdLineArgs`** (286 B, 22 BN blocks, 33 r2 edges): delimiter constant
  `" \t"` @0x541D6C, tokenizer 0x4A5A0C (strtok-like 32-byte delimiter bitmap, static pointer
  0x936060), case-insensitive prefix match then `handler(token + strlen(name))`; `window` -> "",
  `level1` -> "1".
- **0x498480 `Display_ProbeDirectXVersion`** (catalog `Display_DinputDll` partly wrong): GetVersionExA,
  DINPUT.DLL/DirectInputCreateA, DDRAW.DLL/DirectDrawCreate, DDraw2/D3D COM QI (GUIDs
  0x534A30/50/60, CoInitialize/CoCreateInstance), writes DX tier to `*arg1` and platform to `*arg2`;
  main compares the tier against 0x700 and prints the DX-7 requirement message.
- **0x4F1160 corrected**: not `Script_ParseEnumReal32_5`; 27 B/4 blocks, no args/calls, returns 0 iff
  `[0xA758BE]==0 && [0xA758BD]!=0`, else 1 (flag predicate; writer 0x4EF790, readers 0x4F0660 and
  level-load callers). Name/meaning still unknown.
- **Registry layout:** type slots `0x00AF5EE0` stride 0x30, 384 slots, active byte +0x25, ctor +0x04,
  parent id +0x08, handler table `0x00A96AE0` (event_id + type_id*0x180), presence bytes
  `0x0054FF34`; 0x402930 is one block with 155 ordered registrar calls
  (`registration-order.md`), catalog alias `Flow_RunHandler` is wrong.
- **Do not inherit catalog aliases** for 0x48F280/290/2A0/210/2B0/300/200/130/530/080/920/930/950/960/
  4A5A0C/402930/4F1160 (all wrong) and 0x498480 (partial). Alias correction table in P05b summary §8.

## Runtime correction from Tester (build/tester/20261023T220230Z)

Individual full-game retail probes (suspended spawn, `igi.exe window level1`, cwd
`/Users/haseeb-mir/Downloads/IGI`, 20 s window) confirmed the main call sites and corrected two
expectations:

- **0x48F120 SetScreenMode is called twice**, not once: main at 0x48F61C with 1, then the
  `Window` CLI handler 0x48F350 (`push 0; call 0x48F120; pop ecx; ret`) at 0x48F352 with 0.
- **0x48F130 Display_GetScreenMode returns 0, not 1**, at main's read site 0x48F754: the Window
  handler reset `[0x5C8C00]` to 0 before the read, so main's CreateWindowExA style path takes the
  `0x37C80000` branch (`neg/sbb` gives 0, `and 0x5E380000` gives 0, `add 0x37C80000`).
  The getter was called 64 times in the window: 1 from main plus 63 later display/menu callers;
  all returned 0 because the global stays 0.
- Observed arguments: 0x48F280(0), 0x48F290(0), 0x48F2A0(0), 0x48F210(1 then 0 from the Level
  handler 0x415B26), 0x48F2B0(0), 0x48F300(0x00567C74, returns 1, NUL stored), 0x48F200(0).
- QTaskList retail counts with a 1024-call cap: New 17, Register 449 (earlier 256 was a cap
  artifact), Remove 66. Candidate still observes 0 for all three (task lifecycle not wired).

These nine natives are now integrated as `engine/src/retail/core/app_startup_globals.cpp`
(candidate sha256 02f661b963fcdcd125cb889b237ae278231ff9c59067daceebe92749639466c2); their
candidate runtime remains unobserved until the retail main/parser/handler order is wired.

## P05c corrections and wiring recipe (build/audit/p05c-20261023T223344Z)

- **The CLI parser 0x48F360 builds no handler table.** Its xrefs are only 0x48F080, the tokenizer
  0x4A5A0C and the delimiter constant 0x541D6C. The `0x005C8D28..0x005C8DF8` block is the
  **WinProc callback registry** (16 nodes x 12 bytes at 0x5C8D28, head 0x5C8DF0, sentinel 0x5C8DF4,
  tail 0x5C8DF8; used by 0x48F490 register / 0x48F4F0 remove / 0x4AF8F0 insert / 0x48F970 WndProc).
  CLI wiring needs only four immediate `(name, handler)` parser calls, no table.
- **Handler ABI proven byte-exact:** the parser computes `token + strlen(name)` (repne-scasb
  0x48F441-0x48F44D) then `push suffix; call dword [esp+0x224]` (0x48F44E), `add esp,4`; return
  ignored. Handlers are `int __cdecl f(char* suffix)`.
- **Four handler bodies, byte-exact:** 0x48F350 `push 0; call 0x48F120; pop ecx; ret` (9 B,
  Window); 0x48F920 `push 1; call 0x48F280; pop ecx; ret` (9 B, WinInput); 0x48F930
  `push "Germany"; call 0x48F300; push 1; call 0x48F2B0; add esp,8; ret` (21 B); 0x48F950
  `push "USA"; call 0x48F300; pop ecx; ret` (12 B). Literals 0x541D60 "Germany", 0x5382C8 "USA",
  0x541EAC "WinInput", 0x541EB8 "Window", delimiter 0x541D6C " \t".
- **Byte-exact main wiring block 0x48F5F4-0x48F6EE** (summary §2): StoreCommandLine ->
  StoreInstanceHandle -> SetConfigFileName("filesys.cfg") -> SetAppTitle("Innerloop") ->
  SetIsActive(1) -> SetScreenMode(1) -> SetDebugged(0) -> SetDebugCompileState(1) ->
  SetDebugtextState(0) -> SetLightmapsUsed(1) -> SetTerrainLightmapsUsed(1) -> SetWinInput(0) ->
  0x48F290(0) -> 0x48F2A0(0) -> SetStartupMode(1) -> SetGermanVersion(0) -> SetLanguage(0x00567C74,
  empty BSS) -> 0x48F200(0) -> WinProc-list init (zero 0x30 dwords @0x5C8D28) -> parse
  "Window"/0x48F350, "WinInput"/0x48F920, "Germany"/0x48F930, "USA"/0x48F950 -> 0x498480 DX probe
  (tier, platform) -> compare tier with 0x700.
- **Candidate issues recorded by P05c:** 0x48F120 candidate is semantically equivalent and its
  build-evidence hash matched, but its semantic-review contract wrongly claims no side effects
  despite writing 0x005C8C00, and its recorded command path was stale (both to re-issue before any
  sealing); the USA handler candidate does not compile (`data_5382c8` undeclared); globals existed
  only as extern declarations (now defined in app_startup_globals.cpp); `0x00567C74` is zero-filled
  BSS beyond the .data raw end 0x553000 (empty language string).
- **angr cross-check:** 0x48F360 42 nodes/61 edges within BN extent, 0x498480 94/97 within extent,
  0x402930 157 nodes = 1 entry + 155 call blocks + tail-jump target 0x4CEF30 (confirms BN extent,
  explains r2's merged 837131-byte function); all recovered automatically, no unresolved jumps.

## Parser runtime findings (Tester, build/tester/20261023T220230Z)

- **The parser has 7 call sites in `window level1`, not 5** (all return 0): main registers five
  handlers (Window 0x48F350 @0x48F6A6, WinInput 0x48F920 @0x48F6B5, Germany 0x48F930 @0x48F6C4,
  USA 0x48F950 @0x48F6D3, **HardcoreGerman 0x48F960 @0x48F7BE/return 0x48F7C3**), Game_Init parses
  "Level" (0x41553A -> handler 0x415B10), and HumanPlayer_Init parses "UnlimitedAmmo"
  (0x410066 -> handler 0x410360).
- **Two handler bodies were missing from P05c scope**: 0x48F960 (HardcoreGerman, main startup path)
  and 0x410360 (UnlimitedAmmo, HumanPlayer_Init path, later phase). 0x48F960 must be captured,
  verified and staged before main wiring can be complete.
- Tokenizer 0x4A5A0C: 21 calls in the window; start call from parser 0x48F3A0 (arg0 = parser stack
  copy, arg1 = delimiter literal 0x541D6C), continuation calls from 0x48F464 with arg0 = 0,
  successive in-string pointers, terminating 0.
- Handler runtime (retail, four sequential runs): 0x48F350 suffix = empty string at the tokenizer
  NUL, EAX 0, then SetScreenMode(0); 0x48F920 suffix empty, EAX 1, then F280(1); 0x48F930 suffix
  empty, EAX 1, F300("Germany") EAX 8 then F2B0(1); 0x48F950 suffix empty, EAX 4, F300("USA")
  EAX 4. One run captured an out-of-image F120 return address (0x1DB0A21) for the Window handler
  call; the handler body and the 0x5C8C00 1->0 transition are the causal evidence, recorded as a
  probe limitation, and the earlier run captured the in-image return 0x48F357.
- Staged parser/tokenizer/handlers source (build/staging/natives/app_main_commands.cpp sha256
  0ab8cb02...) passed Tester static review: BN/r2 bytes identical for all six lengths (655 B),
  BN blocks 22/13/1/1/1/1 and angr entries exact, semantics match with only the documented
  0x936060 -> file-scope continuation-pointer substitution.

## Window/loop closure from P05e (build/audit/p05e-20261023T230302Z)

- **WndProc 0x0048F970** (`LRESULT __stdcall`, `retn 0x10`, 258 B, 21 blocks): first runs the
  WinProc callback registry when `[0x5C8DE8]!=0` (12-byte nodes from head `[0x5C8DF0]`, sentinel
  `0x5C8DF4`; a callback returning 0 consumes the message). Messages: WM_CREATE -> 0;
  WM_DESTROY -> clear `0x5C8DE8`, ShowCursor(1), QTask_DispatchEvent, PostQuitMessage(0);
  WM_ACTIVATEAPP -> 0x498460; WM_SYSKEYDOWN -> Alt+F4 when `[0x5C8E04]`; WM_SYSKEYUP clears the
  ALT-down flag; default -> DefWindowProcA.
- **Class binding proven:** RegisterClassA registers `lpszClassName = lpszMenuName = [0x541D68]
  -> 0x53B534 "Innerloop"` with `lpfnWndProc = 0x0048F970`; CreateWindowExA uses the same class
  and title "Innerloop". So class "Innerloop" -> WndProc 0x0048F970.
- **0x0048F0E0 StoreWindowHandle** = `mov eax,[esp+4]; mov [0x005C8BC4],eax; ret` (HWND global).
- **0x0048F170** = getter of `[0x005C8BFC]` (AppContext_GetIsActive, writer 0x48F140).
  **0x0048F490** register / **0x0048F4F0** remove WinProc callbacks (12-byte list nodes, head
  0x5C8DF0, sentinel 0x5C8DF4, tail 0x5C8DF8; errors "Failed to register WinProc callback." /
  "Unable to remove WinProc callback" then spin); list insert/remove via HeapList_Insert 0x4AF8F0 /
  HeapList_Remove 0x4AF960.
- **0x00498460 is NOT Display_ClearSurfaces_16**: it sets the x87 control word
  (`fstcw [0x6E5BC4]; or word [..],0x0C00; fldcw`) to truncate mode; main's loop self-check loads
  -1.75f (`0xBFE00000`), `fistp` and fails with "Invalid FPU rounding mode..." if the low dword
  is not -1 (0x48F8E1 spin). Callers: main 0x48F7A5 + loop 0x48F80D, WndProc 0x48F9EA.
- **Main loop 0x48F7D3-0x48F911:** FPU self-check, 0x498460, getter 0x48F170; inactive ->
  Sleep(500) via IAT 0x5330F0, active -> QTask_OpenEvent 0x401050; `[0x5C8DE8]==0` ->
  DestroyWindow 0x5331E4; message pump PeekMessageA 0x5331C0 / GetMessageA 0x5331E0 /
  TranslateMessage 0x5331C4 / DispatchMessageA 0x5331C8 (WM_QUIT 0 -> shutdown).
- **Shutdown 0x48F8A7:** SetForegroundWindow 0x5331CC, UnregisterClassA("Innerloop",hInstance)
  0x5331D0, CoUninitialize 0x533220, ReleaseMutex 0x533138, return exit code. Built `add esp,0x464;
  retn 0x10`; main extent 0x48F530-0x48F911. Mutex-fail block 0x48F8F0-0x48F90F is the
  "IGI is already running." MessageBox + return 0 path, not a shutdown path.

## Main transcription checklist (0x48F530 -> 0x48F911)

Write `engine/src/retail/core/main.cpp` as the single retail startup flow, in this order, replacing
the candidate's duplicate early init (which currently calls StoreCommandLine/StoreInstanceHandle/
SetIsActive/Lightmaps from different sites, causing the pre-wiring MISMATCH observations):

1. CreateMutexA(NULL,TRUE,"IGIMUTEX"); GetLastError twice; fail path MessageBoxA("IGI is already
   running.","Error",0x10)+return 0 (candidate keeps its documented IGI1MUTEX divergence).
2. CoInitialize(NULL); GetForegroundWindow; LoadIconA(hInstance,0x7F00); LoadCursorA(NULL,0x7F00);
   RegisterClassA(WNDCLASSA{style 3, lpfnWndProc=0x48F970, hInstance=arg1, hIcon, hCursor,
   hbrBackground=7, lpszMenuName=lpszClassName="Innerloop"}).
3. StoreCommandLine(arg); StoreInstanceHandle(hInstance); SetConfigFileName("filesys.cfg");
   SetAppTitle_48F100("Innerloop"); AppContext_SetIsActive(1); Display_SetScreenMode_48F120(1);
   AppContext_SetDebugged(0); SetDebugCompileState_48F1C0(1); AppContext_SetDebugtextState(0);
   AppContext_SetLightmapsUsed(1); AppContext_SetTerrainLightmapsUsed(1);
   Input_SetUseWin32Input_48F280(0); AppMain_Set5C8C0C_48F290(0); AppMain_Set5C8C10_48F2A0(0);
   Game_SetStartupMode_48F210(1); AppContext_SetGermanVersion_48F2B0(0);
   AppContext_SetLanguage_48F300((char const*)0x567C74 empty BSS); AppMain_Set5C8C04_48F200(0).
4. Zero 0x30 dwords at WinProc list base; init head 0x5C8DF0=0x5C8DF4, sentinel 0x5C8DF4=0,
   tail 0x5C8DF8=0x5C8DF0 (port: equivalent list storage).
5. ParseCmdLineArgs_48F360("Window",0x48F350); ("WinInput",0x48F920); ("Germany",0x48F930);
   ("USA",0x48F950).
6. Display_ProbeDirectXVersion 0x498480(&tier,&platform); if tier < 0x700: sprintf
   "This application requires DirectX version %d.%d or greater to run." (0x541E68),
   MessageBoxA(... ,"Error",0x10), exit 0 (0x4A5AD5 Camera_ApplyBandFilter_6-> exit).
7. CreateWindowExA(0, "Innerloop","Innerloop", 0x37C80000 style from mode 0, 640x480 centered,
   hInstance, NULL); if null -> "Couldn't open window" error box; ShowCursor(0);
   ShowWindow(hwnd,10); UpdateWindow(hwnd); StoreWindowHandle 0x48F0E0(hwnd); 0x498460 (x87
   truncate rounding); QTask_CreateTask 0x401000; store AL in byte [0x5C8DE8]; then
   ParseCmdLineArgs_48F360("HardcoreGerman",0x48F960); if AL==0 -> "IGI is already running."-style
   failure path 0x48F8A7.
8. Loop: FPU self-check (-1.75f fistp compare -1, else Debug_Error spin 0x48F8E1); 0x498460;
   GetIsActive; if inactive Sleep(500) else QTask_OpenEvent 0x401050; if [0x5C8DE8]==0
   DestroyWindow(hwnd); PeekMessageA(PM_NOREMOVE) -> while messages: GetMessageA(&msg,NULL,0,0)
   (0 => WM_QUIT exit), TranslateMessage, DispatchMessageA, re-Peek; repeat if [0x5C8DE8]!=0.
9. Shutdown: SetForegroundWindow(hwnd); UnregisterClassA("Innerloop",hInstance); CoUninitialize();
   ReleaseMutex; return exit code (`add esp,0x464; retn 0x10`).
10. Entry switch: make this function the candidate entry (replace scene_main diagnostic entry) and
    delete/redirect the duplicate early init so each native has exactly one call site.

Acceptance after transcription: build, deploy, Tester six-scenario paired run, then manifest status
upgrades only where static+semantic+behavioral+paired runtime evidence exists.

## Blocker after main wiring: QTask_RegisterClasses (0x00402930)

Candidate now runs the retail main order through 0x48F7AA. The first fatal step is
`QTask_CreateTask` -> `QTask_RegisterClasses`, whose first statement in the port is
`ReportNotTranscribed("AI graph")` (throw), because the registrar chain is not yet
transcribed. Retail order of registrars captured so far (P05c/P05d packets):

1. AI graph 0x00508F80
2. AI detection 0x00489560
3. material defines 0x00407D60
4. level flow 0x00488BB0
5. game options 0x00402FF0
6. Level + Game classes 0x00415530  (port has RegisterGameClasses)
7. task list 0x004C1780            (port has QTaskList_Register)
8. Flow class 0x00401FC0           (port has RegisterFlowClass)
9. registrars after it (see 155-registrar order in build/audit/p05c-*/registrars*)

Each registrar is its own native and must pass the per-native protocol (BN + r2 CFG +
angr entry + semantic review + paired Frida) before it can appear in the chain; the
port must not register a subset, because the ids shift for every class after it.
`QTaskList_Register` (0x004C1790) is already linked and retail-observed, so items 7-8
are ready to wire as soon as 1-6 exist.

Consequence for runtime: until this chain is transcribed, candidate capture stops at
0x48F7AA (uncaught throw, documented divergence), so QTaskList trio and everything
after 0x48F7BE stay UNOBSERVED. Level 1 content loading (terrain/models/QVM) begins
only after this point.

### Main transcription deltas still open

- Window-creation block (0x48F596-0x48F7C8) still approximated (centering math and
  WNDCLASSA fields): replace with the byte-exact transcription from
  build/main-0048f530.disasm.txt.
- 0x498480 (DirectX tier gate) and 0x498460 (x87 rounding control word) not linked.
- Port symbols named Host_*/Retail* must be transcribed or removed (AGENTS.md 10.14):
  Display_HostGetWidth/HostGetHeight/HostSetDimensions, ScreenBlock_HostInstallRect,
  Terrain_HostInstallRenderDevice, MefModel_ReadRetailRenderFormat,
  MefSkinnedVertex_ReadRetailRecord, TaskTypes_RegisterRetailClasses,
  RunRetailModelScene.
