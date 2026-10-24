# QVM argument accessors `0x004B8A20`, `0x004B8A50`, `0x004B8A80` — transcription record

Artifact: retail Project IGI `igi.exe`,
SHA-256 `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`, image base `0x00400000`.
Raw bytes below were re-read from that image at file offset `VA - 0x00400000`; the three bodies were
cross-checked against `build/retail-dis.txt` and radare2 6.2.0 `af`/`pd`.

Status of this record: static transcription only. No build of the three files, no runtime capture, no
manifest/CMake edits.

## 1. Files written

| Retail VA | File | Status |
| --- | --- | --- |
| `0x004B8A20` | `engine/src/retail/script/native_4b8a20.cpp` | TRANSCRIBED, runtime UNOBSERVED |
| `0x004B8A50` | `engine/src/retail/script/native_4b8a50.cpp` | TRANSCRIBED, runtime UNOBSERVED |
| `0x004B8A80` | `engine/src/retail/script/native_4b8a80.cpp` | TRANSCRIBED, runtime UNOBSERVED |

All three are syntax-checked for the host compiler with
`clang++ -std=c++17 -fsyntax-only -Iengine/include`; nothing else was built.

## 2. `0x004B8A20` — `Script_EvalArgInt_4B8A20`

Retail bytes (46 bytes, SHA-256 of the span as read):

```
8b 44 24 04 8b 48 04 85 c9 74 0f 8b 4c 24 08 51 50 e8 7a 40 00 00 83 c4 08 c3
8b 50 08 8b 44 24 08 8b 0c 82 51 e8 d6 06 00 00 83 c4 04 c3 90 90
```

- CFG: 3 blocks, 2 branch edges, 17 instructions, `0x004B8A20-0x004B8A4D`; r2 `nbbs=3`,
  `ninstrs=17`, `nargs=2`, `stackframe=8`.
- ABI: cdecl, two stack arguments, both read: `[esp+4]` argument frame pointer, `[esp+8]` 32-bit index.
  No prologue, plain `ret`.
- Flow: flag dword at frame `+0x04`; non-zero → `Qvm_ReadIndexedInteger_4BCAB0(frame, index)` (two
  pushes, `add esp,8`); zero → table word at `+0x08`, 32-bit scale-4 index (`mov (%edx,%eax,4),%ecx`),
  then `Qvm_EvaluateIntegerNode_4B9120(node)` (one push, `add esp,4`). Both exits return the callee's
  EAX unchanged.
- Retail call sites: 231.
- Candidate disagreement: `scripts/GameNatives/004b8a20-Script_EvalArgInt.cpp` is a Binary Ninja
  pseudo-C dump whose prototype is `int32_t(float, int32_t)`, whose callee call passes four words plus
  an uninitialised local, and which names the callees `Sys_ReadFileBlock_24` /
  `Sys_AllocateMemory_28`. The flag test direction matches retail, the rest does not; the transcription
  follows the bytes only.

## 3. `0x004B8A50` — `Qvm_GetFloatArg`

Retail bytes (44 bytes):

```
8b 44 24 04 8b 48 04 85 c9 74 0f 8b 4c 24 08 51 50 e8 aa 45 00 00 83 c4 08 c3
8b 50 08 8b 44 24 08 8b 0c 82 51 e8 e6 0b 00 00 59 c3 90 90
```

- CFG: 3 blocks, 2 branch edges, 17 instructions, `0x004B8A50-0x004B8A7B`; r2 `nbbs=3`,
  `ninstrs=17`, `nargs=2`, `stackframe=8`. Binary Ninja/radare2 agree on 3 blocks / 2 edges (the
  candidate file records angr recovering 5 blocks because it splits at the calls).
- ABI: cdecl, two stack arguments, both read: `[esp+4]` frame, `[esp+8]` 32-bit index. Returns the real
  on the x87 stack (`long double`); the body performs no conversion, no store and no x87 arithmetic.
- Flow: flag `+0x04` non-zero → `Qvm_ReadIndexedArgument(frame, index)` (`add esp,8`); zero → table
  `+0x08`, 32-bit scale-4 read, `Qvm_EvaluateArgumentValue(node)` with `pop %ecx` as the cleanup.
- Retail call sites: 201 (matches the catalogue count).
- Candidate check (`scripts/GameNatives/qvm/004B8A50-Qvm_GetFloatArg.cpp`):
  - AGREES: entry, 3 blocks / 2 edges, the `+0x04` flag, the `+0x08` scale-4 table, the two x87
    callees returning in ST0, the `long double` return type.
  - DISAGREES: (a) **there is no char-sized indexed branch** — the indexed access is the 32-bit
    `mov (%edx,%eax,4),%ecx`; no byte load exists at `0x004B8A50` (the only byte store in this family
    is `movb $0,(%esi)` inside `0x004BD040`); (b) the candidate reads through file-local helpers plus
    `memcpy` (`load_word`, `pointer_from_word`), which the no-helper rule forbids; (c) it names the
    callees `…_4BD010` / `…_4B9660` while `qvm_execute.cpp` and this file keep one name per VA
    (`Qvm_ReadIndexedArgument`, `Qvm_EvaluateArgumentValue`).
  - The earlier wrong reconstruction is archived at
    `docs/fidelity/evidence/superseded/004B8A50-Profile_ValueSet-ghidra-dump-20261023.cpp`
    (uninitialised returns on the indexed branch, wrong `Profile_ValueSet` identity).
