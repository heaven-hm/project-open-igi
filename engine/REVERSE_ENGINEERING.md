# Retail Reverse-Engineering Evidence

This directory is a new engine implementation. It does not link to the legacy engine, the editor, the converter, or the C# project. Those projects and the native catalogs may be used to locate candidate behavior, but a routine enters this tree only with evidence from the retail executable.

### Naming rule

The C++ names in this directory are descriptive port identifiers chosen from observed behavior (for example, `Sound_ExtractNameFromPath`). They are not claimed to be original retail symbols unless a retail symbol is independently established. Every implementation retains its retail virtual address in this evidence record and in its source declaration. Unimplemented routines are referred to by address and behavior rather than exported as `FUN_XXXXXXXX` placeholders. Candidate names from catalogs or other ports remain provisional until the retail binary confirms them.

## Binary under analysis

- Artifact: `/Users/haseeb-mir/Downloads/IGI/igi.exe`
- Format: PE32 x86
- SHA-256: `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`

## First verified slice: audio string routines

| Retail VA | Local symbol | Ghidra evidence | Radare2 cross-check | Observed behavior |
| --- | --- | --- | --- | --- |
| `0x00497390` | `Sound_ExtractNameFromPath` | pseudocode `ev_3014beff32e4ee2bed84ac8cdbc8932fa0b358a1c35982e4d90781999805c2e2`; assembly `ev_328f1efa85f78fa027c70ac148cd6fb976d4e7ec0ea45911d2d95b8dca80243e` | 124 bytes, 14 basic blocks, 58 instructions, cdecl | Clears output; logs empty/dotless input through `Engine_LogFormattedMessage`; finds the first dot and scans backward for `/`, `\`, or `:`; copies the filename segment before that dot. If no separator is found, returns with output empty and does not log. |
| `0x00497410` | `Sound_ComputeWeightedNameHash` | pseudocode `ev_3014beff32e4ee2bed84ac8cdbc8932fa0b358a1c35982e4d90781999805c2e2`; assembly `ev_43574c2bf4aa848470f93b52598b9a61346825bc4a4fa719eb1b266257c815e9` | 63 bytes, 6 basic blocks, 24 instructions, cdecl | Uppercases ASCII `a` through `z` in place and returns a 32-bit weighted sum. Weight starts at 1, advances by `0x13d`, and is masked to 16 bits per byte. |
| `0x00497450` | `Sound_ComputeRollingNameHash` | pseudocode `ev_3014beff32e4ee2bed84ac8cdbc8932fa0b358a1c35982e4d90781999805c2e2`; assembly `ev_07f85bcd1948d3a7ac236b967ecf33db7823bbc17fe0fe2664d2beca3617e218` | 76 bytes, 6 basic blocks, 30 instructions, cdecl | Uppercases ASCII `a` through `z` in place. The assembly additionally computes a modulo-101 rolling remainder using weights starting at 1 and advancing by `0x13e`; Ghidra's pseudocode declares the function `void` and omits that computation. The C++ slice records and tests the observed register result, while the return-value ABI remains unresolved. |

## Layouts, globals, and platform boundary

This slice requires no game object layouts or engine globals. `Sound_ExtractNameFromPath` calls the retail logger at `0x004af810`; that logger remains an unresolved native dependency and is only stubbed by the focused test executable. The project has no third-party runtime dependencies. Windows x86 calling-convention annotations are retained, but a Windows x86 build and a CrossOver runtime comparison have not yet been performed.

The retail filename routine has no output-capacity argument. The C++ function preserves that pointer-based ABI shape, so callers must provide the same valid writable capacity the retail callers provide. The reverse-engineering record has identified caller output buffers of different sizes; boundary conditions and exact maximum lengths still need to be verified before broader use.

## Verification status

- Static: Ghidra and Radare2 independently agree on the three function sizes and basic-block counts. Ghidra assembly was inspected for the lowercase comparisons, hash recurrences, and return-register discrepancy.
- Unit: local tests cover basename extraction, all three separators, invalid-name logging, embedded NUL behavior, ASCII mutation, and a sample result for each observed hash computation.
- Runtime comparison with retail IGI: not yet performed. No full engine or parity claim is made.


## Second verified slice: retail frame dispatch

| Retail VA | Local symbol | Ghidra evidence | Radare2 cross-check | Observed behavior |
| --- | --- | --- | --- | --- |
| `0x00401050` | `Game_DispatchCurrentFrameCallback` | pseudocode `ev_48d315b3a3fbf99f93b1b83707e82ee2637ba682446b4b105360d056556eb02f`; body `ev_58c27bd42f70a5cdb96758dddcc95033fb3741eed5a53e1e1e485f711c7e3a70` | 68 bytes, 3 basic blocks, 26 instructions, cdecl | Establishes the retail `__setjmp3` frame boundary; if it returns zero, reads a 16-bit index at current-record offset `0x1c`, loads the callback at `0x00A970E0 + index * 4`, and calls it with the current record. A nonzero return stores zero through `Game_SetLoopStatus`. The indirect call target remains unresolved statically. |
| `0x00402880` | `Game_GetFrameDispatchRecord` | `ev_39e797b0762f14540a9660dc9b56c8af8b99575da7914b1d6ebd7aaf8343c349` | 6 bytes, 1 basic block, 2 instructions, cdecl | Returns global pointer `DAT_00567C8C`. |
| `0x004028A0` | `Game_SetFrameDispatchRecordValue` | `ev_3a7b54bb2de9688bce68e415f49522c74c4aa64ecf1b3fe0202110604fe28884` | 14 bytes, 1 basic block, 4 instructions, cdecl | Stores the 32-bit argument at `DAT_00567C8C + 0x2c`. The field meaning is unknown, so the C++ view keeps its field name `unknown_0x2C`. |
| `0x0048F480` | `Game_SetLoopStatus` | `ev_7dc4cf10ce733b10f7e8e89630b03786ffd53ec298ebd92a65be4b6c8c9c604d` | 10 bytes, 1 basic block, 3 instructions, cdecl | Stores its byte argument in `DAT_005C8DE8`. |

The Ghidra and Radare2 extents and basic-block counts agree for these routines. For `Game_DispatchCurrentFrameCallback`, counted conditionals and direct/indirect calls also match across the Ghidra listing and Radare2 disassembly (CFG score 1.0 under the repository's documented score formula). The Windows caller `main` at `0x0048F530` invokes `Game_DispatchCurrentFrameCallback` at `0x0048F81B` from its timed Win32 message loop; that call site was confirmed in Radare2 disassembly. The outer Win32 loop, task-type registration chain reached at `0x00402930`, Level 1 QVM execution, and Level 1 rendering are not yet in the new C++ target.

The C++ implementation uses the host C runtime's `setjmp`/`longjmp` mechanism to represent the nonlocal frame abort. It is not yet verified against MSVC x86 `__setjmp3`, CrossOver execution, or native Windows execution. The dispatch-table extent and reachable indices remain unresolved; tests provide only the entries needed by their controlled cases.

### Current Level 1 status

- `game-engine/` has no executable entry point or platform loop yet.
- No Level 1 assets are parsed or rendered by this target yet.
- The audio helper and frame dispatch slices compile and pass focused tests only.
- Level 1 end-to-end parity is not implemented or claimed.

## REA cross-check: Level 1 human physics candidates (2026-10-17)

Target identity was re-read from the active REA session: `igi.exe`, PE32 x86, SHA-256 `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`, Ghidra 12.1.2. Evidence below is static analysis of that shipped artifact. It does not establish runtime parity.

| Retail VA | REA observation | C# comparison | Status |
| --- | --- | --- | --- |
| `0x00408460` | Body `0x408460..0x408715`, 694 bytes, 27 blocks. Evidence `ev_9b3c7666e662a02e0c10689a86914a7f65e80a50dbbb426f4a8c3612b302c5e1`. | `HumanMotion.Integrate` and `HumanFallImpact.Calculate` in `OpenIGI.Game/Player`. | Core equations align with the current Ghidra pseudocode: vertical velocity subtracts `84.74169`; speed conversion is `abs(vz) * 0.00024414062 * 30`; damage starts above `14`, scales by `0.07692308 * maximumHealth`, and is uncapped; view kick clamps `speed * 0.125` then multiplies by `-1024`; damage sounds branch at 10% and 20% of maximum health. The event payload selects `20480` for no damage and `40960` for damage. Runtime and x87 bit-level parity remain unverified. Air-control scale comes from global `DAT_0056E1F4`; its initializer and relation to the C# `Speed3` parameter still need tracing. |
| `0x004614B0` | Body `0x4614B0..0x461A6B`, 1468 bytes, 31 blocks. Evidence `ev_3e5f1c0885f0b58850b1f2f68776268648bdf1d490c31068a1d769b35e37caac`. Callers include `0x4109A0`; callees include wall sweep `0x461A70`, ground query `0x462390`, and related collision routines. | `PlayerBody.cs`, `HumanMotion.cs`, and `HumanGroundQuery.cs`. | Ghidra shows the position delta starts as velocity (`+0x664..+0x66C`) plus animation root delta (`+0x520..+0x528`). Slope response has angle bands at 45/50/60/80 degrees with constants `18.962963`, `56.88889`, `948`, and `1137`; slope and hit-shove accumulators are separately damped by `0.9`; wall/object collision and ground-query passes follow. This supports the C# decomposition, but exact data-field meanings, full operation order, precision, and native runtime traces remain open. |
| `0x0040BBD0`, `0x0040BE40`, `0x0040E620` | Each wrapper calls `0x408460` before checking the grounded flag; their bodies then clear velocity words `+0x664..+0x678` on landing. Evidence `ev_83e516172df42f30d2419741d86f8ca93c6247f9bb790eeb93d131eb43268393`, `ev_84f5ef9c0df8ea51f1a06ad14ecb9bed3bdf00a7175fb67f2b3c24463ef1a24e`, and `ev_00ed83f104b908322a388ce63cace44062c188bc1e4694c4c8c5032f7a22b4bb`. | `PlayerBody.ProcessPendingLanding`. | Confirms the landing/fall routine executes before the grounded transition clears motion. The C# extra-gravity-step comment is consistent with this call order; verify the exact per-tick transition under CrossOver before claiming runtime match. |

### Native-label and address corrections

- `0x00408320` is a real six-byte function, but REA names it `FUN_00408320` and decompiles it as a return of global `DAT_00538518` (evidence `ev_9161d57ca78da7c04f330d9dfdef8f2241f995857dcd59b5f42b2075b6c5279d`). The Binary Ninja export calls it `HumanPlayer_UpdateMotion`; that label is not evidence that it integrates movement. Do not port it as a movement routine based on the catalog name.
- `0x00408E70` is a real 357-byte, 18-block state/task dispatcher in REA (evidence `ev_c0f4e329b8ed04f8810348daa7dcc53decc4b7d592adbe838f14cf6816ff1532`). It selects callback slots from task state and flag bits; its Binary Ninja label `HumanPlayer_UpdateMotion_2` is likewise provisional. Ghidra reports no direct caller because its use is indirect.
- The current Ghidra session has no procedure at `0x00412990`, despite the Binary Ninja export at that address being titled `HumanPlayer_Update`. That export is not eligible for a port until its exact entry/body mapping is resolved against the retail bytes.
- REA resolved `0x0040F4A0` as a one-block task callback setup routine (evidence `ev_16e5d54f6688eb7d2c122cf01c39585724e7e35cca28cfb9725587697b56ef36`), not a movement integrator.

### Next static/runtime obligation

Trace the retail Level 1 task registration and the `0x4109A0` human update into its indirect state callbacks; identify the jump-entry and locomotion state handlers by call/data flow, not by exported names. Then compare their C# candidates against retail x86 behavior, including the source of `DAT_0056E1F4`, object-field offsets, x87 arithmetic, and a matched CrossOver input/state trace. No native bodies were copied and no engine rebuild was performed for this analysis slice.

## REA cross-check: retail human per-tick orchestration

`0x004109A0` is a confirmed 1480-byte, 45-block Ghidra procedure (`FUN_004109a0`), evidence `ev_4ad28696c728c501aad9235ca76ab529365393f4195ddd4c521e2b194da4051e`. It is the best current retail anchor for the human per-tick orchestration; its high-level name remains unproven because the retail binary is stripped and the observed symbol is Ghidra-generated.

Static call order observed in the pseudocode:

1. Calls several task/state helpers, including the movement-state path, before sampling the animation's new root delta.
2. Transforms animation delta into `Human+0x520..0x528`, applies global scale `DAT_0056E1E8` unless gate byte `Human+0x52C` is set, then rotates the delta through the orientation basis at `Human+0x78..0x98`.
3. Calls `0x4614B0` when attachment field `Human+0x76C` is zero; otherwise calls `0x462CF0`.
4. Continues with weapon/player/AI-related calls after movement, including `0x410F70`, `0x463920`, `0x411240`, `0x411300`, `0x411350`, and `0x4113C0`.
5. Emits formatted diagnostic values for vertical speed, ground task, attachment, movement delta, ground distance/angle, and transformed animation delta.

The C# `PlayerBody.TickAnimated` has the same broad state-before-animation-sample ordering, but its own remarks say only idle transitions drive the animator and the full state graph is not yet executed. `PlayerBody.Tick` covers the movement slice; it is not a substitute for the full retail orchestration at `0x4109A0`, which also coordinates other player systems.

**Implementation order implication:** port/verify the per-tick task orchestration and exact callback tables before treating isolated physics routines as an integrated Level 1 player. The next evidence task is to resolve the indirect state-handler targets and trace `DAT_0056E1E8` / `DAT_0056E1F4` initialization and uses. The export name `Human_RunUpdate` at this address is still a candidate label, not a confirmed retail symbol.


## REA-verified ILSF ADPCM decoder slice

Target identity: retail igi.exe, PE32 x86, SHA-256
dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.

| Retail VA | Port identifier | REA evidence | Radare2 cross-check | Observed behavior |
| --- | --- | --- | --- | --- |
| 0x00496420 | Sound_InitializeAdpcmState_00496420 | body ev_7df35efa43d57ad4f59ef22afeae1dbb06acd349e9120e6793355f7548eebc2, info ev_92af21e086ff941e5903fb2cf433257d4917d71a0340e06051535334a44689ad | 34 bytes, 1 basic block, cdecl | Stores channel count, clears predictor and step-index words at offsets 0x02..0x08, and clears pending-nibble byte at 0x0A. It does not write byte 0x0B. |
| 0x00496280 | Sound_DecodeAdpcmFrames_00496280 | body ev_7a60a515ec8a85fe9d561a02113d64ef1ab68ce48dae525481db4d1a2ba289a7, assembly ev_266834f2400644c7e7aab17f5ad98cb99ca690f6a6545655f0831f9a5fe1e2ba, info ev_9848ca12e1e431e8dab244ed611ac16b8df65249c38181f7ab8b3cdafcc9497c | 409 bytes, 29 basic blocks, cdecl; callers at 0x00495F70 and 0x00496CE0 | Decodes high nibble first, keeps a predictor and step index per channel, interleaves channel samples per frame, clamps step index to 0..88, and saturates samples to signed 16-bit range. |

The step-index bytes read from 0x00543920 are
ffffffff02040608ffffffff02040608 (REA evidence ev_347b7035f8d055aed4a9592de654a289f1cd67c381834e7eeb0ce84436515edc).
The 89 little-endian step values read from 0x00543930 occupy 178 bytes and match the table in the C# candidate
OpenIGI.Formats.Sound.IlsfAdpcmDecoder (REA evidence
ev_7eed7133019edc40e6081e28a968625be445832895de90790e51b604d983dbfb).

**Catalog label conflict:** the generated Binary Ninja native catalog calls 0x00496280
Display_SwapBuffers_10 and 0x00496420 Display_ClearSurfaces_10. REA's shipped-binary bodies,
the matching data tables, and the sound-loader call edges show ADPCM state initialization/decoding at
those addresses. These catalog labels are therefore not used as semantic names in this port.
The address-qualified identifiers above are descriptive port names, not claims about original retail symbols.

**Streaming detail:** the retail decoder does not retain a half-consumed input byte. When the state flag at +0x0A
is set, it reads that call's input[0] low nibble and advances the input cursor past that byte. A caller continuing
a stream must re-present the partially consumed byte at the beginning of the next buffer. The C# candidate
retains the pending byte internally and accepts forward-only buffers; its sample math is a cross-reference,
but its streaming API is not copied because it differs from the retail routine.

The C++ helper preserves the observed record layout and valid-input sample operations. Its spans and explicit
error result add bounds checks around the retail pointer contract; malformed channel counts, indices, or
undersized buffers return before state/output mutation. Unit tests cover initialization offsets, nibble order,
mono/stereo ordering, replayed-byte streaming, sample/index clipping, and input/output bounds.

A full Level 1 asset check decoded MISSIONS/location0/LEVEL1/SOUNDS/m1o_train1.wav (asset SHA-256
6fc93ad64ebbc18dcb0f548e445abab78f9030fdb06c3debef5af9afab4f5501). Both the C++ helper and the existing
C# IlsfAdpcmDecoder produced 87,040 mono samples from 43,520 encoded bytes; their 174,080 decoded PCM bytes
matched exactly (SHA-256 3338f0d4d3342b75affa4ae3b8a94312b21c3cd1df266d62328ed4e0c7a05ca9). One trailing byte
in the asset was not consumed, matching the frame-count-driven decode.

Static REA and Radare2 parity and the C# asset cross-check are recorded above. This slice has not yet been
compared against a live retail process under CrossOver, and it is not connected to the Windows bootstrap or a Level 1 mixer.

### Retail compiled QVM header and bytecode boundary

`Qvm_Load` at `0x004B80B0` is listed in the manually tested native catalog at
`/Users/haseeb-mir/Documents/Code/project-igi-natives/IGI_Natives/IGI-Natives.json`. Its retail path reaches
`FUN_004BD220`, which accepts the `LOOP` signature, revision `8`, a dword greater than `4` at `+0x08`, and a
file length greater than `0x3B`. It obtains the compiled bytecode span from `base + dword[0x2C]` with length
dword `[0x30]` (REA/Ghidra evidence `ev_bd052fcbe23ffdb47535d836372b20f7c7e07a937dfd95604e67a7ccf6b29747`, retail
`igi.exe` SHA-256 `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`).

`Qvm_LoadCompiledProgram` (retail address `0x004BD220`) implements that header gate and code-span selection with bounds checks. Other
header dwords remain named by offset because their semantics are not established by this native. Unit tests cover
signature/revision/code-range rejection and a corpus run against Level 1 `OBJECTS.QVM`, `MISSION.QVM`, and every
`AI/*.QVM` file. The corpus run accepted 39 compiled QVM files. This proves that the header and code ranges are
readable; it does **not** execute bytecode or prove the `Task_New` object transforms. The Level 1 fixed placement
records still need to be followed through the retail QVM interpreter and task constructors.

The same manual catalog identifies `QvmCompile` (`0x004B85B0`), `QscCompile` (`0x004B8410`), `QtaskUpdate`
(`0x004F0E90`), `TasktypeSet` (`0x004B8810`), `QTaskHashTableSet` (`0x004BAAC0`), and `GraphOpen`
(`0x004F9FF0`). These names are used as lookup hints; the current port has not implemented those routines or
verified their runtime parity.