- Duplicate ownership (needs an integrator decision): VA `0x004B8A50` already has an identical
  transcription at `engine/src/retail/script/qvm_execute.cpp:200` with its shared declaration at
  `engine/include/igi/retail/script/qvm_execute.hpp:68`; the manifest records that path as
  STRUCTURALLY_VERIFIED. `qvm_execute.cpp` is deliberately **not** in the `igi` source list — it is an
  `OBJECT` library (`igi_qvm_execute_object`, CMake lines 444-449) used as compile evidence precisely
  because its callees and three QVM state words are missing. The new file is the address-bound owner
  required by the one-file-per-VA rule; one of the two copies must be retired, and the
  `qvm_execute.hpp` comment updated, when the file joins a target.

## 4. `0x004B8A80` — `Sys_ResolveSymbol_23_4B8A80`

Retail bytes (66 bytes):

```
8b 44 24 04 8b 48 04 85 c9 74 19 8b 4c 24 10 8b 54 24 0c 51 8b 4c 24 0c 52 51 50
e8 a0 45 00 00 83 c4 10 c3 8b 54 24 10 8b 4c 24 0c 52 8b 50 08 8b 44 24 0c 51 8b 0c 82 51
e8 22 02 00 00 83 c4 0c c3 90 90
```

- CFG: 3 blocks, 2 branch edges, 25 instructions, `0x004B8A80-0x004B8AC1`; r2 `nbbs=3`,
  `ninstrs=25`, `nargs=4`, `stackframe=16`.
- ABI: cdecl, four stack arguments, all read: `[esp+4]` frame, `[esp+8]` index, `[esp+0xC]`
  destination buffer, `[esp+0x10]` count. Returns the callee's EAX; the six handler call sites
  declare `char*` and ignore it, and the two observed callee exits give `0` and the destination
  pointer, so the return type is the callers' type rather than a proven result.
- Flow: flag `+0x04` non-zero → `Qvm_ReadIndexedBlock_4BD040(frame, index, destination, count)`
  (four pushes, `add esp,0x10`); zero → table `+0x08`, 32-bit scale-4 read, then
  `Qvm_ResolveSymbolBlock_4B8CE0(node, destination, count)` (three pushes, `add esp,0xC`).
- Retail argument staging: destination and count are loaded before the pushes, and frame/index are
  re-read from the shifted stack (`[esp+0xC]` after the first push). That is the compiler's shuffle of
  the same four arguments, not extra arguments; the transcription passes the four parameters directly
  and records the staging in the comments.
- Retail call sites: 75 (matches the catalogue count).
- Name: `Sys_ResolveSymbol_23` is a catalogue label. Its `_23` suffix is **not** evidenced by the body:
  the neighbouring catalogue entry `0x004B8AD0` carries the same suffix (`Sys_CheckHeap_23`), and this
  body never compares anything against 23. The suffix is therefore a catalogue grouping label, not an
  argument count, symbol class or constant; the four-argument ABI is the evidence.
- Catalogue prototype `char*(float arg1, int32_t arg2, char* arg3, char* arg4)` is stale in the first
  argument: the body dereferences it (`mov 0x4(%eax),%ecx`) and the handler call sites pass a frame
  pointer.

## 5. CFG measurements (radare2 6.2.0 `af`/`pd` on the retail image)

| VA | bytes | blocks | branch edges | instructions | r2 nargs | r2 stackframe | retail call sites |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `0x004B8A20` | 46 | 3 | 2 | 17 | 2 | 8 | 231 |
| `0x004B8A50` | 44 | 3 | 2 | 17 | 2 | 8 | 201 |
| `0x004B8A80` | 66 | 3 | 2 | 25 | 4 | 16 | 75 |

## 6. Why these three files do not by themselves make the branch link

Each accessor is a two-branch wrapper whose real work is in untranscribed callees, so adding the three
files to the `igi` link replaces the three undefined symbols with six:

| Callee | Bytes | Blocks | Instrs | Manifest | Referenced by |
| --- | --- | --- | --- | --- | --- |
| `0x004BCAB0` (`Qvm_ReadIndexedInteger_4BCAB0`) | 41 | 1 | 14 | absent | `0x004B8A20` |
| `0x004B9120` (`Qvm_EvaluateIntegerNode_4B9120`) | 1188 | 65 | 484 | absent | `0x004B8A20` |
| `0x004BD010` (`Qvm_ReadIndexedArgument`) | 41 | 1 | 14 | absent | `0x004B8A50` |
| `0x004B9660` (`Qvm_EvaluateArgumentValue`) | 1265 | 75 | 447 | absent | `0x004B8A50` |
| `0x004BD040` (`Qvm_ReadIndexedBlock_4BD040`) | 38 | 1 | 16 | absent | `0x004B8A80` |
| `0x004B8CE0` (`Qvm_ResolveSymbolBlock_4B8CE0`) | 778 | 41 | 262 | absent | `0x004B8A80` |

The three small ones (`0x004BCAB0`, `0x004BD010`, `0x004BD040`) are 13-16-instruction wrappers that all
call the same 274-byte / 15-block / 100-instruction selector routine `0x004BCAE0`, which is also
absent and itself calls `0x004BCFD0` (present: `Qvm_ExecuteInstructions`, VERIFIED), `0x004BCEC0`
(absent), `0x004BCC00` (absent), `0x004A5500` (absent), `0x004AF7B0` (`Debug_Error`, VERIFIED).

The three big ones are the QVM value evaluators (signed type tag at node `+0x01`, jump tables at
`0x004B95C4` / `0x004B9000` / `0x004B8FEC`) and call `0x004B9040`, `0x004BABB0` (both absent),
`0x4AF7B0`/`0x4B7E10` (present) and, for `0x004B8CE0`, `0x5169B0` (present,
`Terrain_RenderQuadtree_4`, STRUCTURALLY_VERIFIED).

Consequences for the branch:

1. Adding `native_4b8a20.cpp`, `native_4b8a50.cpp`, `native_4b8a80.cpp` to the linked `igi` target
   keeps the link red; the missing set becomes the six callees above (plus their own callees).
2. The tree already has the sanctioned pattern for exactly this situation: `qvm_execute.cpp` and
   `humanplayer_defines.cpp` are `OBJECT` libraries that every build compiles but the link does not
   consume (`igi_qvm_execute_object`, `igi_humanplayer_defines_object`, CMake lines 444-472, with the
   rationale recorded in the comments). The six `0x0042D*`/`0x0042F*` handler files plus these three
   accessors fit that same pattern until the evaluator closure lands.
3. If a green link is the goal, the honest order is: `0x004BCAE0` → `0x004BCEC0`, `0x004BCC00`,
   `0x004A5500`, `0x004BCAB0`, `0x004BD010`, `0x004BD040` (small, local closure) → then the three
   evaluators `0x004B9120`, `0x004B9660`, `0x004B8CE0` (the expensive part) → then `0x004B8A20`,
   `0x004B8A50`, `0x004B8A80` can be linked.

## 7. Blast radius inside the current tree

Symbols and their engine references (all in `engine/src`):

| Symbol | References in the igi link | Compile-only references |
| --- | --- | --- |
| `Script_EvalArgInt_4B8A20` | 8 calls + 1 local declaration, all `core/native_42db00.cpp` (`0x0042DB43`, `0x0042DB77`, `0x0042DB8A`, `0x0042DBAF`, `0x0042DC27`, `0x0042DC3D`, `0x0042DC67`, `0x0042DC9D`) | none |
| `Qvm_GetFloatArg` | 4 calls in `core/native_42db00.cpp` (`0x0042DB9C`, `0x0042DBC2`, `0x0042DBE2`, `0x0042DC0B`) + 2 in `core/native_42fe80.cpp` (`0x0042FEB5`, `0x0042FED1`) | 11 calls in `script/humanplayer_defines.cpp` (object-only target, not linked); the definition itself sits in the object-only `script/qvm_execute.cpp` |
| `Sys_ResolveSymbol_23_4B8A80` | 2 calls in `core/native_42db00.cpp` (`0x0042DC57`, `0x0042DCC4`) + 1 in `core/native_42fe80.cpp` (`0x0042FEA0`) | none |

No other engine translation unit references the three symbols. So once the three accessors are in the
link, the only consumers are the six handler files; `humanplayer_defines.cpp` would additionally start
resolving `Qvm_GetFloatArg` only if that object is ever linked, which is currently forbidden by its own
missing closure (13-function list in `docs/fidelity/evidence/humanplayer-defines-link-blockers.json`).

## 8. What this record does not claim

- No build, no link, no runtime observation for any of the three bodies or their callees.
- The descriptive callee names (`Qvm_ReadIndexedInteger_4BCAB0`, `Qvm_EvaluateIntegerNode_4B9120`,
  `Qvm_ReadIndexedArgument`, `Qvm_EvaluateArgumentValue`, `Qvm_ReadIndexedBlock_4BD040`,
  `Qvm_ResolveSymbolBlock_4B8CE0`) are labels for untranscribed addresses; the two unsuffixed ones are
  the names `qvm_execute.cpp` already uses for `0x004BD010` and `0x004B9660`, kept so that one
  definition per VA can satisfy every declaration.
- The meaning of the `+0x04` frame flag, the `+0x08` table contents beyond "indexed value or expression
  node", and the `count` argument of `0x004B8A80` are not established beyond the instructions listed
  here; they stay open questions rather than invented semantics.
